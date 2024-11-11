#pragma warning(disable : 4996)
#pragma warning(disable : 4700)
#include <WinSock2.h>
#include <WinUser.h>
#include <Windows.h>
#include <iostream>
#include <io.h>
#include <fcntl.h>
#include <string>
#include <thread>
#include <tchar.h>
#include <strsafe.h>
#include <vector>
#include <time.h>
#include <cstdlib>
#include <immintrin.h>
#include <mstcpip.h>
HANDLE handle_out;
HANDLE handle_in;

int WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
    AllocConsole();
    handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
    int hCrt = _open_osfhandle((long)handle_out, _O_TEXT);
    FILE* hf_out = _fdopen(hCrt, "w");
    setvbuf(hf_out, NULL, _IONBF, 1);
    *stdout = *hf_out;

	handle_in = GetStdHandle(STD_INPUT_HANDLE);
    hCrt = _open_osfhandle((long)handle_in, _O_TEXT);
    FILE* hf_in = _fdopen(hCrt, "r");
    setvbuf(hf_in, NULL, _IONBF, 128);
    *stdin = *hf_in;
	while (1) {
		char buff1[32] = { 0 };
		DWORD received = 0;
		ReadConsoleA(handle_in, buff1, 32, &received, NULL);
		char* parsed = (char*)malloc(received);
		for (int i = 0; i < received; i++)
			parsed[i] = buff1[i];
		WSADATA wsadata;
		WSAStartup(MAKEWORD(2, 2), &wsadata);
		SOCKET ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (ConnectSocket == INVALID_SOCKET)
		{
			memset(&buff1, 0, 32);
			sprintf(buff1, "error: %d\n", WSAGetLastError());
			WriteConsoleA(handle_out, buff1, strlen(buff1), 0, NULL);
			WSACleanup();
			ExitProcess(1);
		}
		sockaddr_in clientService;
		clientService.sin_family = AF_INET;
		clientService.sin_addr.s_addr = inet_addr(parsed);
		for (int i = 1; i < 65537; i++)
		{
			clientService.sin_port = htons(i);
			int iResult = connect(ConnectSocket, (sockaddr*)&clientService, sizeof(clientService));
			if (WSAGetLastError()!=10038)
			{
				memset(&buff1, 0, 32);
				sprintf(buff1, "port %d code: %d\n", i, WSAGetLastError());
				WriteConsoleA(handle_out, buff1, strlen(buff1), 0, NULL);
				if (!iResult)
				{
					char recvbuf[1024] = "";
					int bytesReceived = recv(ConnectSocket, recvbuf, 1024, 0);
					memset(&buff1, 0, 32);
					sprintf(buff1, "bytes received %d\n :", bytesReceived);
					WriteConsoleA(handle_out, buff1, strlen(buff1), 0, NULL);
					WriteConsoleA(handle_out, recvbuf, 1024, 0, NULL);
					WriteConsoleA(handle_out, "\n", 2, 0, NULL);
				}
			}

			closesocket(ConnectSocket);
		}
	}
	ExitProcess(0xf41);
}
