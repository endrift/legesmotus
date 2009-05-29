/*
 * metaserver/MetaServer.hpp
 *
 * This file is part of Leges Motus, a networked, 2D shooter set in zero gravity.
 * 
 * Copyright 2009 Andrew Ayer, Nathan Partlan, Jeffrey Pfau
 * 
 * Leges Motus is free and open source software.  You may redistribute it and/or
 * modify it under the terms of version 2, or (at your option) version 3, of the
 * GNU General Public License (GPL), as published by the Free Software Foundation.
 * 
 * Leges Motus is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the full text of the GNU General Public License for
 * further detail.
 * 
 * For a full copy of the GNU General Public License, please see the COPYING file
 * in the root of the source code tree.  You may also retrieve a copy from
 * <http://www.gnu.org/licenses/gpl-2.0.txt>, or request a copy by writing to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307  USA
 * 
 */

#ifndef LM_METASERVER_METASERVER_HPP
#define LM_METASERVER_METASERVER_HPP

#include "common/UDPSocket.hpp"
#include "common/IPAddress.hpp"
#include <stdint.h>
#include <map>
#include <list>

class UDPPacket;
class PacketReader;
class PacketWriter;

class MetaServer {
private:
	class ServerInfo;
	typedef std::list<ServerInfo>		ServerList;
	typedef std::map<uint32_t, ServerInfo*>	ServerMap;

	class ServerInfo {
		static uint32_t		next_server_id;

		uint32_t		m_server_id;
		uint32_t		m_token;
		IPAddress		m_address;
		uint64_t		m_last_seen_time;
		ServerList::iterator	m_list_position;

	public:
		ServerInfo();
		void			init(const IPAddress& address, ServerList::iterator list_position);
		uint32_t		get_id() const { return m_server_id; }
		uint32_t		get_token() const { return m_token; }
		IPAddress		get_address() const { return m_address; }
		void			seen(const IPAddress& address, ServerList& server_list);
		uint64_t		get_last_seen_time() const { return m_last_seen_time; }
		ServerList::iterator	get_list_position() const { return m_list_position; }
	};

	uint32_t			m_contact_frequency;	// Servers should contact the meta server this often (in milliseconds)
	uint32_t			m_timeout_time;		// Number of milliseconds until an unseen server is removed
	UDPSocket			m_socket;		// Socket that we're listening on
	ServerList			m_servers;		// List of severs, with the most recently seen servers at the FRONT
	ServerMap 			m_servers_by_id;	// A map of servers from id->server

	void				timeout_servers();	// Timeout old servers

	void				process_packet(const UDPPacket& packet);
	void				request_info(const IPAddress& address, PacketReader& packet);
	void				register_server(const IPAddress& address, PacketReader& packet);
	void				unregister_server(const IPAddress& address, PacketReader& packet);

	void				send_packet(const PacketWriter& packet, const IPAddress& address);

	ServerInfo*			get_server(uint32_t server_id, uint32_t token);

public:
	MetaServer(uint32_t contact_frequency, uint32_t timeout_time);

	bool				start(uint16_t portno);
	void				run();
};

#endif
