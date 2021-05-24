#ifndef LOAD_H
#define LOAD_H

#include <string>
#include <vector>

class Player;
class LinkedList;
class Board; 

class Load {
public:

    Load();
    void loadGame(std::vector<Player*>* playerListPtr, Board** board, 
    LinkedList** tileBag, Player** currentPlayer, std::string path, 
    bool* isEOF);

private:
    void loadPlayerName(Player* player, std::string name);
    void loadPlayerScore(Player* player, std::string scoreString);
    void loadList(LinkedList** list, std::string tiles);
    void loadPlayers(std::vector<Player*>* playerListPtr,
    std::string playerInfo);
};

#endif // LOAD_H