// Leon Santen
// if doesnt connect to arduino:
// sudo chmod a+rw /dev/ttyACM0

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
//#include<LiquidCrystal.h> // if display WIHTOUT I2C doesn't work --> page 140 - https://github.com/Freenove/Freenove_Ultimate_Starter_Kit_for_Arduino/blob/master/Tutorial.pdf 

int waterSensor; //raw value from waterSensor is between 0 and 1023
int waterOnOff = 0; // 0 for water off; 1 for water on 
int thresholdMillis = 10; // thresholdMillis in raw (0-1024)
int waterSeconds; // water usage in seconds

int clockmil; //milliseconds of water usage for counter 
int clocksec; //seconds
int clockmin; //minutes
int clockhr; //hours
int t; //temporary variable for display calculation - in seconds
int clocksec_old; //old seconds
int clockmin_old; //old minutes
int clockhr_old; //old hours
int intervalTrigger = 0;

int dispLine = 1; //line position for water time on display
int dispColumn = 4; //column position for water time on display

int adjSec = 0; //adjustment variables for adjusting placement of digits when >= 10; not adjusted --> 0; adjustement --> 1
int adjMin = 0; 
int adjHr = 0;
int adjSec_old;
int adjMin_old; 
int adjHr_old;

float voltage; //converted value of range 0.0-5.0 V

uint32_t currentMillis; // currentMillis millis 
uint32_t oldMillis = 0; // oldMillis millis()
uint32_t rawTime = 0; // raw time in millisecond
uint32_t timeOff; // millis() when waterOnOff if 0
uint32_t resetInterval = 86400000; //time after which counter resets in millis 
uint32_t intervalOffset = 15 * 60 * 60 * 1000; // offset for dailyt timer to reset at a certain time and not after 24h the first time. will keep reseting in 24 period


// initalize the library with the numbers of the interface pins
//LiquidCrystal lcd(12,11,5,4,3,2);
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  // set up the LCD's number of columns and rows
  lcd.begin(16, 2);

  // Welcome Display
  lcd.setCursor(0, 0); // (column, row) --> it starts with 
  lcd.print("WaterCounter");
  lcd.setCursor(13, 1);
  lcd.print(":-)");
  delay(500); //Time of welcome display
  lcd.clear();
}

void loop() {
  currentMillis = millis();
  waterSensor = analogRead(A1);
  voltage = waterSensor * (5.0/1023.0);
  Serial.println(waterSensor);

  // // MAIN LOGIC - WATER COUNTER
  if (waterSensor > thresholdMillis and waterOnOff == 0) { // if water starts running
    waterOnOff = 1;
    rawTime = rawTime + (currentMillis - oldMillis);
    lcd.clear();
  }
  else if (waterSensor > thresholdMillis and waterOnOff == 1) {
    rawTime = rawTime + (currentMillis - oldMillis);
  }
  else if (waterSensor <= thresholdMillis and waterOnOff == 1) { // if water stops running
    waterOnOff = 0;
    timeOff = currentMillis;
    rawTime = rawTime + (currentMillis - oldMillis);
    lcd.clear();
  }
      
  // // TURN BACKGROUND LIGHT ON OFF - ENERGY SAVER
  if (waterOnOff == 1){
    lcd.backlight();
  }
  else if (currentMillis - timeOff > 3000) {
    lcd.noBacklight();
  }

  // if time interval is exceeded --> reset rawTime 
  if ((intervalOffset + currentMillis) % resetInterval <= 100 and intervalTrigger == 0) {  
    clocksec_old = clocksec; //assign current values to old values in order to reset rawTime to start recording new values
    clockmin_old = clockmin;
    clockhr_old = clockhr;  

    adjSec_old = adjSec;
    adjMin_old = adjMin; 
    adjHr_old = adjHr;
    
    rawTime = 0; // reset current counter time
    intervalTrigger = 1;
  }
  else if (currentMillis % resetInterval > 100){
    intervalTrigger = 0;
  }
  // // CLOCK CALCULATION - divide into hours, minutes, seconds, milliseconds
  oldMillis = currentMillis;
  waterSeconds = int(rawTime / 1000);
  clockmil = rawTime % 1000; 

  t = waterSeconds;
  clocksec = t % 60;
  t = (t - clocksec)/60;
  clockmin = t % 60;
  t = (t - clockmin)/60;
  clockhr = t;

  // Account for two digit numbers for display
  if (clocksec >= 10){
    adjSec = 1;
  }  
  if (clockmin >= 10){
    adjMin = 1;
  }
  if (clockhr >= 10){
    adjHr = 1;
  }

  // // INTERFACE - TEXT - DISPLAY
  if (waterOnOff == 1){
    lcd.setCursor(0,0);
    lcd.print("Your Water Usage");
    
  }
  else {
    if (adjHr_old ==0){
      lcd.setCursor(dispColumn, dispLine - 1);
      lcd.print("0");
    }
    lcd.setCursor(dispColumn + 1 - adjHr_old, dispLine - 1);
    lcd.print(clockhr_old);
    // -- :
    lcd.setCursor(dispColumn + 2, dispLine - 1);
    lcd.print(":");
    // -- minutes
    if (adjMin_old == 0){
      lcd.setCursor(dispColumn + 3, dispLine - 1);
      lcd.print("0");
    }
    lcd.setCursor(dispColumn + 4 - adjMin_old, dispLine - 1);
    lcd.print(clockmin_old);
    // -- :
    lcd.setCursor(dispColumn + 5, dispLine - 1);
    lcd.print(":");
    // -- seconds
    if (adjSec_old == 0){
      lcd.setCursor(dispColumn + 6, dispLine - 1);
      lcd.print("0");
    }  
    lcd.setCursor(dispColumn + 7 - adjSec_old, dispLine - 1);
    lcd.print(clocksec_old);
  }
  
  //Time HH:MM:SS 
  // -- hours
  if (adjHr ==0){
    lcd.setCursor(dispColumn, dispLine);
    lcd.print("0");
  }
  lcd.setCursor(dispColumn + 1 - adjHr, dispLine);
  lcd.print(clockhr);
  // -- :
  lcd.setCursor(dispColumn + 2, dispLine);
  lcd.print(":");
  // -- minutes
  if (adjMin == 0){
    lcd.setCursor(dispColumn + 3, dispLine);
    lcd.print("0");
  }
  lcd.setCursor(dispColumn + 4 - adjMin, dispLine);
  lcd.print(clockmin);
  // -- :
  lcd.setCursor(dispColumn + 5, dispLine);
  lcd.print(":");
  // -- seconds
  if (adjSec == 0){
    lcd.setCursor(dispColumn + 6, dispLine);
    lcd.print("0");
  }  
  lcd.setCursor(dispColumn + 7 - adjSec, dispLine);
  lcd.print(clocksec);

  
  /*
  // DEBUG LCD Display #1
  if (waterSensor >= 10 and waterSensor < 100) {
    lcd.setCursor(0, 0);
    lcd.print("  ");
    lcd.setCursor(2, 0); // (column, row) --> it starts with 
    lcd.print(waterSensor);
  }
  else if (waterSensor >= 100 and waterSensor < 1000) {
    lcd.setCursor(0, 0);
    lcd.print(" ");
    lcd.setCursor(1, 0); // (column, row) --> it starts with 
    lcd.print(waterSensor);
    
  }
  else if (waterSensor >= 1000) {
    lcd.setCursor(0, 0); // (column, row) --> it starts with 
    lcd.print(waterSensor);
  }
  else {
    lcd.setCursor(0, 0);
    lcd.print("   ");
    lcd.setCursor(3, 0); // (column, row) --> it starts with 
    lcd.print(waterSensor);    
  }
  
  lcd.setCursor(0, 1);
  lcd.print(voltage);

  lcd.setCursor(7, 1);
  lcd.print(waterSeconds);

  // DEBUG LCD Display #2
  lcd.setCursor(4, 1);
  lcd.print(waterSeconds);

  //Time HH:MM:SS 
  // -- hours
  if (adjHr ==0){
    lcd.setCursor(dispColumn, dispLine);
    lcd.print("0");
  }
  lcd.setCursor(dispColumn + 1 - adjHr, dispLine);
  lcd.print(clockhr);
  // -- :
  lcd.setCursor(dispColumn + 2, dispLine);
  lcd.print(":");
  // -- minutes
  if (adjMin == 0){
    lcd.setCursor(dispColumn + 3, dispLine);
    lcd.print("0");
  }
  lcd.setCursor(dispColumn + 4 - adjMin, 0);
  lcd.print(clockmin);
  // -- :
  lcd.setCursor(dispColumn + 5, dispLine);
  lcd.print(":");
  // -- seconds
  if (adjSec == 0){
    lcd.setCursor(dispColumn + 6, dispLine);
    lcd.print("0");
  }  
  lcd.setCursor(dispColumn + 7 - adjSec, dispLine);
  lcd.print(clocksec);

  lcd.setCursor(10, 0);
  lcd.print(clockmil);

  lcd.setCursor(10, 1);
  lcd.print(voltage);
  */  
}
