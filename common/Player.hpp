/*
 * common/Player.hpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#ifndef LM_COMMON_PLAYER_HPP
#define LM_COMMON_PLAYER_HPP

#include <cmath>
#include <string>
#include <stdint.h>

class Player {
protected:
	std::string	m_name;
	uint32_t	m_id;
	char		m_team;		// Should be 'A' or 'B'
	int		m_score;
	double		m_x;
	double		m_y;
	double		m_x_vel;
	double		m_y_vel;
	double		m_rotation;	// Always in degrees
	double		m_gun_rotation;	// Always in degrees
	bool		m_is_invisible;
	bool		m_is_frozen;

	//static inline double	PI() { return 3.14159265358979323844; } // TODO: do this more elegantly
public:
	Player();
	Player(const char* name, uint32_t id, char team, double x = 0, double y = 0, double xvel = 0, double yvel = 0, double rotation = 0);
	virtual ~Player();
	
	const char* get_name() const { return m_name.c_str(); }
	uint32_t get_id() const { return m_id; }
	char get_team() const { return m_team; }
	int get_score() const { return m_score; }
	double get_x() const { return m_x; }
	double get_y() const { return m_y; }
	double get_x_vel() const { return m_x_vel; }
	double get_y_vel() const { return m_y_vel; }
	double get_rotation_degrees() const { return m_rotation; }
	double get_rotation_radians() const;
	double get_gun_rotation_degrees() const { return m_gun_rotation; }
	double get_gun_rotation_radians() const;
	bool is_invisible() const { return m_is_invisible; }
	bool is_visible() const { return !m_is_invisible; }
	bool is_frozen() const { return m_is_frozen; }
	bool is_unfrozen() const { return !m_is_frozen; }
	
	void set_name(const char* name);
	virtual void set_id(uint32_t id);
	virtual void set_team(char team);
	virtual void set_score(int score);
	virtual void add_score(int score_increase);
	virtual void set_x(double x);
	virtual void set_y(double y);
	virtual void set_position(double x, double y);
	virtual void update_position(unsigned long timediff); // Subject to change
	virtual void set_x_vel(double xvel);
	virtual void set_y_vel(double yvel);
	virtual void set_velocity(double xvel, double yvel);
	virtual void set_rotation_degrees(double rotation);
	virtual void set_rotation_radians(double rotation);
	virtual void set_gun_rotation_degrees(double gun_rotation);
	virtual void set_gun_rotation_radians(double gun_rotation);
	virtual void set_is_invisible(bool is_invisible);
	virtual void set_is_frozen(bool is_frozen);

};

#endif
