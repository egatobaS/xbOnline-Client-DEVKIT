#include "main.h"

char msg_Buffer[0x10000] = { 0 };

MSG_ReadBitsCompress_t MSG_ReadBitsCompressOriginal;
Detour MSG_ReadBitsCompressedDetour;

Detour _Reset_LiveIdDetour;
_Reset_LiveIdStub _Reset_LiveIdOriginal;

int _Reset_LiveId(int instance)
{
	int ret = _Reset_LiveIdOriginal(instance);

	memset((void*)0x81D13D00, 0, 0x1000);
	memset((void*)0x81D15D00, 0, 0x1000);

	return ret;
}

void AntiPasswordTheft()
{
	srand((unsigned int)time(NULL));
	int randNum = rand() % 2;

	*(int*)0x81BE739C = 0x3D2081D1;
	*(int*)0x81BE73AC = 0x99690000 + (randNum ? 0x3D00 : 0x5D00);
	*(int*)0x81BE8778 = 0x3D4081D1;
	*(int*)0x81BE8788 = 0x9BCA0000 + (randNum ? 0x3D00 : 0x5D00);
	*(int*)0x81BEA524 = 0x3D4081D1;
	*(int*)0x81BEA52C = 0x3BCA0000 + (randNum ? 0x3D00 : 0x5D00);
	*(int*)0x81BEA884 = 0x3D6081D1;
	*(int*)0x81BEA894 = 0x3BEB0000 + (randNum ? 0x3D00 : 0x5D00);
	*(int*)0x81BEAB8C = 0x3D6081D1;
	*(int*)0x81BEAB9C = 0x3BAB0000 + (randNum ? 0x3D00 : 0x5D00);
	*(int*)0x81BE73A0 = 0x3D0081D1;
	*(int*)0x81BE73B0 = 0x99480000 + (randNum ? 0x5D00 : 0x3D00);
	*(int*)0x81BE753C = 0x3D6081D1;
	*(int*)0x81BE7544 = 0x396B0000 + (randNum ? 0x5D00 : 0x3D00);
	*(int*)0x81BE877C = 0x3D2081D1;
	*(int*)0x81BE878C = 0x9BC90000 + (randNum ? 0x5D00 : 0x3D00);
	_Reset_LiveIdOriginal = (_Reset_LiveIdStub)_Reset_LiveIdDetour.HookFunction((DWORD)0x81BE8690, (DWORD)_Reset_LiveId);


}

int MSG_ReadBitsCompressHook(const char *from, char *to, int size)
{
	int Link_register = 0;
	__asm mflr Link_register;

	int Length = MSG_ReadBitsCompressOriginal(from, msg_Buffer, size);

	if (Link_register == 0x822C8420 || Link_register == 0x82253860) 
	{
		if (Length <= 0x800) 
		{
			memcpy(to, msg_Buffer, Length);
			return Length;
		}
	}
	else
		return MSG_ReadBitsCompressOriginal(from, to, size);

	return 0;
}

