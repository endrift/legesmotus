/*
 * common/team.hpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#ifndef LM_COMMON_TEAM_H
#define LM_COMMON_TEAM_H

static inline bool is_valid_team(char team) { return team == 'A' || team == 'B'; }
static inline char get_other_team(char team) { return team == 'A' ? 'B' : 'A'; }

#endif
