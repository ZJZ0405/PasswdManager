#include "passwd.hpp"
#include <iostream>

std::string pm::producePasswd(size_t length, unsigned int seed)
{
	/*根据acsii码表33-126*/
	std::default_random_engine random_engine; //随机数引擎
	std::uniform_int_distribution<unsigned int> random_uniform(33, 126);
	random_engine.seed((unsigned int)seed);
	std::string passwd;
	while (true)
	{
		if (random_uniform(random_engine) != 47)
		{
			passwd += (char)(random_uniform(random_engine));
			break;
		}
	}
	for (int i = 0; i < length - 1; i++)
	{
		passwd += (char)(random_uniform(random_engine));
	}
	return passwd;

}

std::string pm::getMd5(std::string& input)
{
	EVP_MD_CTX* mdctx = EVP_MD_CTX_new();

	EVP_DigestInit_ex(mdctx, EVP_md5(), NULL); //初始化
	EVP_DigestUpdate(mdctx, input.c_str(), input.size());

	byte output_value[EVP_MAX_MD_SIZE] = {};
	unsigned int output_len = 0;

	EVP_DigestFinal_ex(mdctx, output_value, &output_len);

	char* formatted_output_value = new char[output_len];
	for (unsigned int i = 0; i < output_len; i++)
	{
		sprintf_s(formatted_output_value, output_len, "0x%02x", output_value[i]);
	}

	EVP_MD_CTX_free(mdctx); //释放实例

	std::string output{ formatted_output_value };
	delete [] formatted_output_value; //清理内存

	return output;
}

std::string pm::getSHA1(std::string& input)
{
	EVP_MD_CTX* mdctx = EVP_MD_CTX_new();

	EVP_DigestInit_ex(mdctx, EVP_sha1(), NULL); //初始化
	EVP_DigestUpdate(mdctx, input.c_str(), input.size());

	byte output_value[EVP_MAX_MD_SIZE] = {};
	unsigned int output_len = 0;

	EVP_DigestFinal_ex(mdctx, output_value, &output_len);

	char* formatted_output_value = new char[output_len];
	for (unsigned int i = 0; i < output_len; i++)
	{
		sprintf_s(formatted_output_value, output_len, "0x%02x", output_value[i]);
	}

	EVP_MD_CTX_free(mdctx); //释放实例

	std::string output{ formatted_output_value };
	delete[] formatted_output_value; //清理内存

	return output;
}

void pm::getGen(byte key[KEY_SIZE], byte iv[BLOCK_SIZE])
{
	int rc = RAND_bytes(key, KEY_SIZE);
	if (rc != 1)
		throw std::runtime_error("RAND_bytes key failed");

	rc = RAND_bytes(iv, BLOCK_SIZE);
	if (rc != 1)
		throw std::runtime_error("RAND_bytes for iv failed");
}

std::string pm::Encrypt(const std::string ptext, const byte key[KEY_SIZE], const byte iv[BLOCK_SIZE], const EVP_CIPHER* mode)
{
	if (nullptr == mode)
		mode = EVP_aes_256_cbc();

	EVP_CIPHER_CTX_free_ptr ctx(EVP_CIPHER_CTX_new(), ::EVP_CIPHER_CTX_free);
	int rc = EVP_EncryptInit_ex(ctx.get(), mode, NULL, key, iv);
	if (1 != rc)
		throw std::runtime_error("EVP_EncryptInit_ex failed");

	// Recovered text expands upto BLOCK_SIZE
	std::string ctext = {};
	ctext.resize(ptext.size() + BLOCK_SIZE);
	int out_len1 = (int)ctext.size();

	rc = EVP_EncryptUpdate(ctx.get(), (byte*)&ctext[0], &out_len1, (const byte*)&ptext[0], (int)ptext.size());
	if (1 != rc)
		throw std::runtime_error("EVP_EncryptUpdate failed");

	int out_len2 = (int)ctext.size() - out_len1;
	rc = EVP_EncryptFinal_ex(ctx.get(), (byte*)&ctext[0] + out_len1, &out_len2);
	if (1 != rc)
		throw std::runtime_error("EVP_EncryptFinal_ex failed");

	// Set cipher text size now that we know it
	ctext.resize(static_cast<size_t>(out_len1 + out_len2));

	return ctext;
}

std::string pm::Decrypt(const std::string ctext, const byte key[KEY_SIZE], const byte iv[BLOCK_SIZE], const EVP_CIPHER* mode)
{
	if (nullptr == mode)
		mode = EVP_aes_256_cbc();

	EVP_CIPHER_CTX_free_ptr ctx(EVP_CIPHER_CTX_new(), ::EVP_CIPHER_CTX_free);
	int rc = EVP_DecryptInit_ex(ctx.get(), mode, NULL, key, iv);
	if (1 != rc)
		throw std::runtime_error("EVP_DecryptInit_ex failed");

	// Recovered text contracts upto BLOCK_SIZE
	std::string rtext = {};
	rtext.resize(ctext.size());
	int out_len1 = (int)rtext.size();

	rc = EVP_DecryptUpdate(ctx.get(), (byte*)&rtext[0], &out_len1, (const byte*)&ctext[0], (int)ctext.size());
	if (1 != rc)
		throw std::runtime_error("EVP_DecryptUpdate failed");

	int out_len2 = (int)rtext.size() - out_len1;
	rc = EVP_DecryptFinal_ex(ctx.get(), (byte*)&rtext[0] + out_len1, &out_len2);
	if (1 != rc)
		throw std::runtime_error("EVP_DecryptFinal_ex failed");

	// Set recovered text size now that we know it
	rtext.resize(static_cast<size_t>(out_len1 + out_len2));

	return rtext;
}

std::string pm::doAES(std::string input,int enc)
{
	return {};
}