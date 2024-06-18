# espomodoro
An ESP32-based Pomodoro timer using the Arduino framework. Built to work on an 128x160 ST7735/7789 display and those generic Aliexpress 4x4 keypads. Uses the Adafruit ST77XX library and the Keypad library.

Connections:
 - 4x4 keypad (from leftmost pin): D32, D33, D25, D26, D27, D14, D12, D13
 - Display:
   - D5   - LED
   - D17  - SCK
   - D16  - SDA
   - D4   - A0/DC
   - D2   - RST/RESET
   - D15  - CS
You might notice that the pins are sequential in the ESP32-WROOM-32 board.
You might want to change D2 to get more stability since it is not really a good IO pin.

Functionality of the different keys:
 - 0-9: set number for countdown
 - *: move one digit forward when setting the time
 - #: (re)start timer from set time
 - A: increase the number of pomodoros (so that you can turn it on and off and not lose progress)
 - B: pause/unpause
 - C: set timer countown
 - D: stop everything and go back to beginning
