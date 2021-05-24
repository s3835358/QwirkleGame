#include "Load.h"
#include "Tile.h"
#include "Board.h"
#include "Player.h"
#include "TileCodes.h"
#include <string>
#include <fstream>
#include <sstream>
#include <cstring>
#include <vector>

#define FIRST_LINE      1
#define PLAYER_NAME     1
#define PLAYER_SCORE    2
#define PLAYER_HAND     3

#define LOAD_PLAYER     3
#define BOARD_SIZE      1
#define BOARD_INIT      2
#define TILE_INIT       3
#define CURRENT         4
#define COLOURS         "ROYGBP"
#define SKIP_CHAR       1

using std::string;
using std::ifstream;
using std::stringstream;
using std::vector;

Load::Load(){
}

/*
 * Loads a game from a particular file
 * Ensures the following are loaded: player1, player2, the board state, the tile bag
 * the player whose turn it currently is, the path of the file and the isEOF boolean
 * ensuring that the game can be terminated by entering the END text defined in board
*/

void Load::loadGame(vector<Player*>* playerListPtr, Board** board, 
LinkedList** tileBag, Player** currentPlayer, string path, bool* isEOF) {
    
    string savedGameContent = "",
    boardSize = "",
    playerInfo = "";

    int lineNumber = FIRST_LINE;
    int lineCount = 0;
    bool readingPlayer = true,

    playersRead = false;
    ifstream savedGame;
    savedGame.open(path);
    // Saves the line into SAVEDGAMECONTENT
    while (getline(savedGame, savedGameContent)) {       
        
        size_t isPlayer = savedGameContent.find(',');


        if(readingPlayer) {

            lineCount++;
            playerInfo += savedGameContent + '\n';

            if(lineCount % LOAD_PLAYER == 0) {
                loadPlayers(playerListPtr, playerInfo);
                readingPlayer = false;
                playerInfo = "";
            }

        } else if(isPlayer == std::string::npos && !playersRead) {
            
            readingPlayer = true;
            playerInfo += savedGameContent + '\n';
            lineCount++;

        } else {

            playersRead = true;
        
        }

        
        if (lineNumber == BOARD_SIZE + lineCount) {
            // set board size
            boardSize = savedGameContent;

        } else if (lineNumber == BOARD_INIT + lineCount) {
            *board = new Board(boardSize, savedGameContent);

        } else if (lineNumber == TILE_INIT + lineCount) {
            loadList(tileBag, savedGameContent);

        }  else if (lineNumber == CURRENT + lineCount) {

            for(Player* player : *playerListPtr) {
                if(savedGameContent == player->getPlayerName()) {
                    *currentPlayer = player;
                }
            }

        }  

        ++lineNumber;
    }
}

void Load::loadPlayers(vector<Player*>* playerListPtr,
string playerInfo) {

    std::istringstream info(playerInfo);
    std::string savedGameContent = "";
    int lineNumber = FIRST_LINE;
    Player* player = new Player("", PLAYER_SCORE);

    while (getline(info, savedGameContent)) {
       

        if(lineNumber == PLAYER_NAME) {
            std::cout << "Name: " << savedGameContent << std::endl;
            loadPlayerName(player, savedGameContent);

        } else if (lineNumber == PLAYER_SCORE) {
            std::cout << "Score: " << savedGameContent << std::endl;
            loadPlayerScore(player, savedGameContent);
                
        } else if (lineNumber == PLAYER_HAND) {
            std::cout << "Hand: " << savedGameContent << std::endl;

            LinkedList* hand = player->getHand();
            
            loadList(&hand, savedGameContent);
        }

        lineNumber++;
    }

    playerListPtr->push_back(player);
}

/*
 * Gets a players name and sets it
*/

void Load::loadPlayerName(Player* player, string name){
   player->setPlayerName(name);
}

/*
 * Loads a players score and sets it
*/

void Load::loadPlayerScore(Player* player, string scoreString){
   stringstream ss;
   int score;
   ss << scoreString;
   ss >> score;
   player->setPlayerScore(score);
}

/*
 * Loads a players tiles and adds it to their list
*/

void Load::loadList(LinkedList** listPtr, string tiles){
    string colours(COLOURS);

    size_t commaIndex = tiles.find(',');
    Tile* tile = nullptr;
    LinkedList* list = *listPtr;

    while(commaIndex!=string::npos){
        tile = new Tile(tiles.substr(0,commaIndex));
        list->addBack(tile);
        delete tile;
        tiles = tiles.substr(commaIndex + SKIP_CHAR);
        commaIndex = tiles.find(',');
    }
    tile = new Tile(tiles);
    list->addBack(tile);
    delete tile;
}

/*
 *
 *    MILESTONE 3 ONWARDS
 *
 */