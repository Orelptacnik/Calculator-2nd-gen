#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <pitches.h>

// functions
void mathematic(void);
void equations(void);
void aritAverage(void);
void average(void);
void flyGame(void);
void myTime(void);
void credit(void);

// initialize lcd
LiquidCrystal_I2C lcd(0x27, 20, 4);

// keypad setup
// dimensions
const byte ROWS = 4;
const byte COLS = 4;

// assign pins to keypad
byte rowPins[ROWS] = { 5, 4, 3, 2 };
byte colPins[COLS] = { A1, A2, A3, A4 };

char mainKeys[ROWS][COLS] = 
{
  { '1', '2', '3', '+' },
  { '4', '5', '6', '-' },
  { '7', '8', '9', '/' },
  { '*', '0', '#', '=' }
};

// create var for certain keypad - add other later if needed
Keypad mainKeypad = Keypad(makeKeymap(mainKeys), rowPins, colPins, ROWS, COLS);
char mainKey;

void setup() {
  Serial.begin(9600);

  // prepare lcd
  lcd.begin(16, 2);
  lcd.backlight();
  
  // choose mod
  while(true)
  {
    mainKey = mainKeypad.getKey();
    Serial.println("while");
    delay(1000);

    if (mainKey)
    {
      Serial.println("pressed");
      if (mainKey == '1')
      {
        mathematic();
        break;
      }
      else if (mainKey == '2')
      {
        equations();
        break;
      }
      else if (mainKey == '3')
      {
        aritAverage();
        break;
      }
      else if (mainKey == '+')
      {
        average();
        break;
      }
      else if (mainKey == '4')
      {
        flyGame();
        break;
      }
      else if (mainKey == '5')
      {
        myTime();
        break;
      }
      else if (mainKey == '/')
      {
        credit();
      }
    }
  }
}

void loop() {
  // this doesn't stop
  Serial.println("loop");
  delay(1000);
}

void mathematic(void)
{
  // check for keys being pressed
  while(true)
  {
    mainKey = mainKeypad.getKey();

    if (mainKey)
    {
      lcd.print(mainKey);
      
      if (mainKey == '0')
      {
        break;
      }
    }
  }
}

void equations(void)
{

}

void aritAverage(void)
{

}

void average(void)
{

}

void flyGame(void)
{

}

void myTime(void)
{

}

void credit(void)
{

}

