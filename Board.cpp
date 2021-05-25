#include "Board.h" 
#include "Tile.h"
#include <iostream> 
#include <iomanip>
#include <fstream>
#include <vector>
#include <sstream>

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
#define MAX_UPPERCASE           'Z'
#define CHARS_SKIPPED           6
#define INIT                    0

using std::vector;
using std::string;
using std::cout;
using std::endl;
using std::to_string;



/*
 *  Our game board. Assigns tiles to a location [row][column].
 *  Default constructor.
 */
Board::Board(int* dimensions) {

    rows = dimensions[ROW_IND];
    columns = dimensions[COL_IND];

    // Loads Board
    for (int row = 0; row < rows; row++) {
        vector<Tile*> rowVec;
        for (int column = 0; column < columns; column++) { 
            rowVec.push_back(nullptr);
        }
        board.push_back(rowVec);
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

    for (int row = 0; row < rows; row++) {
        vector<Tile*> rowVec;
        for (int column = 0; column < columns; column++) { 
            rowVec.push_back(nullptr);
        }
        board.push_back(rowVec);
    } 
    
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
    checkRight = col < columns - TILE,
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
        if((valid || *firstCheck) && other != nullptr) {
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
    char row = locStr[ROW_IND];

    if(row > MAX_UPPERCASE) {
        row = row - CHARS_SKIPPED;
    }

    location[ROW_IND] = row - MIN_CHAR;
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

    string colStr = "",
    rowStr = "";

    if(locStr.size() > COL_IND) {

        colStr = locStr.substr(COL_IND);
        rowStr = locStr.substr(0,COL_IND);

    } else {
        valid = false;
    }

    for(char c : colStr) {

        if(!isdigit(c)){
            valid = false;
        }

    }

    if(valid) {

        int column = stoi(locStr.substr(COL_IND));
        valid =  column < columns && column >= 0;
    
    }

    int rowBound = MIN_CHAR + rows;

    if(rowStr[ROW_IND] > MAX_UPPERCASE) {
        rowBound += CHARS_SKIPPED;
    }

    bool rowInBounds = rowStr[ROW_IND] < (rowBound);

    // If our input is valid we get our location
    if(rowInBounds && valid){
        // We assume our location has been initialised
        delete[] *location;
        *location = getLocation(locStr);
    } else {
        valid = false;
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

        if(rowLabel == 'Z') {
            rowLabel += CHARS_SKIPPED;
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
        
        if(rowLabel == MAX_UPPERCASE) {
            rowLabel += CHARS_SKIPPED;
        }
        
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
 *  Checks the validity of our multiple tile placement, then, if valid,
 *  places our tiles in our board and updates the player score.
 * 
 *  Returns true if tiles are placed succesfully.
 */
bool Board::multiplePlace(vector<int*>* locations, vector<Tile*>* tiles, 
int* score, bool sameRow) {
    
    bool placed = false;

    // Already know locations are within bounds, don't know if 'valid'
    int* min = nullptr;
    int* max = nullptr;
    int rCount = 0,
    cCount = 0,
    dist = 0;

    placed = checkMultiple(locations, tiles, sameRow, &min, &max, &rCount,
    &cCount, &dist);
    
    if(placed) {

        int counter = 0;
        isEmpty = false;

        for(int* location : *locations) {

            int row = location[ROW_IND],
            col = location[COL_IND];
            
            if(board[row][col] == nullptr) {
                
                board[row][col] = tiles->at(counter); 

            } else {

                placed = false;
                clearLocations(locations);
                
            }

            counter++;
        }

        if(placed) {
            multipleScore(locations, score, tiles, rCount, cCount, dist);
        }
        
    }

    return placed;
}

/*
 *  Performs all checks neccessary to validate our multiple tile placements.
 *  
 *  Returns true if our multiple tile placements are valid.
 */
bool Board::checkMultiple(vector<int*>* locations, vector<Tile*>* tiles,
bool sameRow, int** min, int** max, int* rPtr, int* cPtr, int* dist) {
    
    // Find location with 'lowest' value and location with 'highest' value
    findMinMax(locations, min, max, sameRow);
    
    bool reached = false;

    // Are our tiles all connceted to each other
    bool valid = checkLine(min, max, sameRow, &reached, locations,
    tiles, dist);

    if(valid && !isEmpty) {
        valid = checkBoard(locations, tiles, sameRow, &reached, rPtr, cPtr);
    }
    
    bool notConnected = !reached && !isEmpty;
    
    if(notConnected) {
        valid = false;
    }
    
    return valid;
}

/*
 *  Handles all checks related to the 'line' (row or column) our tiles are
 *  placed in.
 * 
 *  Returns true if all checks pass.
 */
bool Board::checkLine(int** minPtr, int** maxPtr, bool sameRow, 
bool* reached, vector<int*>* locations, vector<Tile*>* tiles, int* dist) {

    int* min = *minPtr;
    int* max = *maxPtr;

    if(sameRow) {
        *dist = max[COL_IND] - min[COL_IND];
    } else {
        *dist = max[ROW_IND] - min[ROW_IND];
    }

    int locSize = locations->size();

    // Check that the tiles we are placing are connected
    bool connected = lineConnected(*dist, min, locSize, sameRow);

    if(connected) {

        bool checkGreater = true;

        connected = checkBeyond(min, max, dist, reached, tiles, 
        sameRow, !checkGreater);

        if(connected) {

            connected = checkBeyond(min, max, dist, reached, tiles, 
            sameRow, checkGreater);
        }
    }

    // Checks that our min and max tiles are not placed more than 6 tiles
    // apart.
    if(*dist >= QWIRKLE) {
        connected = false;
    }
    
    return connected;   
}

/*
 *  Handles checks related to all existing tiles on the board not in
 *  the same line as our tiles placed. That is, if our tiles are placed
 *  in a row, our method checks the validity of the placement against
 *  all tiles not in that row. Same for if the tiles are placed within
 *  a column.
 * 
 *  Returns true if all checks pass.
 */
bool Board::checkBoard(vector<int*>* locations, vector<Tile*>* tiles,
bool sameRow, bool* reached, int* rPtr, int* cPtr) {

    int counter = 0,
    locSize = locations->size();

    bool valid = false,
    checking = true;

    while(checking && counter < locSize) {

        Tile* tile = tiles->at(counter);
        int* location = locations->at(counter);
        
        bool connected = false;

        int row = location[ROW_IND],
        col = location[COL_IND];
        
        int rCount = 0,
        cCount = 0;

        connected = checkNeighbours(row, col, tile, sameRow, &rCount, &cCount,
        reached);

        if(rCount > 0) {
            rCount += TILE;
        } else if(cCount > 0) {
            cCount += TILE;
        }
        
        // If one of our tiles are connected our placement is valid
        if(connected) {
            valid = true;
        }

        if(!checkQwirkle(&rCount, &cCount)) {
            valid = false;
            checking = false;
        }

        // Need to assign to rCount then increment to avoid unused variable
        *rPtr += rCount;
        *cPtr += cCount;

        counter++;
    }
    
    // If our placement isn't connected to any tiles but our board is empty,
    // our placement is valid
    if(!valid) {
        valid = isEmpty;
    }
    
    return valid;
}

/*
 *  Checks qwirkle condition and increments score by 6 if player
    completes a line of 6.

 *  Returns false if a line of greater than 6 has been created, 
 *  true otherwise. 
 * 
 *  Note true does NOT neccasraily mean a qwirkle has been achieved.
 */
bool Board::checkQwirkle(int* rCount, int* cCount) {
    bool valid = true;
    
    if(*rCount > QWIRKLE || *cCount > QWIRKLE) {
        
        valid = false;

    } else if (*rCount == QWIRKLE) {
        
        *rCount += QWIRKLE;

        cout << "QWIRKLE!!!" << endl << endl;

    } else if (*cCount == QWIRKLE) {

        *cCount += QWIRKLE;

        cout << "QWIRKLE!!!" << endl << endl;
    }
    return valid;
}

/*
 *  Checks 'beyond' the minimum/maximum tiles placed by the player. The
 *  
 *  The check is limited to the 'line' (row or column) that the tiles
 *  were placed in.
 * 
 *  Sets the bool pointer 'reached' to true if a tile 'beyond' is found.
 *  
 *  Returns true if all tiles found were of the same suit/shape.
 */
bool Board::checkBeyond(int* min, int* max, int* distPtr, bool* reached,
vector<Tile*>* tilesPtr, bool sameRow, bool checkGreater) {

    int row = min[ROW_IND],
    col = min[COL_IND],
    dist = *distPtr;

    if(checkGreater) {
        row = max[ROW_IND],
        col = max[COL_IND];
    }

    vector<Tile*> tiles = *tilesPtr;
    Tile* other = nullptr;

    bool valid = true;

    bool searching = getNextTile(sameRow, &col, &row, checkGreater, &other);

    while(searching) {
        
        if (other != nullptr) {

            for(Tile* tile : tiles) {
                
                if(valid) {
                    valid = tile->compare(other);
                }
            }

            dist++;
            *reached = true;

        } else {
            searching = false;
        }
        
        if(searching) {
            searching = getNextTile(sameRow, &col, &row, checkGreater, &other);
        }
    }

    *distPtr = dist;

    return valid;
}

/*
 *  Increments our row and col values, within our bounds, in a direction
 *  based upon 'checkGreater' and 'sameRow' values, before assigning our 
 *  tile pointer other to the corresponding tile in our board. 
 * 
 *  Note that we may assign other to a nullptr.
 * 
 *  Returns true if we have not reached our board's bound.
 */
bool Board::getNextTile(bool sameRow, int* col, int* row, bool checkGreater,
Tile** other) {
    
    bool searching = true,
    northBound = *row > 0,
    southBound = *row < (rows - TILE),
    eastBound = *col < (columns - TILE),
    westBound = *col > 0;

    if(sameRow && westBound && !checkGreater) {
        
        *col -= TILE;
        *other = board[*row][*col];

    } else if (!sameRow && northBound && !checkGreater) {
        
        *row -= TILE;
        *other = board[*row][*col];

    } else if(sameRow && eastBound && checkGreater) {
        
        *col += TILE;
        *other = board[*row][*col];

    } else if (!sameRow && southBound && checkGreater) {

        *row += TILE;
        *other = board[*row][*col];

    } else {
        searching = false;
    }

    return searching;
}


/*
 *  Compares a tile to it's 'neighbours', that is if our multiple tiles
 *  are placed in a row, we compare our tile to any connecting tiles in
 *  the same column or vice-versa.
 * 
 *  Score is added to our rPtr and cPtr where tiles are found.
 * 
 *  Returns true if a tile with the same shape or colour is found.
 */
bool Board::checkNeighbours(int row, int col, Tile* tile, bool sameRow,
int* rPtr, int* cPtr, bool* reached) {

    bool valid = false;
    int score = 0;

    if(sameRow) {

        // Valid if connected to a tile (of same suit/shape)
        // If no tiles are found we return the existing boolean value
        valid = searchDirection(row, col, tile, &score,
        UP, reached);

        if(valid) {
            valid = searchDirection(row, col, tile, &score,
            DOWN, reached);
        }
        
    } else {

        valid = searchDirection(row, col, tile, &score,
        LEFT, reached);

        if(valid) {
            valid = searchDirection(row, col, tile, &score,
            RIGHT, reached);
        }
    }

    if(sameRow) {
        *cPtr += score;
    } else {
        *rPtr += score;
    }

    return valid;
}

/*
 *  Checks that all tiles placed are connected in a single line.
 *  
 *  Returns true if line is connected.
 */
bool Board::lineConnected(int dist, int* min, int locSize, bool sameRow) {

    int gaps = 0,
    row = min[ROW_IND],
    col = min[COL_IND];

    for(int i = TILE; i < dist; i++) {
        
        if(sameRow) {
            col++;
        } else {
            row++;
        }

        if(board[row][col] == nullptr) {
            gaps++;
        }
    }    
    
    // We have not yet place our tiles, hence we expect some nullptrs
    int toPlace = locSize - TILE - TILE;
    
    bool connected = true,
    missingTiles = (gaps > toPlace);

    if(missingTiles) {
        connected = false;
    }

    return connected;
}

/*
 *  Finds the loactions with the minimum/lowest and maximum/highest row/col
 *  values in our vector. 
 * 
 *  Assumes locations vector isn't empty and that
 *  locations either share a row or a column value.
 * 
 *  Assigns minimum and maximum values to namesake pointers.
 */
void Board::findMinMax(vector<int*>* locations, int** minPtr, 
int** maxPtr, bool sameRow) {

    int* min = locations->at(INIT);
    int* max = locations->at(INIT);

    for(int* location : *locations) {

        int row = location[ROW_IND],
        col = location[COL_IND];
            
        if(sameRow && (col > max[COL_IND])) {
            max = location;

        } else if (sameRow && (col < min[COL_IND])) {
            min = location;

        } else if(!sameRow && (row > max[ROW_IND])) {
            max = location;

        } else if (!sameRow && (row < min[ROW_IND])) {
            min = location;
        }    
    }

    *minPtr = min;
    *maxPtr = max;
}

/*
 *  Checks for any tiles in a direction from a location given by row, column,
 *  any tiles found are compared to 'tile'.
 * 
 *  Returns true if no tiles found or if tiles found share the same colour
 *  or shape.
 */
bool Board::searchDirection(int row, int col, Tile* tile, int* scorePtr, 
int direction, bool* reached) {
    
    Tile* other = board[row][col];
    int count = 0,
    score = 0;

    bool firstCheck = true,
    valid = true;

    while(other != nullptr || count <= TILE){
        
        // firstCheck allows us to init valid = false,
        // once an invalid tile has been found we stop checking

        if((valid || firstCheck) && other != nullptr) {

            valid = tile->compare(other);
            
            firstCheck = false;
            *reached = true;
            score++;
        }

        if(direction == DOWN) {

            row++;

        } else if(direction == UP) {

            row--;

        } else if(direction == RIGHT) {

            col++;

        } else if(direction == LEFT) {

            col--;

        }
        
        // Within loop to allow checking edge but not beyond
        bool inBounds = row >= 0 && col >= 0 && row < rows && col < columns;

        if(inBounds) {
            other = board[row][col];
            
        } else {
            other = nullptr;
        }

        count++;
    }

    // Can't directly increment *scorePtr otherwise 'unused variable warning'
    *scorePtr += score; 

    return valid;
}

/*
 *  Assigns all locations in the board, given by 'locations' to nullptr;
 */
void Board::clearLocations(vector<int*>* locations) {
    
    for(int* location : *locations) {
        
        int row = location[ROW_IND],
        col = location[COL_IND];
        
        board[row][col] = nullptr;
    }
}

/*
 *  Updates the score (via scorePtr) for player following a multiple 
 *  tile placement.
 */
void Board::multipleScore(vector<int*>* locations, int* scorePtr,
vector<Tile*>* tiles, int rCount, int cCount, int dist) {

    int tilesPlaced = tiles->size(),
    score = 0,
    count = 0;

    if(rCount + cCount > 0) {
        // Only for an 'unconnected' placement should we
        // add +1 for a tile
        count++;
    }

    dist += TILE;
    

    while(count < tilesPlaced) {
        
        score += dist;
        dist -= TILE;

        count++;
    }

    score += rCount + cCount;

    *scorePtr += score;
}