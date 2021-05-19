#include "Load.h"
#include "Tile.h"
#include "Board.h"
#include "Player.h"
#include "TileCodes.h"
#include <string>
#include <fstream>
#include <sstream>
#include <cstring>

#define P1_NAME         1
#define P1_SCORE        2
#define P1_HAND         3
#define P2_NAME         4
#define P2_SCORE        5
#define P2_HAND         6
#define BOARD_SIZE      7
#define BOARD_INIT      8
#define TILE_INIT       9
#define CURRENT         10
#define COLOURS         "ROYGBP"
#define SKIP_CHAR       1

using std::string;
using std::ifstream;
using std::stringstream;

Load::Load(){
}

/*
 * Loads a game from a particular file
 * Ensures the following are loaded: player1, player2, the board state, the tile bag
 * the player whose turn it currently is, the path of the file and the isEOF boolean
 * ensuring that the game can be terminated by entering the END text defined in board
*/

void Load::loadGame(Player* player1, Player* player2, Board** board, 
LinkedList** tileBag, Player** currentPlayer, string path, bool* isEOF){
    
    string savedGameContent = "";
    string boardSize = "";
    int lineNumber = P1_NAME;
    ifstream savedGame;
    savedGame.open(path);
    
    // Saves the line into SAVEDGAMECONTENT
    while (getline(savedGame, savedGameContent)){       

        if (lineNumber == P1_NAME){
            loadPlayerName(player1, savedGameContent);

        } else if (lineNumber == P1_SCORE){
            loadPlayerScore(player1, savedGameContent);
            
        } else if (lineNumber == P1_HAND){
            LinkedList* hand = player1->getHand();
            loadList(&hand, savedGameContent);
            
        } else if (lineNumber == P2_NAME){
            loadPlayerName(player2, savedGameContent);

        } else if (lineNumber == P2_SCORE){
            loadPlayerScore(player2, savedGameContent);

        } else if (lineNumber == P2_HAND){
            LinkedList* hand = player2->getHand();
            loadList(&hand, savedGameContent);

        } else if (lineNumber == BOARD_SIZE){
            // set board size
            boardSize = savedGameContent;
        } else if (lineNumber == BOARD_INIT){
            *board = new Board(boardSize, savedGameContent);
        } else if (lineNumber == TILE_INIT){
            loadList(tileBag, savedGameContent);
        }  else if (lineNumber == CURRENT){
            if (savedGameContent == player1->getPlayerName()){
                *currentPlayer = player1;
            } else if(savedGameContent == player2->getPlayerName()){
                *currentPlayer = player2;
            }
        }  

        ++lineNumber;
    }
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