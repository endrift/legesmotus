/*
 * SoundController.hpp
 *
 * Copyright 2009 - Nathan Partlan, Andrew Ayer, Daniel Schneider, and Jeffrey Pfau
 * 
 */

#ifndef LM_CLIENT_SOUNDCONTROLLER_HPP
#define LM_CLIENT_SOUNDCONTROLLER_HPP

class SoundController {
public:
	SoundController();
	~SoundController();
	
	void	playSound(String sound);
	int	main();
	
};

#endif
