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

// RealGUID there
typedef std::map<uint64,WorldSession*> IRPlayerSessions;

class InterRealmClient: public ACE_Based::Runnable
{
	public:
		InterRealmClient(SOCKET sock, SOCKADDR_IN sin, socklen_t rsize);
		~InterRealmClient();
		
		// Thread
		void run();
		
		void printInfos();
		/*
		 *  Handlers
		 */
		 // IR Player handling
		 void Handle_RegisterPlayer(WorldPacket& recvPacket);
		 void Handle_LogoutPlayer(WorldPacket& recvPacket);
		 
		 // IR Negociation
		 void Handle_Hello(WorldPacket& recvPacket);
		 void Handle_WhoIam(WorldPacket &packet);
		 // IR Tunnel
		 void Handle_TunneledPacket(WorldPacket& recvPacket);
		 // Useless
		 void Handle_Unhandled(WorldPacket& recvPacket);
		 void Handle_Null(WorldPacket& recvPacket);
		 void Handle_ServerSide(WorldPacket& recvPacket) { }
		 
		 // Packet
		 void SendPacket(WorldPacket const* packet);
		 void SendTunneledPacket(uint64 playerGuid, WorldPacket const* packet);
		 // WorldSessions
		 void ProcessWorldSessionPacket(WorldSession* _sess, WorldPacket* packet);
		 bool RemovePlayerSession(uint64 guid);
	private:
		void handlePacket(const char* buffer, int byteRecv);
		void RegisterPlayerSession(uint64 guid, WorldSession* sess);
		
		// Realmlist
		int m_realRealmId;
		int m_realmId;
		
		IRPlayerSessions m_sessions;
		
		// Socket, UDP maybe ?
		SOCKET csock;
		SOCKADDR_IN csin;
		socklen_t recsize;
		bool m_tunnel_allowed;
		bool m_force_close;
};
#endif
