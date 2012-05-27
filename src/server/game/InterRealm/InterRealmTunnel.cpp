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

InterRealmTunnel::InterRealmTunnel(): m_rand(0), m_tunnel_open(false)
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
			sleep(1);
			continue;
		}
		
		m_rand = urand(0,255);
		WorldPacket* packet = new WorldPacket(IR_CMSG_HELLO,10+1+1+1+1+1);
		*packet << std::string("HELO");
		*packet << (uint8)m_rand;
		*packet << (uint8)0;
		*packet << (uint8)0;
		*packet << (uint8)1;
		*packet << (uint8)0;
		SendPacket(packet);
		
		while(!World::IsStopped() && m_sock != INVALID_SOCKET && m_force_stop == false) {
			char buffer[10240] = "";
			int byteRecv = recv(m_sock, buffer, 10240, 0);
			if(byteRecv != SOCKET_ERROR && byteRecv != 0) {
				if(byteRecv > 0) {
					// Create packet
					WorldPacket* packet = new WorldPacket(buffer[0]+buffer[1]*256);

					for(int i=2;i<byteRecv;i++)
						*packet << (uint8)buffer[i];
					
					sLog->outDetail("Packet recv with opcode %u",packet->GetOpcode());
					
					// Handle Packet
					if(packet->GetOpcode() < IR_NUM_MSG_TYPES)
					{
						try
						{
							IROpcodeHandler &IRopHandle = IRopcodeTable[packet->GetOpcode()];
							(this->*IRopHandle.handler)(*packet);
						}
						catch(ByteBufferException &)
						{
							sLog->outError("[FATAL] InterRealmTunnel ByteBufferException occured while parsing a packet (opcode: %u) from client %s:%d Skipped packet.",
									packet->GetOpcode(), inet_ntoa(m_sin.sin_addr), htons(m_sin.sin_port),m_sock);
							if (sLog->IsOutDebug())
							{
								sLog->outDebug(LOG_FILTER_NETWORKIO, "Dumping error causing packet:");
								packet->hexlike();
							}
						}
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

void InterRealmTunnel::Handle_WhoIam(WorldPacket& packet)
{
	uint8 _valid;
	packet >> _valid;
	
	if(_valid == 0)
		m_tunnel_open = true;
	else
		m_force_stop = true;
}

void InterRealmTunnel::Handle_Hello(WorldPacket& packet)
{
	std::string _hello;
	uint8 _rand, _resp;
	
	packet >> _hello;
	packet >> _rand;
	packet >> _resp;
	
	if(strcmp(_hello.c_str(),"HELO") != 0)
	{
		sLog->outError("[ERROR] Non polite server, closing socket !");
		m_force_stop = true;
	}
	
	if(_rand != m_rand)
	{
		sLog->outError("[ERROR] Random hello check is incorrect, closing socket !");
		m_force_stop = true;
	}
	
	if(_resp == IR_HELO_RESP_PROTOCOL_MISMATCH)
		sLog->outError("[ERROR] InterRealm Protocol Mismatch, closing doors to me !");
	
	if(_resp == IR_HELO_RESP_POLITE)
		sLog->outError("[ERROR] Server like to be polite, closing doors to me !");
	
	if(!m_force_stop && _resp == IR_HELO_RESP_OK)
	{
		WorldPacket pck(IR_CMSG_WHOIAM,1+10+10);
		pck << (int)realmID;
		pck << "testuser";
		pck << "testpwd";
		SendPacket(&pck);
	}
}

void InterRealmTunnel::Handle_Unhandled(WorldPacket& recvPacket)
{
	sLog->outError("[WARN] Unhandled Packet with IROpcode %u received !",recvPacket.GetOpcode());
}

void InterRealmTunnel::Handle_Null(WorldPacket& recvPacket)
{
	sLog->outError("[WARN] Packet with Invalid IROpcode %u received !",recvPacket.GetOpcode());
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
	
	send(m_sock,buffer,packet->size()+2,0);
	free(buffer);
}

