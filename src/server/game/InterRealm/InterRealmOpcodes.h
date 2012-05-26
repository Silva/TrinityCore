#ifndef __INTERREALM_OPCODES_H__
#define __INTERREALM_OPCODES_H__

#include "WorldPacket.h"
#include "InterRealmClient.h"

enum IROpcodes {
	IR_CMSG_HELLO			= 0x00,
	IR_SMSG_HELLO			= 0x01,
	IR_CMSG_TUNNEL_PACKET	= 0x02,
	IR_SMSG_TUNNEL_PACKET	= 0x03,
	IR_TEST					= 0x04,
	
	IR_NUM_MSG_TYPES		= 0x05,
};

struct IROpcodeHandler
{
    char const* name;
    void (InterRealmClient::*handler)(WorldPacket& recvPacket);
};

extern IROpcodeHandler IRopcodeTable[IR_NUM_MSG_TYPES];



#endif
