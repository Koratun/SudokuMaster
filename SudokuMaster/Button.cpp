#include "Button.h"
#include "ColorHelper.h"

Button::Button(sf::Font& font, int fontSize, const string& label, sf::Color backgroundColor, sf::Color textColor, float x, float y, float width, float height, function<void()> action)
	: Button(font, fontSize, label, backgroundColor, textColor, sf::Vector2f(x, y), sf::Vector2f(width, height), sf::Rect<float>(x, y, width, height), action)
{
}

Button::Button(sf::Font& font, int fontSize, const string& label, sf::Color backgroundColor, sf::Color textColor, sf::Vector2f posVec, sf::Vector2f sizeVec, sf::Rect<float> boundingBox, function<void()> action)
{
	//Saving intial variables
	this->action = action;
	this->font = &font;
	this->fontSize = fontSize;
	this->label = string(label);
	this->backgroundColor = backgroundColor;
	this->textColor = textColor;
	this->pos = posVec;
	this->boundingBox = boundingBox;
	this->size = sizeVec;

	//Buttons default to drawing as true.
	toDraw = true;

	//Constructing new variables from those given
	Transformable::setPosition(pos);

	rect = sf::RectangleShape(size);
	rect.setFillColor(backgroundColor);

	text.setFont(font);
	text.setString(label);
	text.setCharacterSize(fontSize);
	text.setFillColor(textColor);

	//Position text in the center of the button.
	sf::Rect<float> textBox = text.getLocalBounds();
	text.setOrigin(textBox.width / 2.f + textBox.left, textBox.top + textBox.height / 2.f);
	text.setPosition(boundingBox.width / 2.f, boundingBox.height / 2.f);


}

Button::Button(sf::Font& font, int fontSize, const string& label, sf::Color backgroundColor, sf::Color textColor, sf::Rect<float> rect, function<void()> action)
	: Button(font, fontSize, label, backgroundColor, textColor, sf::Vector2f(rect.left, rect.top), sf::Vector2f(rect.width, rect.height), rect, action)
{
}

void Button::mouseMoved(sf::Event event)
{
	if (mouseContained(event)) {
		rect.setFillColor(ColorHelper::getHighlightedColor(backgroundColor));
	}
	else {
		rect.setFillColor(backgroundColor);
	}
}

void Button::mouseClick(sf::Event event)
{
	if (mouseContained(event)) {
		action();
	}
}

void Button::mouseWheelScrolled(sf::Event event)
{
}

inline bool Button::mouseContained(sf::Event event)
{
	//SFML incorrectly fills the data of mouse move if the event is a click.
	if (event.type == sf::Event::MouseMoved) {
		return boundingBox.contains(event.mouseMove.x, event.mouseMove.y);
	} else {
		return boundingBox.contains(event.mouseButton.x, event.mouseButton.y);
	}
}

void Button::invokeAction()
{
	action();
}

void Button::bindSetupFunction(function<void(Button*)> setup)
{
	initButton = setup;
	hasSetupFunction = true;
}

void Button::setup()
{
	if(hasSetupFunction)
		initButton(this);
}

void Button::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= getTransform();
	target.draw(rect, states);
	target.draw(text, states);
}

void Button::setToDraw(bool draw)
{
	toDraw = draw;
}

bool Button::getToDraw()
{
	return toDraw;
}

void Button::setBackgroundColor(sf::Color bg)
{
	rect.setFillColor(bg);
	backgroundColor = bg;
}

void Button::setLabelColor(sf::Color color)
{
	text.setFillColor(color);
	textColor = color;
}

void Button::setLabel(const string& label)
{
	text.setString(label);
	//Position text in the center of the button.
	sf::Rect<float> textBox = text.getLocalBounds();
	text.setOrigin(textBox.width / 2.f + textBox.left, textBox.top + textBox.height / 2.f);
	this->label = label;
}

const string& Button::getLabel()
{
	return label;
}

void Button::setPosition(float x, float y) 
{
	boundingBox.left = x;
	boundingBox.top = y;
	Transformable::setPosition(x, y);
}

void Button::setOrigin(float x, float y)
{
	boundingBox.left -= x;
	boundingBox.top -= y;
	Transformable::setOrigin(x, y);
}
