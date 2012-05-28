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

#ifndef __INTERREALM_MGR_H__
#define __INTERREALM_MGR_H__

#include <ace/Singleton.h>
#include "InterRealmClient.h"

typedef std::map<uint16,InterRealmClient*> IRClientMap;

class InterRealmMgr
{
	public:
		InterRealmMgr();
		~InterRealmMgr();
		
		uint16 RegisterClient(InterRealmClient* irc);
		void RemoveClient(uint16 realmid);
		//void GetRealmId(InterRealmClient* irc);
	private:
		uint16 m_realmid_offset;
		IRClientMap m_clients;
};

#define sIRMgr ACE_Singleton<InterRealmMgr, ACE_Null_Mutex>::instance()

#endif
