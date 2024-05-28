/*
Calculator 2nd gen
v 0.1.1
------------------
Advanced arduino calculator
Arduino MEGA 2560, membrane switch module, lcd 1602, passive buzzer, 3D printed parts
All files on GitHub Orelptacnik/Calculator-2nd-gen
--------------------------------------------------
Created 14. 5. 2024
GitHub: Orelptacnik
-------------------
Delete serial prints after tests
Square root bug
In the end optimize and delete duplicates of code
Edit restoring values to lcd
*/

#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <pitches.h>
#include <string.h>

// functions
void mathematic(void);
void equations(void);
void aritAverage(void);
void average(void);
void flyGame(void);
void myTime(void);
void credit(void);
void play(void);
void clearLcd(void);

// initialize lcd
LiquidCrystal_I2C lcd(0x27, 20, 4);

// keypad setup
// dimensions
const byte ROWS = 4;
const byte COLS = 4;

// assign pins to keypad
byte rowPins[ROWS] = { 2, 3, 4, 5 };
byte colPins[COLS] = { A4, A3, A2, A1 };

// create map for certain keypad - add other later if needed
char mainKeys[ROWS][COLS] = 
{
  { '1', '2', '3', '+' },
  { '4', '5', '6', '-' },
  { '7', '8', '9', '/' },
  { '*', '0', '#', '=' }
};

char aditionMathKeys[ROWS][COLS] = 
{
  { '.', '-', '^', 'd' }, // decimal, negative, square x, delete
  { '√', 'x√', 'p', 'c' }, // sqroot 2, sqroot x, pi, clear
  { '%', 'x', 'x', 'a' }, // remain, nothing, nothing, ans
  { 'm', 'x', '#', 'y' } // main menu, nothing, changeKey, approve - sqr(root)
};

char goniometricKeys[ROWS][COLS] = 
{
  { 's', 'S', 'x', 'u' }, // sinus, arcusSinus, nothing, change units
  { 'c', 'C', 'x', 'd' }, // cosinus, arcusCosinus, nothing, degrees
  { 't', 'T', 'x', 'c' }, // tangens, cotangens, nothing, clear
  { 'd', 'a', '#', 'y' } // delete, ans, changeKey, approve - goniometric
};

// create var for certain keypad - add other later if needed
Keypad mainKeypad = Keypad(makeKeymap(mainKeys), rowPins, colPins, ROWS, COLS);

Keypad aditionMathKeypad = Keypad(makeKeymap(aditionMathKeys), rowPins, colPins, ROWS, COLS);

Keypad goniometricKeypad = Keypad(makeKeymap(goniometricKeys), rowPins, colPins, ROWS, COLS);

// variables to store which keyboard use
char mainKey;
int keyboardType = 0;

// buzzer melody and duration
int melody[] = 
{
  NOTE_D4, NOTE_D4, NOTE_E4, NOTE_F4, NOTE_D4, NOTE_C4, NOTE_C4, NOTE_A3, NOTE_B3, NOTE_D4, NOTE_D4, NOTE_E4, NOTE_F4, NOTE_D4, NOTE_C4, NOTE_C4, NOTE_F4, NOTE_G4, 
  NOTE_G4, NOTE_G4, NOTE_A4, NOTE_AS4, NOTE_G4, NOTE_F4, NOTE_F4, NOTE_D4, NOTE_E4, NOTE_G4, NOTE_G4, NOTE_A4, NOTE_AS4, NOTE_G4, NOTE_F4, NOTE_F4, NOTE_AS4, NOTE_C5, 
  NOTE_G4, NOTE_G4, NOTE_G4, NOTE_G4, NOTE_G4, NOTE_A4, NOTE_G4, NOTE_AS4, NOTE_G4, NOTE_A4, NOTE_G4, NOTE_G4, NOTE_G4, NOTE_G4, NOTE_G4, NOTE_AS4
};

int durationMelody[] =
{
  500, 500, 150, 150, 600, 600, 600, 600, 1100,
  500, 500, 150, 150, 600, 600, 600, 600, 1100,
  500, 500, 150, 150, 600, 600, 600, 600, 1100,
  500, 500, 150, 150, 600, 600, 600, 600, 1100,
  360, 360, 150, 150, 150, 150, 150, 150, 150, 150, 150, 360, 360, 150, 150, 1100
};

int startup[] = 
{
  NOTE_DS5, NOTE_DS4, NOTE_AS4, NOTE_GS4, NOTE_DS5, NOTE_DS5, NOTE_AS4
};

int startupDuration[] = 
{
  300, 150, 350, 200, 250, 250, 250
};

// var of buzzer sequence
int seq = 0;
bool allowSound = true;

// run when booted
void setup() {
  Serial.begin(9600);

  // prepare lcd
  lcd.begin(16, 2);
  lcd.backlight();

  // play startup song
  for (int i = 0; i < 7; i++)
  {
    tone(12, startup[i], startupDuration[i]);
    delay(startupDuration[i] + 10);
  }

  // print menu select
  lcd.setCursor(0, 0);
  lcd.print("Choose mod 1-5");
  lcd.setCursor(0, 1);
  lcd.print("Credits -> D");
  
  // choose mod
  while(true)
  {
    mainKey = mainKeypad.getKey();

    // when key is pressed
    if (mainKey)
    {
      Serial.println("pressed");

      // start proper function
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
      else if (mainKey == '=')
      {
        credit();
      }
    }
  }
}

// unused loop - maybe something will be here later
void loop() {
  // this doesn't stop
  Serial.println("loop");
  delay(1000);
}

// main mathematic calculator - operations, functions
void mathematic(void)
{
  // storing calculators values
  float num = 0;
  float num1 = 0;
  float num2 = 0;
  // storing operator
  char op;
  // managing print section variables
  int eCount = 0;
  char firstRow[] = "                ";
  char secondRow[] = "                ";
  int fPos = 0;
  int sPos = 0;

  clearLcd();

  // check for keys being pressed
  while(true)
  {
    // defines which map to use
    if (keyboardType == 0)
    {
      mainKey = mainKeypad.getKey();
    }
    else if (keyboardType == 1)
    {
      mainKey = aditionMathKeypad.getKey();
    }
    else if (keyboardType == 2)
    {
      mainKey = goniometricKeypad.getKey();
    }
    else
    {
      lcd.setCursor(0, 0);
      lcd.print("Error 3         ");
      lcd.setCursor(0, 1);
      lcd.print("Undefined Keypad");
    }

    // when key is pressed
    if (mainKey)
    {
      // non-functional condition - sqrroot is invalid char - try to fix it
      if (mainKey == '√')
      {
        lcd.print("\xE8");
      }
      // print every key except #
      else if (mainKey != '#')
      {
        lcd.print(mainKey);
      }

      // play music function
      play();
      
      // store operations on each row
      if (eCount % 2 == 0)
      {
        if (mainKey != '#')
        {
          firstRow[fPos] = mainKey;
          fPos++;
        }
        
        Serial.print("First: ");
        Serial.println(firstRow);
      }
      else
      {
        if (mainKey != '#')
        {
          secondRow[sPos] = mainKey;
          sPos++;
        }
        
        Serial.print("Second: ");
        Serial.println(secondRow);
      }

      // when 1 was pressed
      if (mainKey == '1') 
      { 
        if (num == 0) 
        {
          num = 1;
        }
        else 
        {
          num = (num * 10) + 1;
        }
      }

      // when 2 was pressed
      if (mainKey == '2') 
      { 
        if (num == 0) 
        {
          num = 2;
        }
        else 
        {
          num = (num * 10) + 2;
        }
      }

      // when 3 was pressed
      if (mainKey == '3') 
      { 
        if (num == 0) 
        {
          num = 3;
        }
        else 
        {
          num = (num * 10) + 3;
        }
      }

      // when 4 was pressed
      if (mainKey == '4') 
      { 
        if (num == 0) 
        {
          num = 4;
        }
        else 
        {
          num = (num * 10) + 4;
        }
      }

      // when 5 was pressed
      if (mainKey == '5') 
      { 
        if (num == 0) 
        {
          num = 5;
        }
        else 
        {
          num = (num * 10) + 5;
        }
      }

      // when 6 was pressed
      if (mainKey == '6') 
      { 
        if (num == 0) 
        {
          num = 6;
        }
        else 
        {
          num = (num * 10) + 6;
        }
      }
      // when 7 was pressed
      if (mainKey == '7') 
      { 
        if (num == 0) 
        {
          num = 7;
        }
        else 
        {
          num = (num * 10) + 7;
        }
      }

      // when 8 was pressed
      if (mainKey == '8') 
      { 
        if (num == 0) 
        {
          num = 8;
        }
        else 
        {
          num = (num * 10) + 8;
        }
      }

      // when 9 was pressed
      if (mainKey == '9') 
      {
        if (num == 0) 
        {
          num = 9;
        }
        else 
        {
          num = (num * 10) + 9;
        }
      }

      // when = was pressed
      if (mainKey == '=')
      {
        // choose operation
        if (op == '+')
        {          
          num2 = num;
          num = num1 + num2;
        }
        else if (op == '-')
        {          
          num2 = num;
          num = num1 - num2;
        }
        else if (op == '/')
        {          
          num2 = num;
          num = num1 / num2;
        }
        else if (op == '*')
        {          
          num2 = num;
          num = num1 * num2;
        }

        // choose row to print
        if (eCount % 2 == 0)
        {
          // clear lcd
          lcd.setCursor(0, 1);
          lcd.print("                ");
          lcd.setCursor(0, 1);
          lcd.print(num);

          // clear stored row
          for (int i = 0; i < sPos; i++)
          {
            secondRow[i] = ' ';
          }
          sPos = 0;
        }
        else
        {
          // clear lcd
          lcd.setCursor(0, 0);
          lcd.print("                ");
          lcd.setCursor(0, 0);
          lcd.print(num);

          // clear stored row
          for (int i = 0; i < fPos; i++)
          {
            firstRow[i] = ' ';
          }
          fPos = 0;
        }

        // increase number that decides row where to print
        eCount++;
      }

      // when # was pressed
      if (mainKey == '#')
      {
        // print menu
        clearLcd();
        lcd.print("Choose mod 0-3");
        lcd.setCursor(0, 1);
        lcd.print("A -> DEL, B -> C");

        // loop that runs until keyboard map is choosed - created paper that you can glue to the calculator that shows mapping
        while(true)
        {
          mainKey = mainKeypad.getKey();
          if (mainKey)
          {
            if (mainKey == '0')
            {
              keyboardType = 0; // main
            }
            else if (mainKey == '1')
            {
              keyboardType = 1; // aditionMath

            }
            else if (mainKey == '2')
            {
              keyboardType = 2; // goniometricKeys
            }
            else if (mainKey == '+')
            {
              // make function that deletes last character
            }
            else if (mainKey == '-')
            {
              // completely clear all values and lcd
            }
            else
            {
              keyboardType = 4; // wrong input
            }

            // restore values to lcd - bad cursor position - bug
            if (eCount % 2 == 0)
            {
              clearLcd();
              lcd.print(num);
              lcd.print(firstRow);
              lcd.setCursor(0, 1);
              lcd.print(secondRow);
              break;
            }
            else
            {
              clearLcd();
              lcd.print(firstRow);
              lcd.setCursor(0, 1);
              lcd.print(num);
              lcd.print(secondRow);
              break;
            }
          }
        }
        Serial.print(keyboardType);
      }

      // when some op is pressed
      if (mainKey == '+' || mainKey == '-' || mainKey == '/' || mainKey == '*')
      {
        op = mainKey;
        num1 = num;
        num = 0;
      }
    }
  }
}

void equations(void)
{
  // make equations function
}

void aritAverage(void)
{
  // make arithmetic function
}

void average(void)
{
  // make weighted average function
}

void flyGame(void)
{
  // make a game - you have to dodge obstacles by moving up and down
}

void myTime(void)
{
  // display time?
}

void credit(void)
{
  // print credits
}

// melody playing function
void play(void)
{
  if (allowSound == true)
  {
    tone(12, melody[seq], durationMelody[seq]);
    seq++;
    if (seq > 51)
    {     
      seq = 0;
    }
  }
}

// clear lcd function
void clearLcd(void)
{
  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 0);
}
