
#ifndef ASSIGN2_NODE_H
#define ASSIGN2_NODE_H

// Forward Declaration, neccessary?
class Tile;

class Node {
public:

   Node(Tile* tile, Node* next, Node* prev);
   Node(Node& other);
   ~Node();

   Tile* getTile();
   Node* getNext();
   Node* getPrev();
   void setTile(Tile* tile);
   void setNext(Node* next);
   void setPrev(Node* prev);


private:
   Tile*    tile;    
   Node*    next;
   Node*    prev;
};

#endif // ASSIGN2_NODE_H
