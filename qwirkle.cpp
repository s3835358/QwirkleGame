
#include "Input.h"
#include "GameEngine.h"
#include <iostream>
    


#define EXIT_SUCCESS    0
#define START           1
#define LOAD            2
#define CREDITS         3
#define QUIT            4

#define EOF_CHAR        "^D"        

using std::cout;
using std::endl;

void runFeature(Input* input);
void showCredits();
void displayMainMenu();
void quit(bool isEOF);

int main(void) {
   Input* input = new Input();

   displayMainMenu();
   runFeature(input);

   delete input;
   
   return EXIT_SUCCESS;
}

void runFeature(Input* input){
   bool isEOF = false;

   int userInput = input->getFeature(&isEOF);
   
   GameEngine* gameEngine = new GameEngine(input);

   if (userInput == START){    
      gameEngine->startNewGame(&isEOF);

   } else if (userInput == LOAD){
      gameEngine->loadGame(&isEOF);

   } else if (userInput == CREDITS){
      showCredits();
      displayMainMenu();
      runFeature(input);

   } else if (userInput == QUIT){
      quit(isEOF);
   }

   delete gameEngine; 
}

void showCredits(){        
   cout << "Name: Josiah Webster" << endl
   << "Student ID: S3785546" << endl
   << "Email: s3785546@student.rmit.edu.au" << endl;

   cout << "\nName: Declan Baker" << endl 
   << "Student ID: S3835358" << endl
   << "Email: s3835358@student.rmit.edu.au" << endl;

   cout << "\nName: Joanna Tepaa" << endl
   << "Student ID: S3680413" << endl
   << "Email: s3680413@student.rmit.edu.au" << endl;

   cout << "\nName: Shaun Jia Wen Chow" << endl
   << "Student ID: S3699052" << endl
   << "Email: s3699052@student.rmit.edu.au" << endl;
}

void displayMainMenu(){
   cout << "\nWelcome to Qwirkle!" << endl 
      << "------------------\n" << endl;

   cout << "Menu\n" << "----" << endl;

   cout << "1. New Game" << endl << "2. Load Game" << endl 
      << "3. Credits" << endl << "4. Quit" << endl;
}

void quit(bool isEOF) {      
   if(!isEOF){
      cout << "Goodbye" << endl;
   }
   
}

/*
 *
 *    MILESTONE 3 ONWARDS
 *
 */