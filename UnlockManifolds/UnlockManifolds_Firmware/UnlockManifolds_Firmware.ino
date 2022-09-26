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

#define BTN_ROWS 2
#define BTN_COLS 5
#define NUM_BTNS  BTN_COLS*BTN_ROWS          // Number of buttons used in task

uint8_t hexaKeys[BTN_ROWS][BTN_COLS] = { {1, 2, 3, 4, 5}, {6, 7, 8, 9, 10} };     // Map key numbers to button matrix -- Note: can't use '0' due to how function works

const uint8_t BTN_ROW_PINS[BTN_ROWS] = {20, 21}; //connect to the row pinouts of the keypad
const uint8_t BTN_COL_PINS[BTN_COLS] = {1, 4, 7, 8, 9}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad BtnMtx = Keypad( makeKeymap(hexaKeys), BTN_ROW_PINS, BTN_COL_PINS, BTN_ROWS, BTN_COLS); 


///////////////////////////////////////////////////////////////////////////// 7-Seg Setup
#include <DigitLed72xx.h>
#define SSEG_CS_PIN 19
#define SSEG_NCHIP 2

DigitLed72xx sSeg = DigitLed72xx(SSEG_CS_PIN, SSEG_NCHIP);    // Initialise 7-segment displays


///////////////////////////////////////////////////////////////////////////// Pixel LEDs
#include <FastLED.h>

#define NUM_LEDS NUM_BTNS*2    
#define DATA_PIN 6

#define LED_BRIGHTNESS 10

CRGB leds[NUM_LEDS];


// Colours!
#define COL_RED      0xFF0000
#define COL_YELLOW   0xFF8F00
#define COL_GREEN    0x00FF10
#define COL_BLUE     0x0000FF
#define COL_PINK     0xFF002F
#define COL_CYAN     0x00AAAA
#define COL_ORANGE   0xFF1C00

#define COL_WHITE    0xFFFF7F
#define COL_BLACK    0x000000


///////////////////////////////////////////////////////////////////////////// Game Vars


uint8_t sequence[10];           // Stores current sequence being used for task

uint8_t currentStep = 0;            // Holds current sequence step


bool gameRunning = false;

///////////////////////////////////////////////////////////////////////////// Functions


void setup() 
{
  // Initialise Serial debug
  #ifdef DEBUG
    Serial.begin(115200);         // Open comms line
    while (!Serial) ;           // Wait for serial port to be available

    Serial.println(F("Wassup?"));
  #endif

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);  // GRB ordering is assumed

  // 7-Seg setup
  sSeg.setBright(2, SSEG_NCHIP);

  sSeg.on(SSEG_NCHIP);        // Turn on all displays
  sSeg.clear(SSEG_NCHIP);     // Clear all displays


  FastLED.setBrightness(LED_BRIGHTNESS);
  fill_solid(leds, NUM_LEDS, COL_BLACK);
  FastLED.show();

  // Sneaky animation to show we're ready
  for (uint8_t i = 0; i < NUM_BTNS; ++i)
  {
    write7SegRAW(i, B00000001);
    delay(20);
  }

  for (uint8_t i = 0; i < NUM_BTNS; ++i)
  {
    write7SegRAW(i, 0x00);
    delay(20);
  }

  // startGame();
}

void loop() 
{
  uint8_t btnPressed = BtnMtx.getKey();
  
  if (btnPressed)
  {

    if (!gameRunning)
    {
      startGame();
      return;
    }


    btnPressed--;   // make buttons count up from 0

    DPRINT("Current step = ");
    DPRINT(currentStep);
    DPRINT("\tSeqVal = ");
    DPRINT(sequence[btnPressed]);
    DPRINT("\tBtnPressed = ");
    DPRINTLN(btnPressed);

    if (sequence[btnPressed] == currentStep++)  // Correct 
      setBtnLed(btnPressed, COL_GREEN);
    else                                        // Incorrect
    {
      for (uint8_t i = 0; i < NUM_BTNS; ++i)    // Light all pressed buttons red
      {
        if (sequence[i] < (currentStep-1))
          setBtnLed(i, COL_RED);
      }

      for (uint8_t i = 0; i < 3; ++i)           // Flash incorrect button red 
      {
        setBtnLed(btnPressed, COL_RED);
        delay(80);
        setBtnLed(btnPressed, COL_BLUE);
        delay(80);
      }
      endGame();
    }

    if (currentStep >= NUM_BTNS)                // Winner!
    {
      for (int i = 0; i < 4; ++i)
      {
        fill_solid(leds, NUM_LEDS, COL_BLUE);
        FastLED.show();
        delay(100);
        fill_solid(leds, NUM_LEDS, COL_GREEN);
        FastLED.show();
        delay(100);
      }
      endGame();
    }
  }

}

void startGame()
{
  // Animates game in

  gameRunning = true;

  fill_solid(leds, NUM_LEDS, COL_YELLOW);
  FastLED.show();

  for (uint8_t i = 0; i < NUM_BTNS; ++i)
  {
    generateSequence();
    displaySequence();
    delay(80);
  }

  fill_solid(leds, NUM_LEDS, COL_BLUE);
  FastLED.show();
  currentStep = 0;

  return;
}


void endGame()
{
  // Animates game out

  gameRunning = false;

  for (uint8_t i = 0; i < NUM_BTNS; ++i)
  {
    setBtnLed(i, COL_BLACK);
    write7SegRAW(i, 0x00);
    delay(100);
  }

  return;
}



bool write7SegNum(uint8_t digit, uint8_t val)
{
  // Writes given value to given segment number
  // Returns 'true' if success, 'false' if bad parameters given and nothing written

  if (val > 9)          // Can only print a single number - return if given something else
    return false;

  if (digit < 8)        // First 8 digits
    sSeg.printDigit(val,0,digit);
  else if (digit < NUM_BTNS)  // Last two digits
    sSeg.printDigit(val,1,digit-8);
  else
    return false;  

  return true;
}

bool write7SegRAW(uint8_t digit, uint8_t val)
{
  //Writes given value to given 7-seg number

  if (digit++ < 8)        // First 8 digits - Note: for some lame reason this function counts up from 1
    sSeg.write(digit, val, 0);
  else if (digit <= NUM_BTNS)  // Last two digits
    sSeg.write(digit-8, val, 1);
  else
    return false; 

  return true;
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


void displaySequence()
{
  // prints sequence to displays

  for (uint8_t i = 0; i < NUM_BTNS; ++i)
    write7SegNum(i, sequence[i]);

  return;
}


void setBtnLed(uint8_t btnNum, uint32_t col)
{
  // Sets given button number LEDs to given colour

  /* MAP

  BTN   LED
  0     19-20
  1     17-18
  2     15-16
  3     13-14
  4     11-12
  5     0-1
  6     2-3
  7     4-5
  8     6-7
  9     8-9

  */

  if (btnNum < NUM_BTNS/2)
    fill_solid(leds+NUM_LEDS - (btnNum+1) *2, 2, col);
  else if (btnNum < NUM_BTNS)
    fill_solid(leds + (btnNum - NUM_BTNS/2) *2, 2, col);

  FastLED.show();

  return;
}