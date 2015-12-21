#ifndef __MYCONF_H__
#define __MYCONF_H__
#include <assert.h>
#include <string>
#include <sstream>
#include <map>
#include <utility>//std::move
#include <fstream>



namespace elton
{
class MyConf
{
public:
	void loadConf(std::string & path)
	{
		std::ifstream ifs(path.c_str());
		if(!ifs.is_open())
		{
			std::cout << "conf file open error" << std::endl;
			exit (EXIT_FAILURE);
		}
		std::string line;
		while(getline(ifs, line))
		{
		    std::string key,value;
			std::istringstream is(line);
			is >> key;
			is >> value;
     		map_conf_.insert(std::make_pair(key, value));
		}
	}
	
	std::map<std::string, std::string> & getMapOfConf()
	{
		return map_conf_;
	}

	

private:
	std::map<std::string, std::string> map_conf_;

};





}







#endif
