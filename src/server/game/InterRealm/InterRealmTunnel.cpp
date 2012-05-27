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

#include "InterRealmTunnel.h"
#include "InterRealm_defs.h"
#include "InterRealmOpcodes.h"
#include "World.h"

InterRealmTunnel::InterRealmTunnel()
{
}

InterRealmTunnel::~InterRealmTunnel()
{
	if(m_sock != INVALID_SOCKET)
		close(m_sock);
}

void InterRealmTunnel::run()
{
	while(!World::IsStopped())
	{
		m_sock = socket(AF_INET, SOCK_STREAM, 0);
		if(m_sock == INVALID_SOCKET)
		{
			sLog->outError("Unable to create InterRealm Socket, must stop !!");
			World::StopNow(10);
			return;
		}
		
		m_sin.sin_addr.s_addr = inet_addr("127.0.0.1");
		m_sin.sin_family = AF_INET;
		m_sin.sin_port = htons(INTERREALM_PORT);
		
		int m_err = connect(m_sock, (SOCKADDR*)&m_sin, sizeof(m_sin));
		if(m_err == SOCKET_ERROR)
		{
			sLog->outError("Unable to connect to InterRealm Server...");
			return;
		}
		
		WorldPacket* packet = new WorldPacket(IR_CMSG_HELLO);
		*packet << std::string("HELO");
		*packet << (uint8)urand(0,255);
		*packet << (uint8)0;
		*packet << (uint8)1;
		*packet << (uint8)0;
		SendPacket(packet);
		
		while(!World::IsStopped() && m_sock != INVALID_SOCKET && m_force_stop == false) {
			char buffer[10240] = "";
			int byteRecv = recv(m_sock, buffer, 10240, 0);
			if(byteRecv != SOCKET_ERROR && byteRecv != 0) {
				if(strlen(buffer) > 0 ) {
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
			}
			else
				m_force_stop = true;
		}
		sLog->outString("Connection Lost with %s:%d (sock %d)",inet_ntoa(m_sin.sin_addr), htons(m_sin.sin_port),m_sock);
		if(m_sock != INVALID_SOCKET)
			close(m_sock);
	}
	sLog->outString("Close InterRealm Thread",inet_ntoa(m_sin.sin_addr), htons(m_sin.sin_port),m_sock);
}

void InterRealmTunnel::SendPacket(WorldPacket const* packet)
{
	if(packet == NULL || m_sock == INVALID_SOCKET)
		return;
	
	char* buffer = (char*)malloc((packet->size()+2)*sizeof(char));
	bzero(buffer,packet->size()+2);
	if(buffer == NULL)
		return;

	uint8 u8low = packet->GetOpcode() & 0xFF;
	uint8 u8high = (packet->GetOpcode() >> 8) & 0xFF;

	buffer[0] = u8low;
	buffer[1] = u8high;

	for(int i=0;i<packet->size();i++)
		buffer[i+2] = packet->contents()[i];
	buffer[packet->size()+2] = '\0';
	
	send(m_sock,buffer,packet->size()+2,0);
	free(buffer);
	delete packet;
}

void InterRealmTunnel::Handle_Unhandled(WorldPacket& recvPacket)
{
	sLog->outError("[WARN] Unhandled Packet with IROpcode %u received !",recvPacket.GetOpcode());
}

void InterRealmTunnel::Handle_Null(WorldPacket& recvPacket)
{
	sLog->outError("[WARN] Packet with Invalid IROpcode %u received !",recvPacket.GetOpcode());
}

