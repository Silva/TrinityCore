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

#include "InterRealmClient.h"
#include "InterRealmOpcodes.h"
#include "InterRealmSocketHandler.h"
#include "World.h"

InterRealmClient::InterRealmClient(SOCKET sock, SOCKADDR_IN sin, socklen_t rsize, InterRealmSocket* ssock)
{
	csock = sock;
	csin = sin;
	recsize = rsize;
	ssock = ssock;
	m_force_close = false;
}

InterRealmClient::~InterRealmClient()
{
}

void InterRealmClient::run()
{
	while(!World::IsStopped() && csock != INVALID_SOCKET && m_force_close == false) {
		char buffer[10240] = "";
		int byteRecv = recv(csock, buffer, 10240, 0);
		sLog->outString("byteRecv %d",byteRecv);
		if(byteRecv != SOCKET_ERROR && byteRecv != 0) {
			if(byteRecv > 1) {
				// Create packet
				WorldPacket* packet = new WorldPacket(buffer[0]+buffer[1]*256);

				for(int i=2;i<byteRecv;i++)
					*packet << (uint8)buffer[i];
				
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
			else
				sLog->outError("Invalid Packet with too short size recv from %s:%d (sock %d)",inet_ntoa(csin.sin_addr), htons(csin.sin_port),csock);
		}
		else
			m_force_close = true;
	}
	sLog->outString("Closing connection with %s:%d (sock %d)",inet_ntoa(csin.sin_addr), htons(csin.sin_port),csock);
	if(csock != INVALID_SOCKET)
		close(csock);
	ssock->deleteClient(this);
}

void InterRealmClient::Handle_Hello(WorldPacket& packet)
{
	std::string hello;
	uint8 _rand,_compress,protocolVer,protocolSubVer;
	
	packet >> hello;
	packet >> _rand;
	packet >> _compress;
	packet >> protocolVer;
	packet >> protocolSubVer;
	
	sLog->outString("Hello received from %s:%d (compress %u) Protocol version %u.%u",inet_ntoa(csin.sin_addr), htons(csin.sin_port),
		_compress,protocolVer,protocolSubVer);
}

void InterRealmClient::printInfos()
{
	sLog->outString("|-%s-|---%d---|---%d---|",inet_ntoa(csin.sin_addr), htons(csin.sin_port),csock);
}

void InterRealmClient::Handle_Unhandled(WorldPacket& recvPacket)
{
	sLog->outError("[WARN] Unhandled Packet with IROpcode %u received !",recvPacket.GetOpcode());
}

void InterRealmClient::Handle_Null(WorldPacket& recvPacket)
{
	sLog->outError("[WARN] Packet with Invalid IROpcode %u received !",recvPacket.GetOpcode());
}
