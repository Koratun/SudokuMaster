#pragma once
#include "Button.h"
#include <map>

class SudokuNumberButton : public Button
{
public:
	SudokuNumberButton(sf::Font& font, float x, float y, function<void()> action);

	//this is called when the button has been clicked
	//numberSelected determines what number the user has selected from the tray on the left.
	//markMode is true if we are marking true numbers, it is false if we are just writing down notes.
	void clicked(int numberSelected, bool markMode);

	void updateNotes(vector<int>& notes);

	static enum class inputSource { computer, hint, incorrect, user, empty };

	static sf::Color& getInputColor(inputSource source);

	inputSource getSource();

	//To be called when all label data from this button should be deleted
	void wipeData();

	//Called when the computer needs to input a number
	void giveComputerNumber(int n);

	//Input a number as a hint
	void updateHint(int num);

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

private:
	static map<inputSource, sf::Color> colorDict;

	inputSource source = inputSource::empty;

	static vector<sf::Text> numberNoteTexts;

	vector<int> numbersNoted;

};

