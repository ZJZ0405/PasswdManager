#pragma once

#include <string>
#include <random>
#include <ctime>
#include <log4cplus/log4cplus.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <openssl/conf.h>

/*指定长度*/
constexpr unsigned int KEY_SIZE = 32;
constexpr unsigned int BLOCK_SIZE = 32;

namespace pm
{
	typedef unsigned char byte;
	using EVP_CIPHER_CTX_free_ptr = std::unique_ptr<EVP_CIPHER_CTX, decltype(&::EVP_CIPHER_CTX_free)>;
	std::string producePasswd(size_t length, unsigned int seed);
	//std::string producePasswd(size_t length);

	std::string getMd5(std::string& input);
	std::string getSHA1(std::string& input);
	void getGen(byte key[KEY_SIZE], byte iv[BLOCK_SIZE]); //生成key和iv
	std::string Encrypt(const std::string ptext, const byte key[KEY_SIZE], const byte iv[BLOCK_SIZE], const EVP_CIPHER* mode); //加密

	std::string Decrypt(const std::string ptext, const byte key[KEY_SIZE], const byte iv[BLOCK_SIZE], const EVP_CIPHER* mode); //解密
	std::string doAES(std::string input,int enc);
}