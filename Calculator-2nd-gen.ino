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
void play(void);

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
bool allowSound;

void setup() {
  Serial.begin(9600);

  // prepare lcd
  lcd.begin(16, 2);
  lcd.backlight();

  for (int i = 0; i < 7; i++)
  {
    tone(12, startup[i], startupDuration[i]);
    delay(startupDuration[i] + 10);
  }
  
  // choose mod
  while(true)
  {
    mainKey = mainKeypad.getKey();

    // when key is pressed
    if (mainKey)
    {
      Serial.println("pressed");

      if (mainKey == '1')
      {
        allowSound = true;
        mathematic();
        break;
      }
      else if (mainKey == '2')
      {
        allowSound = false;
        mathematic();
        break;
        /* 
        equations();
        break;
        */
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
  float num = 0;
  float num1 = 0;
  float num2 = 0;
  char op;
  int eCount = 0;

  // check for keys being pressed
  while(true)
  {
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
      break;
    }

    if (mainKey)
    {
      lcd.print(mainKey);
      play();
      
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

      if (mainKey == '=')
      {

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

       if (eCount % 2 == 0)
       {
        lcd.setCursor(0, 1);
        lcd.print("                ");
        lcd.setCursor(0, 1);
        lcd.print(num);
       }
       else
       {
        lcd.setCursor(0, 0);
        lcd.print("                ");
        lcd.setCursor(0, 0);
        lcd.print(num);
       }
       eCount++;
      }

      if (mainKey == '#')
      {
        lcd.setCursor(0, 0);
        lcd.print("Choose mod 0-3  ");
        lcd.setCursor(0, 1);
        lcd.print("                ");
        lcd.setCursor(0, 0);

        while(true)
        {
          mainKey = mainKeypad.getKey();
          if (mainKey)
          {
            if (mainKey == '0')
            {
              keyboardType = 0;
              break;
            }
            else if (mainKey == '1')
            {
              keyboardType = 1;
              break;
            }
            else if (mainKey == '2')
            {
              keyboardType = 2;
              break;
            }
          }
        }
        Serial.print(keyboardType);
      }

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
