#include<iostream>
#include<iomanip>
#include<fstream>
#include<Windows.h>
using namespace std;

const int line_size = 120;	//Length of the program's lines displayed
const int puzzle_max = 20;	//Max size the puzzle can take
const int word_max = 40;	//Max size of words in the puzzle
const int word_length = 20;	//Max length of word
const int word_found_foreground = 0;	//The foreground color of found words, o = black, 7 = white
const char file_name[] = { "Word Puzzle.txt" };	//Name of the export file

//Target word details
struct details {
	bool found = false;		//Status of the word
	char word[word_length];	//Word
	int direction;			//0 = VerticalF, 1 = VerticalB, 2 = HorizontalF, 3 = HorizontalB, 4 = DiagonalDLR, 5 = DiagonalDRL, 6 = DiagonalULR, 7 = DiagonalURL
	int location_SR;		//Start location of the word - Row
	int location_SC;		//Start location of the word - Column
};

void line();
void header();
void info();
void color_code(int[][puzzle_max], int, details[]);
bool int_check(int, int, int);
bool char_check(char&);
void input_data(int&, char[][puzzle_max], int&, details[]);
void horizontalF_search(int, int, int, char[][puzzle_max], details[], int);
void horizontalB_search(int, int, int, char[][puzzle_max], details[], int);
void verticalF_search(int, int, int, char[][puzzle_max], details[], int);
void verticalB_search(int, int, int, char[][puzzle_max], details[], int);
void diagonalDLR_search(int, int, int, char[][puzzle_max], details[], int);
void diagonalDRL_search(int, int, int, char[][puzzle_max], details[], int);
void diagonalULR_search(int, int, int, char[][puzzle_max], details[], int);
void diagonalURL_search(int, int, int, char[][puzzle_max], details[], int);
void puzzle_search(char[][puzzle_max], details[], int, int);
void display_puzzle(int, int, details[], char[][puzzle_max], int[][puzzle_max]);
void display_data(int, details[], int[][puzzle_max]);
bool export_q();
void file_export(int, details[], int[][puzzle_max], char[][puzzle_max], int);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////// MAIN FUNCTION ////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main() {
	int puzzle_size;						//The puzzle size
	int word_search;						//How many words to search for
	int color[puzzle_max][puzzle_max];		//Puzzle colors
	char puzzle[puzzle_max][puzzle_max];	//Puzzle letters
	details word[word_max];

	header();
	info();
	input_data(puzzle_size, puzzle, word_search, word);
	puzzle_search(puzzle, word, word_search, puzzle_size);

	system("cls");
	header();

	display_puzzle(puzzle_size, word_search,word, puzzle, color);
	display_data(word_search, word, color);
	file_export(word_search, word, color, puzzle, puzzle_size);
	
	return 0;
}

//Print's a line of preconfigured lenght
void line() {
	for (int r = 0; r < line_size; r++) {
		cout << "-";
	}
	cout << endl;
}

//Print's the header of the program
void header() {
	line();
	cout << setw((line_size / 2) + 9) << "Hidden Word Puzzle" << endl;
	line();
}

void info() {
	cout << setw((line_size / 2) + 4) << "- INFO -" << endl;
	cout << "- The program can find hidden words: Diagonal, Forward, Backward, Up, and Down" << endl;
	cout << "- Ability to save your solved puzzle to a file" << endl;
	cout << "- Warning: Uses Windows specific libraries" << endl;
	line();
}

//Assign color codes to an array
void color_code(int code[][puzzle_max], int word_search, details word[]) {
	int color = word_found_foreground;

	for (int r = 0; r < puzzle_max; r++) {
		for (int c = 0; c < puzzle_max; c++) {
			code[r][c] = 7;
		}
	}

	for (int position = 0; position < word_search; position++) {
		if (word[position].found) {
			color += 16;	//After some research i discover that every 16 the backround color changes

			if (color > 255) {
				color = word_found_foreground + 16;
			}

			switch (word[position].direction) {
			case 0:
				for (int r = 0; r < strlen(word[position].word); r++) {
					code[word[position].location_SR + r][word[position].location_SC] = color;
				}
				break;
			case 1:
				for (int r = 0; r < strlen(word[position].word); r++) {
					code[word[position].location_SR - r][word[position].location_SC] = color;
				}
				break;
			case 2:
				for (int r = 0; r < strlen(word[position].word); r++) {
					code[word[position].location_SR][word[position].location_SC + r] = color;
				}
				break;
			case 3:
				for (int r = 0; r < strlen(word[position].word); r++) {
					code[word[position].location_SR][word[position].location_SC - r] = color;
				}
				break;
			case 4:
				for (int r = 0; r < strlen(word[position].word); r++) {
					code[word[position].location_SR + r][word[position].location_SC + r] = color;
				}
				break;
			case 5:
				for (int r = 0; r < strlen(word[position].word); r++) {
					code[word[position].location_SR + r][word[position].location_SC - r] = color;
				}
				break;
			case 6:
				for (int r = 0; r < strlen(word[position].word); r++) {
					code[word[position].location_SR - r][word[position].location_SC - r] = color;
				}
				break;
			case 7:
				for (int r = 0; r < strlen(word[position].word); r++) {
					code[word[position].location_SR - r][word[position].location_SC + r] = color;
				}
				break;
			}
		}
	}
}

//Checks if an input is an intiger and between given limits
bool int_check(int min, int max, int input) {
	if (!cin.good() || (input < min || input > max)) {
		cin.clear();
		cin.ignore(256, '\n');
		cout << "\aIncorect Value! Give an integer number between " << min << "-" << max << endl << endl;
		return false;
	}
	else {
		return true;
	}
}

//Checks if the input is a character of the alphabet
bool char_check(char& input) {
	if (input >= 'a' && input <= 'z') {
		input -= 32;
	}

	if (!(input >= 'A' && input <= 'Z')) {
		cin.ignore(256, '\n');
		cout << "'" << input << "'\a is not a valid alphabet character." << endl << endl;
		return false;
	}
	else {
		return true;
	}
}

//Takes data from user
void input_data(int& puzzle_size, char puzzle[][puzzle_max], int& word_search, details word[]) {
	do {
		cout << "Lenght of puzzle(5-" << puzzle_max << "): ";
		cin >> puzzle_size;
	} while (!int_check(5, puzzle_max, puzzle_size));

	cout << endl << "Now it's time to enter the characters. " << puzzle_size << " per line." << endl;

	for (int r = 0; r < puzzle_size; r++) {
		cout << r + 1 << " line:" << endl;

		for (int c = 0; c < puzzle_size; c++) {
			do {
				cin >> puzzle[r][c];
			} while (!char_check(puzzle[r][c]));
		}

		cin.ignore(256, '\n');
	}

	do {
		cout << "Words to look for(0-" << word_max << "): ";
		cin >> word_search;
	} while (!int_check(0, word_max, word_search));

	bool check = true;
	int c = 0;

	for (int r = 0; r < word_search; r++) {
		do {
			cout << r + 1 << " Word: ";
			cin >> word[r].word;

			c = 0;

			do{
				check = char_check(word[r].word[c]);
			}while (check && ++c != strlen(word[r].word));
		} while (!check);
	}
}

//Search the puzzle horizontaly
void horizontalF_search(int row, int coloumn, int location_strc, char search[][puzzle_max], details target[], int lenght) {
	int counter = 0;
	bool check = false;

	do {
		check = (target[location_strc].word[counter] == search[row][coloumn + counter]) ? true : false;
	} while (++counter < strlen(target[location_strc].word) && check && counter < lenght);
	
	if (check) {
		target[location_strc].found = true;
		target[location_strc].direction = 2;
		target[location_strc].location_SR = row;
		target[location_strc].location_SC = coloumn;
	}
}

//Search the puzzle horizontaly Backwards
void horizontalB_search(int row, int coloumn, int location_strc, char search[][puzzle_max], details target[], int lenght) {
	int counter = 0;
	bool check = false;

	do {
		check = (target[location_strc].word[counter] == search[row][coloumn - counter]) ? true : false;
	} while (++counter < strlen(target[location_strc].word) && check && counter < lenght);


	if (check) {
		target[location_strc].found = true;
		target[location_strc].direction = 3;
		target[location_strc].location_SR = row;
		target[location_strc].location_SC = coloumn;
	}
}

//Search the puzzle verticaly
void verticalF_search(int row, int coloumn, int location_strc, char search[][puzzle_max], details target[], int lenght) {
	int counter = 0;
	bool check = false;

	do {
		check = (target[location_strc].word[counter] == search[row + counter][coloumn]) ? true : false;
	} while (++counter < strlen(target[location_strc].word) && check && counter < lenght);

	if (check) {
		target[location_strc].found = true;
		target[location_strc].direction = 0;
		target[location_strc].location_SR = row;
		target[location_strc].location_SC = coloumn;
	}
}

//Search the puzzle verticaly Backwards
void verticalB_search(int row, int coloumn, int location_strc, char search[][puzzle_max], details target[], int lenght) {
	int counter = 0;
	bool check = false;

	do {
		check = (target[location_strc].word[counter] == search[row - counter][coloumn]) ? true : false;
	} while (++counter < strlen(target[location_strc].word) && check && counter < lenght);


	if (check) {
		target[location_strc].found = true;
		target[location_strc].direction = 1;
		target[location_strc].location_SR = row;
		target[location_strc].location_SC = coloumn;
	}
}

//Search the puzzle diagonaly Down - Left to Right
void diagonalDLR_search(int row, int coloumn, int location_strc, char search[][puzzle_max], details target[], int lenght) {
	int counter = 0;
	bool check = false;

	do {
		check = (target[location_strc].word[counter] == search[row + counter][coloumn + counter]) ? true : false;
	} while (++counter < strlen(target[location_strc].word) && check && counter < lenght);

	if (check) {
		target[location_strc].found = true;
		target[location_strc].direction = 4;
		target[location_strc].location_SR = row;
		target[location_strc].location_SC = coloumn;
	}
}

//Search the puzzle diagonaly Up - Left to Right
void diagonalULR_search(int row, int coloumn, int location_strc, char search[][puzzle_max], details target[], int lenght) {
	int counter = 0;
	bool check = false;

	do {
		check = (target[location_strc].word[counter] == search[row - counter][coloumn - counter]) ? true : false;
	} while (++counter < strlen(target[location_strc].word) && check && counter < lenght);

	if (check) {
		target[location_strc].found = true;
		target[location_strc].direction = 6;
		target[location_strc].location_SR = row;
		target[location_strc].location_SC = coloumn;
	}
}

//Search the puzzle diagonaly Down - Right to Left
void diagonalDRL_search(int row, int coloumn, int location_strc, char search[][puzzle_max], details target[], int lenght) {
	int counter = 0;
	bool check = false;

	do {
		check = (target[location_strc].word[counter] == search[row + counter][coloumn - counter]) ? true : false;
	} while (++counter < strlen(target[location_strc].word) && check && counter < lenght);

	if (check) {
		target[location_strc].found = true;
		target[location_strc].direction = 5;
		target[location_strc].location_SR = row;
		target[location_strc].location_SC = coloumn;
	}
}

//Search the puzzle diagonaly Down - Right to Left
void diagonalURL_search(int row, int coloumn, int location_strc, char search[][puzzle_max], details target[], int lenght) {
	int counter = 0;
	bool check = false;

	do {
		check = (target[location_strc].word[counter] == search[row - counter][coloumn + counter]) ? true : false;
	} while (++counter < strlen(target[location_strc].word) && check && counter < lenght);

	if (check) {
		target[location_strc].found = true;
		target[location_strc].direction = 7;
		target[location_strc].location_SR = row;
		target[location_strc].location_SC = coloumn;
	}
}

//The algorithm that searches the puzzle for the words
void puzzle_search(char search[][puzzle_max], details target[], int how_many, int lenght) {
	for (int r = 0; r < lenght; r++) {
		for (int c = 0; c < lenght; c++) {
			for (int pos = 0; pos < how_many; pos++) {
				if (target[pos].word[0] == search[r][c] && !target[pos].found){
					horizontalF_search(r, c, pos, search, target, lenght);
					horizontalB_search(r, c, pos, search, target, lenght);
					verticalF_search(r, c, pos, search, target, lenght);
					verticalB_search(r, c, pos, search, target, lenght);
					diagonalDLR_search(r, c, pos, search, target, lenght);
					diagonalDRL_search(r, c, pos, search, target, lenght);
					diagonalULR_search(r, c, pos, search, target, lenght);
					diagonalURL_search(r, c, pos, search, target, lenght);
				}
			}
		}
	}
}

//Displays the puzzle
void display_puzzle(int puzzle_size, int word_search, details word[], char puzzle[][puzzle_max], int code[][puzzle_max]) {
	HANDLE color = GetStdHandle(STD_OUTPUT_HANDLE);

	color_code(code, word_search, word);

	for (int r = 0; r < puzzle_size; r++) {
		cout << right << setw((line_size / 2) - ((puzzle_size) - 1)) << " ";

		for (int c = 0; c < puzzle_size; c++) {
			SetConsoleTextAttribute(color, code[r][c]);	//Set to prereconfigured colour
			cout << left << setw(2) << puzzle[r][c];
			SetConsoleTextAttribute(color, 7);			//Set back to default
		}
		cout << endl;
	}
}

//Displays the details of the target words
void display_data(int word_search, details word[], int code[][puzzle_max]) {
	int counter = 0;

	HANDLE color = GetStdHandle(STD_OUTPUT_HANDLE);

	line();
	cout << right << setw((line_size / 2) + 5) << "INFO BOARD" << endl;
	line();

	cout << left << setw(5) << "N/N" << setw(word_length) << "Word" << setw(12) << "Status" << setw(5) << "Row" << setw(8) << "Column" << setw(33) << "Direction" << setw(6) << "Color" << endl;
	line();

	for (int r = 0; r < word_search; r++) {
		cout << left << setw(5) << r + 1 << setw(word_length) << word[r].word << setw(12);

		if (word[r].found) {
			SetConsoleTextAttribute(color, 10);
			cout << setw(12) << "FOUND";
			SetConsoleTextAttribute(color, 7);
			cout << setw(5) << word[r].location_SR << setw(8) << word[r].location_SC << setw(33);

			counter++;

			switch (word[r].direction) {
			case 0:
				cout << "Vertically (Forward)";
				break;
			case 1:
				cout << "Vertically (Backwards)";
				break;
			case 2:
				cout << "Horizontally (Forward)";
				break;
			case 3:
				cout << "Horizontally (Backwards)";
				break;
			case 4:
				cout << "Diagonally Down (Left to Right)";
				break;
			case 5:
				cout << "Diagonally Down (Rigth to Left)";
				break;
			case 6:
				cout << "Diagonally Up (Left to Right)";
				break;
			case 7:
				cout << "Diagonally Up (Rigth to Left)";
				break;
			}

			SetConsoleTextAttribute(color, code[word[r].location_SR][word[r].location_SC]);
			cout << setw(6) << "------";
			SetConsoleTextAttribute(color, 7);
		}
		else {
			SetConsoleTextAttribute(color, 4);
			cout << setw(12) << "NOT FOUND";
			SetConsoleTextAttribute(color, 7);
			cout << setw(5) << "-" << setw(8) << "-" << setw(33) << "-" << setw(6) << "------";
		}

		cout << endl;
	}
	line();
	cout << "Found: " << counter << " / " << word_search << " words" << endl;
	line();
}

//Export question
bool export_q() {
	bool check_if;
	char repeat;

	do {
		cout << "Do you want to export your solved puzzle to a file? Y/N: ";
		cin >> repeat;

		if (repeat != 'y' && repeat != 'Y' && repeat != 'n' && repeat != 'N') {
			cout << "'" << repeat << "' is not recognized as a program command." << endl << endl;
			check_if = true;
		}
		else {
			check_if = false;
		}
	} while (check_if);

	if (repeat == 'y' || repeat == 'Y') {
		return true;
	}
	else {
		return false;
	}
}

//Exports the solved puzzle to a file
void file_export(int word_search, details word[], int code[][puzzle_max], char puzzle[][puzzle_max], int puzzle_size) {
	if (export_q()) {
		ofstream output;
		output.open(file_name);

		//Header

		for (int r = 0; r < line_size; r++) {
			output << "-";
		}
		output << endl;

		output << setw((line_size / 2) + 9) << "Hidden Word Puzzle" << endl;

		for (int r = 0; r < line_size; r++) {
			output << "-";
		}
		output << endl;

		//Puzzle

		for (int r = 0; r < puzzle_size; r++) {
			output << right << setw((line_size / 2) - ((puzzle_size)-1)) << " ";

			for (int c = 0; c < puzzle_size; c++) {
				output << left << setw(2) << puzzle[r][c];
			}
			output << endl;
		}

		int counter = 0;

		for (int r = 0; r < line_size; r++) {
			output << "-";
		}
		output << endl;

		//Info

		output << right << setw((line_size / 2) + 5) << "INFO BOARD" << endl;

		for (int r = 0; r < line_size; r++) {
			output << "-";
		}
		output << endl;

		output << left << setw(5) << "N/N" << setw(word_length) << "Word" << setw(12) << "Status" << setw(5) << "Row" << setw(8) << "Column" << setw(33) << "Direction" << setw(6) << "Color" << endl;

		for (int r = 0; r < line_size; r++) {
			output << "-";
		}
		output << endl;

		for (int r = 0; r < word_search; r++) {
			output << left << setw(5) << r + 1 << setw(word_length) << word[r].word << setw(12);

			if (word[r].found) {
				output << setw(12) << "FOUND";
				output << setw(5) << word[r].location_SR << setw(8) << word[r].location_SC << setw(33);

				counter++;

				switch (word[r].direction) {
				case 0:
					output << "Vertically (Forward)";
					break;
				case 1:
					output << "Vertically (Backwards)";
					break;
				case 2:
					output << "Horizontally (Forward)";
					break;
				case 3:
					output << "Horizontally (Backwards)";
					break;
				case 4:
					output << "Diagonally Down (Left to Right)";
					break;
				case 5:
					output << "Diagonally Down (Rigth to Left)";
					break;
				case 6:
					output << "Diagonally Up (Left to Right)";
					break;
				case 7:
					output << "Diagonally Up (Rigth to Left)";
					break;
				}

				output << setw(6) << "------";
			}
			else {
				output << setw(12) << "NOT FOUND";
				output << setw(5) << "-" << setw(8) << "-" << setw(33) << "-" << setw(6) << "------";
			}

			output << endl;
		}

		for (int r = 0; r < line_size; r++) {
			output << "-";
		}
		output << endl;

		output << "Found: " << counter << " / " << word_search << " words" << endl;

		for (int r = 0; r < line_size; r++) {
			output << "-";
		}
		output << endl;

		cout << "File export Complited!" << endl;
		output.close();
	}
}
