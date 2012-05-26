#include "InterRealmSocketHandler.h"
#include "World.h"

InterRealmSocket::InterRealmSocket()
{
	this->m_clients.clear();
	
	this->m_err = false;
	int sock_err = 0;

    this->ir_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(this->ir_sock == INVALID_SOCKET)
    {
		this->m_err = true;
		return;
	}

	this->ir_sin.sin_addr.s_addr    = htonl(INADDR_ANY);
	this->ir_sin.sin_family         = AF_INET;
	this->ir_sin.sin_port           = htons(INTERREALM_PORT);
	
	sock_err = bind(this->ir_sock, (SOCKADDR*)&(this->ir_sin), sizeof(this->ir_sin));
	if(sock_err == SOCKET_ERROR)
	{
		this->m_err = true;
		return;
	}
		
	sock_err = listen(this->ir_sock, 15);
	if(sock_err == SOCKET_ERROR)
	{
		this->m_err = true;
		return;
	}
}

InterRealmSocket::~InterRealmSocket()
{
	if(this->m_err)
		close(this->ir_sock);
}

void InterRealmSocket::run()
{
	if(this->m_err)
	{
		sLog->outError("Unable to listen InterRealm, must stop !!");
		return;
	}
	
	sLog->outString("InterRealm is now Listening on port %d", INTERREALM_PORT);
	
	while(!World::IsStopped()) {
		SOCKET csock;
		SOCKADDR_IN csin;
		socklen_t recsize = sizeof(csin);
		csock = accept(this->ir_sock, (SOCKADDR*)&csin, &recsize);
		sLog->outString("Accepting Client from %s:%d (sock %d)", inet_ntoa(csin.sin_addr), htons(csin.sin_port),csock);
		this->createClient(csock, csin, recsize);
	}
}

void InterRealmSocket::createClient(SOCKET sock, SOCKADDR_IN sin, socklen_t rsize)
{
	InterRealmClient* client = new InterRealmClient(sock, sin, rsize);
	ACE_Based::Thread client_thread(client);
	this->m_clients.push_back(client);
}
