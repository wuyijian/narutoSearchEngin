#include "mysplit.h"
#include "/home/wuyijian/simhash/src/Simhasher.hpp"
#include "/home/wuyijian/simhash/src/CppJieba/QuerySegment.hpp"
#include <fstream>
#include <string>
#include <sstream>
#include <assert.h>
#include <algorithm>
#include <unordered_set>
#include <math.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>
//#include <MyLog.h>
#include "redis.h"


using namespace Simhash;


namespace elton
{



void getFileName(const char * dir, std::vector<std::string> & vec_path)
{
	DIR * dp = opendir(dir);
	if (NULL == dp)
	{
//		LOG_FATAL("open dir error");
		return;
	}
	chdir(dir);
	struct dirent * entry;
	struct stat statbuf;

	while((entry = readdir(dp)) != NULL)
	{
		stat(entry->d_name, &statbuf);
		if (S_ISDIR(statbuf.st_mode))
		{
			if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0)
			{
				continue;
			}
			getFileName(entry->d_name, vec_path);
		}else{
			char buf[512], path[512];
			bzero(buf, sizeof buf);
			getcwd(buf, sizeof buf);
			sprintf(path, "%s/%s", buf, entry->d_name);
			vec_path.push_back(std::string(path));
		}
	}
	chdir("..");
	closedir(dp);
}

void MySplit::getVecForPage()//加载微博库文件
{
	std::string path = "sourse";
	std::vector<std::string> vec_path;
	getFileName(path.c_str(), vec_path);

	std::string text;
	
	for (size_t idx = 0; idx < vec_path.size(); ++idx)
	{
		std::ifstream ifs((vec_path[idx]).c_str());
		if (!ifs.good())
		{
//			LOG_FATAL("sourse open error");
			return ;
		}
		std::string line;
		
		while(getline(ifs, line))
		{
			MyWeibo weibo;
			std::string::size_type pos_h = 0;
			std::string::size_type pos_t = 0;
			while(1)
			{
				pos_t = line.find("||||", pos_h);
				weibo.weiboID_ = ::atoll(line.substr(0, pos_t).c_str());

	//			std::cout << "weiboID" << weibo.weiboID_ << std::endl;
				pos_h = pos_t + 4;
				pos_t = line.find("||||", pos_h);
				weibo.timestamp_ = ::atol(line.substr(pos_h, pos_t - pos_h).c_str());

				pos_h = pos_t + 4 + 10 + 4; //忽略掉用户id 
				pos_t = line.find("||||", pos_h);
				weibo.username_ = line.substr(pos_h, pos_t - pos_h);
	//			std::cout << "username" << weibo.username_ << std::endl;

				pos_h = pos_t + 4;
				pos_t = line.find("||||", pos_h);
				weibo.reportscount_ = ::atol(line.substr(pos_h, pos_t - pos_h).c_str());
	//			std::cout << "reportscount" << weibo.reportscount_ << std::endl;
				
				pos_h = line.find_last_of('|') + 1;
				weibo.text_ = line.substr(pos_h, line.size() - pos_h);
	//			std::cout << "text" << weibo.text_ << std::endl;
				
				break; 

			}

			vec_old_page_.push_back(std::move(weibo));
		}
	}
}


void MySplit::getVectorForSimhash()
{
	Simhasher simhasher("/home/wuyijian/simhash/dict/jieba.dict.utf8",
						"/home/wuyijian/simhash/dict/hmm_model.utf8",
	         			"/home/wuyijian/simhash/dict/idf.utf8",
						"/home/wuyijian/simhash/dict/stop_words.utf8");

	std::vector<std::pair<std::string, double> > res;
	size_t docID = 0, topN = 5; //微博文档内容相对比较短
	uint64_t u64;
	for (size_t idx = 0; idx < vec_old_page_.size(); ++idx)
	{
     	simhasher.extract(vec_old_page_[idx].text_, res, topN);
		simhasher.make(vec_old_page_[idx].text_, topN, u64);
		size_t i;
		for (i = 0; i < vec_simhash_.size(); ++ i)
		{	
			if (Simhasher::isEqual(vec_simhash_[i], u64) == 1)
				break;
		}
		if (i == vec_simhash_.size())
		{
			vec_simhash_.push_back(u64);   //指纹库
			vec_libpage_.push_back(vec_old_page_[idx]);//微博库，无重复
		}
	}
	std::cout << vec_libpage_.size() << std::endl;
	std::cout << vec_simhash_.size() << std::endl;
}

#if 1

void MySplit::writeWeiboToDB()                             //write fo redis
{
	//构造一个redis对象
	//建立连接
	//循环插入redis SELECT 1   0 为cache ,数据结构为hash   
	Redis * r = new Redis();
	if(!r->connect("127.0.0.1", 6379))
	{
		printf("connect error\n");
		exit(-1);
	}
	r->select(1);
	for(auto weibo : vec_libpage_)
	{
		r->hmset(weibo.weiboID_, "timestamp", weibo.timestamp_, "username", weibo.username_, "reportscount", weibo.reportscount_, "text", weibo.text_);
	}

}
#endif

//get index

void MySplit::getVecForTfmap()
{
	//获取每篇网页的词频
	//遍历微博库
	//调用jieba分词库
	CppJieba::QuerySegment app("/home/wuyijian/simhash/dict/jieba.dict.utf8",
							   "/home/wuyijian/simhash/dict/hmm_model.utf8",
							   "/home/wuyijian/simhash/dict/user_dict.utf8");
	
	std::ifstream ifs_stopwords("/home/wuyijian/simhash/dict/stop_words.utf8");
	if(!ifs_stopwords.is_open())
	{
//		LOG_FATAL("stop_words open filed");
		return;
	}
	std::unordered_set<std::string> set_stop_words;           //set有提供find方法 vector 没有vector方法 
	std::vector<std::string> vec_tmp_words;
	std::string stop_words, tmp_words;
	//获取停用词vector
	while(ifs_stopwords >> stop_words)
	{
		set_stop_words.insert(stop_words);
	}
	for (size_t idx = 0; idx < vec_libpage_.size(); ++idx)
	{
		app.cut(vec_libpage_[idx].text_, vec_tmp_words);
		std::unordered_map<std::string, int> tfmap;
		//获得map
		for (auto & tmp : vec_tmp_words)
		{
			auto res = tfmap.insert({tmp, 1});
			if (!res.second)
				++res.first->second;
		}
		//去停用词
		for(std::unordered_map<std::string, int>::iterator itr = tfmap.begin(); itr != tfmap.end();)
		{
			if(set_stop_words.end() != set_stop_words.find(itr->first))
			{
				tfmap.erase(itr++);
				continue;
			}
			itr ++;
		}
		//将tfmap 插入vec
		vec_tfmap_.push_back(std::move(tfmap));
	}
	ifs_stopwords.close();
//	LOG_INFO("word frequency map init");
	
}


void MySplit::getMapForIndex()
{
	std::unordered_map<std::string, std::set<std::pair<uint64_t, int> > > map_tf;
	//遍历vector的每一个map
	for(size_t idx = 0; idx < vec_tfmap_.size(); ++idx)
	{
		//遍历map中的单词
		auto map_fre = vec_tfmap_[idx];
		std::unordered_map<std::string, int>::iterator itr = map_fre.begin();
		for(; itr != map_fre.end(); ++itr)
		{
			auto res = map_tf.insert(std::make_pair(itr->first, std::set<std::pair<uint64_t, int> >{std::make_pair(vec_libpage_[idx].weiboID_, itr->second)}));
			if (!res.second)
				res.first->second.insert(make_pair(vec_libpage_[idx].weiboID_, itr->second));
		}
	}

	std::cout << "map_tf ready" << std::endl;
	
	//将map进一步转换为map_index;
	//遍历每个string

	size_t N = vec_libpage_.size();
	std::unordered_map<std::string, std::set<std::pair<uint64_t, int> > >::iterator itr = map_tf.begin();

	for(; itr != map_tf.end(); ++itr)
	{	
		double df = itr->second.size();      //set  的大小
		double idf = log((double)N/df) + 0.0001;//避免idf 为零的情况
		double w1 = 0;
	
		for (std::set<std::pair<uint64_t, int> >::iterator itr_set = itr->second.begin(); itr_set != itr->second.end(); ++itr_set)
		{
			w1 += idf * (itr_set->second) * idf * (itr_set->second);
		}
		double w2 = sqrt(w1);
		for (std::set<std::pair<uint64_t, int> >::iterator itr_set = itr->second.begin(); itr_set != itr->second.end(); ++itr_set)
		{
			map_weight_[itr->first].insert(std::make_pair(itr_set->first, (double(itr_set->second))/w2));	
		}

	}
	std::cout << "map_weight_ ready " << std::endl;
}


#if 1
					
void MySplit::writeIndexToDB() //write to redis
{
	Redis * r = new Redis();
	if(!r->connect("127.0.0.1", 6379))
	{
		printf("connect error");
		exit(-1);
	}
	r->select(2);
	for(auto index : map_weight_)
	{
		for(auto weight : index.second)
		{
			r->sadd(index.first, weight.first, weight.second);
		}
	}		
	
}
#endif


}//namespace elton?
