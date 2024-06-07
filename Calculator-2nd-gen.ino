/*
Calculator 2nd gen
v 0.3.0
------------------
Advanced arduino calculator
Arduino MEGA 2560, membrane switch module, lcd 1602, passive buzzer, 3D printed parts
All files on GitHub Orelptacnik/Calculator-2nd-gen
--------------------------------------------------
Created 14. 5. 2024
GitHub: Orelptacnik
-------------------
Delete serial prints after tests
In the end optimize and delete duplicates of code
*/

#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <pitches.h>
#include <string.h>
#include <math.h>

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
bool decimal(float num);

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
  { 'm', '.', '^', '°' }, // negative, decimal, square x, degrees
  { 'N', 'r', 'p', 'D' }, // arcus Cotangens, sqroot x, pi, change to degrees
  { 's', 'S', 'c', 'C' }, // sinus, arcus sinus, cosinus, arcus cosinus
  { 't', 'T', '#', 'n' } // tangens, arcus tangens, changeKey, cotangens
};

// create var for certain keypad - add other later if needed
Keypad mainKeypad = Keypad(makeKeymap(mainKeys), rowPins, colPins, ROWS, COLS);

Keypad aditionMathKeypad = Keypad(makeKeymap(aditionMathKeys), rowPins, colPins, ROWS, COLS);

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
  
  //print opening message
  lcd.setCursor(0, 0);
  lcd.print("Calculator 2.Gen");
  lcd.setCursor(0, 1);
  lcd.print("Version 0.3.0");
  delay(3000);
  lcd.clear();

  // print menu select
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
  String firstRow = "";
  String secondRow = "";
  bool numberType;
  int printInt;
  float printFloat;

  // define special characters
  const char pi {247};
  const char sqrRoot {232};
  const char degree {223};

  // decimal point variables
  bool isDecimal = false;
  int dCount = 10;

  //negative num var
  bool negative = false;

  // variables for square
  float sqrNum;
  bool square = false;

  // variables for square root
  float sqrRootNum;
  bool squareRoot = false;

  // goniometric function variables
  bool function = false;
  char functionType;
  float multipleF = 1;


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
      Serial.println(num);
      // print every key except:
      if (mainKey != '#' && mainKey != 'p' && mainKey != 'r' && mainKey != 'm' && mainKey != 's' && mainKey != 'S' && mainKey != 'c'
          && mainKey != 'C' && mainKey != 't' && mainKey != 'T' && mainKey != 'n' && mainKey != 'N' && mainKey != 'D')
      {
        lcd.print(mainKey);
      
        // store operations on each row
        if (eCount % 2 == 0)
        {
          firstRow = firstRow + mainKey;      
          Serial.print("First: ");
          Serial.println(firstRow);
        }
        else
        {
          secondRow = secondRow + mainKey;
          Serial.print("Second: ");
          Serial.println(secondRow);
        }
      }

      // play music function
      play();

      if (mainKey == 'p')
      {
        lcd.print(pi);
        num = M_PI;

        if (eCount % 2 == 0)
        {
          firstRow = firstRow + pi;
        }
        else
        {
          secondRow = secondRow + pi;
        }
      }

      if (mainKey == 'r')
      {
        squareRoot = true;
        sqrRootNum = num;
        num = 0;

        lcd.print(sqrRoot);

        if (eCount % 2 == 0)
        {
          firstRow = firstRow + sqrRoot;
        }
        else
        {
          secondRow = secondRow + sqrRoot;
        }
      }

      if (mainKey == 'D')
      {
        eCount++;
        int deg = int(num);
        float fractionalPart = num - deg;
        int minutes = int(fractionalPart * 60);
        float seconds = (fractionalPart * 60 - minutes) * 60;

        if (eCount % 2 == 0)
        {
          firstRow = String(deg) + degree + String(minutes) + "\'" + String(seconds) + "\"";
        }
        else
        {
          secondRow = String(deg) + degree + String(minutes) + "\'" + String(seconds) + "\"";
        }

        clearLcd();
        lcd.print(firstRow);
        lcd.setCursor(0, 1);
        lcd.print(secondRow);
      }

      // when 0 is pressed
      if (mainKey == '0')
      {

        if (isDecimal == true)
        {
          if (num == 0)
          {
            num = 0.0;
          }
          else 
          {
            num = (num * dCount + 0) / dCount;
            
          }
          dCount *= 10;
        }
        else
        {
          if (num == 0) 
          {
            num = 0;
          }
          else 
          {
            num = (num * 10) + 0;
          }
        }
      }

      // when 1 was pressed
      if (mainKey == '1') 
      { 
        if (isDecimal == true)
        {
          if (num == 0)
          {
            num = 0.1;
          }
          else 
          {
            num = (num * dCount + 1) / dCount;
            
          }
          dCount *= 10;
        }
        else
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
      }

      // when 2 was pressed
      if (mainKey == '2') 
      { 
        if (isDecimal == true)
        {
          if (num == 0)
          {
            num = 0.2;
          }
          else 
          {
            num = (num * dCount + 2) / dCount;
            
          }
          dCount *= 10;
        }
        else
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
      }

      // when 3 was pressed
      if (mainKey == '3') 
      { 
        if (isDecimal == true)
        {
          if (num == 0)
          {
            num = 0.3;
          }
          else 
          {
            num = (num * dCount + 3) / dCount;
            
          }
          dCount *= 10;
        }
        else
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
      }

      // when 4 was pressed
      if (mainKey == '4') 
      { 
        if (isDecimal == true)
        {
          if (num == 0)
          {
            num = 0.4;
          }
          else 
          {
            num = (num * dCount + 4) / dCount;
            
          }
          dCount *= 10;
        }
        else
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
      }

      // when 5 was pressed
      if (mainKey == '5') 
      { 
        if (isDecimal == true)
        {
          if (num == 0)
          {
            num = 0.5;
          }
          else 
          {
            num = (num * dCount + 5) / dCount;
            
          }
          dCount *= 10;
        }
        else
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
      }

      // when 6 was pressed
      if (mainKey == '6') 
      { 
        if (isDecimal == true)
        {
          if (num == 0)
          {
            num = 0.6;
          }
          else 
          {
            num = (num * dCount + 6) / dCount;
            
          }
          dCount *= 10;
        }
        else
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
      }

      // when 7 was pressed
      if (mainKey == '7') 
      { 
        if (isDecimal == true)
        {
          if (num == 0)
          {
            num = 0.7;
          }
          else 
          {
            num = (num * dCount + 7) / dCount;
            
          }
          dCount *= 10;
        }
        else
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
      }

      // when 8 was pressed
      if (mainKey == '8') 
      { 
        if (isDecimal == true)
        {
          if (num == 0)
          {
            num = 0.8;
          }
          else 
          {
            num = (num * dCount + 8) / dCount;
            
          }
          dCount *= 10;
        }
        else
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
      }

      // when 9 was pressed
      if (mainKey == '9') 
      {
        if (isDecimal == true)
        {
          if (num == 0)
          {
            num = 0.9;
          }
          else 
          {
            num = (num * dCount + 9) / dCount;
            
          }
          dCount *= 10;
        }
        else
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
      }

      // when negative (-) was presse
      if (mainKey == 'm')
      {
        negative = true;
        lcd.print("-");

        if (eCount % 2 == 0)
        {
          firstRow = firstRow + "-";
        }
        else
        {
          secondRow = secondRow + "-";
        }
      }

      // when square was pressed
      if (mainKey == '^')
      {
        sqrNum = num;
        num = 0;
        square = true;
      }

      // when sinus was pressed
      if (mainKey == 's')
      {
        lcd.print("sin");

        if (num != 0)
        {
          multipleF = num;
          num = 0;
        }                

        function = true;
        functionType = 's';

        if (eCount % 2 == 0)
        {
          firstRow = firstRow + "sin";
        }
        else
        {
          secondRow = secondRow + "sin";
        }
      }

      // when arcus sinus was pressed
      if (mainKey == 'S')
      {
        lcd.print("aSin");

        if (num != 0)
        {
          multipleF = num;
          num = 0;
        }

        function = true;
        functionType = 'S';

        if (eCount % 2 == 0)
        {
          firstRow = firstRow + "aSin";
        }
        else
        {
          secondRow = secondRow + "aSin";
        }
      }

      // when cosinus was pressed
      if (mainKey == 'c')
      {
        lcd.print("cos");

        if (num != 0)
        {
          multipleF = num;
          num = 0;
        }

        function = true;
        functionType = 'c';

        if (eCount % 2 == 0)
        {
          firstRow = firstRow + "cos";
        }
        else
        {
          secondRow = secondRow + "cos";
        }
      }

      // when arcus cosinus was pressed
      if (mainKey == 'C')
      {
        lcd.print("aCos");

        if (num != 0)
        {
          multipleF = num;
          num = 0;
        }
        
        function = true;
        functionType = 'C';

        if (eCount % 2 == 0)
        {
          firstRow = firstRow + "aCos";
        }
        else
        {
          secondRow = secondRow + "aCos";
        }
      }

      // when tangens was pressed
      if (mainKey == 't')
      {
        lcd.print("tan");

        if (num != 0)
        {
          multipleF = num;
          num = 0;
        }

        function = true;
        functionType = 't';

        if (eCount % 2 == 0)
        {
          firstRow = firstRow + "tan";
        }
        else
        {
          secondRow = secondRow + "tan";
        }
      }

      // when arcus tangens was pressed
      if (mainKey == 'T')
      {
        lcd.print("aTan");

        if (num != 0)
        {
          multipleF = num;
          num = 0;
        }

        function = true;
        functionType = 'T';

        if (eCount % 2 == 0)
        {
          firstRow = firstRow + "aTan";
        }
        else
        {
          secondRow = secondRow + "aTan";
        }
      }

      // when cotangens was pressed
      if (mainKey == 'n')
      {
        lcd.print("ctn");

        if (num != 0)
        {
          multipleF = num;
          num = 0;
        }

        function = true;
        functionType = 'n';

        if (eCount % 2 == 0)
        {
          firstRow = firstRow + "ctn";
        }
        else
        {
          secondRow = secondRow + "ctn";
        }
      }

      // when arcus cotanges was pressed
      if (mainKey == 'N')
      {
        lcd.print("aCtn");

        if (num != 0)
        {
          multipleF = num;
          num = 0;
        }

        function = true;
        functionType = 'N';

        if (eCount % 2 == 0)
        {
          firstRow = firstRow + "aCtn";
        }
        else
        {
          secondRow = secondRow + "aCtn";
        }
      }

      // when = was pressed
      if (mainKey == '=')
      {
        //initalize num as negative if selected
        if (negative == true)
        {
          num = num * -1;
          negative = false;
        }

        // if square is used
        if (square == true)
        {
          num = pow(sqrNum, num);
          square = false;
          op = 'n';
        }

        // if square root is used
        if (squareRoot == true)
        {
          num = pow(num, 1 / sqrRootNum);
          squareRoot = false;
          op = 'n';
        }

        // if goniometric function is used
        if (function == true)
        {
          if (functionType == 's')
          {
            num = sin(radians(num));    
            num *= multipleF;
          }
          else if (functionType == 'S')
          {
            num = asin(num);
            num = num * 180 / M_PI; // convert to degrees
          }
          else if (functionType == 'c')
          {
            num = cos(radians(num));
          }
          else if (functionType == 'C')
          {
            num = acos(num);
            num = num * 180 / M_PI;
          }
          else if (functionType == 't')
          {
            num = tan(radians(num));           
          }
          else if (functionType == 'T')
          {
            num = atan(num);
            num = num * 180 / M_PI;
          }
          else if (functionType == 'n')
          {
            num = cos(radians(num)) / sin (radians(num));
          }
          else if (functionType == 'N')
          {
            num = atan(1 / num);
            num = num * 180 / M_PI;
          }
          
          multipleF = 1;
          functionType = false;
        }

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

        if (op == '+' && num1 == 1 && num2 == 1)
        {
          clearLcd();
          lcd.print("We are checking.");
          delay(3000);
          clearLcd();
          lcd.print("Touch some grass");
          lcd.setCursor(0, 1);
          lcd.print(";.,;;:,:;,:.;,.;");
          delay(3000);
          clearLcd();

          if (eCount % 2 == 0)
          {
            lcd.print(firstRow);
          }
          else
          {
            lcd.setCursor(0, 1);
            lcd.print(secondRow);
          }
        }

        // delete unnecessary zeros 1.00 - store the new number
        numberType = decimal(num);

        // choose row to print
        if (eCount % 2 == 0)
        {
          if (numberType == false)
          {
            printFloat = num;
            secondRow = String(printFloat, 3);
          }
          else 
          {
            printInt = num;
            secondRow = printInt;
          }
          
          lcd.setCursor(0, 1);
          lcd.print("                ");
          lcd.setCursor(0, 1);
          lcd.print(secondRow);
        }
        else
        {
          if (numberType == false)
          {
            printFloat = num;
            firstRow = String(printFloat, 3);
          }
          else 
          {
            printInt = num;
            firstRow = printInt;
          }

          lcd.setCursor(0, 0);
          lcd.print("                ");
          lcd.setCursor(0, 0);
          lcd.print(firstRow);
        }
        // increase number that decides row where to print and change cursor position
        eCount++;
        isDecimal = false;
        dCount = 10;
      }

      // when some op is pressed
      if (mainKey == '+' || mainKey == '-' || mainKey == '/' || mainKey == '*')
      {
        Serial.print("num: ");
        Serial.println(num);

        //initalize num as negative if selected
        if (negative == true)
        {
          num = num * -1;
          negative = false;
        }

        // if square is used
        if (square == true)
        {
          num = pow(sqrNum, num);
          square = false;
        }

        // if square root is used
        if (squareRoot == true)
        {
          num = pow(num, 1 / sqrRootNum);
          squareRoot = false;
        }

        // if goniometric function is used
        if (function == true)
        {
          if (functionType == 's')
          {
            num = sin(radians(num));    
            num *= multipleF;
          }
          else if (functionType == 'S')
          {
            num = asin(num);
            num = num * 180 / M_PI; // convert to degrees
          }
          else if (functionType == 'c')
          {
            num = cos(radians(num));
          }
          else if (functionType == 'C')
          {
            num = acos(num);
            num = num * 180 / M_PI;
          }
          else if (functionType == 't')
          {
            num = tan(radians(num));
          }
          else if (functionType == 'T')
          {
            num = atan(num);
            num = num * 180 / M_PI;
          }
          else if (functionType == 'n')
          {
            num = cos(radians(num)) / sin (radians(num));
          }
          else if (functionType == 'N')
          {
            num = atan(1 / num);
            num = num * 180 / M_PI;
          }
          
          multipleF = 1;
          functionType = false;
        }

        op = mainKey;
        num1 = num;
        num = 0;
        isDecimal = false;
        dCount = 10;
      }

      if (mainKey == '.')
      {
        isDecimal = true;
      }

      // automatically change keypad to default after using optional key
      if (keyboardType == 1)
      {
        keyboardType = 0;
      }

      // when # was pressed
      if (mainKey == '#')
      {
        // print menu
        clearLcd();
        lcd.print("Choose mod 1-2");
        lcd.setCursor(0, 1);
        lcd.print("A -> DEL, B -> C");

        // loop that runs until keyboard map is choosed - created paper that you can glue to the calculator that shows mapping
        while(true)
        {
          mainKey = mainKeypad.getKey();

          if (mainKey)
          {
            if (mainKey == '1')
            {
              keyboardType = 0; // main
            }
            else if (mainKey == '2')
            {
              keyboardType = 1; // aditionMath

            }
            else if (mainKey == '+')
            {
              // make function that deletes last character
            }
            else if (mainKey == '-')
            {
              // completely clear all values and lcd
              num = 0;
              num1 = 0;
              num2 = 0;
              op = 'n';
              eCount = 0;
              firstRow = "";
              secondRow = "";
            }
            else
            {
              keyboardType = 4; // wrong input
            }

            // restore values to lcd
            Serial.print("Stored number:");
            Serial.println(num);

            clearLcd();
            
            if (eCount % 2 != 0)
            {
              lcd.print(firstRow);
              lcd.setCursor(0, 1);
              lcd.print(secondRow);
            }
            else
            {
              lcd.setCursor(0, 1);
              lcd.print(secondRow);
              lcd.setCursor(0, 0);
              lcd.print(firstRow);
            }
            break;
          }
        }
        delay(300);
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

// decimal point editor function
bool decimal(float num)
{
  int x = num;
  float check = num - x;

  // if number is float return false, if integer return true
  if (check > 0)
  {
    return false;
  }
  else
  {
    return true;
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
