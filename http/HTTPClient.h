// HTTPClient.h: interface for the HTTPClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HTTPCLIENT_H__2A78F088_AB6C_4971_BF0F_F9DC0C5B80AD__INCLUDED_)
#define AFX_HTTPCLIENT_H__2A78F088_AB6C_4971_BF0F_F9DC0C5B80AD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <winsock2.h>

#define MAX_RECEIVABLE	80000
#define MAX_URL_LENGTH	256

#define HTTP	0

typedef struct {
	int protocol;
	int port;

	char host[MAX_URL_LENGTH];
	char url[MAX_URL_LENGTH];	
	} CONNECTSTRUCT, *LPCONNECTSTRUCT;

#define HTTP_WAITING				0
#define HTTP_OK						1
#define HTTP_ERROR					2
#define HTTP_CANCEL					3
#define HTTP_TIMEOUT				4
#define MAX_RECEIVABLE				80000

class HTTPClient {
	public:
		void Request(char *host, char *url);
		HTTPClient();
		virtual ~HTTPClient();

		WSADATA WSAData;

		struct sockaddr_in server_addr;
		struct hostent *hostent;
		//CONNECTSTRUCT connect;

		BOOL ready;
		int HTTPsocket;
		HANDLE RecvThread;
		DWORD ThreadID;
		long basetime;

	public:
		BOOL Wait(int time);
		void Cancel(void);
		
		int result;
		char buffer[MAX_RECEIVABLE+1];
	};

#endif // !defined(AFX_HTTPCLIENT_H__2A78F088_AB6C_4971_BF0F_F9DC0C5B80AD__INCLUDED_)
