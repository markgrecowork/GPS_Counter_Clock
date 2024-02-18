// Thanks Rual
  // Jan 2024 ReDo
  // Completed Housing 1/17/2024
  // Baud Rate Check GPS Set to 9600 !!!!!!!!!!!!!!!!!!!!!!!!!!! 
  // EEPROM holds and same $GPZDA string works
  // Use ONLY $GNZDA String M8N Mogule 9/26/2021 READ !!!!!!!!!!!!!!!!!!!!
  // 14:34:53  $GPZDA,143453.00,03,02,2024,00,00*67 Set up GPS and save to EEPROM !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // Using Nano
  // 2/14/2024 Delay on first screen works 
  // NEO6M GPS MODULE
  // Today Code cleaning and Commenting 2_18_2024



  #include <Wire.h>
  #include <LiquidCrystal_I2C.h>

  LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 20 chars and 4 line display
  //Connect the Counter to pin 5
  //Connect the TX of the GPS to RX pin D1 on the Nano
  //Connect the RX of the GPS to pin D0 I think this connection does nothing????????
  //Connect SCL and SCA fron the Arduino to the LCD This is SCL to pin 5 and SCA to pin 4
  //
  //
  //DO NOT CONNECT A GROUND WIRE TO THE COUNTER THIS WILL MAKE THE ARDUINO COUNT UP FAST AND IGNORE THE PROGRAM????????
  //ONLY ABLE TO USE 5V USB POWER BANK TO POWER THIS PROJECT AS A WALWART MAKES THE ARDUINO COUNT UP FAST????

  
  uint8_t rows = 4;
  uint8_t cols = 20;
  uint8_t timeset = 0;
  int clearneeded;

  bool ledState = LOW;
  volatile bool ppsTriggered = false;
  char serialBuffer[100];

///////////////////////
void ppsHandler(void);

void ppsHandler(void)
{
  ppsTriggered = true;
}
//////////////////////////////////
void pulseTheTime(int);

void pulseTheTime(long counter) {
  digitalWrite(4, HIGH);   //pulse the reset output
  delay(10);
  digitalWrite(4, LOW);
  for (int x = 0; x < counter; x++) {    //count up the time for right now.
    digitalWrite(5, LOW);
    digitalWrite(5, HIGH);    //Connect the Counter to pin 5
    digitalWrite(5, LOW);

  }
  return;
}

///////////////////////////Start Up Screen////////////////
void setup() {

  lcd.init();       //Copied From Example Code
  lcd.init();       //Copied From Example Code
  lcd.backlight();  //Copied From Example Code

                      //Start Up Screen
  lcd.setCursor(0, 1);
  lcd.print(" -G-[Rual GPS]-G-");    //Start Screen Added G shows its me looking at code
  lcd.setCursor(0, 2);
  lcd.print("..PROGRAM STARTING..");    //Start Screen
  delay(3000);    // Would like the above to show on the lcd screen for 3 seconds
  Serial.begin(9600);  //Depends on the GPS Module !!!!!!!!!!!!!!!
  pinMode(13, OUTPUT);
  pinMode(5, OUTPUT);   //Connect the Counter to pin 5
  digitalWrite(5, LOW);
  pinMode(2, INPUT);
  attachInterrupt(digitalPinToInterrupt(2), ppsHandler, RISING);
  for (int x = 0; x < 20; x++) {
    digitalWrite(13, HIGH);
    delay(10);
    digitalWrite(13, LOW);
    delay(10);
  }
  lcd.clear();

}


//////////////////////////////////////////////
void loop() {

  lcd.setCursor(0, 0);
  if (ppsTriggered) {
    ppsTriggered = false;
    ledState = !ledState;
    digitalWrite(13, ledState);
    digitalWrite(5, LOW);
    digitalWrite(5, HIGH);
    digitalWrite(5, LOW);
  }

  if (Serial.available()) { // wait a bit for the entire message to arrive
    delay(400);

    while (Serial.available() > 0) {
      for (int n = 0; n < 100; n++) {
        serialBuffer[n] = Serial.read();
      }
    }
    String gpsString = serialBuffer;
    char gpsValid = gpsString.charAt(24);   //??WRONG?? ??WRONG?? All im doing is using the TWO (2) in the year as a good signal
    lcd.print(gpsString.substring(7, 9)); //HOUR
    lcd.print(":");
    lcd.print(gpsString.substring(9, 11));  //MINUTE
    lcd.print(":");
    lcd.print(gpsString.substring(11, 13)); //SECONDS
    lcd.print(" UTC");  // UTC TIME
    lcd.print(" ruel.io");

    // Good Above Here in reguards to the display
    
    String timestring = gpsString.substring(7, 9) + gpsString.substring(9, 11) + gpsString.substring(11, 13);
    long timeint = timestring.toInt();
    long advance = 0;

    if (!timeset) {
      advance = 1;      //Must be or me doing something wrong 2_10_2024
      pulseTheTime(advance);
      advance = timeint;
      pulseTheTime(advance);
      timeset = 40;
    }
                //Can we set the time zone???????????
                //Can we set Daylight Savings Time ??????????
    if (gpsString.substring(11, 13) == "00") {
      advance = 40;
      pulseTheTime(advance);
    }
    if (gpsString.substring(9, 11) == "00") {
      advance = 4040;
      pulseTheTime(advance);
    }

    lcd.setCursor(0, 3);
    
    clearneeded--;

    if (advance > 0) {
      lcd.setCursor(0, 3);
      lcd.print("Advance Pulse:");
      clearneeded = 2;
      lcd.print(advance);
    } else if (!clearneeded) {
      lcd.print("GPS SYNC TIME");
      lcd.print("   ");
      lcd.write(127);   //What does this do? what is 127? its the arrow dummy?????
      lcd.print("  |");
    }
    lcd.setCursor(0, 2);

    if (gpsValid == '2') {     //All im doing is using the TWO in the year as a good signal GPS Lock I THINK
      lcd.print("  NO POSITION LOCK  ");
      lcd.setCursor(0, 2);
      if (ledState) {
        lcd.print("--**<RuhNet GPS>**--");
      } else {
        lcd.print("Searching for GPS");
      }     
    } else {
      lcd.print(" GPS LOCK");

      lcd.setCursor(18, 2);    //Data validity status 2 = data valid, the only thing im using Using the 2 in 2024
      lcd.print(gpsString.substring(23, 24));   //Using the 2 in 2024 Data validity status 2 = data valid, the only thing im using
      
      lcd.setCursor(5, 1);
      lcd.print(gpsString.substring(20, 22));   // Month
      lcd.print("-");
      lcd.print(gpsString.substring(17, 19));   // Day
      lcd.print("-");
      lcd.print(gpsString.substring(23, 27));   // Year
      
    }
  }



}
