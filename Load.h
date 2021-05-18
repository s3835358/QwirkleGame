#ifndef LOAD_H
#define LOAD_H

#include <string>

class Player;
class LinkedList;
class Board; 

class Load {
public:

    Load();
    void loadGame(Player* player1, Player* player2, Board** board, 
    LinkedList** tileBag, Player** currentPlayer, std::string path, 
    bool* isEOF);

private:
    void loadPlayerName(Player* player, std::string name);
    void loadPlayerScore(Player* player, std::string scoreString);
    void loadList(LinkedList** list, std::string tiles);
};

#endif // LOAD_H