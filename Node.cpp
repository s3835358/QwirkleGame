
#include "Node.h"
#include "Tile.h"

/*
 * Node constructor, takes a tile, and next previous nodes
*/
Node::Node(Tile* tile, Node* next, Node* prev)
{
   this->tile = tile;
   this->next = next;
   this->prev = prev;
}

/*
 * Second node for comparison
*/

Node::Node(Node& other)
{
   this->tile = other.getTile();
   this->next = other.getNext();
   this->prev = other.getPrev();
}

/*
 * Deconstructor
*/
Node::~Node()
{
   delete tile;
}

/*
 * Relevant getters and setters
*/

Tile* Node::getTile() {
   return tile;
}

Node* Node::getNext() {
   return next;
}

void Node::setTile(Tile* tile) {
   this->tile = tile;
}

void Node::setNext(Node* next) {
   this->next = next;
}

void Node::setPrev(Node* prev) {
   this->prev = prev;
}

Node* Node::getPrev() {
   return prev;
}

/*
 *
 *    MILESTONE 3 ONWARDS
 *
 */
