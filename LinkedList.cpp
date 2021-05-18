#include "LinkedList.h"
#include "Node.h"
#include "Tile.h"
#include "TileCodes.h"
#include <random>
#include <array>

#define COMBINATIONS       36
#define OPTIONS            6
#define REPEATS            2
#define MIN_SHAPE          1
#define NOT_FOUND          -1

using std::random_device;
using std::uniform_int_distribution;
using std::string;

// Constructor
LinkedList::LinkedList() {
    head = nullptr;
    tail = nullptr;
}

//  Destructor
LinkedList::~LinkedList(){
    Node* counter = head;
    Node* next = nullptr;

    while(!(counter==nullptr)){
        next = counter->getNext();
        delete counter;
        counter = next;
    }
}

/*
 *  Returns the node at a give index, if it doesn't exists, returns nullptr.
 */  
Node* LinkedList::get(int index){
    Node* target = head;

    for(int i = 0; i < index; i++){
        target = target->getNext();
    }

    // May return nullptr
    return target;
}

/*
 *  Returns the Tile at a given index. Assumes it exists.
 */ 
Tile* LinkedList::getTile(int index){
    return get(index)->getTile();
}

int LinkedList::size(){
    int count = 0;
    Node* counter = head;

    while(!(counter==nullptr)){
        counter = counter->getNext();
        count++;
    }
    
    return count;
}

/*
 *  Adds a deep copy of the tile to the back of the list.
 */ 
void LinkedList::addBack(Tile* other){
    Tile* tile = new Tile(*other);
    Node* node = new Node(tile, nullptr, nullptr);
    addBack(node);
}

/*
 *  Adds node to back of list.
 */ 
void LinkedList::addBack(Node* node){
    if(tail == nullptr){
        head = node;
        node->setPrev(nullptr);

    } else {
        node->setPrev(tail);
        tail->setNext(node);
    }
    
    node->setNext(nullptr);
    tail = node;
}

/*
 *  Returns node at the front of list. Removes from list.
 */ 
Node* LinkedList::popFront(){
    Node* toPop = head;

    if(head != nullptr){
        head = head->getNext();

        if(head == nullptr){
            tail = nullptr;

        } else {
            head->setPrev(nullptr);
        }
    }
    return toPop;
}

/*
 *  Pops from 'other' and adds to the back of this list.
 */ 
bool LinkedList::popFrom(LinkedList* other){
    Node* node = other->popFront();   
    
    // To indicate other is empty
    bool notNull = false;

    if(node != nullptr){
        addBack(node);
        notNull = true;
    }

    return notNull;
}


void LinkedList::removeTile(Tile* tile) {

    int index = find(tile);

    if(index != NOT_FOUND){
        Node* target = get(index);
        Node* next = target->getNext();
        Node* prev = target->getPrev();
        
        if(target == tail){
            tail = prev;
        } else {
            next->setPrev(prev);
        }

        if(target == head){
            head = next;
        } else {
            prev->setNext(next);
        }
        
        delete target;
    }
}

/*
 *  Searches for 'tile' and if found, returns the index of said tile.
 *  Otherwise returns -1. If duplicates of the tile exist, the index
 *  of the first copy found are returned.
 */ 
int LinkedList::find(Tile* tile) {
    int index = NOT_FOUND;
    bool found = false;
    Node* target = head;

    while(target!=nullptr && !found){
        index++;
        found = tile->equals(target->getTile());
        target = target->getNext();
    }

    if(!found){
        index = NOT_FOUND;
    }
    return index;
}

/*
 *  Initialises list as a tilebag. Fills with all tiles needed for game.
 */ 
void LinkedList::initTilebag()
{
    // Create random device
     random_device rd;
    uniform_int_distribution<int> uniform_dist(0, COMBINATIONS*REPEATS - 1);

    char colours[] = {RED, ORANGE, YELLOW, GREEN, BLUE, PURPLE};

    // We want two of each tile, hence combination*repeats
    Tile* tileArr[COMBINATIONS*REPEATS] = {0};

    // Create all possible tiles
    for(int colour = 0; colour < OPTIONS*REPEATS; colour++) {

        for(int shape = 0; shape < OPTIONS; shape++) {
            int index = colour*OPTIONS + shape;
            char c_colour = colours[colour%OPTIONS];
            tileArr[index] = new Tile(c_colour,(shape + MIN_SHAPE));
        }
    }

    int i = 0,
    toAdd = COMBINATIONS*REPEATS,
    tIndex = 0;

    // Add tiles to tileBag in a random order
    while(i < toAdd){
        bool added = false;
        tIndex = uniform_dist(rd);

        if(tileArr[tIndex] != nullptr) {
            addBack(tileArr[tIndex]);
            added = true;
            i++;
        }

        if(added){
            delete tileArr[tIndex];
            tileArr[tIndex] = nullptr;
        }
    }
}


string LinkedList::toString(){
    string str = "";

    for(int i = 0; i < size(); i++) {
        str += getTile(i)->toString() + ",";
    }

    if(str.size() > 0){
        str.pop_back();
    }

    return str;
}