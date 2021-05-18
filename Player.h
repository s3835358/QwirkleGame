// #ifndef ASSIGN2_PLAYER_H     // causing errors
#define ASSIGN2_PLAYER_H

#include "LinkedList.h"
#include "Node.h"
#include <iostream>

class Player{
public:

    Player(std::string playerName, int playerScore);
    ~Player();
    
    std::string getPlayerName();

    int getPlayerScore();

    void setPlayerName(std::string playerName);

    void setPlayerScore(int playerScore);

    LinkedList* getHand();
private:
    std::string playerName;
    int playerScore;
    LinkedList* hand;

};

// #endif ASSIGN2_PLAYER_H