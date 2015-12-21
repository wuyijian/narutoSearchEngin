#ifndef _REDIS_H_
#define _REDIS_H_

#include <iostream>
#include <string>
#include <string.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <sstream>

#include <hiredis/hiredis.h>



class Redis
{
public:
	Redis()
	{}
	~Redis()
	{
		this->connect_ = NULL;
		this->reply_ = NULL;
	}

	bool connect(std::string host, int port)
	{
		this->connect_ = redisConnect(host.c_str(), port);
		if(this->connect_ != NULL && this->connect_->err)
		{
			printf("connect error: %s\n", this->connect_->errstr);
			return 0;
		}
		return 1;
	}
	/*get*/
	std::string get(std::string key)
	{
		this->reply_ = (redisReply * )redisCommand(this->connect_, "GET %s", key.c_str());
		if(reply_->str == 0)
			return std::string();
		std::string str = this->reply_->str;
		/*reply object has to be cleared*/
		freeReplyObject(this->reply_);              
		return str;
	}
	/*hmset -- weibo lib*/
	void hmset(uint64_t key, std::string field1, uint64_t timestamp, std::string field2, std::string username,
				std::string field3, uint64_t reportscount, std::string field4, std::string text)
	{
		redisCommand(this->connect_, "HMSET %lld %s %lld %s %s %s %lld %s %s", 
													key, field1.c_str(), timestamp, field2.c_str(), username.c_str(), field3.c_str(), reportscount, 
													field4.c_str(), text.c_str());
	}
	/*hmset -- index*/
	void hmset_index(std::string key, std::string field1, uint64_t weiboID, std::string field2, double weight)
	{
		redisCommand(this->connect_, "HMSET %s %s %lld %s %lf", key.c_str(), field1.c_str(), weiboID, field2.c_str(), weight);
	}

	void select(unsigned short num)
	{
		redisCommand(this->connect_, "SELECT %u", num);
	}

	void set(std::string key, std::string value)
	{
		redisCommand(this->connect_, "SET %s %s", key.c_str(), value.c_str());
	}
	/*string list and value is number*/
	void append(size_t value)
	{
		redisCommand(this->connect_, "APPEND %d", value);
	}

	void rpush(std::string key, size_t value)
	{
		redisCommand(this->connect_, "RPUSH %s %d", key.c_str(), value);
	}
#if 1
	/*	这个接口尚未有好办法解决 */	
   std::vector<std::string> lrange(std::string key, int start, int stop)
   {
	   this->reply_ = (redisReply * )redisCommand(this->connect_, "LRANGE %s %d %d", key.c_str(), start, stop);
	   std::vector<std::string> vec;
	   for(size_t idx  = 0; idx < this->reply_->elements; ++idx)
	   {
		    	   
			redisReply * childReply = this->reply_->element[idx];
			vec.push_back(childReply->str);
	   }
	 //  std::cout << "size:" << reply_->elements << std::endl;
	   freeReplyObject(this->reply_);
	   return vec;
   }
#endif	
   /*sadd*/
   void sadd(std::string key, uint64_t weiboID, double weight)
   {
		std::ostringstream os;
		os << weiboID << " " << weight;
		redisCommand(this->connect_, "SADD %s %s", key.c_str(), os.str().c_str());
   }
#if 0
   bool isExists(std::string key)
   {
	   this->reply_ = (redisReply * )redisCommand(this->connect_, "EXISTS %s", key.c_str());
	   bool bo;
	   bo = bool(this->reply_->str);
	   freeReplyObject(this->reply_);
	   return bo; 
   }
#endif

private:
	redisContext * connect_;
	redisReply * reply_;
};



#endif

