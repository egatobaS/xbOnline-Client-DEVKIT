#include "main.h"

void EncryptedDecryptPacket(unsigned char* Buffer, int length)
{
	for (int i = 0; i < length; i++)
	{
		Buffer[i] = (unsigned char)(Buffer[i] ^ length);
		Buffer[i] = (unsigned char)(Buffer[i] ^ 0xE4);
		Buffer[i] = (unsigned char)(Buffer[i] ^ 0x61);
		Buffer[i] = (unsigned char)(Buffer[i] ^ 0x91);
		Buffer[i] = (unsigned char)(Buffer[i] ^ 0xD6);
		Buffer[i] = (unsigned char)(Buffer[i] ^ 0x1);
		Buffer[i] = (unsigned char)(Buffer[i] ^ 0xCF);
		Buffer[i] = (unsigned char)(Buffer[i] ^ 0xFF);
	}
}

void EncryptedDecryptPacket_Second(unsigned char* Buffer, int length)
{
	for (int i = 0; i < length; i++)
	{
		Buffer[i] = (unsigned char)(Buffer[i] ^ 0x30);
		Buffer[i] = (unsigned char)(Buffer[i] ^ 0x24);
		Buffer[i] = (unsigned char)(Buffer[i] ^ 0x44);
		Buffer[i] = (unsigned char)(Buffer[i] ^ 0x34);
		Buffer[i] = (unsigned char)(Buffer[i] ^ 0x25);
		Buffer[i] = (unsigned char)(Buffer[i] ^ 0xCE);
		Buffer[i] = (unsigned char)(Buffer[i] ^ 0xF1);
	}
}


