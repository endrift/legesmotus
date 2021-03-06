/*
 * common/GameParameters.cpp
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

#include "GameParameters.hpp"
#include "ConfigManager.hpp"
#include "StringTokenizer.hpp"
#include "PacketReader.hpp"
#include <limits>
#include <istream>
#include <ostream>
#include <cstring>

using namespace LM;
using namespace std;

void 	GameParameters::reset() {
	max_players = 32;
	gate_open_time = 15000;
	gate_close_time = 5000;
	gate_stick_time = 0;
	freeze_time = 10000;
	friendly_fire = true;
	game_timeout = 0; // No timeout
	game_mode = CLASSIC;
	radar_mode = RADAR_ON;
	radar_scale = 0.1f;
	radar_blip_duration = 1000;
	game_start_delay = 5000;
	late_join_delay = 5000;
	team_change_period = 30000; // 30 seconds
	autobalance_teams = false;
	recharge_amount = 1;
	recharge_rate = 150;
	recharge_delay = 300;
	recharge_continuously = false;
	jump_velocity = 250.0f;
	weapon_switch_delay = 300;
	late_spawn_frozen = true;
	weapon_set = "standard";
	
	update_params();
}

void	GameParameters::init_from_config(const ConfigManager& config) {
	if (config.has("max_players"))		max_players = config.get<int>("max_players");
	if (config.has("gate_open_time"))	gate_open_time = config.get<uint64_t>("gate_open_time");
	if (config.has("gate_close_time"))	gate_close_time = config.get<uint64_t>("gate_close_time");
	if (config.has("gate_stick_time"))	gate_stick_time = config.get<uint64_t>("gate_stick_time");
	if (config.has("freeze_time"))		freeze_time = config.get<uint64_t>("freeze_time");
	if (config.has("friendly_fire"))	friendly_fire = config.get<bool>("friendly_fire");
	if (config.has("game_timeout"))		game_timeout = config.get<uint64_t>("game_timeout");
	if (config.has("game_mode"))		game_mode = config.get<GameMode>("game_mode");
	if (config.has("radar_mode"))		radar_mode = config.get<RadarMode>("radar_mode");
	if (config.has("radar_scale"))		radar_scale = config.get<double>("radar_scale");
	if (config.has("radar_blip_duration"))	radar_blip_duration = config.get<uint64_t>("radar_blip_duration");
	if (config.has("game_start_delay"))	game_start_delay = config.get<uint64_t>("game_start_delay");
	if (config.has("late_join_delay"))	late_join_delay = config.get<uint64_t>("late_join_delay");
	if (config.has("team_change_period"))	team_change_period = config.get<uint64_t>("team_change_period");
	if (config.has("autobalance"))		autobalance_teams = config.get<bool>("autobalance");
	if (config.has("recharge_amount"))	recharge_amount = config.get<int>("recharge_amount");
	if (config.has("recharge_rate"))	recharge_rate = config.get<uint64_t>("recharge_rate");
	if (config.has("recharge_delay"))	recharge_delay = config.get<uint64_t>("recharge_delay");
	if (config.has("recharge_continuously"))recharge_continuously = config.get<bool>("recharge_continuously");
	if (config.has("jump_velocity"))	jump_velocity = config.get<double>("jump_velocity");
	if (config.has("weapon_switch_delay"))	weapon_switch_delay = config.get<uint64_t>("weapon_switch_delay");
	if (config.has("late_spawn_frozen"))	late_spawn_frozen = config.get<bool>("late_spawn_frozen");
	if (config.has("weapon_set"))		weapon_set = config.get<string>("weapon_set");
	
	update_params();
}

bool GameParameters::process_param_packet(PacketReader& packet) {
	string param_name;
	packet >> param_name;

	if (param_name == "radar_mode") {
		packet >> radar_mode;
	} else if (param_name == "radar_scale") {
		packet >> radar_scale;
	} else if (param_name == "radar_blip_duration") {
		packet >> radar_blip_duration;
	} else if (param_name == "recharge_amount") {
		packet >> recharge_amount;
	} else if (param_name == "recharge_rate") {
		packet >> recharge_rate;
	} else if (param_name == "recharge_delay") {
		packet >> recharge_delay;
	} else if (param_name == "recharge_continuously") {
		packet >> recharge_continuously;
	} else if (param_name == "jump_velocity") {
		packet >> jump_velocity;
	} else if (param_name == "weapon_switch_delay") {
		packet >> weapon_switch_delay;
	} else {
		return false;
	}
	update_params();
	return true;
}

void GameParameters::update_params() {
	// XXX: This method is ugly, and should be changed when this class is changed to use the map more effectively.

	m_params.clear();
	stringstream s;
	s << max_players << " " << gate_open_time << " " << gate_close_time << " " << gate_stick_time << " " << freeze_time << " " << friendly_fire << " " << game_timeout << " " << game_mode << " " << radar_mode << " " << radar_scale << " " << radar_blip_duration << " " << game_start_delay << " " << late_join_delay << " " << team_change_period << " " << autobalance_teams << " " << recharge_amount << " " << recharge_rate << " " << recharge_delay << " " << recharge_continuously << " " << jump_velocity << " " << weapon_switch_delay << " " << late_spawn_frozen << " " << weapon_set;
	
	s >> m_params["max_players"] >> m_params["gate_open_time"] >> m_params["gate_close_time"] >> m_params["gate_stick_time"] >> m_params["freeze_time"] >> m_params["friendly_fire"] >> m_params["game_timeout"] >> m_params["game_mode"] >> m_params["radar_mode"] >> m_params["radar_scale"] >> m_params["radar_blip_duration"] >> m_params["game_start_delay"] >> m_params["late_join_delay"] >> m_params["team_change_period"] >> m_params["autobalance_teams"] >> m_params["recharge_amount"] >> m_params["recharge_rate"] >> m_params["recharge_delay"] >> m_params["recharge_continuously"] >> m_params["jump_velocity"] >> m_params["weapon_switch_delay"] >> m_params["late_spawn_frozen"] >> m_params["weapon_set"];
}

const std::map<std::string, std::string>& GameParameters::get_params() {
	update_params();
	return m_params;
}

ostream& LM::operator<<(ostream& out, GameMode mode) {
	return out << format_game_mode(mode);
}

ostream& LM::operator<<(ostream& out, RadarMode mode) {
	return out << format_radar_mode(mode);
}

istream& LM::operator>>(istream& in, GameMode& mode) {
	string	str;
	in >> str;
	mode = parse_game_mode(str.c_str());
	return in;
}

istream& LM::operator>>(istream& in, RadarMode& mode) {
	string	str;
	in >> str;
	mode = parse_radar_mode(str.c_str());
	return in;
}


GameMode LM::parse_game_mode(const char* str) {
	if (str) {
		if (strcasecmp(str, "CLASSIC") == 0)	return CLASSIC;
		if (strcasecmp(str, "DEATHMATCH") == 0)	return DEATHMATCH;
		if (strcasecmp(str, "RACE") == 0)	return RACE;
		if (strcasecmp(str, "ZOMBIE") == 0)	return ZOMBIE;
	}

	return CLASSIC;
}

RadarMode LM::parse_radar_mode(const char* str) {
	if (str) {
		if (strcasecmp(str, "ON") == 0)		return RADAR_ON;
		if (strcasecmp(str, "OFF") == 0)	return RADAR_OFF;
		if (strcasecmp(str, "AURAL") == 0)	return RADAR_AURAL;
	}

	return RADAR_OFF;
}

const char* LM::format_game_mode(GameMode mode) {
	switch (mode) {
	case CLASSIC:	return "CLASSIC";
	case DEATHMATCH:return "DEATHMATCH";
	case RACE:	return "RACE";
	case ZOMBIE:	return "ZOMBIE";
	}
	return "";
}

const char* LM::format_radar_mode(RadarMode mode) {
	switch (mode) {
	case RADAR_ON:		return "ON";
	case RADAR_OFF:		return "OFF";
	case RADAR_AURAL:	return "AURAL";
	}
	return "";
}

StringTokenizer& LM::operator>>(StringTokenizer& tokenize, GameMode& mode) {
	mode = parse_game_mode(tokenize.get_next());
	return tokenize;
}

StringTokenizer& LM::operator>>(StringTokenizer& tokenize, RadarMode& mode) {
	mode = parse_radar_mode(tokenize.get_next());
	return tokenize;
}

