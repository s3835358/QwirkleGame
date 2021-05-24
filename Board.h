#ifndef BOARD_H 
#define BOARD_H

#include <iostream>
#include <vector>

class Tile;

class Board { 
public: 
    Board(int* dimensions); 
    Board(std::string size, std::string tiles);
    ~Board();
    
    int getRows();

    int getColumns();

    void printBoard();
    
    bool placeTile(Tile* tile, int row, int col, int* rPtr,
    int* cPtr);

    bool getLocation(std::string locStr, int** location);
    std::string getTiles();

    bool multiplePlace(std::vector<int*>* locations,
    std::vector<Tile*>* tiles, int* score, bool sameRow);


private: 

    int* getLocation(std::string locStr);
    
    bool checkDirection(int row, int col, Tile* tile, int* rCount, 
    int* cCount, bool valid, bool* firstCheck, int direction);

    bool searchDirection(int row, int col, Tile* tile, int* scorePtr, 
    int direction, bool* checked);

    bool isLegalPlace(int row, int col, Tile* tile, int* rPtr, int* cPtr);

    void fill(std::string tiles);

    bool checkMultiple(std::vector<int*>* locations,
    std::vector<Tile*>* tiles, bool sameRow, int** min, int** max,
    int* rPtr, int* cPtr, int* dist);

    bool checkBoard(std::vector<int*>* locations, std::vector<Tile*>* tiles,
    bool sameRow, bool* reached, int* rPtr, int* cPtr);

    int rows;
    
    int columns;

    bool isEmpty;

    void multipleScore(std::vector<int*>* locations, int* scorePtr,
    std::vector<Tile*>* tiles, int rCount, int cCount, int dist);

    void getCount(int tRow, int tCol, int* count, bool isRow);

    bool lineConnected(int dist, int* min, int locSize, bool sameRow);

    bool checkLine(int** minPtr, int** maxPtr, bool sameRow, bool* reached,
    std::vector<int*>* locations, std::vector<Tile*>* tiles, int* dist);

    bool checkNeighbours(int row, int col, Tile* tile, bool sameRow,
    int* rPtr, int* cPtr, bool* reached);

    void findMinMax(std::vector<int*>* locations, int** minPtr, 
    int** maxPtr, bool sameRow);

    bool checkBeyond(int* min, int* max, int* dist, bool* reached,
    std::vector<Tile*>* tiles, bool sameRow, bool checkGreater);

    bool getNextTile(bool sameRow, int* col, int* row, bool checkGreater,
    Tile** other);

    void clearLocations(std::vector<int*>* locations);

    std::vector<std::vector<Tile*>> board;
}; 

 

#endif //BOARD_H 