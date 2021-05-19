
#include "Tile.h"
#include<string>

#define COLOUR      0
#define SHAPE       1
#define MIN_C_INT   '0'

using std::string;
using std::to_string;

/*
 * Tile constructor, takes a colour and shape
*/
Tile::Tile(Colour colour, Shape shape){
    this->colour = colour;
    this->shape = shape;
}

/*
 * Assumes tile string is of size 2, and contains valid colour + valid shape
*/ 

Tile::Tile(string tile){
    this->colour = tile[COLOUR];
    this->shape = (int) tile[SHAPE] - MIN_C_INT;
}

/*
 * Second tile for comparison
*/

Tile::Tile(Tile& other){
    this->colour = other.colour;
    this->shape = other.shape;
}

Tile::~Tile(){

}

/*
 * Relevant getters and setters
*/

Colour Tile::getColour(){
    return colour;
}

Shape Tile::getShape(){
    return shape;
}

void Tile::setColour(Colour colour){
    this->colour = colour;
}

void Tile::setShape(Shape shape){
    this->shape = shape;
}

/*
 * toString method for tile
*/
string Tile::toString(){
    return colour + to_string(shape);
}

/*
 * Checks if a tile is equal to another tile
*/
bool Tile::equals(Tile* other){
    return other->toString() == toString();
}

/*
 * True if tiles either share colour or shape
*/
bool Tile::compare(Tile* other){
    bool valid = false;
    if(other != nullptr){
        valid = (other->getShape() == shape) 
        || (other->getColour() == colour);
    }
    return valid;
}

/*
 *
 *    MILESTONE 3 ONWARDS
 *
 */