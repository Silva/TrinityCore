#ifndef __INTERREALM_CLIENT_H__
#define __INTERREALM_CLIENT_H__

#include "InterRealm_defs.h"

class InterRealmSocket;

class InterRealmClient: public ACE_Based::Runnable
{
	public:
		InterRealmClient(SOCKET sock, SOCKADDR_IN sin, socklen_t rsize, InterRealmSocket* ssock);
		~InterRealmClient();
		
		// Thread
		void run();
		
		void printInfos();
		/*
		 *  Handlers
		 */
		 
		 void Handle_Unhandled(WorldPacket& recvPacket);
		 void Handle_Null(WorldPacket& recvPacket);
	private:
		
		char* serverAddr;
		uint16_t serverPort;
		uint16_t realServerId;
		uint32_t serverId;
		
		// Socket, UDP maybe ?
		SOCKET csock;
		SOCKADDR_IN csin;
		socklen_t recsize;
		InterRealmSocket* ssock;
		bool m_force_close;
};
#endif
