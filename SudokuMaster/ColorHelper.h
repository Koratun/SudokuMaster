#pragma once
#include <SFML/Graphics.hpp>

class ColorHelper
{
public:
	static inline sf::Color getHighlightedColor(sf::Color color)
	{
		return color + sf::Color(10, 10, 10);
	}

	static sf::Color LightBlue;
	static sf::Color Red;
	static sf::Color Green;
	static sf::Color DarkRed;
	static sf::Color Yellow;
	static sf::Color Gray;
	static sf::Color AlmostBlack;
	static sf::Color Tan;
	static sf::Color Orange;

};

