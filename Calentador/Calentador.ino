/*
Control remoto de una regleta que mide el consumo y lo muestra por el LCD
Usa RTC para establecer con precision el tiempo

Conexion del LCD
-----------------------------
Vin (Voltage In)    ->  5V
Gnd (Ground)        ->  Gnd
SDA (Serial Data)   ->  A4 on Uno/Pro-Mini, 20 on Mega2560/Due, 2 Leonardo/Pro-Micro
SCK (Serial Clock)  ->  A5 on Uno/Pro-Mini, 21 on Mega2560/Due, 3 Leonardo/Pro-Micro

Relé                ->  pin 11

Sensor de corriente ->  pin A0

Conexion del RTC 1302

VCC      5V
Gnd     GND
SCK_PIN  4
IO_PIN  5  //También conocido como Data
CE_PIN  6  //También conocido como RST

 */



/* ==== Includes ==== */
// Librerias para la pantalla LCD
#include <Wire.h>               // Needed for legacy versions of Arduino.
#include <LiquidCrystal_I2C.h>  // Pantalla LCD

// Libreria para filtrado
#include <Filters.h>

// Librerias para RTC
#include <DS1302RTC.h>
#include <Time.h>

// Librerias para sensor temperatura DS18x20
#include <OneWire.h>
#include <DallasTemperature.h>

/* ====  END Includes ==== */

/* ==== Defines ==== */
#define SERIAL_BAUD 9600

#define PIN_SENSOR_CORRIENTE A0

#define PIN_RELE 11

#define SCK_PIN 4
#define IO_PIN  5
#define CE_PIN  6

#define ONE_WIRE_BUS 3

/* ==== END Defines ==== */

/* ==== Global Variables ==== */
// Set the LCD address to 0x27 or 0x3F for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);
//LiquidCrystal_I2C lcd(0x3F, 16, 2);

// Medida de corriente

float testFrequency = 60;                     // test signal frequency (Hz)
float windowLength = 20.0/testFrequency;     // how long to average the signal, for statistist
int sensorValue = 0;
float intercept = -0.02;//-0.1129; // to be adjusted based on calibration testing
float slope = 0.066; // 0.0405; // to be adjusted based on calibration testing
float current_amps; // estimated actual current in amps

unsigned long printPeriod = 1000; // in milliseconds
// Track time in milliseconds since last reading
unsigned long previousMillis = 0;

// RTC DS1302
// Set pins:  CE, IO,CLK
DS1302RTC RTC(CE_PIN, IO_PIN, SCK_PIN);

// Sensor de temperatura DS18x20

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

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

void setup_RTC(){
  lcd.print("RTC activado");

  delay(500);

  lcd.clear();
  if (RTC.haltRTC()) {
    lcd.print(F("RTC parado!"));
  }
  else
    lcd.print(F("RTC Funcionando"));

  lcd.setCursor(0,1);
  if (RTC.writeEN())
    lcd.print(F("Escritura Permitida"));
  else
    lcd.print(F("Escritura Bloqueada"));

  delay ( 2000 );

  // Setup time library
  lcd.clear();
  lcd.print(F("RTC Sincronizando"));
  setSyncProvider(RTC.get);          // the function to get the time from the RTC
  if(timeStatus() == timeSet)
    lcd.print(F(" Ok!"));
  else
    lcd.print(F(" Error!"));

  delay ( 2000 );

  lcd.clear();

}


void setup() {
  setup_Serial();
  setup_LCD();
  setup_Rele();

  enciende_Rele();

  setup_RTC();

  setup_SensorTemp();
}
/* ==== END Setup ==== */

/* ==== Loop ==== */
void loop() {

  // Para ajustar el tiempo si es necesario
  if (Serial.available() >= 12){
    setTime();
  }

  RunningStatistics inputStats;                 // create statistics to look at the raw test signal
  inputStats.setWindowSecs( windowLength );

  while( true ) {
    sensorValue = analogRead(A0);  // read the analog in value:
    inputStats.input(sensorValue);  // log to Stats function

    if((unsigned long)(millis() - previousMillis) >= printPeriod) {
      previousMillis = millis();   // update time



      // convert signal sigma value to current in amps
      current_amps = intercept + slope * inputStats.sigma();
      Serial.print( current_amps );
      Serial.print( "," );
      Serial.println( inputStats.sigma() );
      lcd.setCursor(0,0);
      lcd.print(current_amps);
    }
  lcd.print(" ");
  showTemp();
  Serial.println();
  lcd.setCursor(0,1);
  showTime();
  Serial.println();
  
  }


}
/* ==== End Loop ==== */

/* ==== Functions ==== */
void enciende_Rele(){
  digitalWrite(PIN_RELE,HIGH);
}

void apaga_Rele(){
  digitalWrite(PIN_RELE,LOW);
}


void setTime(){

    //check for input to set the RTC, minimum length is 12, i.e. yy,m,d,h,m,s
     {
        tmElements_t tm;
        time_t t;
        //note that the tmElements_t Year member is an offset from 1970,
        //but the RTC wants the last two digits of the calendar year.
        //use the convenience macros from Time.h to do the conversions.
        int y = Serial.parseInt();
        if (y >= 100 && y < 1000)
            lcd.println(F("Error: Year must be two digits or four digits!"));
        else {
            if (y >= 1000)
                tm.Year = CalendarYrToTm(y);
            else    //(y < 100)
                tm.Year = y2kYearToTm(y);
            tm.Month = Serial.parseInt();
            tm.Day = Serial.parseInt();
            tm.Hour = Serial.parseInt();
            tm.Minute = Serial.parseInt();
            tm.Second = Serial.parseInt();
            t = makeTime(tm);
      //use the time_t value to ensure correct weekday is set
            if(RTC.set(t) == 0) { // Success
              setTime(t);
              lcd.clear();
              lcd.print(F("RTC set!"));
      }       else  {
              lcd.clear();
              lcd.print(F("RTC not set!"));
      }
            //dump any extraneous input
            while (Serial.available() > 0) Serial.read();
        }
    }

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

void showTime(){
  print2Digit(hour());
  printSerialAndLCD(":");
  print2Digit(minute());
  printSerialAndLCD(":");
  print2Digit(second());

}

void showTemp(){
  sensors.requestTemperatures(); // Send the command to get temperatures
  int iCuantos=sensors.getDeviceCount();
  for(int i=0;i<iCuantos;i++){
    if(i!=0){
      Serial.print(",");
      lcd.print(" ");
    }
   
    printSerialAndLCD(String(sensors.getTempCByIndex(i)));    
  }
}
/* ==== END Functions ==== */
