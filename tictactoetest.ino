#include "math.h" 
//int move =0;
const int empty = 0;
const int x = 1;
const int o = -1;

boolean x_player = true;
boolean o_player = false;
boolean current_player = x_player;

int board[9];

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  startGame();
}

void loop() {
  
  
  while (Serial.available() > 0) {
  prompt();
  int move = getMove();
  Serial.print("the move is: ");
  Serial.println(move);
  if (validMove(move) == false){
    Serial.println("Invalid move");
  }
  else{
  displayBoard();
  //if (gameOver() == true){
    //Serial.println("Game Over");
    //startGame();
  //}
  //else {
  //}
  current_player = !current_player;
  }
  } 
}

void startGame(){
  Serial.println("Start game");
  for(int i = 0; i<9; i++){
    board[i] = empty;
  }
}

int getMove() {
  return Serial.parseInt();
}

void prompt(){
  if (current_player == x_player){
      Serial.println("X's move");
  }
  else if(current_player == o_player){
      Serial.println("O's move");
  }
  else{Serial.println("Bug here");}
}

boolean validMove(int i){
  if (board[i-1] != empty || (i>9 || i<1)){
    return false;
  }
    else if (current_player == x_player){
      board[i-1] = 1;
    }
    else if(current_player == o_player){
      board[i-1]= -1;
    }
  }

void displayBoard(){
  int k = 0;
  for (int r=1; r<4; r++){
    for (int c=1; c<4; c++){
      printCell(board[k]);
      k=k+1;
    }
    Serial.println();
  }
}

void printCell(int cell){
  if (cell == x){
    Serial.print("X");
  }
  else if (cell == o){
    Serial.print("O");
  }
  else if (cell == empty){
    Serial.print("_ ");
  }
}

//boolean gameOver{
  
//}

//boolean swapPlayers{
//  return !current_player;
//}

