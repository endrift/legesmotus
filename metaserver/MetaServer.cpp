/*
 * metaserver/MetaServer.cpp
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

#include "MetaServer.hpp"
#include "common/timer.hpp"
#include "common/network.hpp"
#include "common/UDPPacket.hpp"
#include "common/PacketReader.hpp"
#include "common/PacketWriter.hpp"
#include <stdlib.h>
#include <string>
#include <iostream>

using namespace std;


uint32_t	MetaServer::ServerInfo::next_server_id = 1L;


MetaServer::ServerInfo::ServerInfo() {
	m_server_id = 0;
	m_token = 0;
	m_last_seen_time = 0;
}

void MetaServer::ServerInfo::init(const IPAddress& address, ServerList::iterator list_position) {
	m_server_id = next_server_id++;
	m_token = rand() * rand();
	m_address = address;
	m_last_seen_time = get_ticks();
	m_list_position = list_position;
}

void MetaServer::ServerInfo::seen(const IPAddress& address, ServerList& server_list) {
	m_address = address;
	m_last_seen_time = get_ticks();

	// Move to the front of the list
	server_list.splice(server_list.begin(), server_list, m_list_position);
	m_list_position = server_list.begin();
}

MetaServer::MetaServer(uint32_t contact_frequency, uint32_t timeout_time) {
	m_contact_frequency = contact_frequency;
	m_timeout_time = timeout_time;
}

bool	MetaServer::start(uint16_t portno) {
	return m_socket.bind(portno);
}

void	MetaServer::run() {
	UDPPacket	raw_packet(MAX_PACKET_LENGTH);
	while (m_socket.recv(raw_packet)) {
		timeout_servers();
		process_packet(raw_packet);
	}
}

void	MetaServer::process_packet(const UDPPacket& packet) {
	PacketReader		reader(packet);

	switch (reader.packet_type()) {
	case INFO_PACKET:
		request_info(packet.get_address(), reader);
		break;
	case REGISTER_SERVER_PACKET:
		register_server(packet.get_address(), reader);
		break;
	case UNREGISTER_SERVER_PACKET:
		unregister_server(packet.get_address(), reader);
		break;
	}
}

void	MetaServer::timeout_servers() {
	uint64_t	now = get_ticks();
	// Working from the back of the list, pop off any server which hasn't been seenf for a while
	while (!m_servers.empty() && now - m_servers.back().get_last_seen_time() >= m_timeout_time) {
		cerr << "Timing out " << m_servers.back().get_address() << "..." << endl;
		m_servers_by_id.erase(m_servers.back().get_id());
		m_servers.pop_back();
	}
}

void	MetaServer::register_server(const IPAddress& address, PacketReader& request_packet) {
	int		server_protocol_version;
	string		server_version;
	uint16_t	portno;
	uint32_t	server_id;
	uint32_t	token;
	request_packet >> server_protocol_version >> server_version >> portno >> server_id >> token;

	IPAddress	listening_address(address.host, htons(portno));

	if (ServerInfo* server = get_server(server_id, token)) {
		server->seen(listening_address, m_servers);
		cerr << "Re-Registering " << server->get_address() << "..." << endl;
	} else {
		m_servers.push_front(ServerInfo());
		ServerInfo&	new_server = m_servers.front();
		new_server.init(listening_address, m_servers.begin());
		m_servers_by_id[new_server.get_id()] = &new_server;

		PacketWriter	response_packet(REGISTER_SERVER_PACKET);
		response_packet << new_server.get_id() << new_server.get_token() << m_contact_frequency;
		send_packet(response_packet, address);

		cerr << "Registering " << new_server.get_address() << "..." << endl;
	}
}

void	MetaServer::unregister_server(const IPAddress& address, PacketReader& request_packet) {
	uint32_t	server_id;
	uint32_t	token;
	request_packet >> server_id >> token;

	if (ServerInfo* server = get_server(server_id, token)) {
		cerr << "Unregistering " << server->get_address() << "..." << endl;
		m_servers.erase(server->get_list_position());
		m_servers_by_id.erase(server_id);
	}
}

void	MetaServer::request_info(const IPAddress& address, PacketReader& request_packet) {
	int		client_protocol_version;
	uint32_t	scan_id;
	uint64_t	scan_start_time;
	string		client_version;
	request_packet >> client_protocol_version >> scan_id >> scan_start_time >> client_version;

	for (ServerList::const_iterator it(m_servers.begin()); it != m_servers.end(); ++it) {
		PacketWriter	response_packet(INFO_PACKET);
		response_packet << scan_id << scan_start_time << it->get_address();
		send_packet(response_packet, address);
	}
}

void	MetaServer::send_packet(const PacketWriter& packet_data, const IPAddress& address) {
	UDPPacket	raw_packet(MAX_PACKET_LENGTH);
	raw_packet.set_address(address);
	raw_packet.fill(packet_data);
	m_socket.send(raw_packet);
}

MetaServer::ServerInfo*	MetaServer::get_server(uint32_t server_id, uint32_t token) {
	ServerMap::iterator it(m_servers_by_id.find(server_id));
	return it != m_servers_by_id.end() && it->second->get_token() == token ? it->second : NULL;
}
