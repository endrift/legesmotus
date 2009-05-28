/*
 * common/UDPSocket.hpp
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

#ifndef LM_COMMON_UDPSOCKET_HPP
#define LM_COMMON_UDPSOCKET_HPP

#include <stdint.h>

class UDPPacket;

class UDPSocket {
private:
	int	fd;

	void	init();
	void	close();

public:
	UDPSocket();
	explicit UDPSocket(unsigned int portno);
	~UDPSocket();

	bool	bind(unsigned int portno);

	// return true as soon as a packet is ready for reading
	// will wait for up to wait_time milliseconds for a packet, after which it will return false
	bool	has_packets(uint32_t wait_time =0); 

	bool	send(const UDPPacket&);
	bool	recv(UDPPacket&);

	operator const void* () const { return fd >= 0 ? this : 0; }
	bool	operator! () const { return fd < 0; }

};

#endif
