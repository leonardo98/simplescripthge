// HTTPClient.cpp: implementation of the HTTPClient class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HTTPClient.h"

DWORD WINAPI RecvProc(LPVOID lpParameter) {
	HTTPClient *c=(HTTPClient*)lpParameter;

	int returncode=recv(c->HTTPsocket,c->buffer,MAX_RECEIVABLE,0);
	if (returncode<0) {
		closesocket(c->HTTPsocket);
		c->result=HTTP_ERROR;
		strcpy(c->buffer,"Could not receive server response");
		return TRUE;
		}

	c->result=HTTP_OK;
	closesocket(c->HTTPsocket);
	return TRUE;
	}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

HTTPClient::HTTPClient() {
	hostent=NULL;
	ready=FALSE;
	HTTPsocket=-1;

	if (WSAStartup(MAKEWORD(1,1),&WSAData)!=0) {
		char szError[1024];
	    wsprintf(szError,TEXT("WSAStartup failed. Error: %d"),WSAGetLastError());
		MessageBox (NULL, szError, TEXT("Error"), MB_OK);
		return;
		}

	ready=TRUE;
	}

HTTPClient::~HTTPClient() {
	if (hostent!=NULL) delete hostent;
	if (ready==TRUE) WSACleanup();
	}
	
void HTTPClient::Request(char *host, char *url) {
	if (result==HTTP_WAITING) Cancel();

	if (hostent!=NULL) delete hostent;
	if (ready==FALSE) {
		result=HTTP_ERROR;
		strcpy(buffer,"Winsock did not start up correctly");
		return;
		}

	if (strlen(url)>MAX_URL_LENGTH || strlen(host)>MAX_URL_LENGTH) {
		result=HTTP_ERROR;
		strcpy(buffer,"Host or URL string is too long.");
		return;
		}

	hostent=gethostbyname(host);
	if (hostent==NULL) {
		result=HTTP_ERROR;
		strcpy(buffer,"Host was not found.");
		return;
		}

	char *temp=(char *)inet_ntoa(*((struct in_addr *)hostent->h_addr));
	int IPInteger=inet_addr(temp);

	HTTPsocket=socket(AF_INET,SOCK_STREAM,0);
	if (HTTPsocket<0) {
		result=HTTP_ERROR;
		strcpy(buffer,"Could not create HTTP socket");
		return;
		}

	server_addr.sin_family=AF_INET;
	server_addr.sin_port=htons(80);
	server_addr.sin_addr.s_addr=IPInteger;
	ZeroMemory(&server_addr.sin_zero,8);

	int addrlen = sizeof(struct sockaddr);
	int returncode=connect(HTTPsocket,(struct sockaddr*)&server_addr,sizeof(struct sockaddr)); 
	if (returncode<0) {
		closesocket(HTTPsocket);
		result=HTTP_ERROR;
		strcpy(buffer,"Could not connect to server");
		return;
		}

	buffer[0]=0;
	strcat(buffer,"GET ");
	strcat(buffer,url);
	strcat(buffer," HTTP/1.0");
	strcat(buffer,"\015\012\000");	// CRLF
	strcat(buffer,"User-Agent:CustomClient/1.0");
	strcat(buffer,"\015\012\000");	// CRLF
	strcat(buffer,"\015\012\000");	// CRFL

	result=HTTP_WAITING;

	int bytes_sent=send(HTTPsocket,buffer,strlen(buffer),0);
	if (bytes_sent<0) {
		closesocket(HTTPsocket);
		result=HTTP_ERROR;
		strcpy(buffer,"Could not send URL request");
		return;
		}

	basetime=GetTickCount();
	RecvThread=CreateThread(NULL,NULL,RecvProc,this,0,&ThreadID);
	}

void HTTPClient::Cancel() {
	result=HTTP_CANCEL;
	TerminateThread(RecvThread,0);
	}

BOOL HTTPClient::Wait(int time) {
	if ((long)GetTickCount()>(basetime+time)) result=HTTP_TIMEOUT;
	if (result!=HTTP_WAITING) return TRUE;
	return FALSE;
	}
