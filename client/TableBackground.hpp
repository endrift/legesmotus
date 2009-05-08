/*
 * client/TableBackground.hpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#ifndef LM_CLIENT_TABLEBACKGROUND_HPP
#define LM_CLIENT_TABLEBACKGROUND_HPP

#include "Graphic.hpp"
#include <vector>

class TableBackground : public Graphic {
private:
	int	m_num_rows;
	std::vector<Color>	m_cell_colors;
	std::vector<int>	m_row_heights;

	Color	m_border_color;
	int	m_border_width;

	Color	m_intensity;

	void draw_row(int row) const;
public:
	TableBackground(int num_rows, int width);
	virtual TableBackground* clone() const;

	int	get_num_rows() const;
	int	get_row_height(int row) const;

	void	set_num_rows(int num_rows);
	void	set_row_height(int row, int height);
	void	set_image_width(int width);

	void	set_border_width(int pixels);
	void	set_border_color(Color color);
	void	set_cell_color(int row, Color color);

	virtual void	set_alpha(double alpha);
	virtual void	set_red_intensity(double r);
	virtual void	set_green_intensity(double g);
	virtual void	set_blue_intensity(double b);

	virtual void	draw(const GameWindow* window) const;
};

#endif
