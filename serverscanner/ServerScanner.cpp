#include "ServerScanner.hpp"
#include "ServerScannerNetwork.hpp"
#include "common/PacketReader.hpp"
#include "common/PacketWriter.hpp"
#include "common/network.hpp"
#include "common/misc.hpp"
#include "common/timer.hpp"

#include <vector>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <iostream>
#include <fstream>
#include <limits>
#include <sstream>
#include <map>

using namespace LM;
using namespace std;

ServerScanner::ServerScanner() : m_network(*this) {
	m_client_version = LM_VERSION;
	m_protocol_number = 5;
	
	if (const char* metaserver_address = getenv("LM_METASERVER")) {
		// Address specified by $LM_METASERVER environment avariable
		if (!resolve_hostname(m_metaserver_address, metaserver_address)) {
			cerr << "Unable to resolve metaserver hostname, `" << metaserver_address << "' as specified in the $LM_METASERVER environment variable.  Internet-wide server browsing will not be enabled." << endl;
		}
	} else if (!resolve_hostname(m_metaserver_address, METASERVER_HOSTNAME, METASERVER_PORTNO)) {
		cerr << "Unable to resolve metaserver hostname.  Internet-wide server browsing will not be enabled." << endl;
	}
}

void 	ServerScanner::run(string list_location) {
	if (list_location == "") {
		m_list_location = "serverlist.txt";
	} else {
		m_list_location = list_location;
	}
	uint32_t last_scan = 0;
	while(true) {
		if (last_scan == 0 || get_ticks() - last_scan > 30000) {
			m_server_list.clear();
			ofstream myfile;
			myfile.open (m_list_location.c_str(), ios::out | ios::trunc);
			myfile << "";
			myfile.close();
			cerr << "\n";
			scan_all();
			last_scan = get_ticks();
		}
		m_network.receive_packets(5000);
	}
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
		int		server_protocol_version;
		string		current_map_name;
		int		team_count[2];
		int		max_players;
		uint64_t	uptime;
		uint64_t	time_left_in_game;
		string		server_name;
		string		server_location;
		info_packet >> server_protocol_version >> current_map_name >> team_count[0] >> team_count[1] >> max_players >> uptime >> time_left_in_game >> server_name >> server_location;
		
		m_ping = get_ticks() - scan_start_time;
		
		//cerr << "Received INFO packet from " << format_ip_address(server_address, true) << ": Protocol=" << server_protocol_version << "; Map=" << current_map_name << "; Blue players=" << team_count[0] << "; Red players=" << team_count[1] << "; Ping time=" << get_ticks() - scan_start_time << "ms"  << "; Uptime=" << uptime << endl;
		
		if (server_protocol_version != m_protocol_number) {
			//cerr << "Server with different protocol found: " << format_ip_address(server_address, true) << ": Protocol=" << server_protocol_version << "; Map=" << current_map_name << "; Blue players=" << team_count[0] << "; Red players=" << team_count[1] << "; Ping time=" << get_ticks() - scan_start_time << "ms" << endl;
			return;
		}
		
		if (m_server_list.find(server_address) == m_server_list.end()) {
			int totalplayers = team_count[0] + team_count[1];
			m_server_list[server_address] = 1;
			ofstream myfile;
			myfile.open (m_list_location.c_str(), ios::out | ios::app);
			myfile  << server_address << " | " << server_name << " | " << server_location << " | Map: "<< current_map_name << " | Players: " << totalplayers << "/" << max_players << " (" << team_count[0] << " vs. " << team_count[1] << ") | Ping: " << m_ping << "\n";
			myfile.close();

			cerr << server_address << " | " << server_name << " | " << server_location << " | Map: "<< current_map_name << " | Players: " << totalplayers << "/" << max_players << " (" << team_count[0] << " vs. " << team_count[1] << ") | Ping: " << m_ping << "\n";
		}
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
	
	cerr << "Update Available: " << latest_version;
}

void	ServerScanner::scan_all() {
	PacketWriter info_request_packet(INFO_PACKET);
	m_current_scan_id = get_next_scan_id();
	info_request_packet << m_protocol_number << m_current_scan_id << get_ticks() << m_client_version;
	m_network.broadcast_packet(DEFAULT_PORTNO, info_request_packet);
	IPAddress localhostip;
	if (resolve_hostname(localhostip, "localhost", DEFAULT_PORTNO)) {
		m_network.send_packet_to(localhostip, info_request_packet);
	}
	m_network.send_packet_to(m_metaserver_address, info_request_packet);
}

void    ServerScanner::check_for_upgrade() {
	PacketWriter packet(UPGRADE_AVAILABLE_PACKET);
	packet << m_client_version;
	m_network.send_packet_to(m_metaserver_address, packet);
}

void	ServerScanner::scan_local_network() {
	PacketWriter info_request_packet(INFO_PACKET);
	m_current_scan_id = get_next_scan_id();
	info_request_packet << m_protocol_number << m_current_scan_id << get_ticks();
	m_network.broadcast_packet(DEFAULT_PORTNO, info_request_packet);
}

void	ServerScanner::contact_metaserver() {
	PacketWriter info_request_packet(INFO_PACKET);
	m_current_scan_id = get_next_scan_id();
	info_request_packet << m_protocol_number << m_current_scan_id << get_ticks() << m_client_version;
	m_network.send_packet_to(m_metaserver_address, info_request_packet);
}

void	ServerScanner::scan_server(const IPAddress& server_address) {
	PacketWriter info_request_packet(INFO_PACKET);
	info_request_packet << m_protocol_number << m_current_scan_id << get_ticks();
	m_network.send_packet_to(server_address, info_request_packet);
}

void	ServerScanner::send_packet(const PacketWriter& packet) {
	m_network.send_packet(packet);
}

void	ServerScanner::send_reliable_packet(const PacketWriter& packet) {
	m_network.send_reliable_packet(packet);
}

uint32_t	ServerScanner::get_next_scan_id() {
	static uint32_t		next_scan_id = 1L;
	return next_scan_id++;
}
