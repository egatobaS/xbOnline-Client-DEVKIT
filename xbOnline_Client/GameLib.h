#pragma once

extern int Func_ptr[2];
extern long long spoofedMachineID;
extern unsigned char spoofedMacAddress[6];
extern unsigned char Text_SerialNumber[12];
extern unsigned char Text_ConsoleID[13];
extern unsigned char spoofedConsoleID[8];
extern unsigned char spoofedSerialNumber[8];

void RandomValues();

extern Detour XNetXnAddrToMachineIdDetour;
extern Detour NetDll_XNetGetTitleXnAddrDetour;

DWORD NetDll_XNetGetTitleXnAddrHook(XNCALLER_TYPE xnc, XNADDR * pxna);
int XNetXnAddrToMachineIdHook(XNCALLER_TYPE xnc, XNADDR* pxnaddr, unsigned long long* qwMachineId);