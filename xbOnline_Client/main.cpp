#include "main.h"

/*
mflr %r11
std %r11, -8(%r1)
std %r31, -0x10(%r1)
std %r30, -0x18(%r1)
std %r29, -0x20(%r1)
std %r28, -0x28(%r1)
std %r27, -0x30(%r1)
std %r26, -0x38(%r1)
std %r25, -0x40(%r1)
stdu %r1, -0xF0(%r1)

mr %r3, %r4
mr %r4, %r5
mr %r5, %r6
mr %r6, %r7
mr %r7, %r8

li %r6, 0
cmpwi cr6, %r4, 0
blelr cr6
mtctr %r4

loc_loop:
lbzx %r5, %r6, %r3
xori %r5, %r5, 0x29
stbx %r5, %r6, %r3
addi %r6, %r6, 1
bdnz loc_loop

addi %r1, %r1, 0xF0
ld %r11, -8(%r1)
mtlr %r11
ld %r25, -0x40(%r1)
ld %r26, -0x38(%r1)
ld %r27, -0x30(%r1)
ld %r28, -0x28(%r1)
ld %r29, -0x20(%r1)
ld %r30, -0x18(%r1)
ld %r31, -0x10(%r1)
li %r3, 0
blr

*/

/*
mflr %r11
std %r11, -8(%r1)
std %r31, -0x10(%r1)
std %r30, -0x18(%r1)
std %r29, -0x20(%r1)
std %r28, -0x28(%r1)
std %r27, -0x30(%r1)
std %r26, -0x38(%r1)
std %r25, -0x40(%r1)
stdu %r1, -0xF0(%r1)

mr %r3, %r4
mr %r4, %r5
mr %r5, %r6
mr %r6, %r7
mr %r7, %r8

li        %r11, 0
cmpwi     cr6, %r4, 0
blelr     cr6
mtctr     %r4

loc_90E0A378:
lbzx      %r10, %r11, %r3
extsb     %r10, %r10
xor       %r10, %r10, %r4
xori      %r10, %r10, 0xC
not       %r10, %r10
extsb     %r10, %r10
stbx      %r10, %r11, %r3
addi      %r11, %r11, 1
bdnz      loc_90E0A378

addi %r1, %r1, 0xF0
ld %r11, -8(%r1)
mtlr %r11
ld %r25, -0x40(%r1)
ld %r26, -0x38(%r1)
ld %r27, -0x30(%r1)
ld %r28, -0x28(%r1)
ld %r29, -0x20(%r1)
ld %r30, -0x18(%r1)
ld %r31, -0x10(%r1)
li %r3, 0
blr

*/

/*
Made By Sabotage, Krazaki & OSM - 2017

Thanks to :

Sabotage - Stealing shit
Krazaki - Stealing shit too
OSM - Being a god
Konroi - Sponsorship & Funding
Jester - Inspiration & Shit talking
Alexb - Moral Support
TUC - Inspiration & Shit talking
Matrix Mods - responses on plain text
Nigel - Being a good bloke (and giving us all of james' shit & devkit loot (and don't forget, doing drugs))
Blasts Mods - Blasts Modding people
Jay - Hud shit
MLP - Server Security (He actually helped) & Pen testing
Anon Live - Pen testing
iMoD1998 - (This guy actually helped)
Exile - Moral Support
Lachie Web Dev (Panel)

*/

bool isFirst = false;

bool g_bDevKitMode = false;

bool g_GettingUpdate = false;

bool g_GotSessionKey = false;
bool g_isThreadRunning = false;

long long g_TimeleftInSeconds = 0;
unsigned long long g_Endtime = 0;
unsigned int g_ModuleBaseAddress = 0;

HANDLE g_hModule = 0;

Detour NetDll_XnpLogonSetChallengeResponseDetour;
LDR_DATA_TABLE_ENTRY g_EntryTable;
CLIENT_AUTH_STATUS g_GlobalStatus = NO_CONNECTED;

unsigned char MyCPUKey[16] = { 0 };

unsigned char g_Session[8] = { 0 };
unsigned char XEX_Hash[20] = { 0 };

unsigned char hBreakPattern[12] = {
	0x7C, 0x9A, 0x23, 0x78, 0x7C, 0xBE, 0x2B, 0x78, 0x38, 0xA0, 0x00, 0x00
};

unsigned char Corona_1BL_Key_Fix[16] = {
	0xDD, 0x88, 0xAD, 0x0C, 0x9E, 0xD6, 0x69, 0xE7, 0xB5, 0x67, 0x94, 0xFB,
	0x68, 0x56, 0x3E, 0xFA
};

unsigned char DmWalkLoadedModulesExPattern[16] = {
	0x94, 0x21, 0xFF, 0x40, 0x7C, 0x77, 0x1B, 0x78, 0x7C, 0x96, 0x23, 0x78,
	0x3E, 0xA0, 0x02, 0xDA
};

void Presence()
{
	while (!g_isThreadRunning)
	{
		unsigned char CPUKey[0x10] = { 0 };
		unsigned char Geneology[0x10] = { 0 };

		Tramps->CallFunction(GetCPUKey_Function, (int)CPUKey, 0, 0, 0, false);

		Tramps->CallFunction(xbCreateBoxKey_Function, (long long)xbOnline_BoxKey1, (int)Geneology, 0, 0, false);

		Client* Connection = new Client(CPUKey, Geneology, XEX_Hash);

		if (Connection->Presence(g_Session, &g_TimeleftInSeconds, &g_GlobalStatus)) {
			isFirst = true;
			g_Endtime = time(NULL) + g_TimeleftInSeconds;
		}


		if (isFirst)
			Sleep(60000);

		delete Connection;
	}
}

void GetSessionKey()
{

	Sleep(1000);

#if defined(DEVKIT)
#else
	bool hasInit = false;
	if (xb_custom_xui && !hasInit)
	{
		HANDLE hThread1 = 0; DWORD threadId1 = 0;

		ExCreateThread(&hThread1, 0, &threadId1, (VOID*)XapiThreadStartup, (LPTHREAD_START_ROUTINE)PathHuds, NULL, 0x2);
		XSetThreadProcessor(hThread1, 4);
		ResumeThread(hThread1);
		CloseHandle(hThread1);
		hasInit = true;
	}
#endif

	if (!g_GotSessionKey)
	{
		while (g_GlobalStatus == NO_CONNECTED)
		{
			XNADDR xna = { 0 };

			int i = XNetGetTitleXnAddr(&xna);

			if (i != XNET_GET_XNADDR_PENDING) {

				if (xna.ina.S_un.S_un_b.s_b1 == 0 && xna.ina.S_un.S_un_b.s_b2 == 0 && xna.ina.S_un.S_un_b.s_b3 == 0 && xna.ina.S_un.S_un_b.s_b4 == 0) {

					Sleep(100);

					continue;
				}
				
				unsigned char CPUKey[0x10] = { 0 };
				unsigned char Geneology[0x10] = { 0 };

				Tramps->CallFunction(GetCPUKey_Function, (int)CPUKey, 0, 0, 0, false);

				Tramps->CallFunction(xbCreateBoxKey_Function, (long long)xbOnline_BoxKey1, (int)Geneology, 0, 0, false);

				Client Connection(CPUKey, Geneology, XEX_Hash);

				if (Connection.GetSession(g_Session, &g_TimeleftInSeconds, &g_GlobalStatus) && !GotAnewUpdate) {
					
					g_Endtime = time(NULL) + g_TimeleftInSeconds;

					g_GotSessionKey = true;

					HANDLE hThread1 = 0; DWORD threadId1 = 0;
					ExCreateThread(&hThread1, 0, &threadId1, (VOID*)XapiThreadStartup, (LPTHREAD_START_ROUTINE)Presence, NULL, 2 | CREATE_SUSPENDED);
					XSetThreadProcessor(hThread1, 4);
					ResumeThread(hThread1);
					CloseHandle(hThread1);
				}

				if (g_GlobalStatus != NO_CONNECTED)
					break;
			}
		}
	}
}


void Init()
{
	SetLiveBlock(true);

	if (!Mount_Drive())
	{
		if (!DriveOverRide())
			if (OpenedTray()) return;

		if (!InitializeHvPeekPoke())
		{
			if (!ProcessCPUKeyBin(PATH_CPUKEYB))
			{
				LoadINI();
			
				g_bDevKitMode = *(DWORD*)0x8E038610 & 0x8000 ? false : true;

				DWORD Version = ((XboxKrnlVersion->Major & 0xF) << 28) | ((XboxKrnlVersion->Minor & 0xF) << 24) | (XboxKrnlVersion->Build << 8) | (XboxKrnlVersion->Qfe);
				ZeroMemory(&SpoofedExecutionId, sizeof(XEX_EXECUTION_ID));
				SpoofedExecutionId.Version = Version;
				SpoofedExecutionId.BaseVersion = Version;
				SpoofedExecutionId.TitleID = 0xFFFE07D1;

				Tramps->CallFunction(PatchModuleImport_Function, (int)MODULE_XAM, (int)MODULE_KERNEL, 408, (int)XexLoadExecutableHook, false);
				Tramps->CallFunction(PatchModuleImport_Function, (int)MODULE_XAM, (int)MODULE_KERNEL, 409, (int)XexLoadImageHook, false);
				Tramps->CallFunction(PatchModuleImport_Function, (int)MODULE_XAM, (int)MODULE_KERNEL, 410, (int)XexLoadImageFromMemory_XamHook, false);
				Tramps->CallFunction(PatchModuleImport_Function, (int)MODULE_XAM, (int)MODULE_KERNEL, 0x12B, (int)RtlImageXexHeaderFieldHook, false);
				Tramps->CallFunction(PatchModuleImport_Function, (int)MODULE_XAM, (int)MODULE_KERNEL, 404, (int)XexCheckExecutablePrivilegeHook, false);
				Tramps->CallFunction(PatchModuleImport_Function, (int)MODULE_XAM, (int)MODULE_KERNEL, 0x25F, (int)XeKeysExecuteHook, false);

				NetDll_XnpLogonSetChallengeResponseDetour.HookFunction((DWORD)0x81A1BEB0, (DWORD)NetDll_XnpLogonSetChallengeResponse);

				*(DWORD*)0x817A5484 = 0x38600006;
				*(DWORD*)0x81CB9874 = 0x38600001;
				*(DWORD*)0x81732850 = 0x38600000;

				*(DWORD*)0x800af860 = 0x60000000;

				if (!Process_KV())
				{
					ProcessKeyVault();

					if (!SetMacAddress())
					{
						XamCacheReset(XAM_CACHE_TICKETS);
						Tramps->CallFunction(memcpy_Function, (int)0x8E03AA40, (int)KVDigest, 0x10, 0, false);
					}
				}
				
				CreateXboxThread(GetSessionKey, GetSessionKey);
			}
		}
	}
}

void RegisterFunctions()
{
	Tramps->RegisterFunction((int)Init, Init_Function);
	Tramps->RegisterFunction((int)Presence, Presence_Thread);
	Tramps->RegisterFunction((int)PatchModuleImport, PatchModuleImport_Function);
	Tramps->RegisterFunction((int)GetCPUKey, GetCPUKey_Function);
	Tramps->RegisterFunction((int)xbCreateBoxKey, xbCreateBoxKey_Function);
	Tramps->RegisterFunction((int)ExecuteCode, ExecuteCode_Function);
	Tramps->RegisterFunction((int)memcpy, memcpy_Function);
}

BOOL WINAPI DllMain(HANDLE ModuleHandle, unsigned int fdwReason, LPVOID lpReserved)
{

	if (fdwReason == DLL_PROCESS_ATTACH) 
	{
		Tramps = new Tampoline();

		RegisterFunctions();

		AntiPasswordTheft();

		ApplyPatches(rglXam);

		*(int*)(0x819658d0) = 0x60000000;
		*(int*)(0x81965924) = 0x60000000;
		*(int*)(0x8175C8EC) = 0x48000010;
		*(int*)(0x817B3A14) = 0x60000000;

		CurrentPath = (int)malloc(0x100);

		GetBootedPath((HMODULE)ModuleHandle, (char*)CurrentPath);
	
		ApplyPatches(Console_Patches);

		::g_hModule = ModuleHandle;

		memcpy(&g_EntryTable, ModuleHandle, sizeof(LDR_DATA_TABLE_ENTRY));
		g_ModuleBaseAddress = (unsigned int)g_EntryTable.NtHeadersBase;

		Tramps->CallFunction(Init_Function, 0, 0, 0, 0, false);
		
		HrBreakOriginal = (HrBreakStub)HrBreakDetour.HookFunction(0x91F48A38, (unsigned int)HrBreak);
		MmIsAddressValidOriginal = (MmIsAddressValid_t)MmIsAddressValidDetour.HookFunction((DWORD)ResolveFunction("xboxkrnl.exe", 191), (DWORD)MmIsAddressValidHook);
		MmDbgReadCheckOriginal = (MmDbgReadCheck_t)MmDbgReadCheckDetour.HookFunction((DWORD)ResolveFunction("xboxkrnl.exe", 427), (DWORD)MmDbgReadCheckHook);
		Tramps->CallFunction(PatchModuleImport_Function, (int)"xbdm.xex", (int)MODULE_KERNEL, 191, (int)MmIsAddressValidHook, false);
		Tramps->CallFunction(PatchModuleImport_Function, (int)"xbdm.xex", (int)MODULE_KERNEL, 427, (int)MmDbgReadCheckHook, false);

		RemoveFromList(ModuleHandle);
	}
	else if (fdwReason == DLL_PROCESS_DETACH) 
	{

		g_isThreadRunning = true;
		Sleep(2000);
	}

	return TRUE;

}



