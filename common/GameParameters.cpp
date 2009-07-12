/*
 * common/GameParameters.cpp
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
	radar_scale = 0.1;
	radar_blip_duration = 1000;
	game_start_delay = 5000;
	late_join_delay = 5000;
	team_change_period = 30000; // 30 seconds
	firing_recoil = 1.5;
	firing_delay = 700;
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
	if (config.has("firing_recoil"))	firing_recoil = config.get<double>("firing_recoil");
	if (config.has("firing_delay"))		firing_delay = config.get<uint64_t>("firing_delay");
}

void	GameParameters::process_param_packet(PacketReader& packet) {
	string		param_name;
	packet >> param_name;

	if (param_name == "radar_mode") {
		packet >> radar_mode;
	} else if (param_name == "radar_scale") {
		packet >> radar_scale;
	} else if (param_name == "radar_blip_duration") {
		packet >> radar_blip_duration;
	} else if (param_name == "firing_recoil") {
		packet >> firing_recoil;
	} else if (param_name == "firing_delay") {
		packet >> firing_delay;
	}
}

ostream&	LM::operator<<(ostream& out, GameMode mode) {
	return out << format_game_mode(mode);
}

ostream&	LM::operator<<(ostream& out, RadarMode mode) {
	return out << format_radar_mode(mode);
}

istream&	LM::operator>>(istream& in, GameMode& mode) {
	string	str;
	in >> str;
	mode = parse_game_mode(str.c_str());
	return in;
}

istream&	LM::operator>>(istream& in, RadarMode& mode) {
	string	str;
	in >> str;
	mode = parse_radar_mode(str.c_str());
	return in;
}


GameMode	LM::parse_game_mode(const char* str) {
	if (str) {
		if (strcasecmp(str, "CLASSIC") == 0)	return CLASSIC;
		if (strcasecmp(str, "DEATHMATCH") == 0)	return DEATHMATCH;
		if (strcasecmp(str, "RACE") == 0)	return RACE;
		if (strcasecmp(str, "ZOMBIE") == 0)	return ZOMBIE;
	}

	return CLASSIC;
}

RadarMode	LM::parse_radar_mode(const char* str) {
	if (str) {
		if (strcasecmp(str, "ON") == 0)		return RADAR_ON;
		if (strcasecmp(str, "OFF") == 0)	return RADAR_OFF;
		if (strcasecmp(str, "AURAL") == 0)	return RADAR_AURAL;
	}

	return RADAR_OFF;
}

const char*	LM::format_game_mode(GameMode mode) {
	switch (mode) {
	case CLASSIC:	return "CLASSIC";
	case DEATHMATCH:return "DEATHMATCH";
	case RACE:	return "RACE";
	case ZOMBIE:	return "ZOMBIE";
	}
	return "";
}

const char*	LM::format_radar_mode(RadarMode mode) {
	switch (mode) {
	case RADAR_ON:		return "ON";
	case RADAR_OFF:		return "OFF";
	case RADAR_AURAL:	return "AURAL";
	}
	return "";
}

StringTokenizer&	LM::operator>>(StringTokenizer& tokenize, GameMode& mode) {
	mode = parse_game_mode(tokenize.get_next());
	return tokenize;
}

StringTokenizer&	LM::operator>>(StringTokenizer& tokenize, RadarMode& mode) {
	mode = parse_radar_mode(tokenize.get_next());
	return tokenize;
}

