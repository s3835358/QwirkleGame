
#ifndef ASSIGN2_TILE_H
#define ASSIGN2_TILE_H

// Define a Colour type
typedef char Colour;

// Define a Shape type
typedef int Shape;

#include<string>

class Tile {
public:
   Tile(Colour colour, Shape shape);
   Tile(std::string tile);
   Tile(Tile& other);
   ~Tile();

   Colour getColour();
   Shape getShape();

   void setColour(Colour colour);
   void setShape(Shape shape);
   std::string toString();
   bool equals(Tile* other);
   bool compare(Tile* other);

private:
   Colour colour;
   Shape  shape;
};

#endif // ASSIGN2_TILE_H
