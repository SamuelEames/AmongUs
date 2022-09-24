/* Unlock Manifolds

  S.Eames 12.06.2022



*/

#define DEBUG   //If you comment this line, the DPRINT & DPRINTLN lines are defined as blank.
#ifdef DEBUG
  #define DPRINT(...)   Serial.print(__VA_ARGS__)   //DPRINT is a macro, debug print
  #define DPRINTLN(...) Serial.println(__VA_ARGS__) //DPRINTLN is a macro, debug print with new line
#else
  #define DPRINT(...)                       //now defines a blank line
  #define DPRINTLN(...)                     //now defines a blank line
#endif

///////////////////////////////////////////////////////////////////////////// BUTTON MATRIX SETUP

#include <Keypad.h>

const uint8_t BTN_ROWS = 2; //four rows
const uint8_t BTN_COLS = 5; //four columns
//define the cymbols on the buttons of the keypads
uint8_t hexaKeys[BTN_ROWS][BTN_COLS] = { {'0','1','2','3','4'}, {'5','6','7','8','9'} };

const uint8_t BTN_ROW_PINS[BTN_ROWS] = {20, 21}; //connect to the row pinouts of the keypad
const uint8_t BTN_COL_PINS[BTN_COLS] = {1, 4, 7, 8, 9}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad BtnMtx = Keypad( makeKeymap(hexaKeys), BTN_ROW_PINS, BTN_COL_PINS, BTN_ROWS, BTN_COLS); 


///////////////////////////////////////////////////////////////////////////// 7-Seg Setup
#include <DigitLed72xx.h>
#define SSEG_CS_PIN 19
#define SSEG_NCHIP 2

DigitLed72xx sSeg = DigitLed72xx(SSEG_CS_PIN, SSEG_NCHIP);    // Initialise 7-segment displays


///////////////////////////////////////////////////////////////////////////// Game Vars
#define NUM_BTNS  BTN_COLS*BTN_ROWS          // Number of buttons used in task

uint8_t sequence[10];           // Stores current sequence being used for task


void setup() 
{
  // Initialise Serial debug
  #ifdef DEBUG
    Serial.begin(115200);         // Open comms line
    while (!Serial) ;           // Wait for serial port to be available

    Serial.println(F("Wassup?"));
  #endif

    // 7-Seg setup
    sSeg.setBright(2, SSEG_NCHIP);


    sSeg.on(SSEG_NCHIP);        // Turn on all displays
    sSeg.clear(SSEG_NCHIP);     // Clear all displays
    // sSeg.clear(1);
    // sSeg.printDigit(12,0,0);
    // sSeg.printDigit(2,0,2);

    for (uint8_t i = 0; i < 10; ++i)
    {
      sSeg.clear(SSEG_NCHIP);     // Clear all displays
      write7Seg(i,1);
      delay(300);
    }

    generateSequence();

}

void loop() 
{
  uint8_t customKey = BtnMtx.getKey();
  
  if (customKey)

    Serial.println(customKey);
}



void write7Seg(uint8_t digit, uint8_t val)
{
  // Writes given value to given segment number

  if (val > 9)          // Can only print a single number - return if given something else
    return;

  if (digit < 8)        // First 8 digits
    sSeg.printDigit(val,0,digit);
  else if (digit < 10)  // Last two digits
    sSeg.printDigit(val,1,digit-8);
  

  return;
}


void generateSequence()
{
  // Generates a random button sequence for the task

  bool numUsed = false;

  randomSeed(millis());               // Seed random number generator (first time it will be the same, but after that it should be good)
  sequence[0] = random(NUM_BTNS);     // Generate first random number in sequence

  for (uint8_t i = 1; i < NUM_BTNS; ++i)
  {
    do
    {
      sequence[i] = random(NUM_BTNS);   // Generate random number
      numUsed = false;

      for (uint8_t j = 0; j < i; ++j)   // Check we haven't used that number yet
      {
        if (sequence[i] == sequence[j])
          numUsed = true;
      }
    }
    while (numUsed == true);            // Try again if we have already used that number
  }


  #ifdef DEBUG
    DPRINT(F("Sequence = "));
    for (uint8_t i = 0; i < NUM_BTNS; ++i)
    {
      DPRINT(F("\t"));
      DPRINT(sequence[i]);
    }
    DPRINTLN();
  #endif

  return;
}