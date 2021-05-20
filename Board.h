#ifndef BOARD_H 
#define BOARD_H

#include <iostream>
#include <vector>

class Tile;

class Board { 
public: 
    Board(); 
    Board(std::string size, std::string tiles);
    ~Board();
    
    int getRows();

    int getColumns();

    void printBoard();
    
    bool placeTile(Tile* tile, int row, int col, int* rPtr,
    int* cPtr);

    bool getLocation(std::string locStr, int** location);
    std::string getTiles();

    bool multiplePlace(std::vector<int*>* locationsPtr,
    std::vector<Tile*>* tilesPtr);


private: 

    int* getLocation(std::string locStr);
    
    bool checkDirection(int row, int col, Tile* tile, int* rCount, 
    int* cCount, bool valid, bool* firstCheck, int direction);

    bool isLegalPlace(int row, int col, Tile* tile, int* rPtr, int* cPtr);

    void fill(std::string tiles);

    bool checkMultiple(std::vector<int*>* locationsPtr,
    std::vector<Tile*>* tilesPtr);

    int rows;
    
    int columns;

    bool isEmpty;

    void deleteMultiple(std::vector<int*>* locationsPtr);
}; 

 

#endif //BOARD_H 