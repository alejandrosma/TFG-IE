#include "Adafruit_MLX90395.h"
#include <Wire.h>

Adafruit_MLX90395 sensor0 = Adafruit_MLX90395(); // sensor es una instancia de la clase Adafruit_MLX90395
Adafruit_MLX90395 sensor1 = Adafruit_MLX90395();
Adafruit_MLX90395 sensor2 = Adafruit_MLX90395();
Adafruit_MLX90395 sensor3 = Adafruit_MLX90395();

#define TCAADDR 0x70

void tcaselect(uint8_t i){
  if (i > 7) return;
 
  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();  
}

void setup(void){
  Serial.begin(115200); // inicializa la comunicación serie a 115200 bits por segundo
  /* Wait for serial on USB platforms. */
  while (!Serial){
      delay(10);
  }
  Wire.begin();
  /* Initialise the 1st sensor */
  tcaselect(0);
  if(!sensor0.begin_I2C()){
    /* There was a problem detecting the MLX90395 ... check your connections */
    Serial.println("No sensor found at PORT 0 ... check your wiring?");
    delay(100);
  }
  
  /* Initialise the 2nd sensor */
  tcaselect(1);
  if(!sensor1.begin_I2C()){
    /* There was a problem detecting the MLX90395 ... check your connections */
    Serial.println("No sensor found at PORT 1 ... check your wiring?");
    delay(100);
  }

  /* Initialise the 3rd sensor */
  tcaselect(2);
  if(!sensor2.begin_I2C()){
    /* There was a problem detecting the MLX90395 ... check your connections */
    Serial.println("No sensor found at PORT 2 ... check your wiring?");
    delay(100);
  }

  /* Initialise the 4th sensor */
  tcaselect(3);
  if(!sensor3.begin_I2C()){
    /* There was a problem detecting the MLX90395 ... check your connections */
    Serial.println("No sensor found at PORT 3 ... check your wiring?");
    delay(100);
  }

  sensor0.setOSR(MLX90395_OSR_8);
  sensor0.setResolution(MLX90395_RES_17);
  sensor1.setOSR(MLX90395_OSR_8);
  sensor1.setResolution(MLX90395_RES_17);
  sensor2.setOSR(MLX90395_OSR_8);
  sensor2.setResolution(MLX90395_RES_17);
  sensor3.setOSR(MLX90395_OSR_8);
  sensor3.setResolution(MLX90395_RES_17);
}


int num_media = 100; // número de medidas realizadas para obtener la media

void loop(void){
  float Bx0 = 0; // inicialización de las componentes a 0
  float By0 = 0;
  float Bz0 = 0;

  float Bx1 = 0;
  float By1 = 0;
  float Bz1 = 0;

  float Bx2 = 0;
  float By2 = 0;
  float Bz2 = 0;

  float Bx3 = 0;
  float By3 = 0;
  float Bz3 = 0;

  tcaselect(0); // El sensor 0 es aquel que está conectado al canal 0

  for (int i=0; i<num_media; i++){
    /* Get a new sensor event, normalized to uTesla */
    sensors_event_t event; 
    sensor0.getEvent(&event);
    Bx0 += event.magnetic.x;
    By0 += event.magnetic.y;
    Bz0 += event.magnetic.z;
    delay(10);
    }
  Bx0 = Bx0 / num_media;
  By0 = By0 / num_media;
  Bz0 = Bz0 / num_media;
  
  tcaselect(1); // El sensor 1 es aquel que está conectado al canal 1

  for (int i=0; i<num_media; i++){
    /* Get a new sensor event, normalized to uTesla */
    sensors_event_t event; 
    sensor1.getEvent(&event);
    Bx1 += event.magnetic.x;
    By1 += event.magnetic.y;
    Bz1 += event.magnetic.z;
    delay(10);}
  Bx1 = Bx1 / num_media;
  By1 = By1 / num_media;
  Bz1 = Bz1 / num_media;

  tcaselect(2); // El sensor 2 es aquel que está conectado al canal 2

  for (int i=0; i<num_media; i++){
    /* Get a new sensor event, normalized to uTesla */
    sensors_event_t event; 
    sensor2.getEvent(&event);
    Bx2 += event.magnetic.x;
    By2 += event.magnetic.y;
    Bz2 += event.magnetic.z;
    delay(10);}
  Bx2 = Bx2 / num_media;
  By2 = By2 / num_media;
  Bz2 = Bz2 / num_media;

  tcaselect(3); // El sensor 3 es aquel que está conectado al canal 3

  for (int i=0; i<num_media; i++){
    /* Get a new sensor event, normalized to uTesla */
    sensors_event_t event; 
    sensor2.getEvent(&event);
    Bx3 += event.magnetic.x;
    By3 += event.magnetic.y;
    Bz3 += event.magnetic.z;
    delay(10);}
  Bx3 = Bx3 / num_media;
  By3 = By3 / num_media;
  Bz3 = Bz3 / num_media;
  
  /* Display the results (mean magnetic field is measured in uTesla) */
  Serial.print("Sensor 1:\t"); 
  Serial.print(Bx0);
  Serial.print("\t"); Serial.print(By0); 
  Serial.print("\t"); Serial.print(Bz0); 
  Serial.print("\t"); Serial.println(sqrt(pow(Bx0,2)+pow(By0,2 )+pow(Bz0,2)));

  Serial.print("Sensor 2:\t"); 
  Serial.print(Bx1);
  Serial.print("\t"); Serial.print(By1); 
  Serial.print("\t"); Serial.print(Bz1); 
  Serial.print("\t"); Serial.println(sqrt(pow(Bx1,2)+pow(By1,2 )+pow(Bz1,2)));

  Serial.print("Sensor 3:\t"); 
  Serial.print(Bx2);
  Serial.print("\t"); Serial.print(By2); 
  Serial.print("\t"); Serial.print(Bz2); 
  Serial.print("\t"); Serial.println(sqrt(pow(Bx2,2)+pow(By2,2 )+pow(Bz2,2)));

  Serial.print("Sensor 4:\t"); 
  Serial.print(Bx3);
  Serial.print("\t"); Serial.print(By3); 
  Serial.print("\t"); Serial.print(Bz3); 
  Serial.print("\t"); Serial.println(sqrt(pow(Bx3,2)+pow(By3,2 )+pow(Bz3,2)));

  Serial.println("------------------------------------------------");

}