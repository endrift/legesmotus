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
	std::vector<double>	m_row_heights;

	Color	m_border_color;
	double	m_border_width;

	void draw_row(int row) const;
public:
	TableBackground(int num_rows, double width);
	virtual TableBackground* clone() const;

	int	get_num_rows() const;
	double	get_row_height(int row) const;

	void	set_num_rows(int num_rows);
	void	set_row_height(int row, double height);
	void	set_image_width(double width);

	void	set_border_width(double pixels);
	void	set_border_color(Color color);
	void	set_cell_color(int row, Color color);

	virtual void	set_alpha(double alpha) { (void)(alpha); }
	virtual void	set_red_intensity(double r) { (void)(r); }
	virtual void	set_green_intensity(double g) { (void)(g); }
	virtual void	set_blue_intensity(double b) { (void)(b); }

	virtual void	draw(const GameWindow* window) const;
};

#endif
