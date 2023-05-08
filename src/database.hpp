#pragma once

#include <string>
#include <vector>
#include <sqlite3.h>
#include <log4cplus/log4cplus.h>


namespace pm
{
	class DataBase;

	struct Datas //���ݿ�����
	{
		int id;
		std::string user;
		std::string passwd;
		int level;
	};

	/*
	����using��typedef
	https://stackoverflow.com/a/10748056/21311496
	*/
	using MultiData = std::vector<Datas>;  //���ݱ���
	using SingleData = std::vector<std::string>; //����������
}

/*
* @brief sqlite3���ݿ��װ
*
*/
class pm::DataBase
{
public:
	//explicit DataBase(); //Ĭ�Ϲ��캯��
	explicit DataBase(const std::string path/*���ݿ�·��*/);

	/*���ݱ����*/
	bool createTable(const std::string table_name/*���ݱ�����*/); //�������ݱ�

	bool deleteTable(const std::string table_name/*���ݱ�����*/); //ɾ�����ݱ�

	bool insertData(const std::string table_name, const Datas& data); //��������

	//��ѯģʽ
	enum QueryMethods
	{
		ALL,//ȫ��
		USER,//�û���
		PASSWD,//����
		LEVEL//����ǿ��
	};
	bool queryData(const std::string table_name, MultiData& data); //��ȡ���ݱ�

	bool queryData(const std::string table_name, const int id, Datas& data);

	bool updateData(const std::string table_name, const Datas& data);

	//bool deleteData(const std::string tablename, const std::string id);
	bool deleteData(const std::string tablename, const int id);

	~DataBase();
private:
	sqlite3* sql; //���ݿ�ʵ��
	sqlite3_stmt* stmt; //stmt�����
	int result; // ����ֵ
	std::string sqlSentence; //���ݿ����

	//��־
	log4cplus::Initializer initializer; //��ʼ��
	log4cplus::Logger databaseLog; //����һ��Loggerʵ��
};