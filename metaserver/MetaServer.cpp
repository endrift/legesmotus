/*
 * metaserver/MetaServer.cpp
 *
 * This file is part of Leges Motus, a networked, 2D shooter set in zero gravity.
 * 
 * Copyright 2009-2010 Andrew Ayer, Nathan Partlan, Jeffrey Pfau
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
#include "common/Version.hpp"
#include <stdlib.h>
#include <string>
#include <iostream>

using namespace LM;
using namespace std;



MetaServer::ServerInfo::ServerInfo() {
	m_token = 0;
	m_last_seen_time = 0;
}

void MetaServer::ServerInfo::init(const IPAddress& address, ServerList::iterator list_position) {
	m_address = address;
	m_token = rand() * rand();
	m_last_seen_time = get_ticks();
	m_list_position = list_position;
}

void MetaServer::ServerInfo::seen(ServerList& server_list) {
	m_last_seen_time = get_ticks();

	// Move to the front of the list
	server_list.splice(server_list.begin(), server_list, m_list_position);
	m_list_position = server_list.begin();
}

MetaServer::MetaServer(uint32_t contact_frequency, uint32_t timeout_time) : m_latest_server_version(LM_VERSION), m_latest_client_version(LM_VERSION) {
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
	case UPGRADE_AVAILABLE_PACKET:
		upgrade_available(packet.get_address(), reader);
		break;
	}
}

void	MetaServer::timeout_servers() {
	uint64_t	now = get_ticks();
	// Working from the back of the list, pop off any server which hasn't been seen for a while
	while (!m_servers.empty() && now - m_servers.back().get_last_seen_time() >= m_timeout_time) {
		cerr << "Timing out " << m_servers.back().get_address() << "..." << endl;
		m_servers_by_address.erase(m_servers.back().get_address());
		m_servers.pop_back();
	}
}

void	MetaServer::register_server(const IPAddress& remote_address, PacketReader& request_packet) {
	int		server_protocol_version;
	Version	server_version;
	IPAddress	server_address;
	request_packet >> server_protocol_version >> server_version >> server_address;

	if (server_version < m_latest_server_version) {
		PacketWriter	upgrade_packet(UPGRADE_AVAILABLE_PACKET);
		upgrade_packet << m_latest_server_version;
		send_packet(upgrade_packet, remote_address);
	}

	// If the server did not specify its own hostname or port, take the values from the address that it's connecting from...
	if (!server_address.host) {
		server_address.host = remote_address.host;
	}
	if (!server_address.port) {
		server_address.port = remote_address.port;
	}

	cerr << "Packet from " << remote_address << ": ";

	ServerInfo*	server = get_server(server_address);

	if (server) {
		server->seen(m_servers);
		cerr << "Re-Registering " << server->get_address() << "..." << endl;
	} else {
		m_servers.push_front(ServerInfo());
		server = &m_servers.front();
		server->init(server_address, m_servers.begin());
		m_servers_by_address[server->get_address()] = server;

		cerr << "Registering " << server->get_address() << "..." << endl;
	}

	PacketWriter	response_packet(REGISTER_SERVER_PACKET);
	response_packet << server->get_token() << m_contact_frequency;
	send_packet(response_packet, server->get_address());
}

void	MetaServer::unregister_server(const IPAddress& remote_address, PacketReader& request_packet) {
	IPAddress	server_address;
	uint32_t	token;
	request_packet >> server_address >> token;

	// If the server did not specify its own hostname or port, take the values from the address that it's connecting from...
	if (!server_address.host) {
		server_address.host = remote_address.host;
	}
	if (!server_address.port) {
		server_address.port = remote_address.port;
	}

	if (ServerInfo* server = get_server(server_address)) {
		if (server->get_token() == token) {
			cerr << "Unregistering " << server->get_address() << "..." << endl;
			m_servers_by_address.erase(server->get_address());
			m_servers.erase(server->get_list_position());
		}
	}
}

void	MetaServer::request_info(const IPAddress& address, PacketReader& request_packet) {
	int		client_protocol_version;
	uint32_t	scan_id;
	uint64_t	scan_start_time;
	Version		client_version;
	request_packet >> client_protocol_version >> scan_id >> scan_start_time >> client_version;

	if (client_version < m_latest_client_version) {
		PacketWriter	upgrade_packet(UPGRADE_AVAILABLE_PACKET);
		upgrade_packet << m_latest_client_version;
		send_packet(upgrade_packet, address);
	}

	for (ServerList::const_iterator it(m_servers.begin()); it != m_servers.end(); ++it) {
		PacketWriter	response_packet(INFO_PACKET);
		response_packet << scan_id << scan_start_time << it->get_address();
		send_packet(response_packet, address);
		send_hole_punch(it->get_address(), address, scan_id);
	}
}

void	MetaServer::upgrade_available(const IPAddress& address, PacketReader& request_packet) {
	Version		client_version;
	request_packet >> client_version;

	if (client_version < m_latest_client_version) {
		PacketWriter	upgrade_packet(UPGRADE_AVAILABLE_PACKET);
		upgrade_packet << m_latest_client_version;
		send_packet(upgrade_packet, address);
	}
}

void	MetaServer::send_packet(const PacketWriter& packet_data, const IPAddress& address) {
	UDPPacket	raw_packet(MAX_PACKET_LENGTH);
	raw_packet.set_address(address);
	raw_packet.fill(packet_data.get_header().make_string());
	raw_packet.append(packet_data.packet_data());
	m_socket.send(raw_packet);
}

MetaServer::ServerInfo*	MetaServer::get_server(const IPAddress& server_address) {
	ServerMap::iterator it(m_servers_by_address.find(server_address));
	return it != m_servers_by_address.end() ? it->second : NULL;
}

void	MetaServer::send_hole_punch(const IPAddress& server_address, const IPAddress& client_address, uint32_t scan_id) {
	PacketWriter	packet(HOLE_PUNCH_PACKET);
	packet << client_address << scan_id;
	send_packet(packet, server_address);
}

