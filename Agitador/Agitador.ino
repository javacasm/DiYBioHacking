/*
Control de motor regulando su velocidad y lo muestra por el LCD

Conexion del LCD
-----------------------------
Vin (Voltage In)    ->  5V
Gnd (Ground)        ->  Gnd
SDA (Serial Data)   ->  A4 on Uno/Pro-Mini, 20 on Mega2560/Due, 2 Leonardo/Pro-Micro
SCK (Serial Clock)  ->  A5 on Uno/Pro-Mini, 21 on Mega2560/Due, 3 Leonardo/Pro-Micro

Relé                ->  pin 11

Potenciometro ->  pin A0

 */



/* ==== Includes ==== */
// Librerias para la pantalla LCD
#include <Wire.h>               // Needed for legacy versions of Arduino.
#include <LiquidCrystal_I2C.h>  // Pantalla LCD

/* ====  END Includes ==== */

/* ==== Defines ==== */
#define SERIAL_BAUD 9600

#define PIN_POTENCIOMETRO A0

#define PIN_MOTOR_A 10
#define PIN_MOTOR_B 9

/* ==== END Defines ==== */

/* ==== Global Variables ==== */
// Set the LCD address to 0x27 or 0x3F for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);
//LiquidCrystal_I2C lcd(0x20, 16, 2);


unsigned long printPeriod = 1000; // in milliseconds
// Track time in milliseconds since last reading
unsigned long previousMillis = 0;

/* ==== END Global Variables ==== */


/* ==== Prototypes ==== */
/* ==== END Prototypes ==== */

/* ==== Setup ==== */
void setup_Serial(){
  Serial.begin(SERIAL_BAUD);
  while(!Serial) {} // Wait

}


void setup_LCD(){
  lcd.init();         // Inicializamos el LCD
  lcd.backlight();    // Encendemos la luz
}

void setup_Motor(){
  pinMode(PIN_MOTOR_A,OUTPUT);
  pinMode(PIN_MOTOR_B,OUTPUT);
}



void setup() {
  setup_Serial();
  setup_LCD();
  setup_Motor();
}
/* ==== END Setup ==== */

/* ==== Loop ==== */
void loop() {


    int valorPot = analogRead(PIN_POTENCIOMETRO);  // read the analog in value:
    int velocidad=map(valorPot,0,1023,0,100);
    int valorPWM=map(valorPot,0,1023,0,255);
    
    if((unsigned long)(millis() - previousMillis) >= printPeriod) {
      previousMillis = millis();   // update time


  
      lcd.setCursor(0,0);

      //showTemp();
      lcd.setCursor(0,1);
      Serial.print(",");
      printSerialAndLCD(String(velocidad));
      
      lcd.print("%  ");

      lcd.setCursor(0,1);
      
          
      Serial.print(",");

      digitalWrite(PIN_MOTOR_A,LOW);
      analogWrite(PIN_MOTOR_B,valorPWM);
      
      Serial.println();
    }
  
  


}
/* ==== End Loop ==== */

/* ==== Functions ==== */
void printSerialAndLCD(String str){
  Serial.print(str);
  lcd.print(str);
}

void print2Digit(int  valor){
  if(valor<10)
    printSerialAndLCD("0");
  printSerialAndLCD(String(valor));
}


/* ==== END Functions ==== */
