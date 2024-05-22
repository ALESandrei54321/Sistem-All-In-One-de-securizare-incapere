#include <LiquidCrystal_I2C.h>
#include <Adafruit_Fingerprint.h>

#include "Wire.h"
#include "I2CKeyPad.h"

const uint8_t KEYPAD_ADDRESS = 0x20;

I2CKeyPad keyPad(KEYPAD_ADDRESS);

char keys[] = "123A456B789C*0#DNF";

#include <TimerOne.h>
#include <dht.h>
#include "pitches.h"

#define MQ2pin 0
#define LOCK_PIN 17
#define BUZZER_PIN 15
#define DHT11_PIN 16
//#define DHTTYPE DHT11

#include <SPI.h>
#include <MFRC522.h>
 
#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.


char password[] = "1987";

SoftwareSerial mySerial(2, 3);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

LiquidCrystal_I2C dis(0x27, 16, 2);

byte ID = 1; // Enter your fingerprint ID
bool foundFinger = false;

//DHT dht(DHT11_PIN, DHTTYPE);
dht DHT; 

int melody[] = {
  NOTE_C4, NOTE_C4, 
  NOTE_D4, NOTE_C4, NOTE_F4,
  NOTE_E4, NOTE_C4, NOTE_C4, 
  NOTE_D4, NOTE_C4, NOTE_G4,
  NOTE_F4, NOTE_C4, NOTE_C4,
  
  NOTE_C5, NOTE_A4, NOTE_F4, 
  NOTE_E4, NOTE_D4, NOTE_AS4, NOTE_AS4,
  NOTE_A4, NOTE_F4, NOTE_G4,
  NOTE_F4
};

int durations[] = {
  4, 8, 
  4, 4, 4,
  2, 4, 8, 
  4, 4, 4,
  2, 4, 8,
  
  4, 4, 4, 
  4, 4, 4, 8,
  4, 4, 4,
  2
};



byte human0[] = {
  B01110,
  B01110,
  B01110,
  B00100,
  B01110,
  B10101,
  B00100,
  B01010
};

byte human1[] = {
  B01110,
  B01110,
  B01110,
  B10101,
  B01110,
  B00100,
  B00100,
  B01010
};

bool isEnteringPassword = false;


void setup() {
  Serial.begin(9600);
  dis.init();
  dis.backlight();
  dis.setCursor(0, 0);

  dis.print("Setting up...");
  
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  mfrc522.PCD_DumpVersionToSerial();

  pinMode(LOCK_PIN, OUTPUT);
  pinMode(DHT11_PIN, INPUT);

  while (!Serial);  // For Yun/Leo/Micro/Zero/...
  delay(100);
  Serial.println("\n\nAdafruit finger detect test");

  // set the data rate for the sensor serial port
  finger.begin(57600);
  delay(5);
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
    foundFinger = true;
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    foundFinger = false;
  }

  if (foundFinger) {
    finger.getParameters();
    finger.getTemplateCount();
    if (finger.templateCount == 0) {
      Serial.print("Sensor doesn't contain any fingerprint data. Please run the 'enroll' example.");
    }
    else {
      Serial.println("Waiting for valid finger...");
      Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
    }
  }
  
  pinMode(BUZZER_PIN, OUTPUT);

  //dht.begin();

  Timer1.initialize(8 * 1000000); // Initialize timer to trigger every 10 seconds
  Timer1.attachInterrupt(fireCheck); // Attach the fireCheck function to the timer interrupt


  Wire.begin();
  Wire.setClock(400000);
  if (keyPad.begin() == false)
  {
    Serial.println("\nERROR: cannot communicate to keypad.\nPlease reboot.\n");
    while(1);
  }


  dis.createChar(0, human0);
  dis.createChar(1, human1);


  dis.clear();




}

void openDoor() {
  digitalWrite(LOCK_PIN, HIGH);

  tone(BUZZER_PIN, 500, 200); // C5 note for 200 milliseconds
  delay(100);
  tone(BUZZER_PIN, 520, 200); // D5 note for 200 milliseconds
  delay(100);
  tone(BUZZER_PIN, 570, 200); // E5 note for 200 milliseconds
  delay(100);
  tone(BUZZER_PIN, 590, 200); // E5 note for 200 milliseconds
  delay(100);
  noTone(BUZZER_PIN); // Stop the buzzer

  delay(3000);
  digitalWrite(LOCK_PIN, LOW);

  tone(BUZZER_PIN, 590, 200); // C5 note for 200 milliseconds
  delay(100);
  tone(BUZZER_PIN, 570, 200); // D5 note for 200 milliseconds
  delay(100);
  tone(BUZZER_PIN, 520, 200); // E5 note for 200 milliseconds
  delay(100);
  tone(BUZZER_PIN, 500, 200); // E5 note for 200 milliseconds
  delay(100);

}

void checkFingerprint() {
  int value = getFingerprintIDez();

  if (value == ID) {
    dis.clear();
    dis.setCursor(0, 0);
    dis.print("Acces granted!");
    openDoor();
    dis.clear();
    isEnteringPassword = false;
  }

}

void checkRFID(){

  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  //Show UID on serial monitor
  Serial.print("UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();

  content.toUpperCase();
  if (content.substring(1) == "2A CA 05 B1") //change here the UID of the card/cards that you want to give access
  {
    dis.clear();
    dis.setCursor(0, 0);
    dis.print("Access granted.");
    isEnteringPassword = false;
    openDoor();
  } else if (content.substring(1) == "93 25 6F A9") //change here the UID of the card/cards that you want to give access
  {
    dis.setCursor(0, 0);
    dis.print("Happy birthday!");
    //openDoor();

    int size = sizeof(durations) / sizeof(int);

    int current_char = 0;
   

    for (int note = 0; note < size; note++) {
      //to calculate the note duration, take one second divided by the note type.
      //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.

      


      int duration = 1000 / durations[note];
      tone(BUZZER_PIN, melody[note], duration);

      if (note % 5 == 0) {
        dis.clear();
        dis.setCursor(0, 0);
        dis.print("Happy birthday!");

        dis.setCursor(0, 1);
        for (int pos = 0; pos < 16; pos++) {
          dis.write(abs(pos % 2 - current_char));
        }
        current_char = 1 - current_char;
        //to distinguish the notes, set a minimum time between them.
        //the note's duration + 30% seems to work well:
        
      }
      

      

      int pauseBetweenNotes = duration * 1.30;
      delay(pauseBetweenNotes);
      
      //stop the tone playing:
      noTone(BUZZER_PIN);
    }

  }
  
  dis.clear();
}


void checkPassword() {
  uint8_t idx = keyPad.getKey();
  static char input[17] = {0}; // Define input and redacted as static to preserve their state
  static char redacted[17] = {0};
  static int len = 0; // Define len as static to preserve its value
  
  if(keys[idx] == 'N') {
    return;
  }

  if (keys[idx] != 'N' && keys[idx] != 'A' && len < 16) {
    input[len] = keys[idx];
    redacted[len] = '*';
    len++;
    dis.clear();
    dis.setCursor(0, 0);
    dis.print(redacted);
    tone(BUZZER_PIN, 700, 100);
    isEnteringPassword = true;
  }
  
  if (keys[idx] == 'A') {
    dis.clear();
    isEnteringPassword = false;
    input[len] = '\0'; // Null terminate the input string
    redacted[len] = '\0'; // Null terminate the redacted string
    len = 0; // Reset len
    if (strcmp(input, password) == 0) {
      dis.setCursor(0, 0);
      dis.print("Correct password.");
      openDoor();
    } else {
      dis.setCursor(0, 0);
      dis.print("Wrong password.");
      tone(BUZZER_PIN, 230, 300);
      delay(300);
      tone(BUZZER_PIN, 180, 300);
    }
    delay(1000);
    dis.clear();
    memset(input, 0, sizeof(input)); // Clear input array
    memset(redacted, 0, sizeof(redacted)); // Reset redacted array
  }
  delay(300);
}

float t, h = 0;
float smoke = 0;

void fireCheck() {
  int chk = DHT.read(DHT11_PIN);
  smoke = analogRead(MQ2pin);
  t = DHT.temperature;
  h = DHT.humidity;


  Serial.print(F("Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print(F("Hunidity: "));
  Serial.print(h);
  Serial.print(F(" "));
  Serial.print(F("Smoke value: "));
  Serial.print(smoke);
  Serial.print(F(" "));
  Serial.println(millis());

  if (t > 40 && smoke > 400) {
    dis.clear();
    dis.setCursor(0, 0);
    dis.print("Fire detected.");
    
    digitalWrite(LOCK_PIN, HIGH);

    unsigned long startTime = millis(); // Record the start time
    
    while (millis() - startTime < 8000) { // Run for 10 seconds
      tone(BUZZER_PIN, 700, 500);
      delay(500);
      tone(BUZZER_PIN, 462, 500);
      delay(500);
    }
    
    digitalWrite(LOCK_PIN, LOW);
    dis.clear();
  }



}

void loop() {
  if (foundFinger)
    checkFingerprint();
  checkPassword();
  checkRFID();
  
  if (!isEnteringPassword) { // Update display only if not entering password
    dis.setCursor(0, 0);
    dis.print("Temp: ");
    dis.print(t);
    dis.print((char)223);
    dis.print("C");

    dis.setCursor(0, 1);
    dis.print("Smoke: ");
    dis.print(smoke);
  }
}


// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;

  return finger.fingerID;
}