/*
 * common/Player.hpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#ifndef LM_COMMON_PLAYER_HPP
#define LM_COMMON_PLAYER_HPP

#include <cmath>

class Player {
private:
	double m_x;
	double m_y;
	double m_x_vel;
	double m_y_vel;
	double m_rotation;
	double PI();
public:
	Player();
	Player(double x, double y, double xvel = 0, double yvel = 0, double rotation = 0);
	~Player();
	
	double get_x();
	double get_y();
	double get_x_vel();
	double get_y_vel();
	double get_rotation_degrees();
	double get_rotation_radians();
	
	void set_x(double x);
	void set_y(double y);
	void set_position(double x, double y);
	void update_position(unsigned long timediff);
	void set_x_vel(double xvel);
	void set_y_vel(double yvel);
	void set_velocity(double xvel, double yvel);
	void set_rotation_degrees(double rotation);
	void set_rotation_radians(double rotation);

};

#endif
