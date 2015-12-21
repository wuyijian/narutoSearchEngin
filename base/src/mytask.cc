#include "mytask.h"
#include <string>
#include "/home/wuyijian/simhash/src/CppJieba/QuerySegment.hpp"
#include "/home/wuyijian/simhash/src/Simhasher.hpp"
#include "json/json.h"
#include "MyLog.h"
#include "redis.h"

namespace elton
{

double getScore(const std::vector<double> & lhs, 
						const std::vector<double> & rhs)
{
#if 0	
	double N = 0;
	double M1 = 0;
	double M2 = 0;
	for (size_t idx; idx < lhs.size(); ++idx)
	{
		N += lhs[idx] * rhs[idx];
		M1 += lhs[idx] * rhs[idx];
		M2 += lhs[idx] * rhs[idx];
	}
	return N/(sqrt(M1) * sqrt(M2));
#endif
	//计算BM25相似性得分,采用相对简单的算法
	double score = 0.0;
	for(size_t idx = 0; idx < lhs.size(); ++idx)
	{
		score += lhs[idx] * rhs[idx];         //并不是完全的bm25算法，查询词文档weight是由 TF-IDF算法求出，查询词weight依据结巴分词计算，依据训练好的文档库
	}	
	return score;
	
}

struct Node{
	uint64_t weiboID_;
	double score_;
	friend bool operator<(const Node & lhs, const Node & rhs)
	{
		if(lhs.score_ < rhs.score_)
			return true;
		else
			return false;
	}
};

MyTask::MyTask(const CppJieba::KeywordExtractor & app,
				const TcpConnectionPtr & conn, const std::string & search_words)
	: app_(app),
	  conn_(conn),
	  search_words_(search_words)
{}

/* 需要每次new到对象 调用连接，后期会更正 */


void MyTask::handleQuery()
{
	
    Redis * r = new Redis();
	if(!r->connect("127.0.0.1", 6379))
	{
		printf("connect error!\n");
	}

	//find in cache
	json_data_ = r->get(search_words_);
	if(json_data_.size() != 0)
	{
		std::cout << "from cache" << std::endl;
		conn_->send(json_data_);
		return;
	}
	//LOG_INFO("server prepared");

	//保留此处weight的计算方式，加上k值作为调节参数
	//时间戳和转发数的的加分项目如何设定？依据具体的score进行判定
	
	//获取相应的index
	//查询redis数据库
	//遍历查询特征词
	std::vector<std::pair<std::string, double> > res;
	std::vector<double> vec_search_weight;
	size_t topN = 5;
	app_.extract(search_words_, res, topN);                 //引用 

	if(res.size() == 0)
	{
		conn_->send("no this world try again!");            
		LOG_ERROR("no word meaningful after cut");
		return;
	}
	//相关数据全在这个map里面
//	delete r;
	if(!r->connect("127.0.0.1", 6379))
	{
		printf("connect error!\n");
	}
	r->select(2);
	for (auto & i : res)
	{
		auto set_index = r->smembers(i.first);
		map_index_.insert(std::make_pair(i.first, set_index));
	}
	std::cout << "map_index_" << map_index_.size() << std::endl;
	for (size_t idx = 0; idx < res.size(); ++idx)
	{
		vec_search_weight.push_back(res[idx].second);//vec里面存放的是权重，查询词的权重，先做保存，我需要对其进行优化，与id文档频率相关
	}
	//get Index intersection
	std::vector<uint64_t> vec_temp;//存放的是weiboID,方便进行交集运算
	for (size_t idx = 0; idx < res.size(); ++idx)
	{
		std::vector<uint64_t> vec_tmp, vec_intersection;
		auto ret = map_index_.find(res[idx].first);
		if (ret == map_index_.end())
		{
			//合理的做法是加上推荐算法
			json_data_ =  std::string(/*"{text:do not have this word}"*/);        //转换为json 可以解析的 格式
			LOG_ERROR("can't find search word");
			conn_->send(json_data_);                            
			return;
		}
		auto itr = ret->second.begin();
		for(; itr != ret->second.end(); ++itr)
		{
			vec_tmp.push_back(itr->first);
		}
		if (idx == 0)
			vec_temp = vec_tmp;
		std::set_intersection(vec_tmp.begin(), vec_tmp.end(),
							  vec_temp.begin(), vec_temp.end(),
							  std::back_inserter(vec_intersection));
		vec_temp = vec_intersection;
	}
	LOG_INFO("doc intersection get");
	//取到的交集为空
	if(vec_temp.size() == 0)                //拿到了weiboID的交集
	{
		json_data_ = std::string("your search has too many key words");//若交集为0,第二版本会加入推荐算法
		LOG_ERROR("the intersection is empty");
		conn_->send(json_data_);
		return;
	}
	std::cout << "vec_temp size :" << vec_temp.size() << std::endl;
	std::set<std::pair<uint64_t, double> >::iterator its;
	std::map<uint64_t, std::vector<double> > map_weight;
	std::vector<uint64_t>::iterator it;
	for(size_t idx = 0; idx < res.size(); ++idx)
	{
		auto ret = map_index_.find(res[idx].first);
		for(its = ret->second.begin(); its != ret->second.end(); ++its)
		{
			for(it = vec_temp.begin(); it != vec_temp.end(); ++it)
			{
				if(its->first == *it)
				{
					map_weight[its->first].push_back(its->second);
				}
			}
		}
	}

	std::map<uint64_t, std::vector<double> >::iterator itv = map_weight.begin();

	std::priority_queue<Node> que_res;
	for(; itv != map_weight.end(); ++itv)
	{
		Node node;
		node.weiboID_ = itv->first;
//		node.distance_ = cosDistance(itv->second, vec_search_words);         //计算余弦相似度
		node.score_ = getScore(itv->second, vec_search_weight);				//计算相关性得分
		que_res.push(std::move(node));
	}
	//封装json
	//从队列里面弹出
	//选取topK
	//先这样,都要进行封装
	size_t topK = 10;
	Json::Value root;
	Json::Value item;
	Json::Reader reader;
	Json::FastWriter writer;
	while(!que_res.empty() && topK > 0)
	{
//		std::cout << que_res.top().weiboID_ << " ";               //封装json
		
		//直接从redis 调出来
		r->select(1);

		item["text"] = r->hget(que_res.top().weiboID_, "text");
		item["reportscount"] = r->hget(que_res.top().weiboID_, "reportscount");

		root.append(item);
		que_res.pop();
		--topK;
	}

	json_data_ = writer.write(root);
//	std::cout << "json_data_:" << json_data_ << std::endl;

	r->select(0);
	
	r->set(search_words_, json_data_);      //设置redis缓存，最好用名称空间实现
	LOG_INFO("get json_data");
	conn_->send(json_data_);

	return;

}

}//endof namespace elton
