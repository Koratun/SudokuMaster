#pragma once

#include <SFML/Window.hpp>

class MouseListener
{
public:
	MouseListener() : listenerEnabled(true) {};

	virtual void mouseMoved(sf::Event event) = 0;

	virtual void mouseClick(sf::Event event) = 0;

	virtual void mouseWheelScrolled(sf::Event event) = 0;

	void setListenerEnabled(bool enabled);

	bool getListenerEnabled();

protected:
	bool listenerEnabled;
};

