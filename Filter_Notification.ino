#include <Wire.h> 
#include <TridentTD_LineNotify.h>
#include "WiFi.h"
#define SSID        "K"    //SSID Wifi
#define PASSWORD    "0123456789"   //Pass Wifi
#define LINE_TOKEN  "iKmEHwuFinGl6QYZtDcHPlkb69i151f97cfnqfzRyQd"   
#define LED_BUILTIN 2
#define SENSOR  5
#include <LiquidCrystal_I2C.h>
const int buttonPin = 4;
int buttonState = 0;
// set the LCD number of columns and rows
LiquidCrystal_I2C lcd(0x27, 16, 2);
long currentMillis = 0;
long previousMillis = 0;
int interval = 1000;
boolean ledState = LOW;
float calibrationFactor = 6.5;
volatile byte pulseCount;
byte pulse1Sec = 0;
float flowRate;
float t=0.0;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;

void IRAM_ATTR pulseCounter()
{
  pulseCount++;
}



void setup() {
      
  Serial.begin(115200);
 pinMode(buttonPin, INPUT);
  lcd.backlight(); 
  Serial.println();
  Serial.println(LINE.getVersion());
  WiFi.begin(SSID, PASSWORD);
  Serial.printf("WiFi connecting to %s\n",  SSID);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(400);
  }
  Serial.printf("\nWiFi connected\nIP : ");
  Serial.println(WiFi.localIP());


  // กำหนด Line Token
  LINE.setToken(LINE_TOKEN);

 pinMode(LED_BUILTIN,OUTPUT);
  pinMode(SENSOR, INPUT_PULLUP);

  pulseCount = 0;
  flowRate = 0.0;
  flowMilliLitres = 0;
  totalMilliLitres = 0;
  previousMillis = 0;

  attachInterrupt(digitalPinToInterrupt(SENSOR), pulseCounter, FALLING);

}

void loop() {
  
  lcd.begin();  
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print(totalMilliLitres);
  lcd.print(" mL");
  lcd.setCursor(0,1);
  lcd.print(t); 
  lcd.print(" Liter"); 
  
   buttonState = digitalRead(buttonPin);
    if (buttonState == HIGH) {  
     pulseCount = 0;
  flowRate = 0.0;
  flowMilliLitres = 0;
  totalMilliLitres = 0;
  previousMillis = 0;
  } 
  currentMillis = millis();
  if (currentMillis - previousMillis > interval) {
    
    pulse1Sec = pulseCount;
    pulseCount = 0;
 flowRate = ((1000.0 / (millis() - previousMillis)) * pulse1Sec) / calibrationFactor;
    previousMillis = millis();

    // Divide the flow rate in litres/minute by 60 to determine how many litres have
    // passed through the sensor in this 1 second interval, then multiply by 1000 to
    // convert to millilitres.
    flowMilliLitres = (flowRate / 60) * 1000;

    // Add the millilitres passed in this second to the cumulative total
    totalMilliLitres += flowMilliLitres;
    
    // Print the flow rate for this second in litres / minute
    Serial.print("Flow rate: ");
    Serial.print(int(flowRate));  // Print the integer part of the variable
    Serial.print("L/min");
    Serial.print("\t");   
    Serial.print("Output Liquid Quantity: ");
    Serial.print(totalMilliLitres);
    Serial.print("mL / ");
    Serial.print(t=totalMilliLitres / 1000);
    Serial.println("L");
      // set cursor to first column, first row
 

  }
  if (buttonState == HIGH) {
    totalMilliLitres = 0;
    t =0 ;
    
  } 
  if ((t >= 2)&&(flowRate>0)) {
    Serial.println("Send Line");
    LINE.notify("ถึงเวลาเปลี่ยนใส้กรองน้ำเเล้วครับ !!");
  delay(10);
  }
   
}  
