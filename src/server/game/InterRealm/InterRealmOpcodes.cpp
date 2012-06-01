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

#include "InterRealmOpcodes.h"

IROpcodeHandler IRopcodeTable[IR_NUM_MSG_TYPES] = {
	{"IR_CMSG_HELLO",					&InterRealmClient::Handle_Hello				},
	{"IR_SMSG_HELLO",					&InterRealmClient::Handle_ServerSide		},
	{"IR_CMSG_TUNNEL_PACKET",			&InterRealmClient::Handle_TunneledPacket	},
	{"IR_SMSG_TUNNEL_PACKET",			&InterRealmClient::Handle_ServerSide		},
	{"IR_CMSG_WHOIAM",					&InterRealmClient::Handle_WhoIam			},
	{"IR_SMSG_WHOIAM_ACK",				&InterRealmClient::Handle_ServerSide		},
	{"IR_CMSG_REGISTER_PLAYER",			&InterRealmClient::Handle_RegisterPlayer	},
	{"IR_SMSG_REGISTER_PLAYER_RESP",	&InterRealmClient::Handle_ServerSide		},
	{"IR_CMSG_PLAYER_LOGOUT",			&InterRealmTunnel::Handle_LogoutPlayer		},
	{"IR_SMSG_PLAYER_LOGOUT_RESP",		&InterRealmTunnel::Handle_ServerSide		},
};
