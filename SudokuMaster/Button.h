#pragma once
#include "MouseListener.h"
#include <SFML/Graphics.hpp>
#include <functional>

using namespace std;

class Button : public MouseListener, public sf::Drawable, public sf::Transformable
{
public:
	Button(sf::Font& font, int fontSize, const string& label, sf::Color backgroundColor, sf::Color textColor, float x, float y, float width, float height, function<void()> action);

	Button(sf::Font& font, int fontSize, const string& label, sf::Color backgroundColor, sf::Color textColor, sf::Vector2f posVec, sf::Vector2f sizeVec, sf::Rect<float> boundingBox, function<void()> action);

	Button(sf::Font& font, int fontSize, const string& label, sf::Color backgroundColor, sf::Color textColor, sf::Rect<float> rect, function<void()> action);

	virtual void mouseMoved(sf::Event event) override;

	virtual void mouseClick(sf::Event event) override;

	virtual void mouseWheelScrolled(sf::Event event) override;

	inline bool mouseContained(sf::Event event);

	//Warning: only use this if you don't need the mouse data to invoke the button action!
	void invokeAction();

	//Binds a given lambda as a function that will be called with 'this' as the button pointer when setup() is called
	void bindSetupFunction(function<void(Button*)> setup);

	void setup();

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	void setToDraw(bool draw);

	bool getToDraw();

	void setBackgroundColor(sf::Color bg);

	void setLabelColor(sf::Color color);

	void setLabel(const string& label);

	const string& getLabel();

	void setPosition(float x, float y);

	void setOrigin(float x, float y);


protected:
	function<void()> action;

	bool hasSetupFunction = false;

	function<void(Button*)> initButton;

	sf::Font* font;

	int fontSize;

	string label;

	sf::Color backgroundColor;

	sf::Color textColor;

	sf::Vector2f pos;

	sf::Vector2f size;

	sf::RectangleShape rect;

	sf::Text text;

	sf::Rect<float> boundingBox;

	bool toDraw;

};

