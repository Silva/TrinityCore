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

#include "InterRealmSocketHandler.h"
#include "InterRealmMgr.h"
#include "World.h"

InterRealmSocket::InterRealmSocket()
{
	m_err = false;
	int sock_err = 0;

    ir_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(ir_sock == INVALID_SOCKET)
    {
		m_err = true;
		return;
	}

	ir_sin.sin_addr.s_addr    = htonl(INADDR_ANY);
	ir_sin.sin_family         = AF_INET;
	ir_sin.sin_port           = htons(INTERREALM_PORT);
	
	sock_err = bind(ir_sock, (SOCKADDR*)&(ir_sin), sizeof(ir_sin));
	if(sock_err == SOCKET_ERROR)
	{
		m_err = true;
		return;
	}
		
	sock_err = listen(ir_sock, 15);
	if(sock_err == SOCKET_ERROR)
	{
		m_err = true;
		return;
	}
}

InterRealmSocket::~InterRealmSocket()
{
	if(!m_err)
		close(ir_sock);
}

void InterRealmSocket::run()
{
	if(m_err)
	{
		sLog->outError("Unable to listen InterRealm, must stop !!");
		World::StopNow(10);
		return;
	}
	
	sLog->outString("InterRealm is now Listening on port %d", INTERREALM_PORT);
	
	while(!World::IsStopped()) {
		SOCKET csock;
		SOCKADDR_IN csin;
		socklen_t recsize = sizeof(csin);
		csock = accept(this->ir_sock, (SOCKADDR*)&csin, &recsize);
		sLog->outString("Accepting Client from %s:%d (sock %d)", inet_ntoa(csin.sin_addr), htons(csin.sin_port),csock);
		createClient(csock, csin, recsize);
	}
}

void InterRealmSocket::createClient(SOCKET sock, SOCKADDR_IN sin, socklen_t rsize)
{
	InterRealmClient* client = new InterRealmClient(sock, sin, rsize);
	ACE_Based::Thread client_thread(client);
}
