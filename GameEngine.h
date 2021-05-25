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
    void runGame(std::vector<Player*>* playerListPtr, Board* board, 
    LinkedList* tileBag, Player* pCurrent, bool* isEOF);
    bool getEOFstate(bool* isEOF);
    
    void initPlayerHand(Player* player, LinkedList* tilebag);
    
    bool runAction(std::vector<Player*>* playerListPtr, Board* board, 
    LinkedList* tileBag, Player* pCurrent, bool* isEOF);

    bool runPlace(std::vector<std::string> actions, Board* board, 
    Player* player, LinkedList* tileBag);
    
    void toggleCurrent(std::vector<Player*>* playerListPtr,
    Player** pCurrent);

    bool runSave(std::vector<std::string> actions, Player* pCurrent,
    std::vector<Player*>* playerListPtr, Board* board, LinkedList* tileBag);
    
    bool checkEndGame(std::vector<Player*>* playerListPtr, Player* pCurrent,
    LinkedList* tileBag);

    void displayRound(std::vector<Player*>* playerListPtr, 
    Player* pCurrent, Board* board);

    void updateScore(int rCount, int cCount, bool* qwirkle, Player* player);

    void multipleScore(std::vector<Tile*>* tilesPtr, int rCount,
    int cCount, bool sameRow, Player* player);

    bool runReplace(std::vector<std::string> actions, Board* board, 
    Player* player, LinkedList* tileBag);

    bool checkMultiple(std::vector<std::string>* actions, Board* board,
    std::vector<int*>* locationsPtr,std::vector<Tile*>* tilesPtr, 
    bool* sameRow);

    bool runMultiple(std::vector<std::string>* actions, Board* board,
    std::vector<int*>* locationsPtr, std::vector<Tile*>* tilesPtr, 
    Player* player, LinkedList* tileBag);

    bool fileExists(std::string path);
    
    Tile* getTile(std::string tile);

    bool sameLine(std::vector<int*>* locations, 
    std::vector<Tile*>* tiles, bool* sameRow);

    void refillHand(std::vector<std::string>* actionsPtr, 
    Player* player, LinkedList* tileBag, std::vector<Tile*>* tilesPtr);

    bool prepareVectors(std::vector<std::string>* actions, Board* board, 
    std::vector<int*>* locations, std::vector<Tile*>* tiles, int* count, 
    bool final, bool* isLoc);

    Input* input;
};

#endif // GAME_ENGINE