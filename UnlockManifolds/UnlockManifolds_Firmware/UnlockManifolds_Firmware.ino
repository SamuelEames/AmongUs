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

DigitLed72xx sSeg = DigitLed72xx(SSEG_CS_PIN, SSEG_NCHIP);



void setup() 
{
  // Initialise Serial debug
  #ifdef DEBUG
    Serial.begin(115200);         // Open comms line
    while (!Serial) ;           // Wait for serial port to be available

    Serial.println(F("Wassup?"));
  #endif

    sSeg.on(0);
    sSeg.on(1);
    sSeg.clear(0);
    sSeg.clear(1);
    sSeg.printDigit(12345678,0,0);
    sSeg.printDigit(98,0,1);

}

void loop() 
{
  uint8_t customKey = BtnMtx.getKey();
  
  if (customKey)

    Serial.println(customKey);

}
