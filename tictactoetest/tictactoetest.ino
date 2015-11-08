#include "math.h"
#include <IRremote.h>

int input_pin = 8; //set D10 as input signal pin
IRrecv irrecv(input_pin);
decode_results signals;

int move;
const int empty = -1;
const int x = 1;
const int o = 0;

const int win = 1;
const int draw = 0;
const int game_cont = -1;

boolean current_player = x;

int board[9];

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  irrecv.enableIRIn(); // enable input from IR receiver
  startGame();
}

void loop() {

  //while (Serial.available() > 0) {
    
    int move = take_move_from_ir();
    Serial.print("the move is: ");
    Serial.println(move);
    if (validMove(move) == false) {
      Serial.println("Invalid move");
    }
    else {
      displayBoard();
      int outcome = move_outcome();
      if (outcome == win) {
        Serial.println("Game Over");
        winPrompt();
        startGame();
      }
      else if (outcome == draw) {
        Serial.println("Draw");
        startGame();
      }
      else if (outcome == game_cont) {
        current_player = !current_player;
        prompt();
      }
    }
  //}
}

void startGame() {
  Serial.println("Start game");
  prompt();
  for (int i = 0; i < 9; i++) {
    board[i] = empty;
  }
}

int getMove() {
  return Serial.parseInt();
}

void prompt() {
  if (current_player == x) {
    Serial.println("X's move");
  }
  else if (current_player == o) {
    Serial.println("O's move");
  }
  else {
    Serial.println("Bug here");
  }
}

void winPrompt() {
  if (current_player == x) {
    Serial.println("X wins");
  }
  else if (current_player == o) {
    Serial.println("O wins");
  }
}

boolean validMove(int i) {
  if (board[i - 1] != empty || (i > 9 || i < 1)) {
    return false;
  }
  else  {
    board[i - 1] = current_player;
    return true;
  }
}

void displayBoard() {
  int k = 0;
  for (int r = 1; r < 4; r++) {
    for (int c = 1; c < 4; c++) {
      printCell(board[k]);
      k = k + 1;
    }
    Serial.println();
  }
}

void printCell(int cell) {
  if (cell == x) {
    Serial.print("X ");
  }
  else if (cell == o) {
    Serial.print("O ");
  }
  else if (cell == empty) {
    Serial.print("_ ");
  }
}

boolean check3(int p1, int p2, int p3) {
  if (board[p1 - 1] == current_player && board[p2 - 1] == current_player && board[p3 - 1] == current_player) {
    return true;
  }
  else {
    return false;
  }
}

boolean check_rows() {
  if (check3 (1, 2, 3) == true || check3(4, 5, 6) == true || check3(7, 8, 9) == true) {
    return true;
  }
  else
    return false;
}

boolean check_columns() {
  if (check3 (1, 4, 7) == true || check3(2, 5, 8) == true || check3(3, 6, 9) == true) {
    return true;
  }
  else
    return false;
}

boolean check_diagonals() {
  if (check3 (1, 5, 9) == true || check3 (3, 5, 7) == true) {
    return true;
  }
  else
    return false;
}

int move_outcome() {
  if (check_rows() == true || check_columns() == true || check_diagonals() == true) {
    return win;
  }
  else if (is_draw() == true) {
    return draw;
  }
  else {
    return game_cont;
  }
}

boolean is_draw() {
  for (int n = 0; n < 8; n++) {
    if (board[n] == empty) {
      return false;
    }
  }
  return true;
}

// Returns 1-9 if either of buttons 1-9 was pressed, or -1 if any other button was pressed.
// Suppresses repetition code.
int take_move_from_ir() {
  int number;
  while (true) {
    if (irrecv.decode(&signals)) {
      number = get_number_from_code(signals.value);
      irrecv.resume();  //get the next signal
      if (number) {
        break;
      }
    }
  }
  return number;
}


//Given the IR code, return the number (1 to 9)
// Return false if the code is a repetition
// Return -1 if any other code
int get_number_from_code(int code) {
  int codes[9] = {16724175, 16718055, 16743045, 16716015, 16726215, 16734885,
                  16728765, 16730805, 16732845};
                  
  int repetition = 4294967295;

  // Return false for repetition code
  if (code == repetition) return false;

  // Look up the code among the number codes
  for (int i = 0; i < 9; i++) {
    if (code == codes[i]) return i + 1;
  }

  // This must be something else (not 1 to 9)
  return -1;
}
//boolean gameOver{

//}

//boolean swapPlayers{
//  return !current_player;
//}

