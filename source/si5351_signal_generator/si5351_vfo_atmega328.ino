
/**

  It is a multipurpose signal generator controlled by Arduino. This project uses the SI5351 from Silicon Labs. 


   Arduino Pro Mini 3.3V (8MHz) and SI5351 breakout wire up

   | Device name        | Device Pin / Description  |  Arduino Pin  |
   | ----------------   | --------------------      | ------------  |
   |   OLED             |                           |               |
   |                    | SDA                       |     A4        |
   |                    | SCL                       |     A5        |
   |   SI5351           |                           |               |
   |                    | SDIO (pin 18)             |     A4        |
   |                    | SCLK (pin 17)             |     A5        |
   |   Button           |                           |               |
   |                    | Step Switch               |      4        |
   |   Encoder          |                           |               |
   |                    | A                         |      3        |
   |                    | B                         |      2        |

   See https://github.com/etherkit/Si5351Arduino  and know how to calibrate your Si5351
   Author: Ricardo Lima Caratti 2020
*/

#include <si5351.h>
#include <Wire.h>
#include "Rotary.h"
#include "SSD1306Ascii.h"
#include "SSD1306AsciiAvrI2c.h"

// Enconder PINs
#define ENCODER_PIN_A 3 // Encoder pin A
#define ENCODER_PIN_B 2 // Encoder pin B
#define SWITCH_CMD 4   // ENCODER button or regular push button
#define BUTTON_UP   6   // Button up command
#define BUTTON_DOWN 5   // Button down command

#define CMD_STEP     0
#define CMD_FAVORITE 1

// OLED Diaplay constants
#define I2C_ADDRESS 0x3C
#define RST_PIN -1 // Define proper RST_PIN if required.

// Change this value bellow  (CORRECTION_FACTOR) to 0 if you do not know the correction factor of your Si5351A.
#define CORRECTION_FACTOR 80000 // See how to calibrate your Si5351A (0 if you do not want).

// VFO range for this project is 3000 KHz to 30000 KHz (3MHz to 30MHz).

#define MIN_VFO 300000000LLU     // VFO min. frequency 3Mhz
#define MAX_VFO 3000000000LLU    // VFO max. frequency 30MHz


// Struct for step
typedef struct
{
  char *name; // step label: 50Hz, 10Hz, 500Hz and 100KHz
  long value; // Frequency value (unit 0.01Hz See documentation) to increment or decrement
} Step;

// Steps database. You can change the Steps and numbers of steps here if you need.
Step step[] = {
  {(char *) "100Hz ", 10000}, // Minimum Frequency step (incremente or decrement) 100Hz
  {(char *) "500Hz ", 50000},
  {(char *) "1KHz  ", 100000},
  {(char *) "5KHz  ", 500000},
  {(char *) "10KHz ", 1000000},
  {(char *) "50KHz ", 5000000},
  {(char *) "500KHz", 50000000}
}; // Maximum frequency step 500KHz

// Calculate the index of last position of step[] array
const int lastStepVFO = (sizeof step / sizeof(Step)) - 1;
int currentStep = 4;


// Your favotite frequencies
uint64_t favorite[] = { 350000000LLU, 720500000LLU, 800000000LLU, 1070000000LLU,
                        1350000000LLU, 1600000000LLU, 2000000000LLU, 2400000000LLU,
                        2700000000LLU, 2800000000LLU, 2800000000LLU
                      };
const int lastFavorite = (sizeof favorite / sizeof(uint64_t) ) - 1;
int currentFavorite = 3;

byte currentCommand = CMD_STEP;

// Encoder controller
Rotary encoder = Rotary(ENCODER_PIN_A, ENCODER_PIN_B);

// OLED - Declaration for a SSD1306 display connected to I2C (SDA, SCL pins)
SSD1306AsciiAvrI2c display;

// The Si5351 instance.
Si5351 si5351;




bool isFreqChanged = true;
bool clearDisplay = false;

uint64_t vfoFreq;
uint64_t vfoLastValue;

// Encoder control variables
volatile int encoderCount = 0;

void setup()
{
  // Encoder pins
  pinMode(ENCODER_PIN_A, INPUT_PULLUP);
  pinMode(ENCODER_PIN_B, INPUT_PULLUP);
  // Si5351 contrtolers pins

  pinMode(SWITCH_CMD, INPUT_PULLUP);
  pinMode(BUTTON_UP,  INPUT_PULLUP);
  pinMode(BUTTON_DOWN, INPUT_PULLUP);

  // Initiating the OLED Display
  display.begin(&Adafruit128x64, I2C_ADDRESS);
  display.setFont(Adafruit5x7);
  display.set2X();
  display.clear();
  display.setCursor(0, 0);
  display.print("\nShortwave");
  display.print("\nTransmitter");
  display.print("\n\nBY PU2CLR");

  delay(3000);
  display.clear();

  vfoFreq = favorite[currentFavorite];

  showStatus();
  // Initiating the Signal Generator (si5351)
  si5351.init(SI5351_CRYSTAL_LOAD_8PF, 0, 0);
  // Adjusting the frequency (see how to calibrate the Si5351 - example si5351_calibration.ino)
  si5351.set_correction(CORRECTION_FACTOR, SI5351_PLL_INPUT_XO);
  si5351.set_pll(SI5351_PLL_FIXED, SI5351_PLLA);
  si5351.set_freq(vfoFreq, SI5351_CLK0); // Start CLK0 (VFO)

  // Disable CLK 1 and 2 outputs
  si5351.output_enable(SI5351_CLK1, 0);
  si5351.output_enable(SI5351_CLK2, 0);
  si5351.update_status();

  delay(500);

  // Encoder interrupt
  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_A), rotaryEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_B), rotaryEncoder, CHANGE);

  delay(1000);
}

// Use Rotary.h and  Rotary.cpp implementation to process encoder via interrupt
void rotaryEncoder()
{ // rotary encoder events
  uint8_t encoderStatus = encoder.process();
  if (encoderStatus)
  {
    if (encoderStatus == DIR_CW)
    {
      encoderCount = 1;
    }
    else
    {
      encoderCount = -1;
    }
  }
}

// Show Signal Generator Information
// Verificar setCursor() em https://github.com/greiman/SSD1306Ascii/issues/53
void showStatus()
{
  double vfo = vfoFreq / 100000.0;
  display.setCursor(0, 0);
  display.set2X();
  display.setCursor(0, 0);
  display.print(vfo);
  display.set1X();
  display.print(" KHz");
  display.setCursor(0, 3);
  display.print("Step: ");
  display.print(step[currentStep].name);
  display.setCursor(0, 5);
  display.print("Command: ");
  display.print((currentCommand == CMD_STEP) ? "STEP" : "FAVORITE");
}

// Change the frequency (increment or decrement)
// direction parameter is 1 (clockwise) or -1 (counter-clockwise)
void changeFreq(int direction)
{
  vfoFreq += step[currentStep].value * direction; // currentStep * direction;
  // Check the VFO limits
  if (vfoFreq > MAX_VFO )
    vfoFreq = MIN_VFO;
  else if (vfoFreq < MIN_VFO)
    vfoFreq = MAX_VFO;

  isFreqChanged = true;
}

void doCommandUp() {
  if ( currentCommand == CMD_STEP )
    currentStep = (currentStep < lastStepVFO) ? (currentStep + 1) : 0;
  else {
    currentFavorite = (currentFavorite < lastFavorite) ? (currentFavorite + 1) : 0;
    vfoFreq = favorite[currentFavorite];
    isFreqChanged = true;
  }

  delay(200);
}

void doCommandDown() {
  if ( currentCommand == CMD_STEP )
    currentStep = (currentStep > 0) ? (currentStep - 1) : lastStepVFO;
  else {
    currentFavorite = (currentFavorite > 0) ? (currentFavorite - 1) : lastFavorite;
    vfoFreq = favorite[currentFavorite];
    isFreqChanged = true;
  }
  delay(200);
}

void loop()
{
  // Check if the encoder has moved.
  if (encoderCount != 0)
  {
    if (encoderCount == 1)
      changeFreq(1);
    else
      changeFreq(-1);
    encoderCount = 0;
  }

  // Switch the current command control (step or favorite frequencies)
  if (digitalRead(SWITCH_CMD) == LOW) {
    currentCommand = (currentCommand == CMD_STEP) ? CMD_FAVORITE : CMD_STEP;
    showStatus();
    delay(200);
  } else if (digitalRead(BUTTON_UP) == LOW) {
    doCommandUp();
    showStatus();
  } else if (digitalRead(BUTTON_DOWN) == LOW) {
    doCommandDown();
    showStatus();
  }

  if (isFreqChanged)
  {
    si5351.set_freq(vfoFreq, SI5351_CLK0);
    showStatus();
    isFreqChanged = false;
  }
  delay(50);
}
