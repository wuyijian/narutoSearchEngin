#ifndef MYSPLIT_H
#define MYSPLIT_H

#include <vector>
#include <map>
#include <set>
#include <string>
#include <unordered_map>

/*用一个struct来存储微博信息*/


struct MyWeibo
{
	uint64_t       weiboID_;
	uint64_t       timestamp_;
	std::string    username_;
	uint64_t       reportscount_;
	std::string    text_;
};

namespace elton
{

class MySplit
{
public:
	//得到新的每个单词的词

	void getVecForPage();
	void getVectorForSimhash();
	void delSimilarPage();
	void writeWeiboToDB();
	void getVecForTfmap();
	void getMapForIndex();
	void writeIndexToDB();
private:
	std::vector<MyWeibo>			     vec_old_page_;//去重前
	std::vector<MyWeibo>                 vec_libpage_;//去重后
	std::vector<uint64_t>                vec_simhash_;
	typedef std::vector<std::unordered_map<std::string, int> > VecTfMap;
	VecTfMap                             vec_tfmap_;
	typedef std::unordered_map<std::string, std::set<std::pair<uint64_t, double> > > MapWeight;
	MapWeight                            map_weight_;

};





}//end of namespace elton
#endif 
