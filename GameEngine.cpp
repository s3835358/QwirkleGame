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
#define AT                 2
#define NOT_FOUND          -1
#define COLOURS            "ROYGBP"
#define END                "^D"
#define QWIRKLE            6
#define ROW                0
#define COLUMN             1
#define MIN_SHAPE          '1'
#define MAX_SHAPE          '6'

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
   Player* p1 = nullptr;
   Player* p2 = nullptr;

   if(!*isEOF){

      cout << " Enter a name for player 1 (uppercase characters only)" << endl;

      p1 = new Player(input->getPlayerName(isEOF), INIT_SCORE);

      if(!*isEOF){

         cout << " Enter a name for player 2 (uppercase characters only)" << endl;
   
         p2 = new Player(input->getPlayerName(isEOF), INIT_SCORE);

         if(!*isEOF){
            cout << "Let's Play!" << endl << endl;
            
            LinkedList* tileBag = new LinkedList();
            tileBag->initTilebag();
         
            initPlayerHand(p1, tileBag);
            initPlayerHand(p2, tileBag);
            
            Board* board = new Board();

            runGame(p1, p2, board, tileBag, p1, isEOF);

            delete p1;
            delete p2;
            delete board;
            delete tileBag;
         }

      } else {
         delete p1;
         delete p2;
      }

   } else {
      delete p1;
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
         Player* p1 = new Player("", INIT_SCORE);
         Player* p2 = new Player("", INIT_SCORE);
         Player* pCurrent = p1;

         LinkedList* tileBag = new LinkedList();
         LinkedList** tileBagPtr = &tileBag;

         Board* board = nullptr;
         Board** boardPtr = &board;

         Load* load = new Load();
         load->loadGame(p1, p2, boardPtr, tileBagPtr,
         &pCurrent, path, isEOF);

         runGame(p1, p2, board, tileBag, pCurrent, isEOF);

         delete p1;
         delete p2;
         delete board;
         delete load;
         delete tileBag;
      }
      
   }

}

/*
 * Runs our main game loop, allowing players to take turns/place tiles etc..
 */
void GameEngine::runGame(Player* p1, Player* p2, Board* board, 
LinkedList* tileBag, Player* pCurrent, bool* isEOF) {

   bool playing = true;

   string p1Name = p1->getPlayerName(),
   p2Name = p2->getPlayerName(); 

   while(playing) {

      displayRound(p1, p2, pCurrent, board);

      bool isSave = runAction(p1, p2, board, tileBag, pCurrent, isEOF);
      
      if(!isSave){
         toggleCurrent(p1, p2, &pCurrent);
      }

      playing = !checkEndGame(p1, p2, tileBag);

      if (*isEOF == true) {
         playing = false;
      }
   }   
}

/*
 * Prints all player names, scores, board and current player hand.
 */
void GameEngine::displayRound(Player* p1, Player* p2,
Player* pCurrent, Board* board) {

   cout << pCurrent->getPlayerName() << ", it's your turn" << endl 
   << "Score for " << p1->getPlayerName() << ": " 
   << p1->getPlayerScore() << endl 
   << "Score for " << p2->getPlayerName() << ": " 
   << p2->getPlayerScore() << endl;

   board->printBoard();
   
   cout << endl << "Your hand is" << endl
   << pCurrent->getHand()->toString() << endl;
}

/*
 * Toggles the current player.
 */
void GameEngine::toggleCurrent(Player* p1, Player* p2,
Player** pCurrent) {

   if(*pCurrent == p1){
      *pCurrent = p2;
   } else{
      *pCurrent = p1;
   }

}

/*
 * Asks our player for input and runs the action taken as input.
 * Returns true if the save action is called.
 */
bool GameEngine::runAction(Player* p1, Player* p2, Board* board, 
LinkedList* tileBag, Player* pCurrent, bool* isEOF){

   bool didRun = false;
   bool isSave = false;

   while(!didRun){
      vector<string> actions = input->getAction(isEOF);

      if (*isEOF) {
         didRun = true;

      } else if (actions.size() == PLACE) {
         
         if (actions.front() == PLACE_STR && actions.at(AT) == "at") {
            didRun = runPlace(actions, board, pCurrent, tileBag);
         }

      } else if (actions.front() == SAVE) {

         didRun = runSave(actions, p1, p2, board, 
         tileBag, pCurrent);

         isSave = true;
         
         cout << "Game successfully saved!" << endl;

      } else if (actions.size() == REPLACE) {

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
bool GameEngine::runSave(vector<string> actions, Player* p1, Player* p2,
Board* board, LinkedList* tileBag, Player* pCurrent){

   ofstream savedGame;
   string savedGameName = input->saveGame(actions);

   bool valid = true;

   if(valid){
      string location = actions.back();
      valid = actions.front() == SAVE;
   }
   
   // putting the information inside the file
   savedGame.open(savedGameName);

   // info to put in:
   // 1. player1's name
   savedGame << p1->getPlayerName() << endl;

   // 2. player1's score
   savedGame << p1->getPlayerScore() << endl;

   // 3. player1's hand
   savedGame << p1->getHand()->toString() << endl;

   // 4. player2's name
   savedGame << p2->getPlayerName() << endl;
   
   // 5. player2's score
   savedGame << p2->getPlayerScore() << endl;
   
   // 6. player2's hand
   savedGame << p2->getHand()->toString() << endl;

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
bool GameEngine::checkEndGame(Player* p1, Player* p2, LinkedList* tileBag) {

   bool gameEnded = false;
   bool emptyHand = p1->getHand()->size() == 0 
   || p2->getHand()->size() == 0;
   bool emptyTiles = tileBag->size() == 0;
   
   if (emptyTiles && emptyHand) {
      cout << "Game Over" << endl;
      cout << "Score for " << p1->getPlayerName() << ":" 
      << p1->getPlayerScore() << endl;
      cout << "Score for " << p2->getPlayerName() << ":" 
      << p2->getPlayerScore() << endl;

      if (p1->getPlayerScore() > p2->getPlayerScore())
      {
         cout << "Player " << p1->getPlayerName() 
         << " won!" << endl; 
      } else if (p1->getPlayerScore() < p2->getPlayerScore()) {
         cout << "Player " << p2->getPlayerName() 
         << " won!" << endl; 
      } else {
         cout << "Game was a draw" << endl;
      }
      gameEnded = true;
   }

   return gameEnded;
}