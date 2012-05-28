/*
	* Copyright (C) 2007-2012 Frost Sapphire Studios <http://www.frostsapphirestudios.com/>
	*
	* This program is free software; you can redistribute it and/or modify
	* it under the terms of the GNU General Public License as published by
	* the Free Software Foundation; either version 2 of the License, or
	* (at your option) any later version.
	*
	* This program is distributed in the hope that it will be useful,
	* but WITHOUT ANY WARRANTY; without even the implied warranty of
	* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	* GNU General Public License for more details.
	*
	* You should have received a copy of the GNU General Public License
	* along with this program; if not, write to the Free Software
	* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#ifndef __INTERREALM_SOCKET__
#define __INTERREALM_SOCKET__

#include "InterRealm_defs.h"
#include "InterRealmClient.h"

#define INTERREALM_PORT				12541
#define IR_PROTOCOL_VERSION			1
#define IR_PROTOCOL_SUBVERSION		0
#define IR_PROTOCOL_COMPRESS		0
#define IR_PROTOCOL_ENCRYPT			0

class InterRealmSocket: public ACE_Based::Runnable
{
	public:
		InterRealmSocket();
		~InterRealmSocket();
		void run();
		void deleteClient(InterRealmClient* client);
	private:
		void createClient(SOCKET sock, SOCKADDR_IN sin, socklen_t rsize);
		bool m_err;
		SOCKET ir_sock;
		SOCKADDR_IN ir_sin;
};
#endif
