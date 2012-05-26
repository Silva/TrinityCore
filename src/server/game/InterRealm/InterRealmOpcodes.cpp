#include "InterRealmOpcodes.h"

IROpcodeHandler IRopcodeTable[IR_NUM_MSG_TYPES] =
	{IR_CMSG_HELLO,				&InterRealmClient::Handle_Null	},
	{IR_SMSG_HELLO,				&InterRealmClient::Handle_Null	},
	{IR_CMSG_TUNNEL_PACKET,		&InterRealmClient::Handle_Null	},
	{IR_SMSG_TUNNEL_PACKET,		&InterRealmClient::Handle_Null	},
};
