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

#ifndef __INTERREALM_TUNNEL_H__
#define __INTERREALM_TUNNEL_H__

#include "InterRealm_defs.h"

#define INTERREALM_PORT		12541

enum irHeloResp
{
	IR_HELO_RESP_OK						= 0,
	IR_HELO_RESP_POLITE					= 1,
	IR_HELO_RESP_PROTOCOL_MISMATCH		= 2,
};

class InterRealmTunnel: public ACE_Based::Runnable
{
	public:
		InterRealmTunnel();
		~InterRealmTunnel();
		
		// Thread
		void run();
		
		/*
		 *  Handlers
		 */
		 void Handle_TunneledPacket(WorldPacket& recvPacket);
		 
		 void Handle_Hello(WorldPacket& packet);
		 void Handle_WhoIam(WorldPacket& packet);
		 
		 void Handle_Unhandled(WorldPacket& recvPacket);
		 void Handle_Null(WorldPacket& recvPacket);
		 void Handle_ClientSide(WorldPacket& recvPacket) { }
		 void SendTunneledPacket(uint64 guid, WorldPacket const* packet);
		 void SendPacket(WorldPacket const* packet);
	private:
		
		bool m_tunnel_open;
		// Socket, UDP maybe ?
		SOCKET m_sock;
		SOCKADDR_IN m_sin;
		socklen_t recsize;
		bool m_force_stop;
		uint8 m_rand;
};

#endif
