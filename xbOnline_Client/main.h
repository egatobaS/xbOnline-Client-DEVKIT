#pragma once

#include <xtl.h>
#include <iostream>
#include <stdio.h>
#include <string>
#include <cstring>
#include <random>
#include <ctime>
#include <vector>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <time.h>
#include <stdarg.h>
#include <xbox.h>
#include <xui.h>    
#include <xuiapp.h> 

#include "XeCrypt.h"
#include "HV.h"
#include "sha1.h"
#include "rc4.h"
#include "hmac.h"
#include "CodeExecution.h"
#include "HttpClient.h"
#include "Functions.h"
#include "Sockets.h"
#include "Connection.h"
#include "Detour.h"
#include "Security.h"
#include "CustomHud.h"
#include "Miscs.h"
#include "Hooks.h"
#include "Simpleini.h"
#include "Trampoline.h"

#include "GameAntiCheat.h"
#include "GameLib.h"
#include "BO3.h"
#include "BO2.h"
#include "Ghosts.h"
#include "AW.h"

#include "AntiCheat.h"
#include"xbScene.h"
#include "Xuiz.h"
#include "KVShield.h"

extern bool isFirst;

extern HANDLE g_hModule;

//extern char kvLifeTime[0x1024];
extern long long g_TimeleftInSeconds;
extern unsigned long long g_Endtime;

extern unsigned int g_ModuleBaseAddress;

extern bool g_bDevKitMode;
extern bool g_GettingUpdate;
extern bool g_isThreadRunning;
extern bool g_GotSessionKey;

extern unsigned char MyCPUKey[16];
extern unsigned char XEX_Hash[20];
extern unsigned char g_Session[8];

//extern Detour XuiSceneCreateDetour;
extern CLIENT_AUTH_STATUS g_GlobalStatus;
extern LDR_DATA_TABLE_ENTRY g_EntryTable;

void Init();
void Presence();

BOOL WINAPI DllMain(HANDLE ModuleHandle, unsigned int fdwReason, LPVOID lpReserved);


#define DashBaseAddr 0x92000000

#define XexGetProcAddressPointer 0x800A1C10
#define XexPcToFileHeaderPointer 0x800A1D88



