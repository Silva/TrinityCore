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

#ifndef __INTERREALM_OPCODES_H__
#define __INTERREALM_OPCODES_H__

#include "WorldPacket.h"
#include "InterRealmClient.h"

enum IROpcodes {
	IR_CMSG_HELLO					= 0x00,
	IR_SMSG_HELLO					= 0x01,
	IR_CMSG_TUNNEL_PACKET			= 0x02,
	IR_SMSG_TUNNEL_PACKET			= 0x03,
	IR_CMSG_WHOIAM					= 0x04,
	IR_SMSG_WHOIAM_RESP				= 0x05,
	IR_CMSG_REGISTER_PLAYER 		= 0x06,
	IR_SMSG_REGISTER_PLAYER_RESP 	= 0x07,
	
	IR_NUM_MSG_TYPES				= 0x08,
};

struct IROpcodeHandler
{
    char const* name;
    void (InterRealmClient::*handler)(WorldPacket& recvPacket);
};

extern IROpcodeHandler IRopcodeTable[IR_NUM_MSG_TYPES];

#endif
