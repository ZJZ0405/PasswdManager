#pragma once

#include <string>
#include <vector>
#include <sqlite3.h>
#include <log4cplus/log4cplus.h>


namespace pm
{
	class DataBase;

	struct Datas //数据库数据
	{
		int id;
		std::string user;
		std::string passwd;
		int level;
	};

	/*
	关于using与typedef
	https://stackoverflow.com/a/10748056/21311496
	*/
	using MultiData = std::vector<Datas>;  //数据表组
	using SingleData = std::vector<std::string>; //单个数据组
}

/*
* @brief sqlite3数据库封装
*
*/
class pm::DataBase
{
public:
	//explicit DataBase(); //默认构造函数
	explicit DataBase(const std::string path/*数据库路径*/);

	/*数据表操作*/
	bool createTable(const std::string table_name/*数据表名字*/); //创建数据表

	bool deleteTable(const std::string table_name/*数据表名字*/); //删除数据表

	bool insertData(const std::string table_name, const Datas& data); //插入数据

	//查询模式
	enum QueryMethods
	{
		ALL,//全部
		USER,//用户名
		PASSWD,//密码
		LEVEL//密码强度
	};
	bool queryData(const std::string table_name, MultiData& data); //读取数据表

	bool queryData(const std::string table_name, const int id, Datas& data);

	bool updateData(const std::string table_name, const Datas& data);

	//bool deleteData(const std::string tablename, const std::string id);
	bool deleteData(const std::string tablename, const int id);

	~DataBase();
private:
	sqlite3* sql; //数据库实例
	sqlite3_stmt* stmt; //stmt语句句柄
	int result; // 返回值
	std::string sqlSentence; //数据库语句

	//日志
	log4cplus::Initializer initializer; //初始化
	log4cplus::Logger databaseLog; //创建一个Logger实例
};