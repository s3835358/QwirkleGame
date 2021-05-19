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
        int getFeature(bool* isEofPtr);
        std::vector<std::string> getAction(bool* isEofPtr);
        std::string getFilepath(bool* isEofPtr);
        std::string saveGame(std::vector <std::string> input);
    
    private: 
        std::string prompt(bool* isEOF);
        bool areLetters(std::string playerName);
        std::vector<std::string> splitActions(std::string action);
        bool getMultiple();
       


}; 

 

#endif //INPUT_H 