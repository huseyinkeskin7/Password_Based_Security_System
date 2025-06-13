#include <Wire.h>
#include <LiquidCrystal_I2C.h>
// We include the necessary libraries for the I2C LCD we use.
volatile boolean setPasswordInterruptFlag = false;
// We create an interrupt to control the button press status.

#define button0Pin  4  // We defined the pin to which the button representing "0" is connected.
#define button1Pin  3  // We defined the pin to which the button representing "1" is connected.
#define setButtonPin  2 // We defined the pin to which the button representing "SET" is connected.
#define greenLEDPin  5 // We define the pin to which the green LED is connected.
#define redLEDPin  6   // We define the pin to which the RED LED is connected.
#define buzzer  12 // We define the pin to which the buzzer is connected.


char password[8];
char enteredPassword[8];
boolean systemLocked = true;
boolean passwordHasBeenSet = false;

unsigned long lastButtonPressTime = 0;
unsigned long debounceDelay = 50;
// Here we defined passwords and variables to control the system status.

LiquidCrystal_I2C lcd(32, 16, 2);
// We set our 16X2 LCD screen to the 32nd address.

void setup() {
    Serial.begin(9600); // We are starting the serial monitor.
    
    lcd.begin(16, 2);
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("***EEE305***"); 
    lcd.print("...Hoşgeldiniz...");
    lcd.scrollDisplayLeft();
    delay(150);
// We provide a login screen for the user by operating our LCD screen. Here we also ensure that the text moves to the left.
    pinMode(buzzer, OUTPUT);
    pinMode(button0Pin, INPUT_PULLUP); //INPUT_PULLUP is microcontroller based resistor system. it use pull up resistor in arduino
    pinMode(button1Pin, INPUT_PULLUP);
    pinMode(setButtonPin, INPUT_PULLUP);
    pinMode(greenLEDPin, OUTPUT);
    pinMode(redLEDPin, OUTPUT);
    // We introduce the components we have defined to the system according to their input and output status.
    digitalWrite(greenLEDPin, LOW); // Initially we set the Green LED to be off.
    digitalWrite(redLEDPin, LOW);  // Initially we set the Red LED to be off.
    //attachInterrupt(digitalPinToInterrupt(setButtonPin), handleSetPasswordInterrupt, RISING);
    setNewPassword(); // setup fonksiyonu 1 kez çalıştıktan.......
}

//void handleSetPasswordInterrupt() {
  //  setPasswordInterruptFlag = true;
//}

void checksetbutton() { // the main purpose of this function is to check the status of the set button. That is, is the button in HIGH or LOW state?
    if (digitalRead(setButtonPin) == HIGH && systemLocked == false) {
        setNewPassword(); // If the condition in the if command is met, we call this function.
    }
}

void loop() {
 

    if (digitalRead(setButtonPin) == HIGH) {
        setNewPassword(); // If the Set button is pressed, our system calls this function.
    }
    else {
        for (int i = 0; i < 8; i++) {
            enteredPassword[i] = '\0';
            // The purpose here is to reset the enteredPassword array. The system fills the characters from 0 to 7 with \0, that is, NULL. 
        }
        delay(100); 
        enterPassword(); // We call the function to enter the password.
    }
}

void setNewPassword() {
    digitalWrite(greenLEDPin, HIGH);
    digitalWrite(redLEDPin, HIGH);
    // To visually inform the user, we turn the red and green LEDs to HIGH position at the time of defining a new password.
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Parolayı Girin : ");
    // We print text on the first line of the LCD screen to show the command to the user.
    Serial.println("Lütfen parolanızı girin (8 karakter) : ");

    for (int i = 0; i < 8; i++) {
        char buttonPressed = '\0'; // We monitor the button status.

        while (buttonPressed == '\0') { //it waits untill button pressed and change the variable value 
            if (digitalRead(setButtonPin) == LOW && (millis() - lastButtonPressTime) > debounceDelay) {
                lastButtonPressTime = millis();
                if (digitalRead(button0Pin) == HIGH) { //if buton 0 pressed it changes the varible value to 0 after that it finish the while loop 
                    buttonPressed = '0';
                    Serial.print("0"); 
                   
                }
                else if (digitalRead(button1Pin) == HIGH) {//if buton 1 pressed it changes the varible value to 1 after that it finish the while loop 
                    buttonPressed = '1';
                    Serial.print("1");

                  
                }
            }// The main purpose here is to keep track of the 0-1 binary numbers that the user will use when creating a 3-bit password.
             // From whichever button the HIGH signal comes, the system will print that value on the screen.
        }

        password[i] = buttonPressed; // every for iteration we assign a varible buttonpressed value which we got from while loop
        // Whichever button is pressed, we assign the corresponding value of that button to the password variable.
        lcd.setCursor(0, 1);
        lcd.print("--> " + String(password)); // We print the password on the LCD screen.
        delay(1000); 

    }

    passwordHasBeenSet = true; // We set the password setting status to true. This situation is a directive for us to lock the system.
    digitalWrite(greenLEDPin, LOW);
    digitalWrite(redLEDPin, LOW); // We turn off our LEDs
    Serial.println("\n Parola Ayarlandı. Sistem Kilitli.");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(" SİSTEM KİLİTLİ ");

    systemLocked = true; //we set the system locked status to true.
}

void enterPassword() {
    Serial.println("Parolayı Girin:");
    char inputBuffer[8];
    for (int i = 0; i < 8; i++) {
        char buttonPressed = '\0'; // We monitor the button status.

        while (buttonPressed == '\0') {
            checksetbutton();
            if (digitalRead(setButtonPin) == LOW) {
                if (digitalRead(button0Pin) == HIGH) {
                    buttonPressed = '0';
                    Serial.print("0");
                    while (digitalRead(button0Pin) == LOW) {} //waits for the button to be released
                }
                else if (digitalRead(button1Pin) == HIGH) {
                    buttonPressed = '1';
                    Serial.print("1");
                    while (digitalRead(button1Pin) == LOW) {} //waits for the button to be released
                }
            }
        }

        inputBuffer[i] = buttonPressed; // Adds the button state to the login password
        delay(1000); 
    }

    if (checkPassword(inputBuffer, password)) { 
    // here compares the inoutBuffer array with the password array.
     // While the password string is the password we set for the system, the inputBuffer string is the password we enter to unlock the lock.
     // The code compares both arrays in this section. If a match is achieved, the lock is removed and the green LED turns on for 1 second.
        digitalWrite(greenLEDPin, HIGH);
        tone(buzzer, 800);
        delay(1000); // Green LED lights for 1 second.
        digitalWrite(greenLEDPin, LOW);
        noTone(buzzer);
        Serial.println("\n Doğru Parola. Sistem Açıldı.");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Doğru Parola");
        lcd.setCursor(0, 1);
        lcd.print(" SİSTEM AÇILDI");

        systemLocked = false; // We set the system lock status to false.
    }
    else {
      // If the inputBuffer and password strings do not match, the system turns on the red LED and the buzzer sounds in a different tone.
        digitalWrite(redLEDPin, HIGH);
        tone(buzzer, 100);
        delay(1000); // Red LED will light for 1 second
        digitalWrite(redLEDPin, LOW);
        noTone(buzzer);
        Serial.println("\nWrong Password. Access Denied.");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Wrong Password ");
        lcd.setCursor(0, 1);
        lcd.print("Access Denied ");
        
        delay(150);
        systemLocked = true;  // we set the locked status of the system to true again.
    }
}

boolean checkPassword(char entered[], char correct[]) {
    for (int i = 0; i < 8; i++) { // every bit comparing in for loop if its not match or matchs returns value as a booolen 
        if (entered[i] != correct[i]) {
            return false;
        }
    } } // Here, the entered and correct password are compared. If there is a match, false is returned, if true, true is returned. 
    return true;
}
