/*
Control de calentador por temperatura y lo muestra por el LCD

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

// Librerias para sensor temperatura DS18x20
#include <OneWire.h>
#include <DallasTemperature.h>

/* ====  END Includes ==== */

/* ==== Defines ==== */
#define SERIAL_BAUD 9600

#define PIN_POTENCIOMETRO A0

#define PIN_RELE 11


#define ONE_WIRE_BUS 7

/* ==== END Defines ==== */

/* ==== Global Variables ==== */
// Set the LCD address to 0x27 or 0x3F for a 16 chars and 2 line display
//LiquidCrystal_I2C lcd(0x27, 16, 2);
LiquidCrystal_I2C lcd(0x3F, 16, 2);


unsigned long printPeriod = 1000; // in milliseconds
// Track time in milliseconds since last reading
unsigned long previousMillis = 0;

// Sensor de temperatura DS18x20

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

float fTemperatura;
float fTemperaturaUmbral=50.0;
/* ==== END Global Variables ==== */


/* ==== Prototypes ==== */
/* ==== END Prototypes ==== */

/* ==== Setup ==== */
void setup_SensorTemp(){
  sensors.begin();
}

void setup_Serial(){
  Serial.begin(SERIAL_BAUD);
  while(!Serial) {} // Wait

}


void setup_LCD(){
  lcd.init();         // Inicializamos el LCD
  lcd.backlight();    // Encendemos la luz
}

void setup_Rele(){
  pinMode(PIN_RELE,OUTPUT);
}



void setup() {
  setup_Serial();
  setup_LCD();
  setup_Rele();

  setup_SensorTemp();
}
/* ==== END Setup ==== */

/* ==== Loop ==== */
void loop() {


    int valorPot = analogRead(PIN_POTENCIOMETRO);  // read the analog in value:
    fTemperaturaUmbral=map(valorPot,0,1023,0,50);
    if((unsigned long)(millis() - previousMillis) >= printPeriod) {
      previousMillis = millis();   // update time


  
      lcd.setCursor(0,0);
/*      printSerialAndLCD(String(current_amps));
      lcd.print("(");
      Serial.print( "," );
      printSerialAndLCD(String(inputStats.sigma()));
      lcd.print(") ");
      Serial.print(",");
  */    
      showTemp();
      lcd.setCursor(0,1);
      Serial.print(",");
      printSerialAndLCD(String(fTemperaturaUmbral));
      lcd.print("  ");

      lcd.setCursor(0,1);
      
          
      Serial.print(",");
      if(fTemperatura>fTemperaturaUmbral){
        apaga_Rele();
      }
      else{
        enciende_Rele();
      }
      Serial.println();
    }
  
  


}
/* ==== End Loop ==== */

/* ==== Functions ==== */
void enciende_Rele(){
  digitalWrite(PIN_RELE,HIGH);
  lcd.setCursor(13,1);
  lcd.print(" On");
  Serial.print(1);
}

void apaga_Rele(){
  digitalWrite(PIN_RELE,LOW);
  lcd.setCursor(13,1);
  lcd.print("Off");
  Serial.print(0);
}


void printSerialAndLCD(String str){
  Serial.print(str);
  lcd.print(str);
}

void print2Digit(int  valor){
  if(valor<10)
    printSerialAndLCD("0");
  printSerialAndLCD(String(valor));
}


void showTemp(){
  sensors.requestTemperatures(); // Send the command to get temperatures
  int iCuantos=sensors.getDeviceCount();
  for(int i=0;i<iCuantos;i++){
    if(i!=0){
      Serial.print(",");
      lcd.print(" ");
      
    }
    fTemperatura=sensors.getTempCByIndex(i);
    printSerialAndLCD(String(fTemperatura));    
  }
  
}
/* ==== END Functions ==== */
