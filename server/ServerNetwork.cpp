/*
 * server/ServerNetwork.cpp
 *
 * This file is part of Leges Motus, a networked, 2D shooter set in zero gravity.
 * 
 * Copyright 2009-2011 Andrew Ayer, Nathan Partlan, Jeffrey Pfau
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

#include "ServerNetwork.hpp"
#include "Server.hpp"
#include "common/Exception.hpp"
#include "common/network.hpp"
#include "common/PacketWriter.hpp"
#include "common/PacketReader.hpp"
#include "common/UDPPacket.hpp"
#include "common/IPAddress.hpp"
#include <stdio.h>
#include <stdlib.h>
#ifndef __WIN32
#include <signal.h>
#endif

using namespace LM;
using namespace std;

bool	ServerNetwork::start(const IPAddress& bind_address) {
	// TODO: check to make sure server isn't listening already

	m_ack_manager.clear();
	m_peers.clear();

	return m_socket.bind(bind_address);
}

void	ServerNetwork::send_reliable_packet(const IPAddress& address, const PacketWriter& packet) {
	Peer*	peer = get_peer(address);
	if (!peer) {
		// Can only send reliable packets to registered peers
		return;
	}

	PacketHeader	header(packet.packet_type(), peer->next_sequence_no++, peer->connection_id);
	send_packet(address, header, packet.packet_data());
	m_ack_manager.add_packet(address, header, packet.packet_data());
}

void	ServerNetwork::broadcast_packet(const PacketWriter& packet, const IPAddress* exclude_peer) {
	for (std::map<IPAddress, Peer>::iterator it(m_peers.begin()); it != m_peers.end(); ++it) {
		if (exclude_peer && *exclude_peer == it->first) {
			continue;
		}

		send_packet(it->first, packet);
	}
}

void	ServerNetwork::broadcast_reliable_packet(const PacketWriter& packet, const IPAddress* exclude_peer) {
	AckManager::PacketHandle	ack_handle(m_ack_manager.add_broadcast_packet(packet.packet_data()));

	for (std::map<IPAddress, Peer>::iterator it(m_peers.begin()); it != m_peers.end(); ++it) {
		if (exclude_peer && *exclude_peer == it->first) {
			continue;
		}

		PacketHeader	header(packet.packet_type(), it->second.next_sequence_no++, it->second.connection_id);
		send_packet(it->first, header, packet.packet_data());
		m_ack_manager.add_broadcast_recipient(ack_handle, it->first, header);
	}
}

void	ServerNetwork::broadcast_reliable_packet(Packet* packet, const IPAddress* exclude_peer) {
	AckManager::PacketHandle	ack_handle(m_ack_manager.add_broadcast_packet(*packet));

	for (std::map<IPAddress, Peer>::iterator it(m_peers.begin()); it != m_peers.end(); ++it) {
		if (exclude_peer && *exclude_peer == it->first) {
			continue;
		}

		PacketHeader header = PacketHeader(packet->type, it->second.next_sequence_no++, it->second.connection_id);
		send_packet_to(it->first, packet);
		m_ack_manager.add_broadcast_recipient(ack_handle, it->first, header);
	}
}

bool	ServerNetwork::receive_packets(uint32_t timeout) {
#ifndef __WIN32
	// Now is an ideal time to handle signals, so unblock all signals
	sigset_t		old_sigset;
	sigset_t		new_sigset;
	sigemptyset(&new_sigset);
	sigprocmask(SIG_SETMASK, &new_sigset, &old_sigset);
#endif

	// Block until packets are received, timeout has elapsed, or a signal has been received.
	bool		has_packets = m_socket.has_packets(timeout);

#ifndef __WIN32
	// Restore the old signal mask
	sigprocmask(SIG_SETMASK, &old_sigset, NULL);
#endif

	if (!has_packets) {
		// Socket does not have packets to receive - timeout must have elapsed, or a signal was received
		return false;
	}

	UDPPacket	raw_packet(MAX_PACKET_LENGTH);

	// Receive all the packets we can.
	while (receive_raw_packet(raw_packet)) {
		PacketReader	packet(raw_packet);

		if (packet.sequence_no()) {
			try {
				// High reliability packet
				// Immediately send an ACK
				send_ack(raw_packet.get_address(), packet);

				if (Peer* peer = get_peer(raw_packet.get_address())) {
					if (packet.connection_id() == peer->connection_id && peer->packet_queue.push_r(packet)) {
						// Ready to be processed now.
						process_packet(raw_packet.get_address(), packet);

						// Process any other packets that might be waiting in the queue from this peer.
						while (peer->packet_queue.has_packet_r()) {
							process_packet(raw_packet.get_address(), peer->packet_queue.peek_r());
							peer->packet_queue.pop_r();
						}
					} else if (packet.connection_id() > peer->connection_id) {
						// From a newer connection than is currently registered
						// Process the packet, but we can't attempt any re-ordering.
						// What should happen (on a JOIN) is the Server class detects the duplicate connection,
						// un-registers the current peer, and re-registers it with the new connection ID
						process_packet(raw_packet.get_address(), packet);
					}
				} else {
					// From an unbound peer, so we can't attempt to re-order it, but we can process it anyways
					process_packet(raw_packet.get_address(), packet);
				}
			} catch (PacketQueue::FullQueueException) {
				m_server.excessive_packet_drop(raw_packet.get_address());
			}
		} else {
			// Low reliability packet - we don't care if, when, or how often it arrives
			process_packet(raw_packet.get_address(), packet);
		}
	}
	return true;
}

void	ServerNetwork::process_packet(const IPAddress& address, PacketReader& reader) {
	switch (reader.packet_type()) {
	case ACK_PACKET:
		process_ack(address, reader);
		break;

	case PLAYER_UPDATE_PACKET:
		m_server.player_update(address, reader);
		break;

	case WEAPON_DISCHARGED_PACKET:
		m_server.weapon_discharged(address, reader);
		break;

	case PLAYER_HIT_PACKET:
		m_server.player_hit(address, reader);
		break;

	case MESSAGE_PACKET:
		m_server.message(address, reader);
		break;

	case GATE_UPDATE_PACKET:
		m_server.gate_update(address, reader);
		break;

	case JOIN_PACKET:
		m_server.join(address, reader);
		break;

	case INFO_client_PACKET:
		m_server.info(address, reader);
		break;

	case LEAVE_PACKET:
		m_server.leave(address, reader);
		break;

	case PLAYER_ANIMATION_PACKET:
		m_server.player_animation(address, reader);
		break;

	case NAME_CHANGE_PACKET:
		m_server.name_change(address, reader);
		break;

	case TEAM_CHANGE_PACKET:
		m_server.team_change(address, reader);
		break;

	case REGISTER_SERVER_metaserver_PACKET:
		m_server.register_server_packet(address, reader);
		break;

	case MAP_INFO_PACKET:
		m_server.map_info_packet(address, reader);
		break;

	case HOLE_PUNCH_PACKET:
		m_server.hole_punch_packet(address, reader);
		break;

	case PLAYER_DIED_PACKET:
		m_server.player_died(address, reader);
		break;
	}
}

CommonNetwork::Peer*	ServerNetwork::get_peer(const IPAddress& addr) {
	map<IPAddress, Peer>::iterator	it(m_peers.find(addr));
	return it != m_peers.end() ? &it->second : NULL;
}

void	ServerNetwork::register_peer(const IPAddress& address, uint32_t connection_id, uint64_t next_send_sequence_no, uint64_t next_receive_sequence_no) {
	m_ack_manager.clear_peer(address);
	m_peers[address].init(connection_id, next_send_sequence_no, next_receive_sequence_no);
}

void	ServerNetwork::unregister_peer(const IPAddress& address) {
	m_peers.erase(address);
}

void	ServerNetwork::excessive_packet_drop(const IPAddress& peer) {
	m_server.excessive_packet_drop(peer);
}

