#include "database.hpp"

using namespace pm;

DataBase::DataBase(const std::string path) : sql{nullptr}, stmt{nullptr}, result{0}, sqlSentence{}, initializer{}, databaseLog{}
{
#pragma region log4cplus初始化
	//建立Appender的名称和输出格式
	log4cplus::SharedAppenderPtr consoleAppender(new log4cplus::ConsoleAppender);
	log4cplus::SharedAppenderPtr fileAppender(new log4cplus::RollingFileAppender(LOG4CPLUS_TEXT("log\\all.log"),
		10 * 1024 * 1024,
		1,
		true,
		true));

	consoleAppender->setName(LOG4CPLUS_TEXT("console"));
	consoleAppender->setLayout(std::unique_ptr<log4cplus::Layout>(new log4cplus::SimpleLayout()));

	fileAppender->setName(LOG4CPLUS_TEXT("console"));
	log4cplus::tstring pattern = LOG4CPLUS_TEXT("%D{%y/%m/%d %H:%M:%S,%Q} [%t] %-5p %c - %m [%l]%n");
	fileAppender->setLayout(std::unique_ptr<log4cplus::Layout>(new log4cplus::PatternLayout(pattern)));

	databaseLog = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("DATABASE"));
	databaseLog.setLogLevel(log4cplus::ERROR_LOG_LEVEL); //默认打开所有日志输出

	databaseLog.addAppender(consoleAppender);
	databaseLog.addAppender(fileAppender);
#pragma endregion

	result = sqlite3_open_v2(path.c_str(), &sql, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_NOMUTEX | SQLITE_OPEN_SHAREDCACHE, NULL); //打开数据库

	if (SQLITE_OK != result)
	{
		LOG4CPLUS_ERROR(databaseLog, "Opened dataBase error: " << sqlite3_errmsg(sql));
		return;
	}

	LOG4CPLUS_TRACE(databaseLog, "Opened dataBase successfully");
	return;
}

bool DataBase::createTable(const std::string table_name)
{
	sqlSentence = "CREATE TABLE " + table_name + "(" \
		"ID INT PRIMARY KEY NOT NULL," \
		"User TEXT NOT NULL," \
		"Passwd TEXT  NOT NULL," \
		"Level INT NOT NULL);";

	result = sqlite3_prepare_v2(sql, sqlSentence.c_str(), -1/*自动计算sql语句长度*/, &stmt, NULL); //准备语句

	if (SQLITE_OK != result)
	{
		LOG4CPLUS_ERROR(databaseLog, "Prepare statement error: " << sqlite3_errmsg(sql));
		sqlite3_finalize(stmt); //清理语句句柄
		return false;
	}

	LOG4CPLUS_TRACE(databaseLog, "Prepare statement successfully");

	result = sqlite3_step(stmt); //执行语句

	if (SQLITE_DONE != result) //创建数据表语句，返回DONE
	{
		LOG4CPLUS_ERROR(databaseLog, "Add datatable error: " << sqlite3_errmsg(sql));
		sqlite3_finalize(stmt);
		return false;
	}

	LOG4CPLUS_TRACE(databaseLog, "Add datatable successfully");
	sqlite3_finalize(stmt); //清理语句句柄
	return true;
}

bool DataBase::deleteTable(const std::string table_name)
{
	sqlSentence = "DROP TABLE " + table_name + ";";

	result = sqlite3_prepare_v2(sql, sqlSentence.c_str(), -1/*自动计算sql语句长度*/, &stmt, NULL); //准备语句

	if (SQLITE_OK != result)
	{
		LOG4CPLUS_ERROR(databaseLog, "Prepare statement error: " << sqlite3_errmsg(sql));
		sqlite3_finalize(stmt); //清理语句句柄
		return false;
	}
	LOG4CPLUS_TRACE(databaseLog, "Prepare statement successful");

	result = sqlite3_step(stmt); //执行语句
	if (SQLITE_DONE != result)
	{
		LOG4CPLUS_ERROR(databaseLog, "Delete datatable error: " << sqlite3_errmsg(sql));
		sqlite3_finalize(stmt); //清理语句句柄
		return false;
	}
	LOG4CPLUS_TRACE(databaseLog, "Delete datatable successfully");
	sqlite3_finalize(stmt); //清理语句句柄

	return true;
}

bool DataBase::insertData(const std::string table_name, const Datas& data)
{
	sqlSentence = "INSERT INTO " + table_name + " (ID, User, Passwd, Level) " \
		"VALUES (?, ?, ?, ?)";

	result = sqlite3_prepare_v2(sql, sqlSentence.c_str(), -1, &stmt, NULL);
	if (SQLITE_OK != result)
	{
		LOG4CPLUS_ERROR(databaseLog, "Prepare statement error: " << sqlite3_errmsg(sql));
		return false;
	}
	LOG4CPLUS_TRACE(databaseLog, "Prepare statement successfully");
	
	/*
	改写前的代码存在潜在的 SQL 注入漏洞，因为它将插入的数据直接拼接成 SQL 语句，可能导致恶意输入破坏数据库或者读取敏感数据。为了避免这种情况，我们可以使用参数化查询，也就是使用 ? 占位符代替直接拼接变量。最终的 SQL 语句将由 SQLite3 执行时通过绑定占位符与变量值的方式生成。
	*/
	
	if (SQLITE_OK != sqlite3_bind_int(stmt, 1, data.id))
	{
		LOG4CPLUS_ERROR(databaseLog, "Bind data error: " << sqlite3_errmsg(sql));
		sqlite3_finalize(stmt);
		return false;
	}
	if (SQLITE_OK != sqlite3_bind_text(stmt, 2, data.user.c_str(), -1, SQLITE_STATIC))
	{
		LOG4CPLUS_ERROR(databaseLog, "Bind data error: " << sqlite3_errmsg(sql));
		sqlite3_finalize(stmt);
		return false;
	}
	if (SQLITE_OK != sqlite3_bind_text(stmt, 3, data.passwd.c_str(), -1, SQLITE_STATIC))
	{
		LOG4CPLUS_ERROR(databaseLog, "Bind data error: " << sqlite3_errmsg(sql));
		sqlite3_finalize(stmt);
		return false;
	}
	if (SQLITE_OK != sqlite3_bind_int(stmt, 4, data.level))
	{
		LOG4CPLUS_ERROR(databaseLog, "Bind data error: " << sqlite3_errmsg(sql));
		sqlite3_finalize(stmt);
		return false;
	}

	result = sqlite3_step(stmt);
	if (SQLITE_DONE != result)
	{
		LOG4CPLUS_ERROR(databaseLog, "Insert data error: " << sqlite3_errmsg(sql));
		sqlite3_finalize(stmt);
		return false;
	}
	LOG4CPLUS_TRACE(databaseLog, "Insert data successfully");
	sqlite3_finalize(stmt);
	return true;
}

bool DataBase::queryData(const std::string table_name, MultiData& data)
{
	sqlSentence = "SELECT * FROM " + table_name;

	result = sqlite3_prepare_v2(sql, sqlSentence.c_str(), -1, &stmt, NULL);

	if (SQLITE_OK != result)
	{
		LOG4CPLUS_ERROR(databaseLog, "Prepare statement error: " << sqlite3_errmsg(sql));
		sqlite3_finalize(stmt);
		return false;
	}

	LOG4CPLUS_TRACE(databaseLog, "Prepare statement successfully");

	
	Datas* p_datas = nullptr;
	

	while (true)
	{
		// 每调一次sqlite3_step()函数，stmt语句句柄就会指向下一条记录
		result = sqlite3_step(stmt);
		if (SQLITE_ROW == result) //查询到数据
		{
			LOG4CPLUS_TRACE(databaseLog, "Query data successfully");
			//输出相关查询的数据
			p_datas = new Datas;
			p_datas->id = sqlite3_column_int(stmt, 0);
			p_datas->user = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
			p_datas->passwd = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
			p_datas->level = sqlite3_column_int(stmt, 3);

			data.emplace_back(*p_datas);
			delete p_datas; //清理内存
			p_datas = nullptr;
		}
		else if (SQLITE_DONE == result)
		{
			LOG4CPLUS_TRACE(databaseLog, "Query data is done");
			break;
		}
	}
	p_datas = nullptr; //清理指针
	sqlite3_finalize(stmt);
	return true;
}

bool DataBase::queryData(const std::string table_name, const int id, Datas& data)
{
	sqlSentence = "SELECT * FROM " + table_name + " WHERE ID = ?;";

	result = sqlite3_prepare_v2(sql, sqlSentence.c_str(), -1, &stmt, NULL);

	if (SQLITE_OK != result)
	{
		LOG4CPLUS_ERROR(databaseLog, "Prepare statement error: " << sqlite3_errmsg(sql));
		sqlite3_finalize(stmt);
		return false;
	}

	LOG4CPLUS_TRACE(databaseLog, "Prepare statement successfully");

	if (SQLITE_OK != sqlite3_bind_int(stmt, 1, id))
	{
		LOG4CPLUS_ERROR(databaseLog, "Bind data error: " << sqlite3_errmsg(sql));
		sqlite3_finalize(stmt);
		return false;
	}

	// 每调一次sqlite3_step()函数，stmt语句句柄就会指向下一条记录
	result = sqlite3_step(stmt);
	if (SQLITE_ROW == result)
	{
		LOG4CPLUS_TRACE(databaseLog, "Query data successfully");
		//输出相关查询的数据
		data.id = sqlite3_column_int(stmt, 0);
		data.user = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
		data.passwd = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
		data.level = sqlite3_column_int(stmt, 3);

		sqlite3_finalize(stmt); //清理语句句柄
		return true;
	}
	else if (SQLITE_DONE == result)
	{
		LOG4CPLUS_TRACE(databaseLog, "Query data is done");
		sqlite3_finalize(stmt); //清理语句句柄
		return false;
	}
	else
	{
		LOG4CPLUS_ERROR(databaseLog, "Query data error: " << sqlite3_errmsg(sql));
		sqlite3_finalize(stmt);
		return false;
	}
}

bool DataBase::updateData(const std::string table_name, const Datas& data)
{
	sqlSentence = "UPDATE " + table_name + " SET " \
		"User = ?, " \
		"Passwd = ?, " \
		"Level = ? " \
		"WHERE ID = ?;";

	result = sqlite3_prepare_v2(sql, sqlSentence.c_str(), -1/*自动计算sql语句长度*/, &stmt, NULL); //准备语句

	if (SQLITE_OK != result)
	{
		LOG4CPLUS_ERROR(databaseLog, "Prepare statement error: " << sqlite3_errmsg(sql));
		sqlite3_finalize(stmt); //清理语句句柄
		return false;
	}
	LOG4CPLUS_TRACE(databaseLog, "Prepare statement successfully");
	if (SQLITE_OK != sqlite3_bind_text(stmt, 1, data.user.c_str(), -1, SQLITE_STATIC))
	{
		LOG4CPLUS_ERROR(databaseLog, "Bind data error: " << sqlite3_errmsg(sql));
		sqlite3_finalize(stmt);
		return false;
	}
	if (SQLITE_OK != sqlite3_bind_text(stmt, 2, data.passwd.c_str(), -1, SQLITE_STATIC))
	{
		LOG4CPLUS_ERROR(databaseLog, "Bind data error: " << sqlite3_errmsg(sql));
		sqlite3_finalize(stmt);
		return false;
	}
	if (SQLITE_OK != sqlite3_bind_int(stmt, 3, data.level))
	{
		LOG4CPLUS_ERROR(databaseLog, "Bind data error: " << sqlite3_errmsg(sql));
		sqlite3_finalize(stmt);
		return false;
	}
	if (SQLITE_OK != sqlite3_bind_int(stmt, 4, data.id))
	{
		LOG4CPLUS_ERROR(databaseLog, "Bind data error: " << sqlite3_errmsg(sql));
		sqlite3_finalize(stmt);
		return false;
	}

	result = sqlite3_step(stmt); //执行语句
	if (SQLITE_DONE != result)
	{
		LOG4CPLUS_ERROR(databaseLog, "Update data error: " << sqlite3_errmsg(sql));
		sqlite3_finalize(stmt); //清理语句句柄
		return false;
	}
	LOG4CPLUS_TRACE(databaseLog, "Update data successfully");
	sqlite3_finalize(stmt); //清理语句句柄

	return true;
}

bool DataBase::deleteData(const std::string table_name, const int id)
{
	sqlSentence = "DELETE FROM " + table_name + " WHERE ID = ?;";

	result = sqlite3_prepare_v2(sql, sqlSentence.c_str(), -1/*自动计算sql语句长度*/, &stmt, NULL); //准备语句

	if (SQLITE_OK != result)
	{
		LOG4CPLUS_ERROR(databaseLog, "Prepare statement error: " << sqlite3_errmsg(sql));
		sqlite3_finalize(stmt); //清理语句句柄
		return false;
	}

	if (SQLITE_OK != sqlite3_bind_int(stmt, 1, id))
	{
		LOG4CPLUS_ERROR(databaseLog, "Bind data error: " << sqlite3_errmsg(sql));
		sqlite3_finalize(stmt); //清理语句句柄
		return false;
	}

	LOG4CPLUS_INFO(databaseLog, "Prepare statement successfully");

	result = sqlite3_step(stmt); //执行语句
	if (SQLITE_DONE != result)
	{
		LOG4CPLUS_ERROR(databaseLog, "Delete data error: " << sqlite3_errmsg(sql));
		sqlite3_finalize(stmt); //清理语句句柄
		return false;
	}
	LOG4CPLUS_TRACE(databaseLog, "Delete data successfully");
	sqlite3_finalize(stmt); //清理语句句柄
	return true;

}

DataBase::~DataBase()
{
	if (sql)
	{
		if (SQLITE_OK == sqlite3_close_v2(sql))
		{
			LOG4CPLUS_TRACE(databaseLog, "Closed datebase successfully");
		}
		else
		{
			LOG4CPLUS_ERROR(databaseLog, "Closed datebase error: " << sqlite3_errmsg(sql));
		}
		/*清理指针*/
		sql = nullptr;
		stmt = nullptr;
	}
}