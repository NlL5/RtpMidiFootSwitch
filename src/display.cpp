#include "Wire.h"
#include "LiquidCrystal.h"

// Info: https://diyi0t.com/lcd-display-tutorial-for-arduino-and-esp8266/
// Init LCD lib:  RS, EN, d4, d5, d6, d7
LiquidCrystal lcd(D6, D7, D4, D3, D2, D1);

void setup_display()
{
    // set up the LCD's number of columns and rows:
    lcd.begin(16, 2);
}

void loop_display()
{
    // set the cursor to column 0, line 1
    // (note: line 1 is the second row, since counting begins with 0):
    //lcd.setCursor(0, 1);
    // print the number of seconds since reset:
    //lcd.print(millis() / 1000);
}
