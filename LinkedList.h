#ifndef LINKEDLIST
#define LINKEDLIST

#include <string>

class Tile;
class Node;

class LinkedList{
public:
    LinkedList();
    ~LinkedList();

    int size();

    void addBack(Tile* other);
    void addBack(Node* node);
    Node* popFront();
    bool popFrom(LinkedList* other);
    Tile* getTile(int index);
    int find(Tile* tile);
    void removeTile(Tile* tile);
    void initTilebag();
    std::string toString();

private:
    Node* get(int index);
    Node* head;
    Node* tail;
    
};


#endif //LINKEDLIST