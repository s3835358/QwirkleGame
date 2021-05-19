#include "Player.h"
#include "LinkedList.h"


using std::string;

/*
 * Player constructor, takes the name and score and initializes hand
*/
Player::Player(string playerName, int playerScore){
    this->playerName = playerName;
    this->playerScore = playerScore;
    hand = new LinkedList();
}

/*
 * Deconstructor
*/
Player::~Player(){
    delete hand;
}

/*
 * Relevant getters and setters
*/
string Player::getPlayerName(){
    return playerName;
}

int Player::getPlayerScore(){
    return playerScore;
}

void Player::setPlayerName(string playerName){
    this->playerName = playerName;
}

void Player::setPlayerScore(int playerScore){
    this->playerScore = playerScore;
}

LinkedList* Player::getHand(){
    return hand;
}

/*
 *
 *    MILESTONE 3 ONWARDS
 *
 */