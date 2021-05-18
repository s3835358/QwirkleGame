#ifndef GAME_ENGINE
#define GAME_ENGINE

#include <string>
#include <vector>

#define INIT_SCORE      0
 
class Player;
class Board;
class LinkedList;
class Input;
class Tile;


class GameEngine {
public:

    GameEngine(Input* input);
    void loadGame(bool* isEOF);
    void startNewGame(bool* isEOF);
    
    

private:
    void runGame(Player* p1, Player* p2, Board* board, 
    LinkedList* tileBag, Player* pCurrent, bool* isEOF);
    bool getEOFstate(bool* isEOF);
    
    void initPlayerHand(Player* player, LinkedList* tilebag);
    
    bool runAction(Player* p1, Player* p2, Board* board, 
    LinkedList* tileBag, Player* pCurrent, bool* isEOF);

    bool runPlace(std::vector<std::string> actions, Board* board, 
    Player* player, LinkedList* tileBag);
    
    void toggleCurrent(Player* p1, Player* p2, Player** pCurrent);

    bool runSave(std::vector<std::string> actions, Player* p1, Player* p2,
    Board* board, LinkedList* tileBag, Player* pCurrent);
    
    bool checkEndGame(Player* p1, Player* p2, LinkedList* tileBag);

    void displayRound(Player* p1, Player* p2, 
    Player* pCurrent, Board* board);

    void updateScore(int rCount, int cCount, bool* qwirkle, Player* player);

    bool runReplace(std::vector<std::string> actions, Board* board, 
    Player* player, LinkedList* tileBag);

    bool fileExists(std::string path);
    
    Tile* getTile(std::string tile);

    Input* input;
};

#endif // GAME_ENGINE