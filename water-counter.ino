// Leon Santen

// if doesnt connect to arduino:
// sudo chmod a+rw /dev/ttyACM0

// if display doesn't work --> page 140 - https://github.com/Freenove/Freenove_Ultimate_Starter_Kit_for_Arduino/blob/master/Tutorial.pdf 

#include<LiquidCrystal.h>

int waterSensor; //raw value from waterSensor is between 0 and 1023
int waterOnOff = 0; // 0 for water off; 1 for water on 
int thresholdMillis = 10; // thresholdMillis in raw (0-1024)
int waterSeconds; // water usage in seconds

int clockmil; //milliseconds of water usage for counter 
int clocksec; //seconds
int clockmin; //minutes
int clockhr; //hours
int t; //temporary variable for display calculation - in seconds

int clockLine = 1; //line position for water time on display
int clockColumn = 4; //column position for water time on display

float voltage; //converted value of range 0.0-5.0 V

uint32_t currentMillis; // currentMillis millis 
uint32_t oldMillis = 0; // oldMillis millis()
uint32_t rawTime = 0; // raw time in millisecond


// initalize the library with the numbers of the interface pins
LiquidCrystal lcd(12,11,5,4,3,2);

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
  // put your main code here, to run repeatedly:

  currentMillis = millis();
  waterSensor = analogRead(A5);
  voltage = waterSensor * (5.0/1023.0);
  Serial.println(waterSensor);

  if (waterSensor > thresholdMillis and waterOnOff == 0) { // if water starts running
    waterOnOff = 1;
    rawTime = rawTime + (currentMillis - oldMillis);
  }
  else if (waterSensor >= thresholdMillis and waterOnOff == 1) {
    rawTime = rawTime + (currentMillis - oldMillis);
  }
  else if (waterSensor <= thresholdMillis and waterOnOff == 1) { // if water stops running
    waterOnOff = 0;
    rawTime = rawTime + (currentMillis - oldMillis);
  }
  
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
  int adjSec = 0; //adjustment variables for adjusting placement of digits when >= 10; not adjusted --> 0; adjustement --> 1
  int adjMin = 0; 
  int adjHr = 0;
  
  if (clocksec >= 10){
    adjSec = 1;
  }  
  if (clockmin >= 10){
    adjMin = 1;
  }
  if (clockhr >= 10){
    adjHr = 1;
  }

  // // INTERFACE - DISPLAY
  if (waterOnOff == 1){
    lcd.setCursor(0,0);
    lcd.print("--            --");
  }
  else {
    lcd.setCursor(0,0);
    lcd.print("Your Water Usage");
  }
  

  //Time HH:MM:SS 
  // -- hours
  if (adjHr ==0){
    lcd.setCursor(clockColumn, clockLine);
    lcd.print("0");
  }
  lcd.setCursor(clockColumn + 1 - adjHr, clockLine);
  lcd.print(clockhr);
  // -- :
  lcd.setCursor(clockColumn + 2, clockLine);
  lcd.print(":");
  // -- minutes
  if (adjMin == 0){
    lcd.setCursor(clockColumn + 3, clockLine);
    lcd.print("0");
  }
  lcd.setCursor(clockColumn + 4 - adjMin, clockLine);
  lcd.print(clockmin);
  // -- :
  lcd.setCursor(clockColumn + 5, clockLine);
  lcd.print(":");
  // -- seconds
  if (adjSec == 0){
    lcd.setCursor(clockColumn + 6, clockLine);
    lcd.print("0");
  }  
  lcd.setCursor(clockColumn + 7 - adjSec, clockLine);
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
    lcd.setCursor(clockColumn, clockLine);
    lcd.print("0");
  }
  lcd.setCursor(clockColumn + 1 - adjHr, clockLine);
  lcd.print(clockhr);
  // -- :
  lcd.setCursor(clockColumn + 2, clockLine);
  lcd.print(":");
  // -- minutes
  if (adjMin == 0){
    lcd.setCursor(clockColumn + 3, clockLine);
    lcd.print("0");
  }
  lcd.setCursor(clockColumn + 4 - adjMin, 0);
  lcd.print(clockmin);
  // -- :
  lcd.setCursor(clockColumn + 5, clockLine);
  lcd.print(":");
  // -- seconds
  if (adjSec == 0){
    lcd.setCursor(clockColumn + 6, clockLine);
    lcd.print("0");
  }  
  lcd.setCursor(clockColumn + 7 - adjSec, clockLine);
  lcd.print(clocksec);

  lcd.setCursor(10, 0);
  lcd.print(clockmil);

  lcd.setCursor(10, 1);
  lcd.print(voltage);
  */  

  

}
