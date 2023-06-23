#include "Adafruit_MLX90395.h"
#include <Wire.h>

Adafruit_MLX90395 sensor1 = Adafruit_MLX90395(); // sensor es una instancia de la clase Adafruit_MLX90395

#define TCAADDR 0x70

void tcaselect(uint8_t i){
  if (i > 7) return;
 
  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();  
}

void setup(void){
  Serial.begin(9600); // inicializa la comunicación serie a 115200 bits por segundo
  /* Wait for serial on USB platforms. */
  while (!Serial){
      delay(10);
  }
  Wire.begin();
  /* Initialise the 1st sensor */
  tcaselect(0);
  if(!sensor1.begin_I2C()){
    /* There was a problem detecting the MLX90395 ... check your connections */
    Serial.println("No sensor found at PORT 0 ... check your wiring?");
    delay(100);
  }
  
  sensor1.setOSR(MLX90395_OSR_8);
  sensor1.setResolution(MLX90395_RES_17);
}


int num_media = 100; // número de medidas realizadas para obtener la media

void loop(void){
  float Bx1 = 0; // inicialización de las componentes a 0
  float By1 = 0;
  float Bz1 = 0;
  float Bx2 = 0;
  float By2 = 0;
  float Bz2 = 0;

  tcaselect(0); // El sensor 1 es aquel que está conectado al canal 0

  for (int i=0; i<num_media; i++){
    /* Get a new sensor event, normalized to uTesla */
    sensors_event_t event; 
    sensor1.getEvent(&event);
    Bx1 += event.magnetic.x;
    By1 += event.magnetic.y;
    Bz1 += event.magnetic.z;
    delay(10);
    }
  Bx1 = Bx1 / num_media;
  By1 = By1 / num_media;
  Bz1 = Bz1 / num_media;
  
  /* Display the results (mean magnetic field is measured in uTesla) */
  Serial.print("Sensor 1:\t"); 
  Serial.print(Bx1);
  Serial.print("\t"); Serial.print(By1); 
  Serial.print("\t"); Serial.print(Bz1); 
  Serial.print("\t"); Serial.println(sqrt(pow(Bx1,2)+pow(By1,2 )+pow(Bz1,2)));
  Serial.println("------------------------------------------------");

  delay (5000);
}
