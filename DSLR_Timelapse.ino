//====================================================================================================
//                         Arduino DSLR Timelapse Controller with OLED Display
//====================================================================================================
// Kode skrevet 120723 af Johannes Dahl
// Revideret 150723 af Johannes Dahl


#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define outputA 3
#define outputB 4
int counter = 0;
int currentStateA;
int lastStateA;
String currentDir ="";

const int buttonPin = 6;
const int ledPin = 12;
int selector = 0;
boolean isPressed = false;
unsigned long previousMillis = 0;
int ledState = LOW;
int cameraState = LOW;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3C for 128x64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16
static const unsigned char PROGMEM DSLR_logo[] =
{ 0b00000000, 0b00000000,
  0b00000000, 0b00000000,
  0b00000011, 0b11000000,
  0b00100111, 0b11001100,
  0b01111111, 0b11111110,
  0b11111100, 0b00111111,
  0b11111011, 0b11011111,
  0b11110110, 0b01101111,
  0b11110100, 0b00101111,
  0b11110100, 0b00101111,
  0b11110110, 0b01101111,
  0b11111011, 0b11011111,
  0b11111100, 0b00111111,
  0b11111111, 0b11111111,
  0b01111111, 0b11111110,
  0b00000000, 0b00000000 };

  // 'DSLR logo', 128x64px
const unsigned char FabLab_splash [] PROGMEM = {
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xf9, 0xff, 0xf9, 0xff, 0xff, 0xff, 0xff, 0xff, 0x8f, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0x80, 0x3f, 0xc0, 0xf8, 0x38, 0x7c, 0x1f, 0xff, 0x83, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xfc, 0x00, 0x1f, 0x00, 0xf0, 0x38, 0x38, 0x0f, 0xc0, 0x02, 0x7f, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xf8, 0x1f, 0x1e, 0x1c, 0xe3, 0x3e, 0x20, 0xcf, 0x80, 0x1c, 0x3f, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xf8, 0xff, 0x9c, 0x38, 0xe6, 0x3e, 0x03, 0xcf, 0xcf, 0x3e, 0x7f, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xfb, 0xff, 0x1c, 0xf1, 0xc6, 0x7e, 0x07, 0x8f, 0xff, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xfe, 0x7f, 0x1c, 0xf3, 0xcc, 0x7c, 0x1f, 0x1f, 0xfe, 0x3f, 0xff, 0xff, 0xff, 0xbf, 0xff, 
	0xff, 0xfc, 0x7e, 0x3c, 0x73, 0xcc, 0xfc, 0x3e, 0x1f, 0xfe, 0x7c, 0xe0, 0xe7, 0xc3, 0x01, 0xff, 
	0xff, 0xfc, 0x7c, 0x7e, 0x3f, 0x8e, 0xfc, 0x78, 0x3f, 0xfe, 0x78, 0xe0, 0x03, 0x81, 0x00, 0xff, 
	0xff, 0xfc, 0xf8, 0x7f, 0x1f, 0x9f, 0xf8, 0xc0, 0xff, 0xfc, 0x78, 0xe0, 0x03, 0x01, 0x01, 0xff, 
	0xff, 0xf8, 0xf0, 0xff, 0x8f, 0x9f, 0xf8, 0x83, 0xff, 0xfc, 0x79, 0xc2, 0x23, 0x23, 0x23, 0xff, 
	0xff, 0xf8, 0xc1, 0xff, 0xcf, 0x1f, 0xf1, 0xc3, 0xff, 0xfc, 0xf1, 0xc4, 0x22, 0x07, 0xc7, 0xff, 
	0xff, 0xf8, 0x07, 0xe0, 0x0f, 0x00, 0x73, 0xf0, 0x7f, 0xf8, 0xf0, 0xcc, 0x62, 0x01, 0xc1, 0xff, 
	0xff, 0xf0, 0x0f, 0xc0, 0x1e, 0x00, 0x63, 0xf8, 0x7f, 0xf8, 0xf0, 0x0c, 0xe0, 0x01, 0xc1, 0xff, 
	0xff, 0xf0, 0x7f, 0xc0, 0x1e, 0x00, 0x47, 0xf8, 0x7f, 0xf9, 0xf8, 0x9c, 0xe3, 0x01, 0xe1, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xfc, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xf0, 0x20, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xc0, 0xf8, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0x80, 0xfe, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0x00, 0x07, 0x87, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xfe, 0x00, 0x01, 0xc3, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xfe, 0x70, 0x00, 0xf3, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xfc, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xfd, 0xfc, 0x00, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xf9, 0xfc, 0x01, 0xfc, 0xf0, 0x01, 0xc7, 0xc0, 0x06, 0x7f, 0xfe, 0x3f, 0x00, 0x3f, 0xff, 
	0xff, 0xf9, 0xff, 0x03, 0xfc, 0xf0, 0x03, 0x87, 0xc0, 0x06, 0x7f, 0xfc, 0x3f, 0x00, 0x1f, 0xff, 
	0xff, 0xf9, 0xff, 0xff, 0xf8, 0xf3, 0xff, 0x87, 0xcf, 0xe6, 0x7f, 0xfc, 0x1f, 0x3f, 0x1f, 0xff, 
	0xff, 0xf1, 0xc7, 0xff, 0xf0, 0x73, 0xff, 0x13, 0xcf, 0xe6, 0x7f, 0xf8, 0x9f, 0x3f, 0x1f, 0xff, 
	0xff, 0xf1, 0x81, 0xfe, 0x00, 0x70, 0x0f, 0x33, 0xc0, 0x06, 0x7f, 0xf8, 0x8f, 0x00, 0x3f, 0xff, 
	0xff, 0xf1, 0x80, 0xfc, 0x00, 0x70, 0x0e, 0x31, 0xc0, 0x06, 0x7f, 0xf9, 0xcf, 0x00, 0x3f, 0xff, 
	0xff, 0xf1, 0x80, 0xf8, 0x00, 0x73, 0xfe, 0x01, 0xc7, 0xe6, 0x7f, 0x30, 0x07, 0x3f, 0x1f, 0xff, 
	0xff, 0xf9, 0x80, 0x70, 0x00, 0x73, 0xfc, 0x00, 0xcf, 0xe6, 0x7f, 0x30, 0x07, 0x3f, 0x3f, 0xff, 
	0xff, 0xf9, 0x80, 0x70, 0x0c, 0xf3, 0xfc, 0xfc, 0xc0, 0x06, 0x00, 0x23, 0xe7, 0x00, 0x1f, 0xff, 
	0xff, 0xf9, 0xc0, 0x70, 0x1c, 0xf3, 0xfc, 0xfc, 0x40, 0x06, 0x00, 0x27, 0xe3, 0x00, 0x3f, 0xff, 
	0xff, 0xf9, 0xc0, 0x70, 0x1c, 0xf7, 0xfd, 0xfe, 0xe0, 0x0f, 0x00, 0x6f, 0xf3, 0x00, 0x7f, 0xff, 
	0xff, 0xfc, 0xe0, 0xf0, 0x3d, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xfe, 0x60, 0xf8, 0x71, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xfe, 0x01, 0xff, 0xc3, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf9, 0xe7, 0x9f, 0xff, 
	0xff, 0xff, 0x01, 0xff, 0x87, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf5, 0xc7, 0x1f, 0xff, 
	0xff, 0xff, 0x81, 0xfe, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe5, 0x96, 0xdf, 0xff, 
	0xff, 0xff, 0xc0, 0xf8, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xcd, 0x34, 0xdf, 0xff, 
	0xff, 0xff, 0xf0, 0x20, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x9c, 0x71, 0xdf, 0xff, 
	0xff, 0xff, 0xfc, 0x21, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3c, 0xf3, 0xdf, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xef, 0xff, 0xbf, 0xff, 0xdb, 0xbd, 0xef, 0xdf, 0x06, 0x0c, 0xd8, 0x3f, 0xff, 
	0xff, 0xf8, 0x30, 0x7e, 0x18, 0x30, 0x60, 0xdb, 0xb8, 0xef, 0xdf, 0x3e, 0xec, 0x5b, 0xff, 0xff, 
	0xff, 0xf8, 0x37, 0x6d, 0xd9, 0xb0, 0x6f, 0xd8, 0x3a, 0xef, 0xdf, 0x06, 0x0d, 0x18, 0x7f, 0xff, 
	0xff, 0xfd, 0xb3, 0x6d, 0xd9, 0xb3, 0x6f, 0xdb, 0xb0, 0x6e, 0xdd, 0x3e, 0xdd, 0x99, 0xff, 0xff, 
	0xff, 0xfc, 0x30, 0x6f, 0xdc, 0x38, 0x7f, 0xdb, 0xb7, 0x60, 0xc1, 0x86, 0xed, 0xd8, 0x3f, 0xff, 
	0xff, 0xff, 0xf7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

// Array of all bitmaps for convenience. (Total bytes used to store images in PROGMEM = 1040)
const int epd_bitmap_allArray_LEN = 1;
const unsigned char* epd_bitmap_allArray[1] = {
	FabLab_splash
};

// 'DSLR_top_logo', 108x16px
const unsigned char DSLR_top_logo [] PROGMEM = {
	0x00, 0x60, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xfc, 
	0x03, 0xf0, 0x7c, 0x78, 0x3e, 0x00, 0x07, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xfe, 0x0f, 0xf0, 
	0xfc, 0x7c, 0x7f, 0x03, 0xff, 0xd8, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x0e, 0x1e, 0x31, 0xcc, 0x1d, 
	0xf3, 0x07, 0xfe, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x70, 0x06, 0x3c, 0x71, 0x9c, 0x1f, 0xc3, 0x03, 
	0x0c, 0x18, 0x00, 0x00, 0x00, 0x00, 0x40, 0x0e, 0x30, 0xe3, 0x98, 0x1f, 0x87, 0x00, 0x0c, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x18, 0x0e, 0x30, 0xc3, 0x38, 0x3e, 0x0e, 0x00, 0x1c, 0x00, 0x00, 0x00, 
	0x04, 0x00, 0x38, 0x1c, 0x38, 0xc3, 0x30, 0x3c, 0x1e, 0x00, 0x18, 0x31, 0xf1, 0x83, 0xcf, 0xe0, 
	0x38, 0x38, 0x1c, 0x07, 0x10, 0x38, 0x7c, 0x00, 0x18, 0x71, 0xff, 0xc7, 0xef, 0xf0, 0x30, 0x78, 
	0x0e, 0x06, 0x00, 0x73, 0xf0, 0x00, 0x38, 0x71, 0xff, 0xcf, 0xef, 0xe0, 0x70, 0xf0, 0x07, 0x06, 
	0x00, 0x77, 0xc0, 0x00, 0x38, 0x63, 0xdd, 0xcd, 0xcd, 0xc0, 0x73, 0xe0, 0x03, 0x0e, 0x00, 0xe3, 
	0xc0, 0x00, 0x30, 0xe3, 0xbd, 0xdf, 0x83, 0x80, 0x7f, 0x81, 0xff, 0x0f, 0xf8, 0xc0, 0xf8, 0x00, 
	0x70, 0xf3, 0x39, 0xdf, 0xe3, 0xe0, 0xff, 0x03, 0xfe, 0x1f, 0xf9, 0xc0, 0x78, 0x00, 0x70, 0xff, 
	0x31, 0xff, 0xe3, 0xe0, 0xf8, 0x03, 0xfe, 0x1f, 0xfb, 0x80, 0x78, 0x00, 0x60, 0x76, 0x31, 0xcf, 
	0xe1, 0xe0, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// Array of all bitmaps for convenience. (Total bytes used to store images in PROGMEM = 240)
const int myBitmapallArray_LEN = 1;
const unsigned char* myBitmapallArray[1] = {
	DSLR_top_logo
};


//====================================================================================================
//                                     		SETUP
//====================================================================================================
void setup() {
  Serial.begin(9600);
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("Skærm fejl"));
    for(;;); // Fortsæt ikke
  }
//void setRotation(0);
pinMode(outputA,INPUT);
pinMode(outputB,INPUT);
lastStateA = digitalRead(outputA);

pinMode(buttonPin, INPUT_PULLUP);
pinMode(ledPin, OUTPUT);

splash();
loading();
topLogo();

display.setTextSize(1);      // Normal 1:1 pixel scale
display.setTextColor(SSD1306_WHITE); // Draw white text
display.setCursor(25, 27);     // Start at top-left corner
display.cp437(true);         // Use full 256 char 'Code Page 437' font
display.write("Time interval");
display.display();
}

//====================================================================================================
//                                          LOOP Start
//====================================================================================================
void loop() {  

//====================================================================================================
//                                            Encoder
//====================================================================================================
  currentStateA = digitalRead(outputA);

  if (currentStateA != lastStateA  && currentStateA == 1){
    if (digitalRead(outputB) != currentStateA) {
      counter ++;
      currentDir ="CW";
    } else {
      if (counter > 0) {
      counter --;
      currentDir ="ACW";
      } else {
        counter == 0;
      }
    }
 
//====================================================================================================
//                                 Skriv tid til display
//====================================================================================================
  display.setTextSize(2);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
  display.setCursor(2, 45);
  display.fillRect(2, 45, 85, 16, SSD1306_BLACK);
  display.print(counter);
  display.write(" s");
  display.display();
  }
 
  lastStateA = currentStateA;
  delay(1);


//====================================================================================================
//                                        Tryk på knappen
//====================================================================================================
 if (digitalRead(buttonPin) == LOW && isPressed == false ) //Der er trykket på kontakten OG det er den første gang der er aktivitet på digitalRead()
  {
    isPressed = true;  //Sat til true, så den ikke kører koden før kontakten er sluppet igen
    doSwitchStatement(); // Kald funktionen doSwitchStatement
 
    selector++; // Det her sker først efter doSwitchStatement(), så case 0 bliver afviklet ved første tryk på kontakten
    if (selector > 1) {
      selector = 0; // Når den når til case 7 nulstiller den til case 0
    }
    // selector = (selector+1) % 4;  // Burde gøre det samme som if-statement, men jeg kan ikke få det til at virke.
  } else if (digitalRead(buttonPin) == HIGH)
  {
    isPressed = false; // kontakten er sluppet og variablen er reset
  }

  if (cameraState == HIGH){
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= counter*1000) {
      previousMillis = currentMillis;

      if (ledState == LOW) {
      ledState = HIGH;
      digitalWrite(ledPin, ledState);
      delay(50);
      digitalWrite(ledPin, LOW);
      } else {
      ledState = LOW;
      digitalWrite(ledPin, ledState);
    }
  }else{}
 }
}
//====================================================================================================
//                                          LOOP Slut
//====================================================================================================




//====================================================================================================
//                                   Switch CASE for kamera logo
//====================================================================================================
void doSwitchStatement() {
  switch(selector) {
    case 0:
    Serial.println("TÆND");
    display.drawBitmap(90, 44, DSLR_logo, 16, 16, WHITE); 
    display.display();
    cameraState = HIGH;
    break;
  
    case 1:
    Serial.println("SLUK");
    display.fillRect(90, 44, 16, 16, SSD1306_BLACK);
    display.display();
    cameraState = LOW;
    }
  }

//====================================================================================================
//                                       Top logo på display
//====================================================================================================
void topLogo(){
  display.clearDisplay();
  display.invertDisplay(false);
  display.drawBitmap(10, 4, DSLR_top_logo, 108, 16, WHITE);
  display.drawRoundRect(0, 0, 126, 24, SSD1306_WHITE, 2);
  display.display();
}
//====================================================================================================
//                                     Splash skærm på display
//====================================================================================================
void splash(){
  display.clearDisplay();
  display.invertDisplay(true);
  display.drawBitmap(0, 0, FabLab_splash, 128, 64, WHITE);
  display.drawRect(1, 0, 126, 64, SSD1306_BLACK);
  display.display();
  delay(2000);
  display.invertDisplay(false);
  display.clearDisplay();
  display.display();
}
//====================================================================================================
//                                     Loading skærm på display
//====================================================================================================
void loading(){
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(25, 28);     // Start at top-left corner
  //display.cp437(true);         // Use full 256 char 'Code Page 437' font
  display.write("Loading");
  display.display();
  delay(200);
  for (int i = 66; i < 100; i=i+4){
    display.setCursor(i, 28);
    display.write(".");
    display.display();
    delay(200);
    }
}
