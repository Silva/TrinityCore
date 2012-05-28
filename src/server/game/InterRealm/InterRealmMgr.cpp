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

#include "InterRealmMgr.h"

InterRealmMgr::InterRealmMgr()
{
	m_realmid_offset = 0;
}

InterRealmMgr::~InterRealmMgr()
{
}

uint16 InterRealmMgr::RegisterClient(InterRealmClient* irc)
{
	m_clients[m_realmid_offset] = irc;
	m_realmid_offset++; 
	return m_realmid_offset;
}

void InterRealmMgr::RemoveClient(uint16 realmId)
{
	IRClientMap::iterator it = m_clients.find(realmId);
	/*if(it != m_clients.end());
		m_clients.erase(it);*/
}
