#include "main.h"

Detour sub_825D99C8Detour;

typedef int(*sub_825D99C8Stub)(XNADDR * pxna, int l);
extern sub_825D99C8Stub sub_825D99C8Original;

sub_825D99C8Stub sub_825D99C8Original;

Detour BO2Detour;

void setLogMessage_BO2(int CheatResp, const char* Message, int Message_Size)
{
	if (Message == 0 || Message_Size == 0) {

		if (CheatResp)
		{
			*(int*)(CheatResp + 0x60) = 0;
			memset((void*)(CheatResp + 0x1E), 0, 0x40);
		}
		return;
	}

	if (Message_Size == 0x36)
	{
		BO2_CHALL_RESP* resp = (BO2_CHALL_RESP*)Message;

		resp->secValue = 0xFB42;

		memcpy(resp->ConsoleID, spoofedConsoleID, 8);

		memcpy(resp->MacAddress, spoofedMacAddress, 6);

		resp->deValue = 1;
		memcpy(resp->SerialNumber, spoofedSerialNumber, 8);

		resp->MachiebeID = spoofedMachineID;

		resp->KenelVersion = 0x4476;
	}

	

	memcpy((void*)(CheatResp + 0x1E), Message, (Message_Size <= 0x40) ? Message_Size : 0x40);
	*(DWORD*)(CheatResp + 0x60) = (Message_Size < 0x40) ? Message_Size : 0x40;
}

bool BO2_LiveAntiCheat_ProcessChallengeResponse(int controller, short funcID, int* params, __int64* result)
{
	if (funcID == 3) {
		__int64 Seed = *(__int64 *)*(int *)(params[0] + 0x04);

		if (Seed == 0x1E38E463BCD292A7)
		{
			if (isChallengeMultiplayer)
				result[0] = 0x8706653CFB4385C7;
			else
				result[0] = 0xD70DCBEB64D8B8B5;
		}


		else if (Seed == 0x8477ADFA96D26D08)
		{
			if (isChallengeMultiplayer)
				result[0] = 0xF651F152DCD36E8A;
			else
				result[0] = 0x79395667B528692;
		}


		else
			XLaunchNewImage(NULL, NULL);;

		return true;
	}
	else if (funcID == 4) {
		char var[0x50] = { 0 };

		memcpy(var, (char*)*(int *)(params[0] + 4), *(int *)(params[0] + 8));

		if (!strcmp("cg_drawFPS", var)) {

			*result = 0; // 0
			return TRUE;
		}
		else if (!strcmp("g_speed", var)) {

			*result = 190; // 999
			return TRUE;
		}
		else
			XLaunchNewImage(NULL, NULL);
	}
	else if (funcID == 8) {

	}
	else
		XLaunchNewImage(NULL, NULL);

	return true;

}



int sub_825D99C8(XNADDR * pxna, int l)
{
	int linkR;

	__asm mflr linkR;

	int ret = sub_825D99C8Original(pxna, l);

	if (linkR == 0x8228D324) {
		return ret;
	}

	srand((unsigned int)time(0));
	long long machineID = 0xFA00000000000000 | (0x2000000 | rand() % 0x7FFFFF);

	*(int*)(pxna->abEnet) = (rand() % 0x7FFFFF);

	*(long long*)(pxna + 0x18) = _byteswap_uint64(machineID);

	return ret;
}

void InitBlackOps2()
{
	RandomValues();

	if (isChallengeMultiplayer)
	{
		sub_825D99C8Original = (sub_825D99C8Stub)sub_825D99C8Detour.HookFunction(0x825D99C8, (DWORD)sub_825D99C8);
		XNetXnAddrToMachineIdDetour.HookFunction(0x8293E534, (int)XNetXnAddrToMachineIdHook);

		*(int*)(0x8228CFA8) = 0x48000044;
		*(int*)0x825C5394 = 0x480000C8; 
	}
	
	BO2Detour.HookFunction(isChallengeMultiplayer ? 0x8259A5E0 : 0x824A7C50, (DWORD)BO2_LiveAntiCheat_ProcessChallengeResponse);
	WriteHookOriginal = (WriteHook_t)WriteHookDetour.HookFunction(isChallengeMultiplayer ? 0x8286A348 : 0x8264D4D0, (DWORD)WriteHook);
	setMessageLogDetour.HookFunction(isChallengeMultiplayer ? 0x82884CC0 : 0x826592D0, (DWORD)setLogMessage_BO2);

	
}

