/**************************************************************************
  This is a library for several Adafruit displays based on ST77* drivers.

  Works with the Adafruit 1.8" TFT Breakout w/SD card
    ----> http://www.adafruit.com/products/358
  The 1.8" TFT shield
    ----> https://www.adafruit.com/product/802
  The 1.44" TFT breakout
    ----> https://www.adafruit.com/product/2088
  The 1.14" TFT breakout
  ----> https://www.adafruit.com/product/4383
  The 1.3" TFT breakout
  ----> https://www.adafruit.com/product/4313
  The 1.54" TFT breakout
    ----> https://www.adafruit.com/product/3787
  The 1.69" TFT breakout
    ----> https://www.adafruit.com/product/5206
  The 2.0" TFT breakout
    ----> https://www.adafruit.com/product/4311
  as well as Adafruit raw 1.8" TFT display
    ----> http://www.adafruit.com/products/618

  Check out the links above for our tutorials and wiring diagrams.
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional).

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 **************************************************************************/

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>
#include <Keypad.h>

// ==================== KEYPAD SETUP
const byte ROWS = 4; //four rows
const byte COLS = 4; //three columns
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {32, 33, 25, 26}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {27, 14, 12, 13}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

// ==================== DISPLAY SETUP
#define TFT_CS        15
#define TFT_RST        2 // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC         4

#define TFT_MOSI 16  // Data out
#define TFT_SCLK 17  // Clock out

#define TFT_LED 5

// For ST7735-based displays, we will use this call
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

// ==================== TIMER SETUP
uint32_t curMin = 0;
uint32_t curSec = 0;
uint32_t setMin = 25;
uint32_t setSec = 0;
uint32_t breakMins = 5;
uint64_t secondCounter = 0;

uint32_t digitCounter = 0; // leftmost minute digit is zero
uint32_t pomodoros = 0;

bool timerRunning = false;
bool timeChanged = false;
bool onBreak = false;
bool changingTime = false;

hw_timer_t * secondsTimer = NULL;

void tickPomodoro(){
  if(timerRunning){
    if(curSec == 0){
      if(curMin == 0){
        if(onBreak){
          timerRunning = false;
          onBreak = false;  
        }else{
          curMin = breakMins;
          onBreak = true;
          pomodoros++;
        }
        
        return;
      }
      curSec = 59;
      curMin--;
    }
    curSec--;
    timeChanged = true;
  }
}

void setDigit(uint32_t digit){
  Serial.print("Setting digit ");
  Serial.println(digitCounter);
  Serial.println("To ");
  Serial.print(digit);
  if(digit > 9){
    digit -= '0';
  }
  switch(digitCounter){
    case 0:
      setMin = setMin % 10 + digit * 10;
      break;
    case 1:
      setMin = (setMin - setMin % 10) + digit;
      break;
    case 2:
      setSec = setSec % 10 + digit * 10;
      break;
    case 3:
      setSec = (setSec - setSec % 10) + digit;
      break;
  }
}

void handleKey(char key){
  Serial.print("Handling key ");
  Serial.println(key);
  Serial.println(setMin);
  Serial.println(setSec);
  switch(key){
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case '0':
      if(!timerRunning){
        setDigit(key);
        displayTime();
      }
      break;
    case '*':
      digitCounter++;
      digitCounter = digitCounter % 4;
      displayTime();
      break;
    case '#':
      timerRunning = true;
      onBreak = false;
      changingTime = false;
      curMin = setMin;
      curSec = setSec;
      break;
    case 'A':
      pomodoros++;
      break;
    case 'B':
      timerRunning = !timerRunning;
      break;
    case 'C':
      changingTime = !changingTime;
      displayTime();
      break;
    case 'D':
      timerRunning = false;
      timeChanged = false;
      onBreak = false;
      changingTime = false;
      break;
    default:
      break;
  }
}

void displayTime(){
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0, 0);
  tft.setTextSize(5);
  if(!changingTime){
    tft.setTextColor(ST77XX_WHITE);
    tft.print((char)(curMin - curMin % 10) / 10);
    Serial.print((char)(curMin - curMin % 10) / 10);
    tft.print((char)curMin % 10);
    Serial.print((char)curMin % 10);
    tft.print(':');
    Serial.print(':');
    tft.print((char)(curSec - curSec % 10) / 10);
    Serial.print((char)(curSec - curSec % 10) / 10);
    tft.println((char)curSec % 10);
    Serial.println((char)curSec % 10);
  }else{
    tft.setTextColor(ST77XX_RED);
    if(digitCounter == 0) tft.setTextColor(ST77XX_BLUE);
    tft.print((char)(setMin - setMin % 10) / 10);
    Serial.print((char)(setMin - setMin % 10) / 10);

    tft.setTextColor(ST77XX_RED);
    if(digitCounter == 1) tft.setTextColor(ST77XX_BLUE);
    tft.print((char)setMin % 10);
    Serial.print((char)setMin % 10);

    tft.setTextColor(ST77XX_RED);
    tft.print(':');
    Serial.print(':');

    tft.setTextColor(ST77XX_RED);
    if(digitCounter == 2) tft.setTextColor(ST77XX_BLUE);
    tft.print((char)(setSec - setSec % 10) / 10);
    Serial.print((char)(setSec - setSec % 10) / 10);

    tft.setTextColor(ST77XX_RED);
    if(digitCounter == 3) tft.setTextColor(ST77XX_BLUE);
    tft.println((char)setSec % 10);
    Serial.println((char)setSec % 10);
    tft.setTextColor(ST77XX_RED);
  }

  tft.setTextSize(2);
  if(!onBreak){
    tft.println("\nWork, bitch");
  }else{
    tft.println("\nBreak");
  }
  tft.print("Pomodoros: ");
  tft.println((char)(pomodoros + '0'));
}

void setup(void) {
  Serial.begin(9600);
  

  // Use this initializer if using a 1.8" TFT screen:
  tft.initR(INITR_BLACKTAB);      // Init ST7735S chip, black tab

  pinMode(TFT_LED, OUTPUT);
  digitalWrite(TFT_LED, HIGH);

  Serial.println(F("Initialized"));
  tft.setRotation(1);
  tft.fillScreen(ST77XX_BLACK);
  
  tft.setCursor(0, 0);
  tft.setTextSize(3);
  tft.setTextColor(ST77XX_WHITE);
  tft.println("Welcome \nto \nPomodoro");

  secondsTimer = timerBegin(1000000);                   //timer 1Mhz resolution
  timerAttachInterrupt(secondsTimer, &tickPomodoro);           //attach callback
  timerAlarm(secondsTimer, 1000000, true, 0); //set time in us
}

void loop() {
  delay(100);

  if(timeChanged){
    displayTime();
    timeChanged = false;
  }
  char key = keypad.getKey();
  if(key){
    handleKey(key);
  }
}
