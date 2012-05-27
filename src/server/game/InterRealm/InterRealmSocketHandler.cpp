#include "InterRealmSocketHandler.h"
#include "World.h"

InterRealmSocket::InterRealmSocket()
{
	m_err = false;
	int sock_err = 0;

    ir_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(ir_sock == INVALID_SOCKET)
    {
		m_err = true;
		return;
	}

	ir_sin.sin_addr.s_addr    = htonl(INADDR_ANY);
	ir_sin.sin_family         = AF_INET;
	ir_sin.sin_port           = htons(INTERREALM_PORT);
	
	sock_err = bind(ir_sock, (SOCKADDR*)&(ir_sin), sizeof(ir_sin));
	if(sock_err == SOCKET_ERROR)
	{
		m_err = true;
		return;
	}
		
	sock_err = listen(ir_sock, 15);
	if(sock_err == SOCKET_ERROR)
	{
		m_err = true;
		return;
	}
}

InterRealmSocket::~InterRealmSocket()
{
	if(!m_err)
		close(ir_sock);
}

void InterRealmSocket::run()
{
	if(m_err)
	{
		sLog->outError("Unable to listen InterRealm, must stop !!");
		World::StopNow(10);
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

void InterRealmSocket::printClientList()
{
	mIRClients::iterator itclient;
	sLog->outString("#---Address---|---Port---|---SocketId---#");
	for(itclient = m_clients.begin(); itclient < m_clients.end(); ++itclient)
		(*itclient)->printInfos();
	sLog->outString("#---------------------------------------#");
}

void InterRealmSocket::createClient(SOCKET sock, SOCKADDR_IN sin, socklen_t rsize)
{
	InterRealmClient* client = new InterRealmClient(sock, sin, rsize, this);
	ACE_Based::Thread client_thread(client);
	m_clients.push_back(client);
}

void InterRealmSocket::deleteClient(InterRealmClient* client)
{
	//mIRClients::iterator itclient;
	//for(itclient = m_clients.begin(); itclient < m_clients.end() && (*itclient) != client; ++itclient);
	//m_clients.erase(itclient);
}
