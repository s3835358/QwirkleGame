#include "Input.h"
#include <vector>
#include <string>
#include <cstring>

#define END_CHAR    "^D"
#define QUIT        "4"
#define INPUT_MIN   "1"
#define INPUT_MAX   "4"
#define MIN_CHAR    'A'
#define MAX_CHAR    'Z'
#define PLAYER_MIN  "2"
#define MAX_FIRST   '4'
#define MAX_SECOND  '1'
#define MIN_DIGIT   '0'
#define MAX_DIGIT   '9'
#define DIGIT       1
#define DIMENSIONS  2
#define ROW         0
#define COLUMN      1
#define TWO_DIGIT   1
#define ONE_DIGIT   0

using std::vector;
using std::string;
using std::cout;
using std::endl;

// Constructor
Input::Input() {

}

/*
 * Takes std::input from the console
 * Places the text into string input for reading
 */
string Input::prompt(bool* isEOF) {         
   cout <<" \n> ";
   string input;
   getline(std::cin, input, '\n');
   cout << endl;

    if(std::cin.eof() || input == END_CHAR){
        *isEOF = true;
        cout << "Goodbye" << endl;
    }
   
   return input;
}

/*
 * Saves the game to a certain directory with the input file name
 */
string Input::saveGame(vector<string> input){
    string savedGameName;
    int saveNameIndex = 1;

    savedGameName = input.at(saveNameIndex);
    
    return savedGameName;
}

int* Input::getBoardSize(bool* isEOF){
    bool valid = false;
    string size = "";
    int* dimensions = new int[DIMENSIONS];

    while (!valid) {

        size = prompt(isEOF);
        
        if(!*isEOF) {

            std::string sizes[DIMENSIONS] = {"",""};
            size_t comma = size.find(',');
            
            if(comma != std::string::npos) {
                sizes[ROW] = size.substr(0,comma);
                sizes[COLUMN] = size.substr(comma + DIGIT);
                
                int count = 0;
                
                if(size.length() > 0) {
                        valid = true;
                }

                for(string size : sizes) {

                    int char_dig = 0;
                    bool max = false;

                    for(char c : size) {
                        
                        if(char_dig == ONE_DIGIT && valid) {

                            valid = (c <= MAX_FIRST) && (c >= MIN_DIGIT);
                            
                            if(c == MAX_FIRST) {
                                max = true;
                            }

                        } else if (char_dig == TWO_DIGIT && valid) {

                            valid = (c >= MIN_DIGIT);

                            if(max) {
                                valid = (c <= MAX_SECOND);
                            } else {
                                valid = (c <= MAX_DIGIT);
                            }

                        } else {
                            
                            valid = false;

                        }
                        
                        char_dig++;
                    }

                    
                    if(valid) {
                        dimensions[count] = std::stoi(size);
                    }

                    count++;
                }
                
            } 

        } else {
            
            valid = true;
        }

        if(!valid) {
            cout << "Invalid input" << endl;
        }
        
    }

    return dimensions;
}

string Input::getPlayerName(bool* isEOF) {
    bool valid = false;
    string name = "";

    while (!valid) {
        name = prompt(isEOF);
        if(!*isEOF) {
            valid = areLetters(name);
        } else{
            valid = true;
        }
        
    }
    return name;
}

int Input::getOption(bool* isEOF, bool isPlayers) {
    string userInput = "";
    bool correctInput = false;

    string inputMin = INPUT_MIN;
    if(isPlayers) {
        inputMin = PLAYER_MIN;
    }
    
    while (!correctInput) {
        
        userInput = prompt(isEOF);

        if (userInput >= inputMin && userInput <= INPUT_MAX 
        && userInput.size() == DIGIT) {

            correctInput = true;
        
        } else if (*isEOF) {
            correctInput = true;
            userInput = QUIT;

        } else {
            cout << "Invalid input" << endl;
            std::cin.clear();
        }
    }

    return stoi(userInput);
}

vector<string> Input::getAction(bool* isEOF) {
    string action = prompt(isEOF);
    vector<string> actions = splitActions(action);
    
    return actions;
}

/*
 * Action splitter
 */
vector<string> Input::splitActions(string action){
    size_t index = action.find(' ');
    vector<string> actions{};

    while(index!=string::npos){
        actions.push_back(action.substr(0, index));    
        action = action.substr(index);
        action = action.erase(0,1);
        index = action.find(' ');

    }
    actions.push_back(action);
    return actions;

}

/*
 * File path getter
 */
string Input::getFilepath(bool* isEOF) {
    string path = prompt(isEOF);
    // TODO checks
    return path;
}

/*
 * Validates player name
 */
bool Input::areLetters(string playerName) {

    bool areLetters = true;
   
    for (string::size_type i = 0; i < playerName.size(); ++i) {
      // checking if player name is an upper-case character
        if ((playerName[i] < MIN_CHAR || playerName[i] > MAX_CHAR)) {
            areLetters = false;
            std::cin.clear();
        }
    }
   
    if(!areLetters){
        cout << "Invalid input" << endl;
    }

    return areLetters;
}
