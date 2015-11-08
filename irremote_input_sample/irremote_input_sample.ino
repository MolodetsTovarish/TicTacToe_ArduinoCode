#include <IRremote.h>
int input_pin = 8; //set D10 as input signal pin
IRrecv irrecv(input_pin);
decode_results signals;

void setup()
{
  Serial.begin(9600);
  irrecv.enableIRIn();  //enable input from IR receiver
}

void loop() {

  Serial.println(take_move_from_ir());

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
