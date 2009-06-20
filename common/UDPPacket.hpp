/*
 * common/UDPPacket.hpp
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

#ifndef LM_COMMON_UDPPACKET_HPP
#define LM_COMMON_UDPPACKET_HPP

#include "common/IPAddress.hpp"
#include <string>

namespace LM {
	class PacketWriter;
	
	class UDPPacket {
	private:
		char*		m_data;
		size_t		m_length;
		size_t		m_max_length;
		IPAddress	m_address;
	
		friend class UDPSocket;
	public:
		explicit UDPPacket(size_t max_length);
		UDPPacket(const UDPPacket& other);
		~UDPPacket();
	
		UDPPacket&		operator=(const UDPPacket& other);
	
		const char*		get_data() const { return m_data; }
		size_t			get_length() const { return m_length; }
		size_t			get_max_length() const { return m_max_length; }
	
		const IPAddress&	get_address() const { return m_address; }
		void			set_address(const IPAddress& address);
	
		// Fill this packet with the given data of the given length:
		void			fill(const char* data, size_t length);
		// Fill this packet with the data in the given PacketWriter:
		void			fill(const PacketWriter& data);
		// Fill this packet with the data in the given string:
		void			fill(const std::string& data);
	
		void			clear();
	};
	
}

#endif
