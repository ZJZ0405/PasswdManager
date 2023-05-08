#include <iostream>
#include <string>
#include <conio.h>
#include "database.hpp"
#include "passwd.hpp"

#define TEST_USER_NAME "zjz"
#define TEST_USER_PASSWD "12345"

int main(int agrc, char** agrv)
{
	pm::DataBase test("./t.db");

	std::cout << "密码管理器-debug-0.0.1\n" \
		"============================\n" \
		"用户名：";
	std::string user_name = {};
	std::string user_passwd = {};

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
			std::cout << "密码正确\n";
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
		///*std::cout << "passwd is " << out << ", AES is " << q << " and the yuanwen is " << w << std::endl;*/
		
	}
	return 0;
}

inline void home_page(void)
{


}