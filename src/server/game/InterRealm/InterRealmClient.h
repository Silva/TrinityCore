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

#ifndef __INTERREALM_CLIENT_H__
#define __INTERREALM_CLIENT_H__

#include "InterRealm_defs.h"

enum irHeloResp
{
	IR_HELO_RESP_OK						= 0,
	IR_HELO_RESP_POLITE					= 1,
	IR_HELO_RESP_PROTOCOL_MISMATCH		= 2,
};

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
		 void Handle_Hello(WorldPacket& recvPacket);
		 void Handle_WhoIam(WorldPacket &packet);
		 
		 //
		 void Handle_Unhandled(WorldPacket& recvPacket);
		 void Handle_Null(WorldPacket& recvPacket);
		 void Handle_ServerSide(WorldPacket& recvPacket) { }
		 
		 // Packet
		 void SendPacket(WorldPacket const* packet);
	private:
	
		// Realmlist
		int m_realRealmId;
		int m_realmId;
		
		// Socket, UDP maybe ?
		SOCKET csock;
		SOCKADDR_IN csin;
		socklen_t recsize;
		InterRealmSocket* ssock;
		bool m_tunnel_allowed;
		bool m_force_close;
};
#endif
