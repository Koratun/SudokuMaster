#include "SudokuNumberButton.h"
#include "ColorHelper.h"

map<SudokuNumberButton::inputSource, sf::Color> SudokuNumberButton::colorDict{
	{SudokuNumberButton::inputSource::computer, sf::Color::White},
	{SudokuNumberButton::inputSource::hint, ColorHelper::Green},
	{SudokuNumberButton::inputSource::incorrect, ColorHelper::Red},
	{SudokuNumberButton::inputSource::user, ColorHelper::Tan},
	{SudokuNumberButton::inputSource::empty, sf::Color::White}
};

vector<sf::Text> SudokuNumberButton::numberNoteTexts;

SudokuNumberButton::SudokuNumberButton(sf::Font& font, float x, float y, function<void()> action) :
	Button(font, 20, " ", sf::Color(10, 10, 10), sf::Color::White, x, y, 45, 45, action)
{
	//If this static vector has not been initialized, do so
	if (SudokuNumberButton::numberNoteTexts.empty()) {
		//This vector will hold the texts for the little notes the user can write on each space. 
		//It is the same text for every space, but each number gets its own little spot in the space.
		//Each instance of SudokuNumberButton will draw a copy of this offset by its own position.
		SudokuNumberButton::numberNoteTexts.resize(9);
		for (int i = 0; i < 9; i++) {
			SudokuNumberButton::numberNoteTexts[i] = sf::Text(to_string(i + 1), font, 10);
			sf::Rect<float> textBounds = SudokuNumberButton::numberNoteTexts[i].getLocalBounds();
			SudokuNumberButton::numberNoteTexts[i].setOrigin(textBounds.left, textBounds.top);
			if (i < 5) {
				SudokuNumberButton::numberNoteTexts[i].setPosition(1 + 10 * i, 1);
			}
			else {
				SudokuNumberButton::numberNoteTexts[i].setPosition(1 + 13 * (i - 5), 35);
			}
		}
	}

}

void SudokuNumberButton::clicked(int numberSelected, bool markMode)
{
	//If the computer input the number, do not allow it to be changed
	if (getLabel() != " " && (source == inputSource::computer))
		return;

	//We are marking numbers if this is true
	if (markMode) {
		//If the number is bbetween 1-9 then show it accordingly
		if (numberSelected != 10) {
			source = inputSource::user;
			setLabel(to_string(numberSelected));
		}
		//Otherwise erase the number in the space
		else {
			source = inputSource::empty;
			setLabel(" ");
		}
		setLabelColor(colorDict[source]);
	}
	//Otherwise we are making notes
	else {
		//If we are erasing, then just empty the note list.
		if (numberSelected == 10) {
			numbersNoted.clear();
		}
		else {
			//If the given number has not been noted, add it to the list.
			vector<int>::iterator i = find(numbersNoted.begin(), numbersNoted.end(), numberSelected);
			if (i == numbersNoted.end()) {
				numbersNoted.push_back(numberSelected);
			}
			//If it has, remove it from the list.
			else {
				numbersNoted.erase(i);
			}
		}
	}
}

void SudokuNumberButton::updateNotes(vector<int>& notes)
{
	//Empty the notes vector
	numbersNoted.clear();
	//Then add all the notes in the given vector
	numbersNoted.insert(numbersNoted.begin(), notes.begin(), notes.end());
}

void SudokuNumberButton::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	//Draw the base button first
	Button::draw(target, states);
	states.transform *= getTransform();
	//Then draw the special notes for the sudoku part of the button
	for (int i = 0; i < 9; i++) {
		if (find(numbersNoted.begin(), numbersNoted.end(), i + 1) != numbersNoted.end()) {
			target.draw(SudokuNumberButton::numberNoteTexts[i], states);
		}
	}
}

sf::Color& SudokuNumberButton::getInputColor(inputSource source)
{
	return colorDict[source];
}

SudokuNumberButton::inputSource SudokuNumberButton::getSource()
{
	return source;
}

void SudokuNumberButton::wipeData()
{
	//Erase all outward facing data
	source = inputSource::empty;
	setLabel(" ");
	numbersNoted.clear();
	setLabelColor(colorDict[source]);
}

void SudokuNumberButton::giveComputerNumber(int n)
{
	//Input a number as the computer
	setLabel(to_string(n));
	source = inputSource::computer;
	setLabelColor(colorDict[source]);
}

void SudokuNumberButton::updateHint(int num)
{
	//Input a number as a hint
	source = inputSource::hint;
	setLabelColor(colorDict[source]);
	setLabel(to_string(num));
}

