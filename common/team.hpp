/*
 * common/team.hpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#ifndef LM_COMMON_TEAM_H
#define LM_COMMON_TEAM_H

// Is the given team a valid team?
static inline bool is_valid_team(char team) { return team == 'A' || team == 'B'; }

// Given one team, return the other team.
static inline char get_other_team(char team) { return team == 'A' ? 'B' : 'A'; }

// Given a case-insensitive string of the form "blue" or "red", return A or B
char parse_team_string(const char* team_string);

#endif
