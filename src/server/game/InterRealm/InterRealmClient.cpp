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
#include "InterRealmMgr.h"
#include "PlayerDump.h"
#include "World.h"

InterRealmClient::InterRealmClient(SOCKET sock, SOCKADDR_IN sin, socklen_t rsize): m_realRealmId(0), m_realmId(0),
m_tunnel_allowed(false), m_force_close(false)
{
	csock = sock;
	csin = sin;
	recsize = rsize;
}

InterRealmClient::~InterRealmClient()
{
}

void InterRealmClient::run()
{
	m_realmId = sIRMgr->RegisterClient(this);
	while(!World::IsStopped() && csock != INVALID_SOCKET && m_force_close == false) {
		char buffer[10240] = "";
		int byteRecv = recv(csock, buffer, 10240, 0);
		if(byteRecv != SOCKET_ERROR)
			handlePacket(buffer,byteRecv);
		else
			m_force_close = true;
	}
	sLog->outDetail("Closing connection with %s:%d (sock %d)",inet_ntoa(csin.sin_addr), htons(csin.sin_port),csock);
	if(csock != INVALID_SOCKET)
		close(csock);
	sIRMgr->RemoveClient(m_realmId);
}

void InterRealmClient::Handle_RegisterPlayer(WorldPacket& recvPacket)
{
	uint32 guidlow, accountid, xp, money, bytes1, bytes2, flags, instanceId;
	uint16 mapId;
	uint8 race,_class,gender,level;
	float posX, posY, posZ, posO;
	std::string _name;
	recvPacket >> guidlow;
	recvPacket >> accountid;
	recvPacket >> _name;
	recvPacket >> race;
	recvPacket >> _class;
	recvPacket >> gender;
	recvPacket >> level;
	recvPacket >> xp;
	recvPacket >> money;
	recvPacket >> bytes1;
	recvPacket >> bytes2;
	recvPacket >> flags;
	recvPacket >> mapId;
	recvPacket >> instanceId;
	recvPacket >> posX;
	recvPacket >> posY;
	recvPacket >> posZ;
	recvPacket >> posO;
	
	WorldSession* _sess;
	Player* _player;
	
	// Check if map exists
	const MapEntry* mapEntry = sMapStore.LookupEntry(mapId);
	if(!mapEntry)
		return;
		
	IRPlayerSessions::iterator itr = m_sessions.find(guidlow);
	if(itr == m_sessions.end())
	{
		_sess = new WorldSession(0,this,SEC_PLAYER,2,0,LocaleConstant(0),0,false);
		_player = new Player(_sess);
		((Object*)_player)->_Create(guidlow, 0, HIGHGUID_PLAYER);
		_player->SetUInt64Value(OBJECT_FIELD_GUID, MAKE_NEW_GUID(guidlow, 0, HIGHGUID_PLAYER));
		_sess->SetPlayer(_player);
		_player->SetRealGUID(guidlow);
	}
	else
	{
		_sess = itr->second;
		_player = _sess->GetPlayer();
		if(!_sess->GetPlayer());
			return;
	}
	
	_player->SetUInt64Value(0,guidlow);
	//_player->Setaccount ??
	_player->SetName(_name);
	_player->SetByteValue(UNIT_FIELD_BYTES_0, 0,race);
	_player->SetByteValue(UNIT_FIELD_BYTES_0, 1,_class);
	_player->SetByteValue(UNIT_FIELD_BYTES_0, 2,gender);
	_player->SetLevel(level);
	_player->SetUInt32Value(PLAYER_XP,xp);
	_player->SetMoney(money);
	_player->SetUInt32Value(PLAYER_BYTES,bytes1);
	_player->SetUInt32Value(PLAYER_BYTES_2,bytes2);
	_player->SetUInt32Value(PLAYER_FLAGS,flags);

	uint16 kills1,kills2;
	uint32 deathExpireTimer, arenaPoints, HonorPoints, todayContrib, yesterdayContrib, lifeTimeHonorKills, chosenTitle;
	uint32 stableSlots, watchedFactionIdx, health,ExtraFlags,atLoginFlags,zoneId,bytes3;
	uint64 KnownCurrencies;
	
	recvPacket >> ExtraFlags;
	recvPacket >> stableSlots;
	recvPacket >> atLoginFlags;
	recvPacket >> zoneId;
	recvPacket >> deathExpireTimer;
	recvPacket >> arenaPoints;
	recvPacket >> HonorPoints;
	recvPacket >> todayContrib;
	recvPacket >> yesterdayContrib;
	recvPacket >> lifeTimeHonorKills;
	recvPacket >> kills1;
	recvPacket >> kills2;
	recvPacket >> chosenTitle;
	recvPacket >> KnownCurrencies;
	recvPacket >> watchedFactionIdx;
	recvPacket >> bytes3;
	recvPacket >> health;
	
	_player->SetExtraFlags(ExtraFlags);
	_player->SetStableSlots(stableSlots);
	_player->SetAtLoginFlags(atLoginFlags);
	
	_player->SetDeathExpireTimer(deathExpireTimer);
	
	_player->SetUInt32Value(PLAYER_FIELD_ARENA_CURRENCY,arenaPoints);
	_player->SetUInt32Value(PLAYER_FIELD_HONOR_CURRENCY,HonorPoints);
	_player->SetUInt32Value(PLAYER_FIELD_TODAY_CONTRIBUTION,todayContrib);
	_player->SetUInt32Value(PLAYER_FIELD_YESTERDAY_CONTRIBUTION,yesterdayContrib);
	_player->SetUInt32Value(PLAYER_FIELD_LIFETIME_HONORABLE_KILLS,lifeTimeHonorKills);
	_player->SetUInt16Value(PLAYER_FIELD_KILLS, 0, kills1);
	_player->SetUInt16Value(PLAYER_FIELD_KILLS, 1, kills2);
	_player->SetUInt32Value(PLAYER_CHOSEN_TITLE, chosenTitle);
	_player->SetUInt64Value(PLAYER_FIELD_KNOWN_CURRENCIES, KnownCurrencies);
	_player->SetUInt32Value(PLAYER_FIELD_WATCHED_FACTION_INDEX,watchedFactionIdx);
	_player->SetUInt32Value(PLAYER_BYTES_3,bytes3);
	_player->SetHealth(health);
	
	Map* map = sMapMgr->CreateMap(mapId, _player);
	if(map)
	{
		_player->GetMotionMaster()->InitDefault();
		_player->SetMap(map);
		map->AddPlayerToMap(_player);
		_player->TeleportTo(mapId, posX, posY, posZ, posO);
	}
	
	for (uint32 i = 0; i < MAX_POWERS; ++i)
	{
		uint32 powerVal;
		recvPacket >> powerVal;
		_player->SetPower(Powers(i),powerVal);
	}

	uint8 specCount, activeSpec;
	recvPacket >> specCount;
	recvPacket >> activeSpec;
	
	_player->SetSpecsCount(specCount);
	_player->SetActiveSpec(activeSpec);

	for (uint32 i = 0; i < EQUIPMENT_SLOT_END * 2; ++i)
	{
		uint32 tmpItem;
		recvPacket >> tmpItem;
		_player->SetUInt32Value(PLAYER_VISIBLE_ITEM_1_ENTRYID + i,tmpItem);
	}

	// ...and bags for enum opcode
	/*for (uint32 i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END; ++i)
	{
		if (Item* item = GetItemByPos(INVENTORY_SLOT_BAG_0, i))
			recvPacket >> uint32(item->GetEntry());
		else
			recvPacket >> uint32(0);
	}

	recvPacket >> uint32(GetUInt32Value(PLAYER_AMMO_ID));

	recvPacket >> uint8(GetByteValue(PLAYER_FIELD_BYTES, 2));
	recvPacket >> uint32(m_grantableLevels);

	// BG Data
	recvPacket >> uint32(m_bgData.bgInstanceID);
    recvPacket >> uint16(m_bgData.bgTeam);
    recvPacket >> float(m_bgData.joinPos.GetPositionX());
    recvPacket >> float(m_bgData.joinPos.GetPositionY());
    recvPacket >> float(m_bgData.joinPos.GetPositionZ());
    recvPacket >> float(m_bgData.joinPos.GetOrientation());
    recvPacket >> uint16(m_bgData.joinPos.GetMapId());
    recvPacket >> uint16(m_bgData.taxiPath[0]);
    recvPacket >> uint16(m_bgData.taxiPath[1]);
    recvPacket >> uint16(m_bgData.mountSpell);

	// Talents
    for (uint8 i = 0; i < MAX_TALENT_SPECS; ++i)
    {
		recvPacket >> uint32(m_talents[i].size());
        for (PlayerTalentMap::iterator itr = m_talents[i]->begin(); itr != m_talents[i]->end();)
        {
                recvPacket >> uint32(itr->first);
                recvPacket >> uint8(itr->second->spec);
        }
     }
     
     recvPacket >> uint32(m_spells.size());
     for (PlayerSpellMap::iterator itr = m_spells.begin(); itr != m_spells.end();)
    {
            recvPacket >> uint32(itr->first);
            recvPacket >> uint8(itr->second->active); //bool
            recvPacket >> uint8(itr->second->disabled); //bool
    }

	recvPacket >> uint32(m_spellCooldowns.size());
    for (SpellCooldowns::iterator itr = m_spellCooldowns.begin(); itr != m_spellCooldowns.end();)
    {
            recvPacket >> itr->first; // ?what
            recvPacket >> uint32(itr->second.itemid);
            recvPacket >> uint64(itr->second.end);
    }
    
    recvPacket >> uint32(m_ownedAuras.size());
    for (AuraMap::const_iterator itr = m_ownedAuras.begin(); itr != m_ownedAuras.end(); ++itr)
    {
        Aura* aura = itr->second;

        int32 damage[MAX_SPELL_EFFECTS];
        int32 baseDamage[MAX_SPELL_EFFECTS];
        uint8 effMask = 0;
        uint8 recalculateMask = 0;
        for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
        {
            if (AuraEffect const* effect = aura->GetEffect(i))
            {
                baseDamage[i] = effect->GetBaseAmount();
                damage[i] = effect->GetAmount();
                effMask |= 1 << i;
                if (effect->CanBeRecalculated())
                    recalculateMask |= 1 << i;
            }
            else
            {
                baseDamage[i] = 0;
                damage[i] = 0;
            }
        }

        recvPacket >> uint64(itr->second->GetCasterGUID());
        recvPacket >> uint64(itr->second->GetCastItemGUID());
        recvPacket >> uint32(itr->second->GetId());
        recvPacket >> uint8(effMask);
        recvPacket >> uint8(recalculateMask);
        recvPacket >> uint8(itr->second->GetStackAmount());
        recvPacket >> uint32(damage[0]);
        recvPacket >> uint32(damage[1]);
        recvPacket >> uint32(damage[2]);
        recvPacket >> uint32(baseDamage[0]);
        recvPacket >> uint32(baseDamage[1]);
        recvPacket >> uint32(baseDamage[2]);
        recvPacket >> uint32(itr->second->GetMaxDuration());
        recvPacket >> uint32(itr->second->GetDuration());
        recvPacket >> uint8(itr->second->GetCharges());
    }*/
    if(itr == m_sessions.end())
    {
		sWorld->AddSession(_sess);
		m_sessions[guidlow] = _sess;
		
	}
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

	WorldPacket* tunPacket = new WorldPacket(opcodeId,recvPacket.size()-(8+2));
	
	for(int i=10;i<recvPacket.size();i++)
	{
		uint8 rawData;
		recvPacket >> rawData;
		*tunPacket << uint8(rawData);
	}
	
	IRPlayerSessions::iterator it = m_sessions.find(playerGuid);
	if(it == m_sessions.end())
	{
		sLog->outError("Unregistered player want to use tunnel !",opcodeId);
		return;
	}
	sLog->outDetail("Tunneled Packet received (opcode %x)",opcodeId);
	
	if(it->second)
		it->second->QueuePacket(tunPacket);
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

void InterRealmClient::SendTunneledPacket(uint64 playerGuid, WorldPacket const* packet)
{
	if(playerGuid == 0 || packet->GetOpcode() > NUM_MSG_TYPES)
		return;

	// If it's a local packet, or don't need to forward it (matching conditions), don't forwar
	if(opcodeTable[packet->GetOpcode()].irPacketProcessing == PROCESS_LOCAL /*|| 
	(opcodeTable[packet->GetOpcode()].irPacketProcessing == PROCESS_FORWARD_IF_NEED && cond)*/)
	{
		sLog->outError("Drop Packet opcode %x (%s)",packet->GetOpcode(),opcodeTable[packet->GetOpcode()].name);
		return;
	}
	
	WorldPacket tmpPacket(IR_SMSG_TUNNEL_PACKET,8+2+packet->size());
	tmpPacket << (uint64)playerGuid;
	tmpPacket << (uint16)packet->GetOpcode();

	for(int i=0;i<packet->size();i++)
		tmpPacket << (uint8)packet->contents()[i];
	
	SendPacket(&tmpPacket);
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
	if(byteRecv == 0)
		return;
		
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

void InterRealmClient::RegisterPlayerSession(uint64 guid, WorldSession* sess)
{
	if(!sess)
		return;
		
	if(m_sessions.find(guid) == m_sessions.end())
	{
		m_sessions[guid] = sess;
		if(sess->GetPlayer())
			sess->GetPlayer()->SetRealGUID(guid);
	}
}

void InterRealmClient::RemovePlayerSession(uint64 guid)
{
	if(m_sessions.find(guid) != m_sessions.end())
		m_sessions.erase(guid);
}
