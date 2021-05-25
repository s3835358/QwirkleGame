#include "GameEngine.h"
#include "Player.h"
#include "LinkedList.h"
#include "Board.h"
#include "Input.h"
#include "Tile.h"
#include "Load.h"
#include <fstream>
#include <cstring>
#include <algorithm>

#define MAXTILEINHAND      6

#define TILE               1
#define TILE_SIZE          2
#define PLACE              4
#define REPLACE            2
#define SAVE               "save"
#define PLACE_STR          "place"
#define REPLACE_STR        "replace"
#define MULTIPLE           "multiple"
#define MULT_TILES         2
#define AT                 2
#define NOT_FOUND          -1
#define COLOURS            "ROYGBP"
#define END                "^D"
#define QWIRKLE            6
#define ROW                0
#define COLUMN             1
#define MIN_SHAPE          '1'
#define MAX_SHAPE          '6'
#define LOCATION           2
#define CHAR               1
#define COL_IND            1 
#define ROW_IND            0
#define PLAYER             1
#define INIT_CURRENT       0
#define SKIP_FIRST         1

using std::vector;
using std::string;
using std::cout;
using std::endl;
using std::ofstream;
using std::ifstream;
using std::cin;

/*
 * Constructor.
 */
GameEngine::GameEngine(Input* input){
   this->input = input;
}

/*
 * Initialises our start game conditions and runs our game.
 */
void GameEngine::startNewGame(bool* isEOF){ 
   cout << "Enter number of players (between 2-4)" << endl;
   
   int numPlayers = input->getOption(isEOF, true);
   
   int* boardDims = nullptr;
    
   if(!*isEOF){

      int playerNum = PLAYER;

      vector<Player*> playerList;
      Player* player = nullptr;

      cout << "Enter board size as <rows>,<columns> ie. 3,3" 
      << " (maximum row/column size of 41)" << endl;

      boardDims = input->getBoardSize(isEOF);

      while(playerNum <= numPlayers && !*isEOF) {
         
         cout << "Enter a name for player " << playerNum 
         << " (uppercase characters only)" << endl;

         player = new Player(input->getPlayerName(isEOF), INIT_SCORE);

         playerList.push_back(player);

         playerNum++;
      }

      if(!*isEOF){
         cout << "Let's Play!" << endl << endl
         << "Note that multiple tile placement is possible, "
         << "use the command 'multiple <tile>@<location>, <tile>@<location>'"
         << endl << endl;
         
         LinkedList* tileBag = new LinkedList();
         tileBag->initTilebag();
      
         for(Player* player : playerList) {
            initPlayerHand(player, tileBag);
         }
                  
         Board* board = new Board(boardDims);

         runGame(&playerList, board, tileBag, playerList.at(INIT_CURRENT),
         isEOF);

         delete board;
         delete tileBag;
      }

      for(Player* player: playerList) {
         delete player;
      }

      delete[] boardDims;
      
   } 
}

/*
 * Calls our Load object to read game conditions from save file
 * and then runs our game.
 */
void GameEngine::loadGame(bool* isEOF) {
   
   cout << "Enter the name of your file" << endl;
   
   string path = input->getFilepath(isEOF);
   
   bool exists = fileExists(path);

   if (!*isEOF){

      while (!exists){
         cout << "File doesn't exist, please enter another file name: "
         << endl;
         
         path = input->getFilepath(isEOF);

         if (!*isEOF){
            exists = fileExists(path);
         } else {
            exists = true;
         } 
      }

      if(!*isEOF){

         vector<Player*> playerList;
         Player* pCurrent = nullptr;

         LinkedList* tileBag = new LinkedList();
         LinkedList** tileBagPtr = &tileBag;

         Board* board = nullptr;
         Board** boardPtr = &board;

         Load* load = new Load();
         load->loadGame(&playerList, boardPtr, tileBagPtr,
         &pCurrent, path, isEOF);
         
         
         runGame(&playerList, board, tileBag, pCurrent, isEOF);

         // Delete Players
         delete board;
         delete load;
         delete tileBag;
      }
      
   }
   
}

/*
 * Runs our main game loop, allowing players to take turns/place tiles etc..
 */
void GameEngine::runGame(vector<Player*>* playerListPtr, Board* board, 
LinkedList* tileBag, Player* pCurrent, bool* isEOF) {

   bool playing = true;

   while(playing) {

      displayRound(playerListPtr, pCurrent, board);

      bool isSave = runAction(playerListPtr, board, tileBag, pCurrent, isEOF);
      
      if(!isSave){
         toggleCurrent(playerListPtr, &pCurrent);
      }

      playing = !checkEndGame(playerListPtr, pCurrent, tileBag);

      if (*isEOF == true) {
         playing = false;
      }
   }   
}

/*
 * Prints all player names, scores, board and current player hand.
 */
void GameEngine::displayRound(vector<Player*>* playerListPtr,
Player* pCurrent, Board* board) {
   
   vector<Player*> playerList = *playerListPtr;

   cout << pCurrent->getPlayerName() << ", it's your turn" << endl;

   for(Player* player : playerList) {
      
      cout << "Score for " << player->getPlayerName() << ": " 
      << player->getPlayerScore() << endl;
   }

   board->printBoard();
   
   cout << endl << "Your hand is" << endl
   << pCurrent->getHand()->toString() << endl;
}

/*
 * Toggles the current player.
 */
void GameEngine::toggleCurrent(vector<Player*>* playerListPtr,
Player** pCurrent) {

   vector<Player*> playerList = *playerListPtr;
   int currentInd = NOT_FOUND,
   index = 0;

   for(Player* player : playerList) {
      if(player == *pCurrent) {
         currentInd = index;
      }
      index++;
   }

   int size = playerList.size() - PLAYER;
   if(currentInd < size) {
      *pCurrent = playerList.at(currentInd + PLAYER);
   } else {
      *pCurrent = playerList.at(INIT_CURRENT);
   }  

}

/*
 * Asks our player for input and runs the action taken as input.
 * Returns true if the save action is called.
 */
bool GameEngine::runAction(vector<Player*>* playerListPtr, Board* board, 
LinkedList* tileBag, Player* pCurrent, bool* isEOF){

   bool didRun = false;
   bool isSave = false;

   while(!didRun){
      vector<string> actions = input->getAction(isEOF);
      int actSize = actions.size();

      if (*isEOF) {
         didRun = true;

      } else if (actions.front() == MULTIPLE && actSize > MULT_TILES) {

         vector<int*> locations;
         vector<Tile*> tiles;

         didRun = runMultiple(&actions, board, &locations, &tiles, pCurrent,
         tileBag);

         for(int* location:locations) {
            delete[] location;
         }

      } else if (actions.size() == PLACE) {
         
         if (actions.front() == PLACE_STR && actions.at(AT) == "at") {
            didRun = runPlace(actions, board, pCurrent, tileBag);
         }

      } else if (actions.front() == SAVE) {

         didRun = runSave(actions, pCurrent, playerListPtr, board, 
         tileBag);

         isSave = true;
         
         cout << "Game successfully saved!" << endl;

      } else if (actSize == REPLACE) {

         if(actions.front() == REPLACE_STR) {
            didRun = runReplace(actions, board, pCurrent, tileBag);
         }

      } 

      if(!didRun) {
         
         cout << "Invalid Input";
         std::cin.clear();
      }  

   }
   return isSave;
}

/*
 * Runs the 'place <tile> at <location>' command.
 * Returns true if command runs succesfully.
 */
bool GameEngine::runPlace(vector<string> actions, Board* board, 
Player* player, LinkedList* tileBag) {

   bool qwirkle = false, valid = false;
   int rCount = 0, cCount = 0; 
   
   string locStr = actions.back();
   int* location = new int[2];
   LinkedList* hand = player->getHand();
   Tile* tile = getTile(actions.at(TILE));
   
   // Checks tile is in player hand && a valid location
   // If so (row,col) is assigned to 'location'
   if(tile != nullptr) {

      valid = (hand->find(tile) != NOT_FOUND)
      && board->getLocation(locStr, &location);
   
   }

   // If our checks above pass then we attempt to place tile
   if(valid){
      
      valid = board->placeTile(tile, location[ROW], location[COLUMN],
      &rCount, &cCount);
      
      // If tile was placed successfully we update score, 
      // remove tile from hand and refill from tilebag

      if(valid){
         updateScore(rCount, cCount, &qwirkle, player);
         hand->removeTile(tile);
         
         if(qwirkle){
            cout << "QWIRKLE!!!" << endl << endl;
         }
         
         hand->popFrom(tileBag);
      } 
   }
   
   if(tile != nullptr){
      delete tile;
   }

   delete[] location;
   return valid;
}


/*
 * Runs the 'replace <tile>' command.
 * Returns true if command runs succesfully.
 */
bool GameEngine::runReplace(vector<string> actions, Board* board, 
Player* player, LinkedList* tileBag){
   
   bool valid = false;
   Tile* tile = getTile(actions.at(TILE));
   
   // Check tile exists
   if(tile != nullptr){

      LinkedList* hand = player->getHand();

      // True if player's hand contains tile
      valid = (hand->find(tile) != NOT_FOUND);
         
      if(valid) {

         hand->removeTile(tile);

         // Get a tile from bag and add to hand
         hand->popFrom(tileBag);
      }

      delete tile;
   }

   return valid;
}

/*
 * Runs the 'save <filename.txt>' command.
 * Returns true if command runs succesfully.
 */
bool GameEngine::runSave(vector<string> actions, Player* pCurrent, 
vector<Player*>* playerListPtr, Board* board, LinkedList* tileBag) {
   

   ofstream savedGame;
   string savedGameName = input->saveGame(actions);

   bool valid = true;

   if(valid){
      string location = actions.back();
      valid = actions.front() == SAVE;
   }
   
   // putting the information inside the file
   savedGame.open(savedGameName);

   for(Player* player : *playerListPtr) {
      savedGame << player->getPlayerName() << endl;
      savedGame << player->getPlayerScore() << endl;
      savedGame << player->getHand()->toString() << endl;
   }

   // 7. board shape
   // not needed for milestone 1 & 2?
   savedGame << board->getRows() << "," << board->getColumns() << endl;

   // 8. board state
   savedGame << board->getTiles() << endl;

   // 9. tile bag contents
   savedGame << tileBag->toString() << endl;

   // 10. current player name
   savedGame << pCurrent->getPlayerName();

   savedGame.close();

   return valid;
}

/*
 * Initialises the hand for 'player'. Takes tiles from 'tilebag'.
 */
void GameEngine::initPlayerHand(Player* player, LinkedList* tilebag)
{
   for (int i = 0; i < MAXTILEINHAND; i++)
   {
      player->getHand()->popFrom(tilebag);
   }
 
}

/*
 * Returns true if file at given path exists.
 */
bool GameEngine::fileExists(string path){
   ifstream file(path.c_str());
   return (bool) file;
}

/*
 * If the input is valid, returns the tile described by tileStr.
 * Otherwise returns a nullptr.
 */
Tile* GameEngine::getTile(string tileStr){
   Tile* tile = nullptr;
   bool valid = false;
   string colours(COLOURS);

   if(tileStr.size() == TILE_SIZE){
      valid = colours.find(tileStr.front())!=string::npos
      && tileStr.back() >= MIN_SHAPE
      && tileStr.back() <= MAX_SHAPE;
   }

   if(valid) {
      tile = new Tile(tileStr);
   }

   return tile;
}

/*
 * Computes and updates the score for 'player' for a given round.
 */
void GameEngine::updateScore(int rCount, int cCount, bool* qwirkle,
Player* player){
   
   int score = player->getPlayerScore();
   
   if(rCount == QWIRKLE){
      score += QWIRKLE;
      *qwirkle = true;
   }

   if(cCount == QWIRKLE){
      score += QWIRKLE;
      *qwirkle = true;
   }
   
   // As rCount = TILE by default
   if(rCount != TILE){
      score += rCount;
   }

   // As cCount = TILE by default
   if(cCount != TILE){
      score += cCount;
   }

   // When first tile on board is placed
   if(score == 0){
      score += TILE;
   }

   player->setPlayerScore(score);
}

/*
 * Returns where the tilebag is empty and at least one player hand is empty.
 * If true, prints out player names, scores and winner.
 */
bool GameEngine::checkEndGame(vector<Player*>* playerListPtr, 
Player* pCurrent, LinkedList* tileBag) {

   bool gameEnded = false,
   emptyHand = true;
   vector<Player*> playerList = *playerListPtr;

   // MAY NOT WORK
   for(Player* player : playerList) {
      if(emptyHand) {
         int handSize = player->getHand()->size();
         emptyHand = handSize == 0;
      }
   }
   
   bool emptyTiles = tileBag->size() == 0;
   
   if (emptyTiles && emptyHand) {
      cout << "Game Over" << endl;

      for(Player* player : playerList) {
         
         cout << "Score for " << player->getPlayerName() << ": " 
         << player->getPlayerScore() << endl;
      }
      
      // TODO WHO WINS?
      
      
      gameEnded = true;
   }

   return gameEnded;
}

/*
 *  Overseas the lifespan of the multiple tile placement command.
 *
 *  Returns true if tile placement executes successfully.
 */
bool GameEngine::runMultiple(vector<string>* actionsPtr, Board* board, 
vector<int*>* locationsPtr, vector<Tile*>* tilesPtr, Player* player,
LinkedList* tileBag) {

   bool sameRow = false;
   bool valid = checkMultiple(actionsPtr, board, locationsPtr, tilesPtr,
   &sameRow);

   if(valid) {

      int score = player->getPlayerScore();

      valid = board->multiplePlace(locationsPtr, tilesPtr, &score, sameRow);

      if(valid) {

         player->setPlayerScore(score);
         
         refillHand(actionsPtr, player, tileBag, tilesPtr);

      }
   }

   return valid;
}

/*
 * Checks that a multiple tile placement command contains valid
 * locations and tiles. Loads into appropriate vector data structures.
 * 
 * Returns true if command is valid and therefore loads correctly.
 */
bool GameEngine::checkMultiple(vector<string>* actions, Board* board,
vector<int*>* locations, vector<Tile*>* tiles, bool* sameRow) {
   
   int count = SKIP_FIRST;

   string action = actions->at(count); 
   
   size_t comma = action.find(','); 

   bool actionExists = (comma != string::npos),
   final = false,
   isLoc = true;

   while(actionExists && isLoc) {

      actionExists = prepareVectors(actions, board, locations, tiles, 
      &count, final, &isLoc);

   }

   if(isLoc) {
      final = true;

      actionExists = prepareVectors(actions, board, locations, tiles, &count, 
      final, &isLoc);
   }

   if(isLoc) {
      
      isLoc = sameLine(locations, tiles, sameRow);

   }
   
   return isLoc;
}

/*
 * Takes the user input 'actions' and loads the vectors 'locations' and
 * 'tiles' with their namesakes.
 * 
 * Returns true is locations and tiles are valid locations and tiles.
 */
bool GameEngine::prepareVectors(vector<string>* actions, Board* board,
vector<int*>* locations, vector<Tile*>* tiles, int* countPtr, 
bool final, bool* isLoc) {

   int* location = new int[LOCATION];
   int count = *countPtr;

   string action = actions->at(count);
   size_t at = action.find('@'),
   comma = action.find(',');

   int start = (at + CHAR), 
   end = (comma - at - CHAR);
   
   string locStr = action.substr(start, end),
   tileStr = action.substr(0,at);

   *isLoc = board->getLocation(locStr, &location);

   Tile* tile = getTile(tileStr);

   if(*isLoc && tile != nullptr) {

      locations->push_back(location);
      tiles->push_back(tile);

   } else {

      delete tile;
      delete[] location;
      *isLoc = false;

   }

   bool actionExists = false;

   if(!final) {
      count++;

      action = actions->at(count);
      comma = action.find(',');

      actionExists = (comma != string::npos);

   }
   
   *countPtr = count;

   return actionExists;
}

/*
 * Removes 'tiles' from player hand and given tile bag is not empty,
 * hand is refilled.
 */
void GameEngine::refillHand(vector<string>* actionsPtr, Player* player,
LinkedList* tileBag, vector<Tile*>* tilesPtr) {

   vector<Tile*> tiles = *tilesPtr;
   vector<string> actions = *actionsPtr;

   for(Tile* tile : tiles) {
      player->getHand()->removeTile(tile);
   }

   int popped = actions.size() - TILE;

   for(int i = 0; i < popped; i++) {
      player->getHand()->popFrom(tileBag);
   }
}

/*
 * For a multiple tile placement, checks that all tiles being placed are
 * being placed in the same line and of the same shape or colour.
 * 
 * Returns true if placement meets these conditions.
 */
bool GameEngine::sameLine(vector<int*>* locations,
vector<Tile*>* tiles, bool* sameRow) {

   int size = locations->size(),
   counter = 0;

   int* prevInt = nullptr;
   int* location = nullptr;

   Tile* tile = nullptr;
   Tile* prevTile = nullptr;

   bool sameCol = false,
   sameLine = true;

   while(counter < size) {
      prevInt = location;
      prevTile = tile;
      
      location = locations->at(counter);
      tile = tiles->at(counter);

      if(prevInt != nullptr) {

         if(prevInt[ROW_IND] == location[ROW_IND]) {
            *sameRow = true;
         } 
         
         if(prevInt[COL_IND] == location[COL_IND]) {
            sameCol = true;
         }

         if(!prevTile->compare(tile)) {
            sameLine = false;
         }

         if(sameCol == *sameRow) {
            sameLine = false;
         }
      } 

      counter++;
   }

   return sameLine;
}
