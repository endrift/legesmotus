/*
 * client/Mask.hpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#ifndef LM_CLIENT_MASK_HPP
#define LM_CLIENT_MASK_HPP

#include "Graphic.hpp"

class Mask : public Graphic {
private:
	Graphic* m_mask;
	Graphic* m_masked;
public:
	Mask(Graphic* mask, Graphic* masked);
	Mask(const Mask& other);
	virtual		~Mask();
	virtual Mask* clone() const;

	Graphic*	get_mask();
	Graphic*	get_masked();
	
	void		set_mask(Graphic* mask);
	void		set_masked(Graphic* masked);

	virtual void	set_alpha(double alpha) {}
	virtual void	set_red_intensity(double r) {}
	virtual void	set_green_intensity(double g) {}
	virtual void	set_blue_intensity(double b) {}

	virtual void	draw(const GameWindow* window) const;
};

#endif
