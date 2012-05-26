#include "InterRealmClient.h"
#include "World.h"

InterRealmClient::InterRealmClient(SOCKET sock, SOCKADDR_IN sin, socklen_t rsize)
{
	this->csock = sock;
	this->csin = sin;
	this->recsize = rsize;
	this->m_force_close = false;
}

InterRealmClient::~InterRealmClient()
{
}

void InterRealmClient::run()
{
	while(!World::IsStopped() && this->csock != INVALID_SOCKET && this->m_force_close == false) {
		char buffer[10240] = "";
		int byteRecv = recv(this->csock, buffer, 10240, 0);
		if(byteRecv != SOCKET_ERROR && byteRecv != 0) {
			if(strlen(buffer) > 0 ) {
				WorldPacket* packet = new WorldPacket();
				packet->Initialize(buffer[0],strlen(buffer)-1);
				for(int i=i;i<strlen(buffer);i++)
					packet->append(buffer[i]);
				packet->print_storage();
				sLog->outString("\nbuffer: %s",buffer);
				
				if(packet != NULL)
					delete packet;
			}
		}
		else
			this->m_force_close = true;
	}
	sLog->outString("Closing connection with %s:%d (sock %d)",inet_ntoa(this->csin.sin_addr), htons(this->csin.sin_port),this->csock);
	if(this->csock != INVALID_SOCKET)
		close(this->csock);
}

void InterRealmClient::Handle_Null(WorldPacket& recvPacket)
{
	sLog->outDebug("Handle_Null");
}
