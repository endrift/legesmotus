/*
 * serverscanner/ServerScanner.hpp
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

#ifndef LM_CLIENT_SERVERSCANNER_HPP
#define LM_CLIENT_SERVERSCANNER_HPP

#include "ServerScannerNetwork.hpp"
#include "ServerList.hpp"
#include "common/PacketReader.hpp"
#include "common/PacketWriter.hpp"

#include <string>
#include <ostream>

namespace LM {
	class IPAddress;
	class PacketWriter;

	// TODO expose methods better for more flexible usage
	class ServerScanner {
		public:
			ServerScanner(const char* metaserver_address = NULL);

			enum ScanBits {
				SCAN_METASERVER = 1,
				SCAN_LOCAL_NETWORK = 2,
				SCAN_LOOPBACK = 4,
				SCAN_UPGRADE = 8,

				SCAN_METASERVER_FULL = 9,

				SCAN_ALL = 0xF
			};

			enum OutputType {
				OUTPUT_HUMAN_READABLE,
				OUTPUT_JSON
			};

			void	server_info(const IPAddress& server_address, PacketReader& reader);
			void	upgrade_available(const IPAddress& server_address, PacketReader& reader);
			void	hole_punch_packet(const IPAddress& server_address, PacketReader& reader);
			void	scan(std::ostream* outfile, OutputType outtype, int to_scan = SCAN_ALL);

		private:
			IPAddress	m_metaserver_address;
			uint32_t	m_current_scan_id;
			int		m_protocol_number;
			std::string 	m_client_version;
			ServerScannerNetwork	m_network;
			ServerList m_server_list;
			std::ostream*	m_output;

			void	output_results(OutputType type);

			// Scan localhost for a server
			void	scan_loopback();

			// Scan the local network for servers:
			void	scan_local_network();

			// Connect to the meta server to scan the Internet
			void	scan_metaserver();

			// Contact the meta server to check for upgrades
			void	check_for_upgrade();

			// Scan a particular server:
			void	scan_server(const IPAddress& server_address);
			void	ping_server(const IPAddress& server_address);
	};
}

#endif
