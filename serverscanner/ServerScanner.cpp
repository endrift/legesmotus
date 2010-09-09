/*
 * serverscanner/ServerScanner.cpp
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

#include "ServerScanner.hpp"
#include "ServerScannerNetwork.hpp"
#include "OutputGenerator.hpp"
#include "JsonGenerator.hpp"
#include "ReadableGenerator.hpp"
#include "common/PacketReader.hpp"
#include "common/PacketWriter.hpp"
#include "common/network.hpp"
#include "common/misc.hpp"
#include "common/timer.hpp"

#include <vector>
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cstring>
#include <iostream>
#include <fstream>
#include <limits>
#include <sstream>
#include <map>

using namespace LM;
using namespace std;

ServerScanner::ServerScanner(const char* metaserver_address) : m_client_compat(COMPAT_VERSION), m_network(*this) {
	m_client_version = LM_VERSION;
	m_protocol_number = PROTOCOL_VERSION;
	
	bool	success = false;

	if (metaserver_address != NULL || (metaserver_address = getenv("LM_METASERVER")) != NULL) {
		// Address passed manually--override all else
		if (strchr(metaserver_address, ':') == NULL) {
			success = resolve_hostname(m_metaserver_address, metaserver_address, METASERVER_PORTNO);
		} else {
			success = resolve_hostname(m_metaserver_address, metaserver_address);
		}
	} else {
		metaserver_address = METASERVER_HOSTNAME;
		success = resolve_hostname(m_metaserver_address, METASERVER_HOSTNAME, METASERVER_PORTNO);
	}

	if (!success) {
		cerr << "Unable to resolve metaserver hostname, `" << metaserver_address << "'. Metaserver scanning disabled." << endl;
	}
}

void 	ServerScanner::scan(ostream* outfile, OutputType outtype, int to_scan) {
	m_start_ticks = get_ticks();
	m_output = outfile;

	srand(time(NULL));
	m_current_scan_id = rand();

	if (outtype == OUTPUT_HUMAN_READABLE) {
		(*m_output) << "Scanning for servers compatible with version " << m_client_version << "..." << endl;
	}

	if (to_scan & SCAN_METASERVER) {
		scan_metaserver();
	}

	if (to_scan & SCAN_LOCAL_NETWORK) {
		scan_local_network();
	}

	if (to_scan & SCAN_LOOPBACK) {
		scan_loopback();
	}

	if (to_scan & SCAN_UPGRADE) {
		check_for_upgrade();
	}

	// Receive packets from all servers
	// TODO customizable delay
	while (m_network.receive_packets(5000));
	output_results(outtype);
}

void	ServerScanner::server_info(const IPAddress& server_address, PacketReader& info_packet) {
	uint32_t	request_packet_id;
	uint64_t	scan_start_time;
	info_packet >> request_packet_id >> scan_start_time;

	if (request_packet_id != m_current_scan_id) {
		// From an old scan - ignore it
		return;
	}
	
	if (server_address == m_metaserver_address) {
		// A response from the meta server
		// Now send an info packet to the server specified in this packet, to measure ping time and get the most up-to-date information
		IPAddress	server_address;
		info_packet >> server_address;
		scan_server(server_address);
	} else {
		// A response from an actual server
		// Get the info on the server, and present it to the user
		int server_protocol_version;
		Version server_compat_version;
		ServerList::Server info;
		info_packet >> server_protocol_version >> server_compat_version;
		info_packet >> info.current_map_name >> info.team_count[0] >> info.team_count[1] >>
			info.max_players >> info.uptime >> info.time_left_in_game >>
			info.server_name >> info.server_location;
		
		info.ping = get_ticks() - scan_start_time;
		
		//cerr << "Received INFO packet from " << format_ip_address(server_address, true) << ": Protocol=" << server_protocol_version << "; Compat version=" << server_compat_version << "; Map=" << info.current_map_name << "; Blue players=" << info.team_count[0] << "; Red players=" << info.team_count[1] << "; Ping time=" << get_ticks() - scan_start_time << "ms"  << "; Uptime=" << info.uptime << endl;
		
		if (server_protocol_version != m_protocol_number || server_compat_version != m_client_compat) {
			//cerr << server_protocol_version << " != " << m_protocol_number << " || " << server_compat_version << " != " << m_client_compat << endl;
			// Different protocol version. Discard.
			return;
		}
	
		m_server_list.add(server_address, info);
	}
}


void	ServerScanner::hole_punch_packet(const IPAddress& server_address, PacketReader& packet) {
	uint32_t	scan_id;
	packet >> scan_id;

	if (scan_id != m_current_scan_id) {
		return;
	}

	scan_server(server_address);
}


void	ServerScanner::upgrade_available(const IPAddress& server_address, PacketReader& packet) {
	string		latest_version;
	packet >> latest_version;
	ostringstream message;
	
	cerr << "Your version of lmscan is out of date." << endl;
	cerr << "The latest version is " << latest_version << endl;
}

void	ServerScanner::output_results(OutputType type) {
	uint64_t final = get_ticks() - m_start_ticks;
	switch (type) {
	case OUTPUT_HUMAN_READABLE: {
		ReadableGenerator out(m_output);
		m_server_list.output(&out, final);
	} break;
	case OUTPUT_JSON: {
		JsonGenerator out(m_output);
		m_server_list.output(&out, final);
	} break;
	}
}

void	ServerScanner::scan_loopback() {
	PacketWriter info_request_packet(INFO_PACKET);
	info_request_packet << m_protocol_number << m_current_scan_id << get_ticks() << m_client_version;
	IPAddress localhostip;
	if (resolve_hostname(localhostip, "localhost", DEFAULT_PORTNO)) {
		m_network.send_packet_to(localhostip, info_request_packet);
	}
}

void    ServerScanner::check_for_upgrade() {
	PacketWriter packet(UPGRADE_AVAILABLE_PACKET);
	packet << m_client_version;
	m_network.send_packet_to(m_metaserver_address, packet);
}

void	ServerScanner::scan_local_network() {
	PacketWriter info_request_packet(INFO_PACKET);
	info_request_packet << m_protocol_number << m_current_scan_id << get_ticks();
	m_network.broadcast_packet(DEFAULT_PORTNO, info_request_packet);
}

void	ServerScanner::scan_metaserver() {
	if (m_metaserver_address.port != 0) {
		PacketWriter info_request_packet(INFO_PACKET);
		info_request_packet << m_protocol_number << m_current_scan_id << get_ticks() << m_client_version;
		m_network.send_packet_to(m_metaserver_address, info_request_packet);
	}
}

void	ServerScanner::scan_server(const IPAddress& server_address) {
	PacketWriter info_request_packet(INFO_PACKET);
	info_request_packet << m_protocol_number << m_current_scan_id << get_ticks();
	m_network.send_packet_to(server_address, info_request_packet);
}
