#ifndef __INTERREALM_SOCKET__
#define __INTERREALM_SOCKET__

#include "InterRealm_defs.h"
#include "InterRealmClient.h"

#define INTERREALM_PORT		12541

typedef std::list<InterRealmClient*> mIRClients;

class InterRealmSocket: public ACE_Based::Runnable
{
	public:
		InterRealmSocket();
		~InterRealmSocket();
		void run();
	private:
		void createClient(SOCKET sock, SOCKADDR_IN sin, socklen_t rsize);
		bool m_err;
		SOCKET ir_sock;
		SOCKADDR_IN ir_sin;
		mIRClients m_clients;
};
#endif
