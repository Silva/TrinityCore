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

InterRealmClient::InterRealmClient(SOCKET sock, SOCKADDR_IN sin, socklen_t rsize, InterRealmSocket* ssock): m_realRealmId(0), m_realmId(0),
m_tunnel_allowed(false), m_force_close(false)
{
	csock = sock;
	csin = sin;
	recsize = rsize;
	ssock = ssock;
}

InterRealmClient::~InterRealmClient()
{
}

void InterRealmClient::run()
{
	while(!World::IsStopped() && csock != INVALID_SOCKET && m_force_close == false) {
		char buffer[10240] = "";
		int byteRecv = recv(csock, buffer, 10240, 0);
		if(byteRecv != SOCKET_ERROR && byteRecv != 0)
			handlePacket(buffer,byteRecv);
		else
			m_force_close = true;
	}
	sLog->outDetail("Closing connection with %s:%d (sock %d)",inet_ntoa(csin.sin_addr), htons(csin.sin_port),csock);
	if(csock != INVALID_SOCKET)
		close(csock);
	ssock->deleteClient(this);
}

void InterRealmClient::Handle_RegisterPlayer(WorldPacket& recvPacket)
{
	
}

void InterRealmClient::Handle_WhoIam(WorldPacket &packet)
{
	int realmId;
	std::string realmuser, realmpwd;
	
	packet >> realmId; 
	packet >> realmuser; // Unhandled for now
	packet >> realmpwd; // Unhandled for now
	
	sLog->outDetail("Handle_WhoIam realmID %d user %s password %s",realmId,realmuser.c_str(),realmpwd.c_str());
	
	WorldPacket pck(IR_SMSG_WHOIAM_ACK,1);
	if(realmId < 1)
	{
		pck << (uint8)1;
		m_force_close = true;
	}
	else
	{
		pck << (uint8)0;
		m_realRealmId = realmId;
	}
	
	SendPacket(&pck);
}

void InterRealmClient::Handle_Hello(WorldPacket& packet)
{
	std::string hello;
	uint8 _rand,_compress,_encrypt,protocolVer,protocolSubVer;
	
	packet >> hello;
	packet >> _rand;
	packet >> _compress;
	packet >> _encrypt;
	packet >> protocolVer;
	packet >> protocolSubVer;
	
	bool non_polite = false, protocol_mismatch = false;

	if(strcmp(hello.c_str(),"HELO") != 0)
		non_polite = true;
		
	if(protocolVer > IR_PROTOCOL_VERSION || protocolSubVer > IR_PROTOCOL_SUBVERSION ||
	_compress > IR_PROTOCOL_COMPRESS || _encrypt > IR_PROTOCOL_ENCRYPT)
		protocol_mismatch = true;
	
	if(non_polite || protocol_mismatch)
	{
		m_force_close = true;
		if(non_polite)
			sLog->outError("Non-polite server on %s:%d, rejecting",inet_ntoa(csin.sin_addr), htons(csin.sin_port));
		if(protocol_mismatch)
			sLog->outError("Protocol mismatch on %s:%d, rejecting",inet_ntoa(csin.sin_addr), htons(csin.sin_port));
	}
	else
		sLog->outDetail("Hello received from %s:%d (compress %u) Protocol version %u.%u",inet_ntoa(csin.sin_addr), htons(csin.sin_port),
		_compress,protocolVer,protocolSubVer);
	
	WorldPacket pck(IR_SMSG_HELLO,10+1+1);
	pck << std::string("HELO"); // Polite
	pck << uint8(_rand); // Return same random number
	pck << uint8((non_polite ? IR_HELO_RESP_POLITE : (protocol_mismatch ? IR_HELO_RESP_PROTOCOL_MISMATCH : IR_HELO_RESP_OK)));
	
	SendPacket(&pck);
}

void InterRealmClient::Handle_TunneledPacket(WorldPacket& recvPacket)
{
	uint64 playerGuid;
	uint16 opcodeId;
	recvPacket >> playerGuid;
	recvPacket >> opcodeId;

	WorldPacket tunPacket(opcodeId,recvPacket.size()-(8+2));
	
	for(int i=10;i<recvPacket.size();i++)
	{
		uint8 rawData;
		recvPacket >> rawData;
		tunPacket << rawData;
	}
	
	sLog->outDetail("Tunneled Packet received (opcode %x)",opcodeId);
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

void InterRealmClient::SendPacket(WorldPacket const* packet)
{
	if(packet == NULL || csock == INVALID_SOCKET)
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
	
	send(csock,buffer,packet->size()+2,0);
	free(buffer);
}

void InterRealmClient::handlePacket(const char* buffer, int byteRecv)
{
	if(byteRecv > 1) {
		// Create packet
		WorldPacket* packet = new WorldPacket(buffer[0]+buffer[1]*256);

		for(int i=2;i<byteRecv;i++)
			*packet << (uint8)buffer[i];
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
				sLog->outError("[FATAL] InterRealmClient ByteBufferException occured while parsing a packet (opcode: %u) from InterRealm Server. Skipped packet.",
						packet->GetOpcode());
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
	else
		sLog->outError("Invalid Packet with too short size recv from %s:%d (sock %d)",inet_ntoa(csin.sin_addr), htons(csin.sin_port),csock);
}
