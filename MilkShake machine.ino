#include <avr/wdt.h>
#include <AccelStepper.h>
#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 9
#define RST_PIN 8

MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance

// Add this section in your code before the `setup` function
byte cardWithMoney[] = {0xC0, 0x30, 0xB9, 0x21};  // Card with money
byte cardWithoutMoney[] = {0x61, 0xAB, 0x19, 0x27};  // Card without money
//string data="";
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Change 0x27 to 0x3F if necessary

// Define the number of steps per revolution for the motor
const int stepsPerRevolution = 200; // Number of steps per revolution for NEMA 17 and NEMA 23

// Define pin numbers for the motors
const int sugarStepPin = 5;
const int sugarDirPin = 6;
const int sugarEnablePin = 26;

const int cremaStepPin = 3;
const int cremaDirPin = 4;
const int cremaEnablePin = 25;

const int bananaStepPin = A9;
const int bananaDirPin = 10;
const int bananaEnablePin = 28;

const int cerelacStepPin = 11;
const int cerelacDirPin = 12;
const int cerelacEnablePin = 29;

const int oreoStepPin = 7;
const int oreoDirPin = 22;
const int oreoEnablePin = 27;

const int bigCircleStepPin = 23;
const int bigCircleDirPin = 24;
const int bigCircleEnablePin = 2;

const int irSensorPin = 31;

const int blenderRelayPin = 30; // Pin to control the relay for the blender
const int milkRelayPin = 46; // Pin to control the relay for the milk valve
const int cleanRelayPin = A5; // Pin to control the relay for the milk valve

// Define pins for the cup holder motor (NEMA 17)
const int cupHolderIn1Pin = 47;
const int cupHolderIn2Pin = 48;
const int cupHolderIn3Pin = 49;
const int cupHolderIn4Pin = 53;

// Define pins for the DC motor gear (conveyor)
const int dcMotorEnablePin = 13;
const int dcMotorIn1Pin = 40;
const int dcMotorIn2Pin = 41;

// Additional DC motor connected via H-bridge L298 without enable pin
const int additionalDcMotorIn1Pin = A1;
const int additionalDcMotorIn2Pin = A2;

const int ldr1Pin = A3; // Pin for LDR 1 D0
const int ldr2Pin = A4; // Pin for LDR 2 D0
const int ldr3Pin = A6; // Pin for LDR 2 D0

const int relayled = A8; // Define the pin connected to the relay

// Setup AccelStepper motors
AccelStepper sugarStepper(AccelStepper::DRIVER, sugarStepPin, sugarDirPin);
AccelStepper cremaStepper(AccelStepper::DRIVER, cremaStepPin, cremaDirPin);
AccelStepper BananaStepper(AccelStepper::DRIVER, bananaStepPin, bananaDirPin);
AccelStepper CerelacStepper(AccelStepper::DRIVER, cerelacStepPin, cerelacDirPin);
AccelStepper oreoStepper(AccelStepper::DRIVER, oreoStepPin, oreoDirPin);
AccelStepper bigCircleStepper(AccelStepper::DRIVER, bigCircleStepPin, bigCircleDirPin);

AccelStepper cupHolderStepper(AccelStepper::FULL4WIRE, cupHolderIn1Pin, cupHolderIn2Pin, cupHolderIn3Pin, cupHolderIn4Pin);

// Define pins for the NEMA 17 motor controlled by H-bridge L298
const int nema17In1 = 42;
const int nema17In2 = 43;
const int nema17In3 = 44;
const int nema17In4 = 45;

AccelStepper spoutStepper(AccelStepper::FULL4WIRE, nema17In1, nema17In2, nema17In3, nema17In4);


// Keypad setup
const byte ROWS = 4; // four rows
const byte COLS = 4; // four columns
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {32, 33, 34, 35}; // connect to the row pinouts of the keypad
byte colPins[COLS] = {36, 37, 38, 39}; // connect to the column pinouts of the keypad

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {


    Serial.begin(9600);  // Communication with PC


  SPI.begin(); // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522
  // Initialize the LCD
  lcd.begin(16, 2);
  lcd.backlight();

  // Set initial motor configurations
  const int maxSpeed2 = 1000; // Max speed (steps per second)
  const int acceleration2 = 500; // Acceleration (steps per second squared) 
  const int maxSpeed = 70; // Max speed for big circle motor
  const int acceleration = 100; // Acceleration for big circle motor


  sugarStepper.setMaxSpeed(maxSpeed2);
  sugarStepper.setAcceleration(acceleration2);
  cremaStepper.setMaxSpeed(maxSpeed2);
  cremaStepper.setAcceleration(acceleration2);
  BananaStepper.setMaxSpeed(maxSpeed2);
  BananaStepper.setAcceleration(acceleration2);
  CerelacStepper.setMaxSpeed(maxSpeed2);
  CerelacStepper.setAcceleration(acceleration2);
  oreoStepper.setMaxSpeed(maxSpeed2);
  oreoStepper.setAcceleration(acceleration2);
  bigCircleStepper.setMaxSpeed(maxSpeed);
  bigCircleStepper.setAcceleration(acceleration);
  cupHolderStepper.setMaxSpeed(200);
  cupHolderStepper.setAcceleration(100);
  spoutStepper.setMaxSpeed(500);
  spoutStepper.setAcceleration(250);

  // Set pins for the H-bridge motor
  pinMode(cupHolderIn1Pin, OUTPUT);
  pinMode(cupHolderIn2Pin, OUTPUT);
  pinMode(cupHolderIn3Pin, OUTPUT);
  pinMode(cupHolderIn4Pin, OUTPUT);

  // Set pins for the NEMA 17 motor
  pinMode(nema17In1, OUTPUT);
  pinMode(nema17In2, OUTPUT);
  pinMode(nema17In3, OUTPUT);
  pinMode(nema17In4, OUTPUT);

  // Set enable pins as outputs
  pinMode(sugarEnablePin, OUTPUT);
  pinMode(cremaEnablePin, OUTPUT);
  pinMode(bananaEnablePin, OUTPUT);
  pinMode(cerelacEnablePin, OUTPUT);
  pinMode(oreoEnablePin, OUTPUT);
  pinMode(bigCircleEnablePin, OUTPUT);

  pinMode(irSensorPin, INPUT);
  pinMode(blenderRelayPin, OUTPUT); // Set the relay pin as output
  pinMode(milkRelayPin, OUTPUT); // Set the milk relay pin as output
  pinMode(cleanRelayPin, OUTPUT); // Set the clean relay pin as output
  pinMode(relayled, OUTPUT); // Set the relay pin as an output

  // Set pins for the DC motor gear
  pinMode(dcMotorEnablePin, OUTPUT);
  pinMode(dcMotorIn1Pin, OUTPUT);
  pinMode(dcMotorIn2Pin, OUTPUT);

  // Set pins for the additional DC motor
  pinMode(additionalDcMotorIn1Pin, OUTPUT);
  pinMode(additionalDcMotorIn2Pin, OUTPUT);

  pinMode(ldr1Pin, INPUT);
  pinMode(ldr2Pin, INPUT); 
  pinMode(ldr3Pin, INPUT); 

  // Disable all motors initially
  disableMotor(sugarEnablePin);
  disableMotor(cremaEnablePin);
  disableMotor(bananaEnablePin);
  disableMotor(cerelacEnablePin);
  disableMotor(oreoEnablePin);
  disableMotor(bigCircleEnablePin);

  // Ensure the blender and milk valve are off initially
  digitalWrite(blenderRelayPin, HIGH);
  digitalWrite(milkRelayPin, HIGH);
    digitalWrite(cleanRelayPin, HIGH);
  digitalWrite(relayled, HIGH); // Turn off the relay (and the LED)


  // Ensure the motor is disabled initially
  analogWrite(dcMotorEnablePin, 0); // Ensure motor is initially stopped



}
const int priceBanana = 10; // Price for Banana milkshake
const int priceCerelac = 9; // Price for Cerelac milkshake
const int priceOreo = 8; // Price for Oreo milkshake
char cupChar;
char flavors_app[10];
char sugarLevel_app[10];
char sizeCup_app[10];

int count1=0;
int count2=0;
int count3=0;

void loop() {

    // Display the "Milkshake Maker" message and proceed
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Milkshake Maker");
    lcd.setCursor(0, 1);
    lcd.print("Welcome!");
    delay(4000);  // Display the welcome message for 3 seconds


     lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("A to khnow price");
   
            lcd.setCursor(0, 1);
    lcd.print("C to clean mode");

delay(4000);


 



 
    while(true){
char key=keypad.getKey();
if (key=='A'){

 lcd.clear();
    lcd.setCursor(0, 0);
   lcd.print("1-bana 10 2-cer9");
    lcd.setCursor(0, 1);
       lcd.print("3-oreo 8 nis");
        delay(3000);

 lcd.clear();
  lcd.setCursor(0, 0);
   lcd.print("Ready to order?");
    lcd.setCursor(0, 1);
       lcd.print("1-Yes 2-NO");

        while(true){
            key=keypad.getKey();

            if(key=='1'){
                break;
                 }
          if(key=='2'){
             return;
              }
        }

         break;




}
 else if(key=='#'){
resetArduino();
}
else if(key=='C'){
    clean(cleanRelayPin,blenderRelayPin);
     return;

}
    }
      



    // Ask the user for the number of cups
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Num of cups (1-3):");
    
    int numCups = 0;
    while (true) {
   char key = keypad.getKey();
      if (key >= '1' && key <= '3') {
        numCups = key - '0'; // Convert char to integer
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Cups: ");
        lcd.print(numCups);
        delay(2000);  // Display for 2 seconds
        break;
      }
       else if(key=='#'){
resetArduino();
}
    }


    // Ask the user for flavors for each cup
    char flavors[numCups];
  char sugarLevel[numCups]; // Add an array to store sugar level
    char sizeCup[numCups]; // Add an array to store sugar level

  int totalPrice = 0; // Variable to store the total price

    for (int i = 0; i < numCups; i++) {

       lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Select size cup  ");
      lcd.print(i + 1);
      lcd.setCursor(0, 1);
      lcd.print("1-small 2-Medium ");
      while (true) {
       char key = keypad.getKey();
        if (key == '1' || key == '2' || key == '3') {
          sizeCup[i] = key;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Cup ");
          lcd.print(i + 1);
          lcd.print(" size: ");
            lcd.setCursor(0, 1);

lcd.print((key == '1') ? "Small" : (key == '2') ? "Medium" : "");
          delay(1000);  // Display for 2 seconds
          break;
        }
         else if(key=='#'){
resetArduino();
}
      }

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Select Flavor ");
      delay(2000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Cup: ");
      lcd.print(i + 1);
      lcd.print(" 1-banana ");

      lcd.setCursor(0, 1);
      
      lcd.print("2-cerlac 3-Oreo");
      char key;
      while (true) {
        key = keypad.getKey();
        if (key == '1' || key == '2' || key == '3') {
          flavors[i] = key;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Cup ");
          lcd.print(i + 1);
          lcd.print(" Flavor: ");
                    lcd.setCursor(0, 1);

lcd.print((key == '1') ? "banana" : (key == '2') ? "cerelac" : "oreo");

          delay(2000);  // Display for 2 seconds
          break;
        }
         else if(key=='#'){
resetArduino();
}
      }

// Update total price based on flavor
    switch (flavors[i]) {
      case '1':
        totalPrice += priceBanana;
        break;
      case '2':
        totalPrice += priceCerelac;
        break;
      case '3':
        totalPrice += priceOreo;
        break;
    }
        // Ask for sugar level for each cup
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Sugar Level ");
        delay(2000);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Cup: ");
        lcd.print(i + 1);
        lcd.print(" 1:Light");

        lcd.setCursor(0, 1);
        lcd.print("2-:Med 3:Heavy");

        while (true) {
            char key = keypad.getKey();
            if (key == '1' || key == '2' || key == '3') {
                sugarLevel[i] = key;
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Sugar Selected");
                lcd.setCursor(0, 1);
                lcd.print((key == '1') ? "light" : (key =='2') ? "medium" : "heavy");


                delay(2000);  // Display for 2 seconds
                break;
            }
             else if(key=='#'){
resetArduino();
}
        
    }

    // After selecting cups, flavors, and sugar levels
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Insert card...");
char key3= keypad.getKey();
    // Wait for card
   while (!checkCard() && key3 !='*') {
     key3= keypad.getKey();
      delay(100); // Wait for card insertion
        if(key3=='#'){
resetArduino();
}
    }

    // If card is valid, process the order
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Card Accepted!");
    delay(2000); // Display for 2 seconds


  // Display the total price
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Total Price: ");
  lcd.setCursor(0, 1);
  lcd.print(totalPrice);
  delay(3000); // Display for 3 seconds

  // Check if the card has enough money
  if (totalPrice > 200) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Insufficient Funds!");
    delay(3000); // Display for 3 seconds
    return; // Return to start loop
  }

    // Process each cup
    for (int i = 0; i < numCups; i++) {
      // Display selected flavor on the LCD
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Processing Cup ");
      lcd.print(i + 1);
      lcd.setCursor(0, 1);
      lcd.print("Flavor: ");
      if(flavors[i]=='1'){
        lcd.print("Banana");
      }
      else if(flavors[i]=='2'){
        lcd.print("Cerelac");

      }
      else{
        lcd.print("Oreo");

      }
       int count=1;
 while(count<=7){
   

       digitalWrite(relayled, LOW); // Turn on the relay (and the LED)
        delay(200);
        digitalWrite(relayled, HIGH); // Turn on the relay (and the LED)
        delay(200);
        count++;
 }

  handleIngredientSelection(flavors[i],sugarLevel[i],sizeCup[i] );

      // Update LCD to show blending status
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Blending...");

   digitalWrite(blenderRelayPin, LOW); // Turn on the blender
  delay(20000); // 
    digitalWrite(blenderRelayPin, HIGH); // Turn off the blender

      // Dispense the cup and update LCD
          delay(2000); 

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Dispensing Cup...");
      
while (digitalRead(ldr3Pin)==HIGH); // Turn on the onboard LED
 {
dispenseCup();
delay(5000);

    // Wait for the LDR1 to detect the cup
  }
  
      // Move conveyor and update LCD
     
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Moving Cup...");
     moveConveyorToLaser1(1); // Move the cup to the valve
      delay(4000); // Wait until the cup reaches the valve
      moveConveyorToLaser2(); // Move the cup to the lid position
      delay(2000); // Wait until the cup reaches the lid position
      enableConveyor();
      delay(7000);
      stopConveyor(); // Stop the conveyor

       //Show order completion message for the cup
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Cup ");
      lcd.print(i + 1);
      lcd.print(" Complete!");
        int count2=1;
 while(count2<=7){
   

       digitalWrite(relayled, LOW); // Turn on the relay (and the LED)
        delay(200);
        digitalWrite(relayled, HIGH); // Turn on the relay (and the LED)
        delay(200);
        count2++;
 }
         clean(cleanRelayPin,blenderRelayPin);

    }

    // Final completion message for the entire order
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Order Complete!");
    delay(3000);  // Display for 3 seconds


    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Ready for next!");
    delay(3000);  // Pause before returning to the initial state
   
  

}


}

void handleIngredientSelection(char key, char level ,char sizeCup) {
 

 lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Dispensing milk...");

  
  digitalWrite(milkRelayPin, LOW); // Open the milk valve (relay on pin 46)
  if(sizeCup=='1'){
  delay(30000); 
  }
  else if(sizeCup=='2'){
delay(45000); 
  
  }
 digitalWrite(milkRelayPin, HIGH); // Close the milk valve


 lcd.clear();
 
    lcd.setCursor(0, 0);
    lcd.print("move to sugar...");

 moveBigCircleSteps(144); 
  delay(2000);

 //Dispense sugar after milk
  if (checkIRSensor()) {
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Dispensing sugar...");
    
    if(level=='1' ){
 
    enableMotor(sugarEnablePin);

    sugarStepper.moveTo(25);
    sugarStepper.runToPosition();

    delay(500);
        sugarStepper.moveTo(0);
    sugarStepper.runToPosition();
    
    disableMotor(sugarEnablePin);
    
    delay(4000);
   
    }
    if(level=='2'){
      Serial.println("Dispensing sugar...");
    enableMotor(sugarEnablePin);

    sugarStepper.moveTo(36);
    sugarStepper.runToPosition();

    delay(500);
        sugarStepper.moveTo(0);
    sugarStepper.runToPosition();
    
    disableMotor(sugarEnablePin);
    
    delay(4000);
    }
    

    if(level=='3'){
      Serial.println("Dispensing sugar...");
    enableMotor(sugarEnablePin);

    sugarStepper.moveTo(50);
    sugarStepper.runToPosition();

    delay(500);
        sugarStepper.moveTo(0);
    sugarStepper.runToPosition();
    
    disableMotor(sugarEnablePin);
    
    delay(4000);
    }

  }

lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("move to crema...");

 moveBigCircleSteps(140); 
  delay(2000);

  if (checkIRSensor()) {

lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Dispensing crema......");
    enableMotor(cremaEnablePin);

    cremaStepper.moveTo(22);
    cremaStepper.runToPosition();

    delay(500);
 cremaStepper.moveTo(0);
    cremaStepper.runToPosition();


    
    disableMotor(cremaEnablePin);
    
    delay(3000);
    
   
   


  }
  

  switch (key) {
    case '1':
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("move to banana......");
    moveBigCircleSteps(110); // Move big circle by 28 steps
      delay(2000);

      if (checkIRSensor()) {
        
        lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Dispensing banana......");
        enableMotor(bananaEnablePin);
        BananaStepper.moveTo(230);
        BananaStepper.runToPosition();


        disableMotor(bananaEnablePin);
        delay(4000);
        
     
      }
                lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("move to initial point...");


 moveBigCircleSteps(-395); 
 

      break;

    case '2':
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("move to cerelac......");
      moveBigCircleSteps(244); 
      delay(2000);

      if (checkIRSensor()) {
        lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Dispensing cerelac...");
      

        enableMotor(cerelacEnablePin);

         CerelacStepper.moveTo(45);
    CerelacStepper.runToPosition();
    delay(500);

 CerelacStepper.moveTo(0);
    CerelacStepper.runToPosition();
        disableMotor(cerelacEnablePin);

        delay(3000);
        
     }

        
               
                lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("move to initial point ");
            moveBigCircleSteps(-530); // Move big circle back to start

      break;

    case '3':
     lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("move to oreo....");
      

   moveBigCircleSteps(385); 
      delay(2000);

      if (checkIRSensor()) {
        
         lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Dispensing Oreo......");
      

        enableMotor(oreoEnablePin);
        oreoStepper.moveTo(450);
        oreoStepper.runToPosition();

        delay(1000);
       oreoStepper.moveTo(-50);
      oreoStepper.runToPosition();
      }
 lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("move to initial point ");
    moveBigCircleSteps(-670); // Move big circle back to start

      break;
  }
  
}

void dispenseCup() {

   lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Moving cup holder....");
        delay(1000); // Wait for a second

  cupHolderStepper.moveTo(96);
  cupHolderStepper.runToPosition();

  delay(1000); // Wait for a second

  cupHolderStepper.moveTo(0);
  cupHolderStepper.runToPosition();

disableMotorH(cupHolderIn1Pin,cupHolderIn2Pin,cupHolderIn3Pin,cupHolderIn4Pin);

}

void moveConveyorToLaser1(int flag) {
      enableConveyor();


  while (digitalRead(ldr1Pin)==LOW); // Turn on the onboard LED
 {
    // Wait for the LDR1 to detect the cup
  }
  delay(1900);
  stopConveyor();
  delay(2000); // Wait for a second

// Move the motor 200 steps forward
spoutStepper.moveTo(-400);
spoutStepper.runToPosition(); // Blocking call
  lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Pour the mixture into the cup");
      if(flag==1){
        delay(15000); // Wait for 15 seconds

      }
else {
          delay(30000); // Wait for 15 seconds
}

         lcd.clear();

spoutStepper.moveTo(0);
spoutStepper.runToPosition(); // Blocking call
 lcd.clear();
 lcd.setCursor(0, 0);
    lcd.print("Moving Cup....");
  delay(1000); // Wait for a second
 disableMotorH(nema17In1,nema17In2,nema17In3,nema17In4);
}

void moveConveyorToLaser2() {
Serial.println("Moving conveyor to Laser 2...");
        enableConveyor();
Serial.println("done...");


  while (digitalRead(ldr2Pin)==LOW) {
    // Continue moving until Laser 2 (LDR2) detects the cup
  }
delay(2500);
  stopConveyor(); // Stop the conveyor when Laser 2 detects the cup
 


  // Move the additional DC motor for a certain period
  Serial.println("Activating additional DC motor...");
  enableDc2();
  delay(4000); // Adjust the delay to control how long the motor runs

  Serial.println("Stopping additional DC motor...");
 disableDc2();
    
}

void stopConveyor() {
  Serial.println("Stopping conveyor...");
  analogWrite(dcMotorEnablePin, 0); // Set motor speed

  digitalWrite(dcMotorIn1Pin, LOW);
  digitalWrite(dcMotorIn2Pin, LOW);
}

void enableConveyor() {
  analogWrite(dcMotorEnablePin, 255); // Set motor speed
  digitalWrite(dcMotorIn1Pin, HIGH);
  digitalWrite(dcMotorIn2Pin, LOW);
}
void enableDc2() {
  digitalWrite(additionalDcMotorIn1Pin, HIGH);
  digitalWrite(additionalDcMotorIn2Pin, LOW);
}
void disableDc2() {
  digitalWrite(additionalDcMotorIn1Pin, LOW);
  digitalWrite(additionalDcMotorIn2Pin, LOW);
}

bool checkIRSensor() {
  return digitalRead(irSensorPin) == LOW;
}

void enableMotor(int enablePin) {
  digitalWrite(enablePin, LOW); // Enable motor driver
}

void disableMotor(int enablePin) {
  digitalWrite(enablePin, HIGH); // Disable motor driver
}

void moveBigCircleSteps(int steps) {
 enableMotor(bigCircleEnablePin);
  bigCircleStepper.move(steps);
  bigCircleStepper.runToPosition();
}




bool checkCard() {
  // Look for new cards
  if ( !mfrc522.PICC_IsNewCardPresent()) {
    return false;
  }

  // Select one of the cards
  if ( !mfrc522.PICC_ReadCardSerial()) {
    return false;
  }

  // Compare the card's UID with the predefined ones
  if (memcmp(mfrc522.uid.uidByte, cardWithMoney, mfrc522.uid.size) == 0) {
    // Card has money, proceed with the transaction
    Serial.println("Card with money detected.");
    return true;
  } else if (memcmp(mfrc522.uid.uidByte, cardWithoutMoney, mfrc522.uid.size) == 0) {
    // Card has no money, show an error message
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("No money on card!");
    delay(3000); // Display for 3 seconds

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("enter the card ");
    lcd.setCursor(0, 1);
    lcd.print("again! ");

    return false;
  } else {
    // Unknown card
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Unknown Card!");
    delay(3000); // Display for 3 seconds
    return false;
  }
}




void clean(int cleanRelayPin, int blenderRelayPin){
   lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("cleaning.....");
        int count3=1;
 while(count3<=7){
   

       digitalWrite(relayled, LOW); // Turn on the relay (and the LED)
        delay(200);
        digitalWrite(relayled, HIGH); // Turn on the relay (and the LED)
        delay(200);
        count3++;
 }
      digitalWrite(cleanRelayPin, HIGH); // Turn off 
      digitalWrite(blenderRelayPin, HIGH); // Turn off 

      digitalWrite(cleanRelayPin, LOW); // Turn on the bumb clean
      digitalWrite(cleanRelayPin, LOW); // Turn on the bumb clean

    delay(15000); // 
      digitalWrite(cleanRelayPin, HIGH); // Turn off 

delay(3000);

      digitalWrite(blenderRelayPin, LOW); // Turn on the blender 
      digitalWrite(blenderRelayPin, LOW); // Turn on the blender 

    delay(6500); // 
      digitalWrite(blenderRelayPin, HIGH); // Turn off 
 delay(2000);
// Dispense the cup and update LCD
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Dispensing Cup...");
    dispenseCup();

      // Move conveyor and update LCD
     
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Moving Cup...");
     moveConveyorToLaser1(2); // Move the cup to the valve
      delay(4000); // Wait until the cup reaches the valve
      moveConveyorToLaser2(); // Move the cup to the lid position
      delay(2000); // Wait until the cup reaches the lid position
      enableConveyor();
      delay(7000);
      stopConveyor(); // Stop the conveyor

       //Show order completion message for the cup
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("clean ");
      lcd.print(" Complete!");
           int count=1;
 while(count<=7){
   

       digitalWrite(relayled, LOW); // Turn on the relay (and the LED)
        delay(200);
        digitalWrite(relayled, HIGH); // Turn on the relay (and the LED)
        delay(200);
        count++;
 }
      delay(3000);  // Display for 3 seconds
      }


void resetArduino() {
  wdt_enable(WDTO_15MS);  // Set the watchdog timer to trigger a reset in 15ms
  while (true) {}          // Wait for the watchdog to reset the Arduino
}


void disableMotorH(int IN1, int IN2, int IN3, int IN4) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void appLication(char flavors_app[], char cupChar,char sugarLevel_app[],char sizeCup[]){
   const int priceBanana = 10; // Price for Banana milkshake
const int priceCerelac = 9; // Price for Cerelac milkshake
const int priceOreo = 8; // Price for Oreo milkshake
  int totalPrice=0;
  int y=(int) cupChar;
for(int i=0;i<y;i++){
    switch (flavors_app[i]) {
      case '1':
        totalPrice += priceBanana;
        break;
      case '2':
        totalPrice += priceCerelac;
        break;
      case '3':
        totalPrice += priceOreo;
        break;
    }
}
    // After selecting cups, flavors, and sugar levels
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Insert card...");
   char key3= keypad.getKey();
    // Wait for card
   while (!checkCard()&& key3!='*' ) {
        key3= keypad.getKey();

      delay(100); // Wait for card insertion
    }
    // If card is valid, process the order
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Card Accepted!");
    delay(2000); // Display for 2 seconds
     
  // Display the total price
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Total Price: ");
  lcd.setCursor(0, 1);
  lcd.print(totalPrice);
  delay(3000); // Display for 3 seconds

  // Check if the card has enough money
  if (totalPrice > 200) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Insufficient Funds!");
    delay(3000); // Display for 3 seconds
    return; // Return to start loop
  }

    // Process each cup
int c = cupChar - '0';

    for (int i = 0; i < c; i++) {
      int count=1;
       while(count<=5){
   

       digitalWrite(relayled, LOW); // Turn on the relay (and the LED)
        delay(200);
        digitalWrite(relayled, HIGH); // Turn on the relay (and the LED)
        delay(200);
        count++;
 }
      // Display selected flavor on the LCD
    handleIngredientSelection(flavors_app[i],sugarLevel_app[i],sizeCup[i] );
    digitalWrite(blenderRelayPin, LOW); // Turn on the blender
    delay(10000); // 
     digitalWrite(blenderRelayPin, HIGH); // Turn off the blender
      // Dispense the cup and update LCD
          delay(2000); 
while (digitalRead(ldr3Pin)==HIGH); // Turn on the onboard LED
 {
dispenseCup();
delay(2000);
  }     
     moveConveyorToLaser1(1); // Move the cup to the valve
      delay(4000); // Wait until the cup reaches the valve
      moveConveyorToLaser2(); // Move the cup to the lid position
      delay(2000); // Wait until the cup reaches the lid position
      enableConveyor();
      delay(10000);
      stopConveyor(); // Stop the conveyor
      delay(7000);

       //Show order completion message for the cup
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Cup ");
      lcd.print(i + 1);
      lcd.print(" Complete!");
  int count3=1;
 while(count3<=5){
   

       digitalWrite(relayled, LOW); // Turn on the relay (and the LED)
        delay(200);
        digitalWrite(relayled, HIGH); // Turn on the relay (and the LED)
        delay(200);
        count3++;
 }
         clean(cleanRelayPin,blenderRelayPin);

    }

    // Final completion message for the entire order
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Order Complete!");
    delay(3000);  // Display for 3 seconds


    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Ready for next!");
    delay(3000);  // Pause before returning to the initial state
   

    
    
    
}