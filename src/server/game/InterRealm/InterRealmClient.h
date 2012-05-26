#ifndef __INTERREALM_CLIENT_H__
#define __INTERREALM_CLIENT_H__

#include "InterRealm_defs.h"

class InterRealmClient: public ACE_Based::Runnable
{
	public:
		InterRealmClient(SOCKET sock, SOCKADDR_IN sin, socklen_t rsize);
		~InterRealmClient();
		
		// Thread
		void run();
		
		/*
		 *  Handlers
		 */
		 
		 void Handle_NULL(WorldPacket& recvPacket);
	private:
		
		char* serverAddr;
		uint16_t serverPort;
		uint16_t realServerId;
		uint32_t serverId;
		
		// Socket, UDP maybe ?
		SOCKET csock;
		SOCKADDR_IN csin;
		socklen_t recsize;
		bool m_force_close;
};
#endif
