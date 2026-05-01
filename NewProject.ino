


#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>

#define SS_PIN 10
#define RST_PIN 8

MFRC522 mfrc522(SS_PIN, RST_PIN);

byte CorrectUid[] = { 0x43, 0xEF, 0x1D, 0x03 };

bool isArmed = false;

bool stateChanged = false;

int sensorState = LOW;

int sensorValue = 0;

bool isAlarmed = false;

bool movementDetected = false;

unsigned long movementDetectTime = 0;

unsigned long countdownStartTime = 0;


LiquidCrystal lcd(6, 7, 5, 4, 3, 2);

void setup() {
  Serial.begin(9600);
  pinMode(13, OUTPUT);
  pinMode(9, OUTPUT);
  lcd.begin(16, 2);
  lcd.print("Hello World");
  // tone(9, 1000);
  // delay(1000);
  // noTone(9);

  SPI.begin();

  pinMode(SS_PIN, OUTPUT);

  pinMode(12, INPUT);

  pinMode(11, OUTPUT);

  pinMode(13, OUTPUT);

  pinMode(A5, INPUT);

  pinMode(A4, OUTPUT);

  pinMode(A3, INPUT);

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("System Status:");
  lcd.setCursor(0, 1);
  lcd.print("Disarmed");


  mfrc522.PCD_Init();

  Serial.println("Hello World");
}

void loop() {
  unsigned long currentMillis = millis();

  unsigned long elapsedTime = currentMillis - countdownStartTime;


  if (digitalRead(A3) == HIGH)
  {
    if (!isArmed)
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Old key");

      getKey();



      stateChanged = true;
    }
  }

  if (stateChanged)
{
  delay(500);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("System Status:");
  if (isArmed)
  {
    lcd.setCursor(0, 1);
    lcd.print("Armed");
  }
  else
  {
    lcd.setCursor(0, 1);
    lcd.print("Disarmed");
  }
    stateChanged = false;
}

sensorValue = digitalRead(A5);

if (sensorValue == HIGH)
{
  digitalWrite(A4, HIGH);

  if (sensorState == LOW && isArmed && !isAlarmed)
  {
    DetectMovement();
  }
}
else
{
  digitalWrite(A4, LOW);
}

  if (mfrc522.PICC_IsNewCardPresent()) {
      Serial.println("Test");
    if (mfrc522.PICC_ReadCardSerial()) {

      Serial.println("Test");


      byte CardUid[mfrc522.uid.size];

      for (byte i = 0; i < mfrc522.uid.size; i++)
      {
        CardUid[i] = mfrc522.uid.uidByte[i];
      }

      if (CompareCard(CardUid, CorrectUid, mfrc522.uid.size))
      {
        noTone(9);
        tone(9, 1000);
        delay(200);
        noTone(9);
        ToggleArm();
      }
      else
      {
        if (!isArmed)
        {
        lcd.clear();
        lcd.print("Invalid Key");
        tone(9, 1000);
        delay(200);
        noTone(9);
        delay(100);
        tone(9, 1000);
        delay(200);
        noTone(9);
        delay(100);
        tone(9, 1000);
        delay(200);
        noTone(9);
        }
      }

      if (!isArmed && !movementDetected)
      {
      stateChanged = true;
      }
    }
  }

  mfrc522.PICC_HaltA();
}

void getKey()
{
      byte CardUid[mfrc522.uid.size];
  while (!mfrc522.PICC_IsNewCardPresent()) {
      Serial.println("Test");
      delay(100);
  }
    if (mfrc522.PICC_ReadCardSerial()) {

      Serial.println("Test");



      for (byte i = 0; i < mfrc522.uid.size; i++)
      {
        CardUid[i] = mfrc522.uid.uidByte[i];
      }

      if (CompareCard(CardUid, CorrectUid, mfrc522.uid.size))
      {
        
        noTone(9);
        tone(9, 1000);
        delay(200);
        noTone(9);

        lcd.clear();
        lcd.print("New Key");

        while (!mfrc522.PICC_IsNewCardPresent()) {
          Serial.println("Test");
          delay(100);
        }
        
        if (mfrc522.PICC_ReadCardSerial()) {

          Serial.println("Test");

          byte NewUid[mfrc522.uid.size];

          for (byte i = 0; i < mfrc522.uid.size; i++)
          {
            NewUid[i] = mfrc522.uid.uidByte[i];
          }

          CorrectUid[0] = NewUid[0];
          CorrectUid[1] = NewUid[1];
          CorrectUid[2] = NewUid[2];
          CorrectUid[3] = NewUid[3];

        noTone(9);
        tone(9, 1000);
        delay(200);
        noTone(9);

        }

      }
      else
      {
        lcd.clear();
        lcd.print("Invalid Key");
        tone(9, 1000);
        delay(200);
        noTone(9);
        delay(100);
        tone(9, 1000);
        delay(200);
        noTone(9);
        delay(100);
        tone(9, 1000);
        delay(200);
        noTone(9);

      }

    }

  mfrc522.PICC_HaltA();
  if (CardUid)
  {

  return CardUid;
  }
  else
  {
    return NULL;
  }
}

void DetectMovement()
{
  movementDetected = true;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Movement Detected!");
  lcd.setCursor(0, 1);
  lcd.print("Alarm in: ");

  for (int i = 10; i > 0; i--)
  {
    ReadRfid();

    if (isArmed)
    {
    lcd.setCursor(10, 1);
    lcd.print(i);
    lcd.print(" ");
    delay(1000);
    }
    else
    {
      break;
    }
  }

  if (isArmed)
  {
    tone(9, 1000);
      lcd.clear();
      lcd.print("ALARM!!!");
      isAlarmed = true;
  }
}

void ReadRfid()
{


  if (mfrc522.PICC_IsNewCardPresent()) {
      Serial.println("Test");
    if (mfrc522.PICC_ReadCardSerial()) {

      Serial.println("Test");

      byte CardUid[mfrc522.uid.size];

      for (byte i = 0; i < mfrc522.uid.size; i++)
      {
        CardUid[i] = mfrc522.uid.uidByte[i];
      }

      if (CompareCard(CardUid, CorrectUid, mfrc522.uid.size))
      {
        noTone(9);
        tone(9, 1000);
        delay(200);
        noTone(9);
        ToggleArm();
      }
      else
      {
        if (!isArmed)
        {
        tone(9, 1000);
        delay(200);
        noTone(9);
        delay(100);
        tone(9, 1000);
        delay(200);
        noTone(9);
        delay(100);
        tone(9, 1000);
        delay(200);
        noTone(9);
        }
      }
    }
  }
  mfrc522.PICC_HaltA();
}

void ToggleArm()
{
  if (isArmed)
  {
    isArmed = false;
    stateChanged = true;
    noTone(9);
    isAlarmed = false;
    movementDetected = false;
  }
  else
  {
    isArmed = true;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Sys activate in:");
    for (int i = 10; i > 0; i--)
    {
      ReadRfid();
      if (isArmed)
      {
      lcd.setCursor(0, 1);
      delay(1000);
      lcd.print(i);
      lcd.print(" ");
      if (i <= 5 && i > 1)
      {
        tone(9, 1000);
        delay(150);
        noTone(9);
      }
      else if (i == 1)
      {
        tone(9, 1000);
        delay(500);
        noTone(9);
      }
      }
      else
      {
        break;
      }
    }
    stateChanged = true;
  }
}

bool CompareCard(byte *uid1, byte *uid2, byte size)
{
  for (byte i = 0; i < size; i++)
  {
    if (uid1[i] != uid2[i])
    {
      return false;
    }
  }

  return true;
}
