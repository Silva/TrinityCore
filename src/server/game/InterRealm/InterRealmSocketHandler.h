#ifndef __INTERREALM_SOCKET__
#define __INTERREALM_SOCKET__

#include "InterRealm_defs.h"
#include "InterRealmClient.h"

#define INTERREALM_PORT				12541
#define IR_PROTOCOL_VERSION			1
#define IR_PROTOCOL_SUBVERSION		0
#define IR_PROTOCOL_COMPRESS		0
#define IR_PROTOCOL_ENCRYPT			0

typedef std::vector<InterRealmClient*> mIRClients;

class InterRealmSocket: public ACE_Based::Runnable
{
	public:
		InterRealmSocket();
		~InterRealmSocket();
		void run();
		void deleteClient(InterRealmClient* client);
		void printClientList();
	private:
		void createClient(SOCKET sock, SOCKADDR_IN sin, socklen_t rsize);
		bool m_err;
		SOCKET ir_sock;
		SOCKADDR_IN ir_sin;
		mIRClients m_clients;
};
#endif
