#include <iostream>
#include <string>
#include <conio.h>
#include "database.hpp"
#include "passwd.hpp"

#define TEST_USER_NAME "admin"
#define TEST_USER_PASSWD "admin"

int main(int agrc, char** agrv)
{
	std::cout << "密码管理器-debug-0.0.1\n" \
		"============================\n" \
		"用户名：";
	std::string user_name = {};
	std::string user_passwd = {};
	pm::DataBase database("./data.db");
	database.createTable("up");

	std::cin >> user_name;

	if (TEST_USER_NAME == user_name)
	{
		std::cout << "密码：";
		int ch_buffer;
		while (true)
		{
			ch_buffer = _getch();
			if (13 != ch_buffer)
			{
				if (ch_buffer == 8)
				{
					if (0 != user_passwd.size())
					{
						printf("\b \b");
						user_passwd.pop_back();
					}
				}
				else
				{
					printf("*");
					user_passwd.append(1, char(ch_buffer));
				}
			}
			else
			{
				std::cout << std::endl;
				break;
			}
		}

		if (TEST_USER_PASSWD == user_passwd)
		{
			pm::MultiData _multidata = {};
			database.queryData("up", _multidata);
			std::cout << "密码正确\n";
			bool out = false;
			while (!out)
			{
				std::cout << "\033c";
				std::cout << "密码管理器-1.0.1\n" \
					"============================\n" \
					"1.添加密码\n" \
					"2.删除密码\n" \
					"3.修改密码\n" \
					"4.查看密码\n" \
					"5.查询密码\n" \
					"6.退出\n" \
					"============================\n" \
					"请输入选项：";
				int option = 0;
				std::cin >> option;
				switch (option)
				{
				case 1:
					{
						std::cout << "添加密码\n-------------------------------\n";
						std::string name = {};
						std::string passwd = {};
						std::string options = {};
						std::cout << "请输入用户名：";
						std::cin >> name;
						std::cout << "是否使用自动密码生成器？[yes]/no:";
						std::cin >> options; getchar();
						if ("yes" == options)
						{
							std::string auto_passwd = pm::producePasswd(30, (unsigned int)time(0));
							std::cout << "生成密码：" << auto_passwd << std::endl;
							pm::Datas _datas;
							if (_multidata.empty())
								_datas.id = 0;
							else
								_datas.id = _multidata.end()[-1].id + 1;
							_datas.user = name;
							_datas.passwd = auto_passwd;
							database.insertData("up", _datas);
						}
						else
						{
							std::cout << "请输入密码：";
							std::cin >> passwd;
							pm::Datas _datas;
							if (_multidata.empty())
								_datas.id = 0;
							else
								_datas.id = _multidata.end()[-1].id + 1;
							_datas.user = name;
							_datas.passwd = passwd;
							database.insertData("up", _datas);
						}
						std::cout << "添加成功\n";
						std::cout << "按任意键继续"; getchar();
					}
					break;
				case 2:
					{
						std::cout << "删除密码\n";
						std::cout << "请输入需要删除的id：";
						int _id = 0;
						std::cin >> _id;
						database.deleteData("up", _id);
						std::cout << "删除成功\n";
						std::cout << "按任意键继续"; getchar();
					}
				break;
				case 3:
					{
						std::cout << "修改密码\n";
						std::cout << "请输入需要修改的id：";
						int id = 0;
						std::cin >> id;
						std::cout << "请输入新密码：";
						std::string new_passwd = {};
						std::cin >> new_passwd;
						pm::Datas _datas;
						database.queryData("up", id, _datas);
						_datas.id = id;
						_datas.passwd = new_passwd;
						database.updateData("up", _datas);
					}
					std::cout << "修改成功\n";
					std::cout << "按任意键继续"; getchar();
					break;
				case 4:
					{
						std::cout << "查看密码\n";
						_multidata.clear();
						database.queryData("up", _multidata);
						if (_multidata.empty())
						{
							std::cout << "无密码数据" << std::endl;
						}
						else
						{
							for (pm::Datas x : _multidata)
							{
								std::cout << "id:" << x.id << std::endl;
								std::cout << "用户名：" << x.user << std::endl;
								std::cout << "密码：" << x.passwd << std::endl << std::endl;
							}
						}
						getchar();
						std::cout << "按任意键继续"; getchar();
					}
					break;
				case 5:
					{
					std::cout << "查询密码\n";
					std::cout << "请输入用户名：";
					std::string name = {};
					std::cin >> name;
					_multidata.clear();
					database.queryData("up", _multidata);
					bool flag = false;
					for (pm::Datas x : _multidata)
					{
						if (name == x.user)
						{
							flag = true;
							std::cout << "查询成功" << std::endl;
							std::cout << "id:" << x.id << std::endl;
							std::cout << "用户名：" << x.user << std::endl;
							std::cout << "密码：" << x.passwd << std::endl << std::endl;
						}
					}
					if (!flag)
					{
						std::cout << "未查询到此密码\n";
					}
					getchar();
					std::cout << "按任意键继续"; getchar();
					}
					break;
				case 6:
					std::cout << "退出\n";
					out = true;
					break;
				default:
					std::cout << "选项错误\n";
					break;
				}
			}
			return 0;
		}
		else
		{
			std::cout << "密码错误\n";
		}
	}
	else
	{
		std::cout << "用户名错误\n";
		//pm::DataBase test("./test.db");

		//std::string out = pm::producePasswd(30, (unsigned int)time(0));
		////std::string out = "test";
		//pm::byte key[KEY_SIZE] = {}, iv[BLOCK_SIZE] = {};
		//pm::getGen(key, iv);
		//std::string q = pm::Encrypt(out, key, iv, EVP_aes_256_cbc());
		//test.createTable("test");
		//pm::Datas t = { 1, "test", q, 1};
		//pm::Datas a = { 2, "test", "123", 1};
		//pm::Datas b = { 3, "test", "86786", 1};
		//test.insertData("test", t);
		//test.insertData("test", a);
		//test.insertData("test", b);

		//pm::MultiData p = {};
		//test.queryData("test", p);

		//for (pm::Datas x : p)
		//{
		//	std::cout << x.user << std::endl;
		//}
		//pm::Datas z = { 2, "test", q, 1 };
		//test.deleteData("test", 1);
		//test.updateData("test", z);

		///*std::string w = pm::Decrypt(p.passwd, key, iv, EVP_aes_256_cbc());*/

		//test.deleteTable("test");
		//OPENSSL_cleanse(key, KEY_SIZE);
		//OPENSSL_cleanse(iv, BLOCK_SIZE);
		////std::string out = "23456";
		///*std::cout << "passwd is " << out << ", AES is " << q << " and theyuanwen is " << w << std::endl;*/
		
	}
	return 0;
}
