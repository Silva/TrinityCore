#include "InterRealmClient.h"
#include "InterRealmOpcodes.h"
#include "InterRealmSocketHandler.h"
#include "World.h"

InterRealmClient::InterRealmClient(SOCKET sock, SOCKADDR_IN sin, socklen_t rsize, InterRealmSocket* ssock)
{
	this->csock = sock;
	this->csin = sin;
	this->recsize = rsize;
	this->ssock = ssock;
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
				// Create packet
				WorldPacket* packet = new WorldPacket(buffer[0]);

				for(int i=1;i<strlen(buffer);i++)
					packet->append((uint8)buffer[i]);
				
				// Handle Packet
				if(packet->GetOpcode() < IR_NUM_MSG_TYPES)
				{
					IROpcodeHandler &IRopHandle = IRopcodeTable[packet->GetOpcode()];
					(this->*IRopHandle.handler)(*packet);
					
				}
				else
					this->Handle_Unhandled(*packet);
					
				// Delete Packet from memory
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
	ssock->deleteClient(this);
}

void InterRealmClient::printInfos()
{
	sLog->outString("|-%s-|---%d---|---%d---|",inet_ntoa(this->csin.sin_addr), htons(this->csin.sin_port),this->csock);
}

void InterRealmClient::Handle_Unhandled(WorldPacket& recvPacket)
{
	sLog->outError("[WARN] Packet with Invalid IROpcode %u received !",recvPacket.GetOpcode());
}

void InterRealmClient::Handle_Null(WorldPacket& recvPacket)
{
	sLog->outDetail("Handle_Null");
	//recvPacket->hexlike();
	sLog->outString("packet Opcode %u Content %s",recvPacket.GetOpcode(),recvPacket.contents());
}
