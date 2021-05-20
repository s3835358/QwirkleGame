#include "Board.h" 
#include "Tile.h"
#include <iostream> 
#include <iomanip>
#include <fstream>
#include <vector>
#include <sstream>

#define DEFAULT_ROWS            26 
#define DEFAULT_COLUMNS         26
#define COL_IND                 1 
#define TILE                    1
#define ROW_IND                 0
#define SKIP_CHAR               1
#define QWIRKLE                 6
#define DOWN                    0
#define UP                      1
#define RIGHT                   2
#define LEFT                    3
#define MIN_CHAR                'A'

using std::vector;
using std::string;
using std::cout;
using std::endl;
using std::to_string;

vector<vector<Tile*>> board (DEFAULT_ROWS, vector<Tile*> (DEFAULT_COLUMNS));

/*
 *  Our game board. Assigns tiles to a location [row][column].
 *  Default constructor.
 */
Board::Board() {

    rows = DEFAULT_ROWS;
    columns = DEFAULT_COLUMNS;

    // Loads Board
    for (int row = 0; row < rows; row++) { 
        for (int column = 0; column < columns; column++) { 
            board[row][column] = nullptr;
        }
        
    }    
    isEmpty = true;
} 

/*
 *  Our game board. Assigns tiles to a location [row][column].
 *  Overloaded constructor creates a board based on input from save file.
 *  Parameters 'size' and 'tiles' are read from our save file before
 *  being passed to our constructor.
 */
Board::Board(string size, string tiles){
    size_t comma = size.find(',');
    rows = stoi(size.substr(0,comma));

    columns = stoi(size.substr(comma + SKIP_CHAR));
    
    isEmpty = true;
    if(tiles.size() > 0){
        fill(tiles);
        isEmpty = false;
    }
    
}

/*
 *  Create and assign tiles to a location in board[row][col].
 *  Tiles/locations are read from parameter 'tiles' in the
 *  format TILE@LOCATION, TILE@LOCATION...
 */
void Board::fill(string tiles) {
    
    int* location = nullptr;
    Tile* tile = nullptr;

    size_t atIndex = tiles.find('@');
    size_t commaIndex = tiles.find(',');
    string locStr = "";
    
    while(commaIndex!=string::npos){
        
        locStr = tiles.substr(atIndex + SKIP_CHAR, commaIndex - atIndex); 
        location = getLocation(locStr);

        tile = new Tile(tiles.substr(0,atIndex));
        
        board[location[0]][location[1]] = tile;
        tiles = tiles.substr(commaIndex + SKIP_CHAR + SKIP_CHAR);
        atIndex = tiles.find('@');
        commaIndex = tiles.find(',');
        delete[] location;
    }
    
    locStr = tiles.substr(atIndex + SKIP_CHAR, commaIndex - atIndex);
    location = getLocation(locStr);
    tile = new Tile(tiles.substr(0,atIndex));
    board[location[0]][location[1]] = tile;

    delete[] location;

}

// Destructor
Board::~Board(){
    for (int row = 0; row < rows; row++){
        for (int column = 0; column < columns; column++){
            if (board[row][column] != nullptr){
                delete board[row][column];
            }
        }
    }
}

/*
 *  If legal/valid, 'tile' is assigned to board['row']['col'].
 *
 *  'rPtr' and 'cPtr' count the tiles traversed during our checks,
 *  used by GameEngine to updateScores and check for QWIRKLE
 */
bool Board::placeTile(Tile* tile, int row, int col, int* rPtr,
int* cPtr){
    bool valid = false;

    if(board[row][col] == nullptr){
        valid = isLegalPlace(row, col, tile, rPtr, cPtr);
    }

    if(isEmpty){
        valid = true;
    }

    if(valid){
        board[row][col] = new Tile(*tile);
        isEmpty = false;
    }
    return valid;
}

/*
 *  Checks that our tile:
 *  
 *  1. Is either the first tile place on the board,
 *  OR connected to atleast one tile of the same colour or shape
 * 
 *  2. Is not connected to any tiles that don't share colour or shape
 * 
 *  3. Won't create a connected line of tiles > 6 
 * 
 *  Note: 'rPtr' and 'cPtr' count the tiles traversed during our checks,
 *  used by GameEngine to updateScores and check for QWIRKLE
 */

bool Board::isLegalPlace(int row, int col, Tile* tile,
int* rPtr, int* cPtr){

    bool valid = false,
    firstCheck = true,
    checkDown = row < rows - TILE,
    checkUp = row != 0,
    checkRight = col < columns,
    checkLeft = col != 0;

    int rCount = TILE,
    cCount = TILE;

    if(checkDown) {
        valid = checkDirection(row + TILE, col, tile, &rCount, &cCount, 
        valid, &firstCheck, DOWN);
    }
    
    if(checkUp) {
        valid = checkDirection(row - TILE, col, tile, &rCount, &cCount,
        valid, &firstCheck, UP);
    }
    
    if(checkRight) {
        valid = checkDirection(row, col + TILE, tile, &rCount, &cCount,
        valid, &firstCheck, RIGHT);
    }

    if(checkLeft) {
        valid = checkDirection(row, col - TILE, tile, &rCount, &cCount,
        valid, &firstCheck, LEFT);
    }
    
    if(rCount > QWIRKLE || cCount > QWIRKLE){
        valid = false;
    }

    *rPtr = rCount;
    *cPtr = cCount;
    
    return valid;
}

/*
 *  Checks the 'direction' given for the conditions 
 *  described in isLegalPlace().
 */
bool Board::checkDirection(int row, int col, Tile* tile, int* rPtr, 
int* cPtr, bool valid, bool* firstCheck, int direction) {
    
    Tile* other = board[row][col];
    int rCount = 0,
    cCount = 0;

    while(other != nullptr){
        
        // firstCheck allows us to init valid = false,
        // once an invalid tile has been found we stop checking
        if(valid || *firstCheck) {
            valid = tile->compare(other);
            *firstCheck = false;
        }

        if(direction == DOWN) {
            rCount++;
            row++;
        } else if(direction == UP) {
            rCount++;
            row--;
        } else if(direction == RIGHT) {
            cCount++;
            col++;
        } else if(direction == LEFT) {
            cCount++;
            col--;
        }
        
        // Within loop to allow checking edge but not beyond
        if(row >= 0 && col >= 0 && row < rows && col < columns) {
            other = board[row][col];
        } else {
            other = nullptr;
        }
    }

    *rPtr += rCount;
    *cPtr += cCount;

    return valid;
}


/*
 *  Gets a location that is known to exist from 'locStr'
 *  Returns an int* of size 2 containing {row,col}
 */
int* Board::getLocation(string locStr){
    int* location = new int[2];
    location[ROW_IND] = locStr[ROW_IND] - MIN_CHAR;
    location[COL_IND] = stoi(locStr.substr(COL_IND));
    return location;
}

/*
 *  Checks if the location from 'locStr' exists. 
 *
 *  If so assigns an int* of size 2 containing {row,col} to
 *  the pointer 'location'.
 * 
 *  Returns true if the location exists.
 */
bool Board::getLocation(string locStr, int** location){
    bool valid = true;

    if(locStr.size() > COL_IND){

        for(char c : locStr.substr(COL_IND)){
            if(!isdigit(c)){
                valid = false;
            }
        }

        if(valid){
            int column = stoi(locStr.substr(COL_IND));
            valid =  column < columns && column >= 0;
        }
        
    } else {
        valid = false;
    }

    // If our input is valid we get our location
    if(valid){
        // We assume our location has been initialised
        delete[] *location;
        *location = getLocation(locStr);
    }

    return valid;
}

/*
 *  Creates a string representation of all tiles in our board, 
 *  in the format TILE@LOCATION, TILE@LOCATION...
 *  
 *  Returns a string 'tiles'.
 */
string Board::getTiles(){
    string tiles = "";
    char rowLabel = MIN_CHAR;
    char colLabel = 0;

    for (int row = 0; row < rows; row++) {

        for (int column = 0; column < columns; column++) {
            if (board[row][column] != nullptr) {
                tiles += board[row][column]->toString() + "@" + rowLabel;
                tiles += to_string(colLabel) + "," + " ";
            }

            colLabel++;
        }

        colLabel = 0;
        rowLabel++;
    }
    
    if(tiles.size() > SKIP_CHAR + SKIP_CHAR) {
        // Removes space character
        tiles.pop_back();
        // Removes comma
        tiles.pop_back();
    }
    

    return tiles;
}

int Board::getRows() {
    return rows;
}

int Board::getColumns() {
    return columns;
}

/*
 *  Prints our board and the tiles stored in it. 
 */
void Board::printBoard() {
    // Prints initial layout (first two rows)
    cout << endl << "  "; 
    
    for(int colLabel = 0; colLabel < columns; colLabel++){
        cout << std::setw(3) << colLabel;
    }
    cout << endl << "   ";
    for(int colLabel = 0; colLabel < columns; colLabel++){
        cout << "---";
    }
    cout << endl;
    char rowLabel = MIN_CHAR;
    
    // For each row, we print the label, grid, and any tiles within
    for (int row = 0; row < rows; row++) {
        cout << rowLabel << "  " << "|";     
        rowLabel++;

        for (int column = 0; column < columns; column++) {
            
            if (board[row][column] == nullptr) { 
                cout << "  ";
            } else {
                cout << board[row][column]->toString();
            }
            cout << "|";   
        } 
        cout << endl; 
    }   
}

/*
 *
 *  MILESTONE 3 ONWARDS
 * 
 */

bool Board::multiplePlace(vector<int*>* locationsPtr, vector<Tile*>* tilesPtr) {
    bool placed = false;

    // tiles are Y6, locations are A2, G23

    // Already know locations are within bounds, don't know if 'valid'
    placed = checkMultiple(locationsPtr, tilesPtr);
    if(!placed) {
        deleteMultiple(locationsPtr);
    }

    return placed;
}

void Board::deleteMultiple(vector<int*>* locationsPtr) {
    vector<int*> locations = *locationsPtr;
    Tile* tile = nullptr;
   
    for(int* location : locations) {
        int row = location[ROW_IND];
        int col = location[COL_IND];
        
        tile = board[row][col];
        board[row][col] = nullptr;
        delete tile;
    }
}

bool Board::checkMultiple(vector<int*>* locationsPtr,
vector<Tile*>* tilesPtr) {
    bool valid = true;
    
    vector<int*> locations = *locationsPtr;
    vector<Tile*> tiles = *tilesPtr;
    int i = 0;
    int rCount = 0,
    cCount = 0;

    Tile* tile = nullptr;

    // Place tiles first as we don't ask user for tiles in any specific order
    for(int* location : locations) {
        int row = location[ROW_IND];
        int col = location[COL_IND];
        
        // Should vector be initialised as vector<Tile*> instead?
        // Assume locations & tiles are same size, haven't checked tile validity
        Tile* tile = tiles.at(i);

        if(board[row][col] == nullptr){
            board[row][col] = tile;
        } else {
            delete tile;
            valid = false;
        }

        i++;
    }

    // Then check validity
    if(valid) {

        for(int* location : locations) {
            // Better to use while(valid) ?
            if(valid) {
                int row = location[ROW_IND];
                int col = location[COL_IND];
                tile =  board[row][col];
                valid = isLegalPlace(row, col, tile, &rCount, &cCount);
            }
        }
    }



    // Do something with rCount + cCount
    // Alter scoring for multiple

    /*
     * Added G2, G5 and Y5
     * |  |  |  |  |  |  |  |
     * |  |  |  |  |  |  |  |
     * |  |Y3|G3|  |  |  |  | 
     * |  |  |G2|  |  |  |  | 
     * |  |  |G5|Y5|  |  |  |
     * |  |  |  |  |  |  |  |
     * |  |  |  |  |  |  |  |
     * |  |  |  |  |  |  |  |   
     * 
     * Y5 2pts
     * G5 5pts
     * G2 3pts
     */


    cout << "Check: " << valid << endl;

    return valid;
}

/*Board::Board(Board &other) {
    rows = other.getRows();
    columns = other.getColumns();
    isEmpty = true;
    Tile* tile = nullptr;

    for (int row = 0; row < rows; row++) { 
        for (int column = 0; column < columns; column++) { 

            board[row][column] = nullptr;
            tile = other.getTile(row, column);
            
            if(tile != nullptr) {
                board[row][column] = new Tile(*tile);
                cout << tile;
                cout << board[row][column];
                isEmpty = false;
            }
        } 
    }    
}

Tile* Board::getTile(int row, int col) {
    return board[row][col];
}*/