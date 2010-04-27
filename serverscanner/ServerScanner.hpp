#ifndef LM_CLIENT_SERVERSCANNER_HPP
#define LM_CLIENT_SERVERSCANNER_HPP

#include "ServerScannerNetwork.hpp"
#include "common/PacketReader.hpp"
#include "common/PacketWriter.hpp"

#include <string>
#include <map>

namespace LM {
	class IPAddress;
	class PacketWriter;
	
	class ServerScanner {
		public:
			ServerScanner();
			
			void		server_info(const IPAddress& server_address, PacketReader& reader);
			void		upgrade_available(const IPAddress& server_address, PacketReader& reader);
			void		hole_punch_packet(const IPAddress& server_address, PacketReader& reader);
			void		run(std::string list_location);
		private:
			IPAddress	m_metaserver_address;
			uint32_t	m_current_scan_id;
			uint64_t	m_ping;
			int		m_protocol_number;
			std::string 	m_client_version;
			ServerScannerNetwork	m_network;
			std::map<IPAddress, int> m_server_list;
			std::string	m_list_location;
			
			// Get the next scan id.
			static uint32_t	get_next_scan_id();
			
			// Scan both the local network and the meta server for servers:
			void		scan_all();

			// Contact the meta server to check for upgrades
			void		check_for_upgrade();
		
			// Network callbacks:
			void		send_packet(const PacketWriter& packet);
			void		send_reliable_packet(const PacketWriter& packet);
			
			// Scan a particular server:
			void		scan_server(const IPAddress& server_address);
			void		ping_server(const IPAddress& server_address);
	
			// Scan the local network for servers:
			void		scan_local_network();
	
			// Connect to the meta server to scan the Internet
			void		contact_metaserver();
			
			
	};
}

#endif
