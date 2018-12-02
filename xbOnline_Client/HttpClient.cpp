//--------------------------------------------------------------------------------------
// HttpClient.cpp
//
// XNA Developer Connection.
// Copyright (C) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#include "main.h"

void MemoryAlloc::AddData(unsigned char* NewData, unsigned int NewDataSize)
{
	unsigned int tempDataSize = NewDataSize + Size;

	unsigned char* newBuff = (unsigned char*)malloc(tempDataSize);

	memset(newBuff, 0, tempDataSize);

	memcpy(newBuff, AllocatedData, Size);

	free(AllocatedData);

	AllocatedData = newBuff;

	memcpy(AllocatedData + Size, NewData, NewDataSize);
	Size += NewDataSize;
}

MemoryAlloc::MemoryAlloc(unsigned int Size)
{
	AllocatedData = (unsigned char*)malloc(1);
	memset(AllocatedData, 0, 1);
	this->Size = 0;
}

void MemoryAlloc::ClearAndAlloc()
{
	if (AllocatedData)
		free(AllocatedData);

	AllocatedData = (unsigned char*)malloc(1);
	memset(AllocatedData, 0, 1);
	this->Size = 0;

}

void MemoryAlloc::ClearBuffer()
{
	if (AllocatedData)
		free(AllocatedData);

	AllocatedData = NULL;
	this->Size = 0;
}

MemoryAlloc::~MemoryAlloc()
{
	if (AllocatedData)
		free(AllocatedData);

	AllocatedData = NULL;
	Size = 0;
}


// Worker thread entrance or main processing routine
DWORD WINAPI HttpSendCommand(LPVOID lpParameter);


DWORD WINAPI HttpSendCommand(LPVOID lpParameter)
{
	HttpClient* pHttpClient = (HttpClient*)lpParameter;

	if (!pHttpClient)
		return S_FALSE;

	// internal buffer
	HTTP_BUFFER& httpBuffer = pHttpClient->GetInternalBuffer();

	int nErrorCode;

	// Create TCP/IP socket
	SOCKET hSocket;
	hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if ((hSocket == SOCKET_ERROR) || (hSocket == INVALID_SOCKET))
	{
		nErrorCode = WSAGetLastError();
		pHttpClient->SetSocketErrorCode(nErrorCode);
		pHttpClient->SetStatus(HttpClient::HTTP_STATUS_ERROR);            // always put this last
		return nErrorCode;
	}

	unsigned long iMode = 1;

	if (setsockopt(hSocket, SOL_SOCKET, 0x5801, (char*)&iMode, 4) != S_OK)
		return false;

	DWORD timeout = 1000;

	if (setsockopt(hSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(DWORD))) {

		nErrorCode = WSAGetLastError();
		shutdown(hSocket, SD_BOTH);
		closesocket(hSocket);
		pHttpClient->SetSocketErrorCode(nErrorCode);
		pHttpClient->SetStatus(HttpClient::HTTP_STATUS_ERROR);    // Always put this last
		return nErrorCode;
	}


	sockaddr_in httpServerAdd;

	httpServerAdd.sin_family = AF_INET;
	httpServerAdd.sin_port = htons(httpBuffer.port);
	httpServerAdd.sin_addr.s_addr = inet_addr(httpBuffer.serverName);

	if (connect(hSocket, (struct sockaddr*)&httpServerAdd, sizeof(httpServerAdd)) != 0)
	{
		nErrorCode = WSAGetLastError();
		shutdown(hSocket, SD_BOTH);
		closesocket(hSocket);
		pHttpClient->SetSocketErrorCode(nErrorCode);
		pHttpClient->SetStatus(HttpClient::HTTP_STATUS_ERROR);    // Always put this last
		return nErrorCode;
	}

	if (SOCKET_ERROR != send(hSocket, (const char*)httpBuffer.MB_request.GetData(),
		httpBuffer.MB_request.GetDataLength(), 0))
	{
		int tempBufSize = 0, nSize = 0;

		char buff[TCP_RECV_BUFFER_SIZE];

		while ((nSize = recv(hSocket, buff, TCP_RECV_BUFFER_SIZE, 0)) != 0)
		{
			if (nSize == -1) break;

			memcpy((void*)(pHttpClient->TempBuffer + tempBufSize), buff, nSize);
			tempBufSize += nSize;
		}

		pHttpClient->smallBUffer = pHttpClient->TempBuffer;

		shutdown(hSocket, SD_BOTH);
		closesocket(hSocket);

		pHttpClient->SetSocketErrorCode(ERROR_SUCCESS);

		return pHttpClient->ParseIncomingHttpResponse(pHttpClient->smallBUffer, tempBufSize);
	}
	else
	{
		shutdown(hSocket, SD_BOTH);
		closesocket(hSocket);
		pHttpClient->SetSocketErrorCode(WSAGetLastError());
		return S_FALSE;
	}
}


HttpClient::HttpClient(BOOL bThread, DWORD dwPort)
{
	smallBUffer = 0;

	TempBuffer = new unsigned char[0x90000];

	m_buffer.port = dwPort;
	m_bUsingThread = bThread;

	m_status = HTTP_STATUS_READY;
}


HttpClient::~HttpClient()
{
	if (TempBuffer)
		delete TempBuffer;

	TempBuffer = 0;
}


HRESULT HttpClient::SendCommand()
{
	if (m_bUsingThread)
	{
		HANDLE hThread1 = 0; DWORD threadId1 = 0;
		ExCreateThread(&hThread1, 0x10000, &threadId1, (VOID*)XapiThreadStartup, (LPTHREAD_START_ROUTINE)HttpSendCommand, this, 0x2);
		XSetThreadProcessor(hThread1, 4);
		ResumeThread(hThread1);
		CloseHandle(hThread1);

		if (hThread1)
		{
			CloseHandle(hThread1);
			return E_PENDING;
		}
		else
			return E_FAIL;;
	}
	else
		return HttpSendCommand(this);
}


HRESULT HttpClient::ParseIncomingHttpResponse(unsigned char* Buffer, unsigned int Size)
{
	UINT nDataLength = Size;

	if (nDataLength <= 4)
	{
		SetStatus(HttpClient::HTTP_STATUS_ERROR);
		return E_FAIL;
	}

	UCHAR* pData = Buffer;

	if (memcmp("HTTP", pData, 4) != 0)
	{
		SetStatus(HttpClient::HTTP_STATUS_ERROR);
		return E_FAIL;
	}

	while (!isspace(*pData) && nDataLength)
	{
		--nDataLength;
		++pData;
	}

	if (nDataLength == 0 )
	{
		SetStatus(HttpClient::HTTP_STATUS_ERROR);
		return E_FAIL;
	}

	m_dwResponseCode = atoi((char*)pData);


	if (m_dwResponseCode != 200)
	{
		SetStatus(HttpClient::HTTP_STATUS_ERROR);
		return E_FAIL;
	}

	BOOL bLineHead = FALSE;
	while (nDataLength)
	{
		if (*pData == '\n')
		{
			if (bLineHead)    // found 2nd LF ?
			{
				--nDataLength;
				++pData;
				break;
			}
			else {
				bLineHead = TRUE;
			}
		}
		else if (*pData != '\r')
		{
			bLineHead = FALSE;
		}

		--nDataLength;
		++pData;
	}

	if (nDataLength == 0)
	{
		SetResponseContentDataLength(0);
		SetResponseContentData(NULL);
		SetStatus(HttpClient::HTTP_STATUS_ERROR);
		return E_FAIL;
	}
	else
	{
		SetResponseContentDataLength(nDataLength);
		SetResponseContentData(pData);
		SetStatus(HttpClient::HTTP_STATUS_DONE);
		return ERROR_SUCCESS;
	}

}

//--------------------------------------------------------------------------------------
// strFileName can't contain unsafe characters, for example, SPACE.
// Otherwise, URL encode function should be added.
// Check RFC 1738 for detail.
//--------------------------------------------------------------------------------------
HRESULT HttpClient::GET(const CHAR* strServerName, const CHAR* strFileName)
{

	char cmdBuff[HTTP_COMMAND_BUFFER_SIZE];

	if (GetStatus() == HTTP_STATUS_BUSY)
		return E_FAIL;

	SetStatus(HttpClient::HTTP_STATUS_BUSY);

	//sprintf_s(cmdBuff, "GET %s HTTP/1.0 \r\n\r\n", strFileName);

	sprintf_s(cmdBuff, "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", strFileName, strServerName);

	m_buffer.MB_request.Rewind();
	m_buffer.MB_request.Add(cmdBuff, strlen(cmdBuff));
	strcpy_s(m_buffer.serverName, strServerName);
	return SendCommand();
}

//--------------------------------------------------------------------------------------
// strFileName can't contain unsafe characters, for example, SPACE.
// Otherwise, URL encode function should be added.
// Check RFC 1738 for detail.
//--------------------------------------------------------------------------------------
HRESULT HttpClient::POST(const CHAR* strServerName, const CHAR* strFileName, const CHAR* pCmd, DWORD dwCmdLength)
{

	char cmdBuff[HTTP_COMMAND_BUFFER_SIZE];

	if (GetStatus() == HTTP_STATUS_BUSY)
		return E_FAIL;

	SetStatus(HttpClient::HTTP_STATUS_BUSY);

	sprintf_s(cmdBuff,
		"POST %s HTTP/1.0 \r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: %d\r\n\r\n", strFileName,
		dwCmdLength);
	m_buffer.MB_request.Rewind();
	m_buffer.MB_request.Add(cmdBuff, strlen(cmdBuff));
	m_buffer.MB_request.Add(pCmd, dwCmdLength);
	strcpy_s(m_buffer.serverName, strServerName);
	return SendCommand();

}

