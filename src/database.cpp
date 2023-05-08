#include "database.hpp"

using namespace pm;

DataBase::DataBase(const std::string path) : sql{nullptr}, stmt{nullptr}, result{0}, sqlSentence{}, initializer{}, databaseLog{}
{
#pragma region log4cplus��ʼ��
	//����Appender�����ƺ������ʽ
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
	databaseLog.setLogLevel(log4cplus::ERROR_LOG_LEVEL); //Ĭ�ϴ�������־���

	databaseLog.addAppender(consoleAppender);
	databaseLog.addAppender(fileAppender);
#pragma endregion

	result = sqlite3_open_v2(path.c_str(), &sql, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_NOMUTEX | SQLITE_OPEN_SHAREDCACHE, NULL); //�����ݿ�

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

	result = sqlite3_prepare_v2(sql, sqlSentence.c_str(), -1/*�Զ�����sql��䳤��*/, &stmt, NULL); //׼�����

	if (SQLITE_OK != result)
	{
		LOG4CPLUS_ERROR(databaseLog, "Prepare statement error: " << sqlite3_errmsg(sql));
		sqlite3_finalize(stmt); //���������
		return false;
	}

	LOG4CPLUS_TRACE(databaseLog, "Prepare statement successfully");

	result = sqlite3_step(stmt); //ִ�����

	if (SQLITE_DONE != result) //�������ݱ���䣬����DONE
	{
		LOG4CPLUS_ERROR(databaseLog, "Add datatable error: " << sqlite3_errmsg(sql));
		sqlite3_finalize(stmt);
		return false;
	}

	LOG4CPLUS_TRACE(databaseLog, "Add datatable successfully");
	sqlite3_finalize(stmt); //���������
	return true;
}

bool DataBase::deleteTable(const std::string table_name)
{
	sqlSentence = "DROP TABLE " + table_name + ";";

	result = sqlite3_prepare_v2(sql, sqlSentence.c_str(), -1/*�Զ�����sql��䳤��*/, &stmt, NULL); //׼�����

	if (SQLITE_OK != result)
	{
		LOG4CPLUS_ERROR(databaseLog, "Prepare statement error: " << sqlite3_errmsg(sql));
		sqlite3_finalize(stmt); //���������
		return false;
	}
	LOG4CPLUS_TRACE(databaseLog, "Prepare statement successful");

	result = sqlite3_step(stmt); //ִ�����
	if (SQLITE_DONE != result)
	{
		LOG4CPLUS_ERROR(databaseLog, "Delete datatable error: " << sqlite3_errmsg(sql));
		sqlite3_finalize(stmt); //���������
		return false;
	}
	LOG4CPLUS_TRACE(databaseLog, "Delete datatable successfully");
	sqlite3_finalize(stmt); //���������

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
	��дǰ�Ĵ������Ǳ�ڵ� SQL ע��©������Ϊ�������������ֱ��ƴ�ӳ� SQL ��䣬���ܵ��¶��������ƻ����ݿ���߶�ȡ�������ݡ�Ϊ�˱���������������ǿ���ʹ�ò�������ѯ��Ҳ����ʹ�� ? ռλ������ֱ��ƴ�ӱ��������յ� SQL ��佫�� SQLite3 ִ��ʱͨ����ռλ�������ֵ�ķ�ʽ���ɡ�
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
		// ÿ��һ��sqlite3_step()������stmt������ͻ�ָ����һ����¼
		result = sqlite3_step(stmt);
		if (SQLITE_ROW == result) //��ѯ������
		{
			LOG4CPLUS_TRACE(databaseLog, "Query data successfully");
			//�����ز�ѯ������
			p_datas = new Datas;
			p_datas->id = sqlite3_column_int(stmt, 0);
			p_datas->user = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
			p_datas->passwd = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
			p_datas->level = sqlite3_column_int(stmt, 3);

			data.emplace_back(*p_datas);
			delete p_datas; //�����ڴ�
			p_datas = nullptr;
		}
		else if (SQLITE_DONE == result)
		{
			LOG4CPLUS_TRACE(databaseLog, "Query data is done");
			break;
		}
	}
	p_datas = nullptr; //����ָ��
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

	// ÿ��һ��sqlite3_step()������stmt������ͻ�ָ����һ����¼
	result = sqlite3_step(stmt);
	if (SQLITE_ROW == result)
	{
		LOG4CPLUS_TRACE(databaseLog, "Query data successfully");
		//�����ز�ѯ������
		data.id = sqlite3_column_int(stmt, 0);
		data.user = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
		data.passwd = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
		data.level = sqlite3_column_int(stmt, 3);

		sqlite3_finalize(stmt); //���������
		return true;
	}
	else if (SQLITE_DONE == result)
	{
		LOG4CPLUS_TRACE(databaseLog, "Query data is done");
		sqlite3_finalize(stmt); //���������
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

	result = sqlite3_prepare_v2(sql, sqlSentence.c_str(), -1/*�Զ�����sql��䳤��*/, &stmt, NULL); //׼�����

	if (SQLITE_OK != result)
	{
		LOG4CPLUS_ERROR(databaseLog, "Prepare statement error: " << sqlite3_errmsg(sql));
		sqlite3_finalize(stmt); //���������
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

	result = sqlite3_step(stmt); //ִ�����
	if (SQLITE_DONE != result)
	{
		LOG4CPLUS_ERROR(databaseLog, "Update data error: " << sqlite3_errmsg(sql));
		sqlite3_finalize(stmt); //���������
		return false;
	}
	LOG4CPLUS_TRACE(databaseLog, "Update data successfully");
	sqlite3_finalize(stmt); //���������

	return true;
}

bool DataBase::deleteData(const std::string table_name, const int id)
{
	sqlSentence = "DELETE FROM " + table_name + " WHERE ID = ?;";

	result = sqlite3_prepare_v2(sql, sqlSentence.c_str(), -1/*�Զ�����sql��䳤��*/, &stmt, NULL); //׼�����

	if (SQLITE_OK != result)
	{
		LOG4CPLUS_ERROR(databaseLog, "Prepare statement error: " << sqlite3_errmsg(sql));
		sqlite3_finalize(stmt); //���������
		return false;
	}

	if (SQLITE_OK != sqlite3_bind_int(stmt, 1, id))
	{
		LOG4CPLUS_ERROR(databaseLog, "Bind data error: " << sqlite3_errmsg(sql));
		sqlite3_finalize(stmt); //���������
		return false;
	}

	LOG4CPLUS_INFO(databaseLog, "Prepare statement successfully");

	result = sqlite3_step(stmt); //ִ�����
	if (SQLITE_DONE != result)
	{
		LOG4CPLUS_ERROR(databaseLog, "Delete data error: " << sqlite3_errmsg(sql));
		sqlite3_finalize(stmt); //���������
		return false;
	}
	LOG4CPLUS_TRACE(databaseLog, "Delete data successfully");
	sqlite3_finalize(stmt); //���������
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
		/*����ָ��*/
		sql = nullptr;
		stmt = nullptr;
	}
}