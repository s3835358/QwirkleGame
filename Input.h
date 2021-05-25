#ifndef INPUT_H 
#define INPUT_H 

#include <iostream>
#include <string>
#include <vector>

class Player;

class Input { 
   public: 
        Input(); 
        std::string getPlayerName(bool* isEOF);
        int getOption(bool* isEofPtr, bool isFeature);
        std::vector<std::string> getAction(bool* isEofPtr);
        std::string getFilepath(bool* isEofPtr);
        std::string saveGame(std::vector <std::string> input);
        int* getBoardSize(bool* isEOF);
    
    private: 
        std::string prompt(bool* isEOF);
        bool areLetters(std::string playerName);
        std::vector<std::string> splitActions(std::string action);
        bool getMultiple();
        bool Input::checkSizeInput(std::string size, int** dimensions);
}; 

 

#endif //INPUT_H 