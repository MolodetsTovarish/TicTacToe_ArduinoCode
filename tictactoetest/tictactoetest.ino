#include "math.h"
#include <IRremote.h>
#include "LedControl.h"

/*
 Now we need a LedControl to work with.
 ***** These pin numbers will probably not work with your hardware *****
 pin 12 is connected to the DataIn
 pin 11 is connected to the CLK
 pin 10 is connected to LOAD
 We have only a single MAX72XX.
 */
LedControl lc = LedControl(12, 11, 10, 1);

int input_pin = 8; //set D10 as input signal pin
IRrecv irrecv(input_pin);
decode_results signals;

int move;
int r;
int c;

const int empty = -1;
const int x = 1;
const int o = 0;

const int win = 1;
const int draw = 0;
const int game_cont = -1;

boolean current_player = x;

int board[9];
int winning_cells[3];

// 2x2 cell representations for 'x', 'o' and empty cell
int x_cell[4] = {true, false, false, true}; // "left slash" shape
int o_cell[4] = {true, true, true, true};   // "square" shape
int empty_cell[4] = {false, false, false, false}; // empty 2x2 cell

// flag to indicate if the game is in progress
boolean waiting_for_start = true;

void setup() {
  // put your setup code here, to run once:
  //Serial.begin(9600);

  irrecv.enableIRIn(); // enable input from IR receiver
  /*
   The MAX72XX is in power-saving mode on startup,
   we have to do a wakeup call
   */
  lc.shutdown(0, false);
  /* Set the brightness to a medium values */
  lc.setIntensity(0, 6);
 
  startGame();
}

void loop() {

  //while (Serial.available() > 0) {

  int move = take_move_from_ir();
  // Clear splash screen, if this is a first move
  if (waiting_for_start) {
    clearBoard();
    waiting_for_start = false;
  }
  //Serial.print("the move is: ");
  //Serial.println(move);
  if (validMove(move) == false) {
    //Serial.println("Invalid move");
  }
  else {
    displayBoard(move);
    int outcome = move_outcome();
    if (outcome == win) {
      //Serial.println("Game Over");
      winPrompt();
      startGame();
    }
    else if (outcome == draw) {
      drawPrompt();
      startGame();
    }
    else if (outcome == game_cont) {
      current_player = !current_player;
      //prompt();
    }
  }
  //}
}

void startGame() {
  clearBoard();
  splashScreen();
  waiting_for_start = true;
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
  show_winning_cells(current_player);
}

void drawPrompt() {
  // Blink all cells 3 times
  int number_of_blinks = 3;
  int blink_delay = 1000;
  for (int i = 0; i < number_of_blinks; i++) {
    for (int i = 1; i < 10; i++) {
      draw_cell(i, empty);
    }
    delay(blink_delay);
    for (int i = 1; i < 10; i++) {
      draw_cell(i, board[i-1]);
    }
    delay(blink_delay);
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


void displayBoard(int move) {
  // We will show the last move
  // The alternative is to redraw the whole board
  show_last_move(move, current_player);
}


boolean check3(int p1, int p2, int p3) {
  if (board[p1 - 1] == current_player && board[p2 - 1] == current_player && board[p3 - 1] == current_player) {
    winning_cells[0] = p1;
    winning_cells[1] = p2;
    winning_cells[2] = p3;
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
  for (int n = 0; n < 9; n++) {
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
                  16728765, 16730805, 16732845
                 };

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

void show_last_move(int move, int current_player) {
  // Find the row and column  in 3x3 matrix, givent the move
  //Serial.println("Showing the move on LED...");
  draw_cell(move, current_player);
 
}

void clearBoard(){
  // empty model (board array)
   for (int i = 0; i < 9; i++) {
    board[i] = empty;
  }
  // Clear LED matrix display
  lc.clearDisplay(0);
}

void splashScreen() {
  byte splash[8] = {B10101110, B01001010, B10101110, B00000000, B11101010, B10100100, B11101010, B00000000};
  for (int i = 0; i < 8; i++) {
    lc.setRow(0, i, splash[i]);  
  }
}

void show_winning_cells(int current_player) {
  int number_of_blinks = 3;
  int blink_delay = 1000;
  for (int i = 0; i < number_of_blinks; i++) {
    for (int i = 0 ; i < 3; i++) {
      draw_cell(winning_cells[i], empty);
    }
    delay(blink_delay);
    for (int i = 0 ; i < 3; i++) {
      draw_cell(winning_cells[i], board[winning_cells[i] - 1]);
    }
    delay(blink_delay);
  }
}

// 'cell' is a number from 1 to 9, 'cell_mode' is x, o or empty
void draw_cell(int cell, int cell_mode) {
  // Calculate top left coordinates of 2x2 cell in 8x8 LED matrix
  const int dim = 3;
  int row = ((int) (cell - 1)) / dim;
  int column = (cell - 1) - dim * row;
  // find top left corner in 8x8 matrix (for LED control)
  int led_row = 3*row;
  int led_column = 3*column;
  
  if (cell_mode == x) {
    draw_matrix_cell(led_row, led_column, x_cell);
  } else if (cell_mode == o) {
    draw_matrix_cell(led_row, led_column, o_cell);
  } else if (cell_mode == empty) {
    draw_matrix_cell(led_row, led_column, empty_cell);
  }
}

// Draws 2x2 cell starting with top left coordinates
// 'lights'  array contains flags (true/false to turn on/off) for each LED cell within 2x2 cell
void draw_matrix_cell(int top_left_row, int top_left_col, int* lights) {
    lc.setLed(0, top_left_row, top_left_col, lights[0]);
    lc.setLed(0, top_left_row+1, top_left_col, lights[1]);
    lc.setLed(0, top_left_row, top_left_col + 1, lights[2]);
    lc.setLed(0, top_left_row + 1, top_left_col + 1, lights[3]);
}



