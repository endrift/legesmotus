/*
 * ClientNetwork.cpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */
 
#include <string>
#include "ClientNetwork.hpp"
#include "GameController.hpp"
#include "common/network.hpp"
#include "common/PacketWriter.hpp"
#include "common/PacketReader.hpp"
#include "common/UDPPacket.hpp"
#include "common/UDPSocket.hpp"
#include "common/IPAddress.hpp"
#include <iostream>

using namespace std;

ClientNetwork::ClientNetwork() {
	m_is_connected = false;
}

ClientNetwork::~ClientNetwork() {
}

bool	ClientNetwork::connect(const char* hostname, unsigned int portno) {
	disconnect();

	if (!resolve_hostname(m_server_address, hostname, portno)) {
		return false;
	}

	m_is_connected = true;
	return true;
}

void	ClientNetwork::disconnect() {
	m_is_connected = false;
}

void	ClientNetwork::send_packet(const PacketWriter& packet) {
	if (is_connected()) {
		send_packet_to(m_server_address, packet);
	}
}

void	ClientNetwork::send_packet_to(const IPAddress& dest, const PacketWriter& packet) {
	UDPPacket	raw_packet(MAX_PACKET_LENGTH);
	raw_packet.set_address(dest);
	raw_packet.fill(packet);
	send_raw_packet(raw_packet);
}

void	ClientNetwork::broadcast_packet(unsigned int portno, const PacketWriter& packet) {
	UDPPacket	raw_packet(MAX_PACKET_LENGTH);
	raw_packet.set_address(IPAddress(htonl(INADDR_BROADCAST), htons(portno))); // TODO: abstract the INADDR_BROADCAST and the htonl/htons
	raw_packet.fill(packet);
	send_raw_packet(raw_packet);
}

void	ClientNetwork::send_raw_packet(const UDPPacket& raw_packet) {
	m_socket.send(raw_packet);
}

bool	ClientNetwork::receive_raw_packet(UDPPacket& raw_packet) {
	return m_socket.has_packets() && m_socket.recv(raw_packet);
}

void	ClientNetwork::receive_packets(GameController& controller) {
	UDPPacket	raw_packet(MAX_PACKET_LENGTH);

	// Keep receiving packets for as long as we can.
	while (receive_raw_packet(raw_packet)) {
		if (is_connected() && raw_packet.get_address() == m_server_address) {
			process_server_packet(controller, raw_packet);
		} else {
			process_unbound_packet(controller, raw_packet);
		}
	}
}

void	ClientNetwork::process_server_packet(GameController& controller, const UDPPacket& raw_packet) {
	PacketReader	reader(raw_packet);

	switch (reader.packet_type()) {
	case PLAYER_UPDATE_PACKET:
		controller.player_update(reader);
		break;

	case GUN_FIRED_PACKET:
		controller.gun_fired(reader);
		break;

	case PLAYER_SHOT_PACKET:
		//send_ack(reader.packet_id());
		controller.player_shot(reader);
		break;

	case MESSAGE_PACKET:
		controller.message(reader);
		break;

	case GAME_START_PACKET:
		//send_ack(reader.packet_id());
		controller.game_start(reader);
		break;

	case GAME_STOP_PACKET:
		//send_ack(reader.packet_id());
		controller.game_stop(reader);
		break;

	case SCORE_UPDATE_PACKET:
		controller.score_update(reader);
		break;

	case WELCOME_PACKET:
		//send_ack(reader.packet_id());
		controller.welcome(reader);
		break;

	case ANNOUNCE_PACKET:
		controller.announce(reader);
		break;

	case GATE_UPDATE_PACKET:
		controller.gate_update(reader);
		break;

	case SHUTDOWN_PACKET:
		//controller.shutdown(reader);
		break;

	case LEAVE_PACKET:
		controller.leave(reader);
		break;

	case PLAYER_ANIMATION_PACKET:
		controller.animation_packet(reader);
		break;

	case REQUEST_DENIED_PACKET:
		controller.request_denied(reader);
		break;

	case NAME_CHANGE_PACKET:
		controller.name_change(reader);
		break;

	case TEAM_CHANGE_PACKET:
		controller.team_change(reader);
		break;
	}
}

void	ClientNetwork::process_unbound_packet(GameController& controller, const UDPPacket& raw_packet) {
	PacketReader	reader(raw_packet);
	
	switch (reader.packet_type()) {
	case INFO_PACKET:
		controller.server_info(raw_packet.get_address(), reader);
		break;
	}
}

