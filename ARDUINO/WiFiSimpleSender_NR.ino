/*
  ArduinoMqttClient - WiFi Simple Sender

  This example connects to a MQTT broker and publishes a message to
  a topic once a second.

  The circuit:
  - Arduino MKR 1000, MKR 1010 or Uno WiFi Rev2 board

  This example code is in the public domain.
*/

#include <ArduinoMqttClient.h>
#if defined(ARDUINO_SAMD_MKRWIFI1010) || defined(ARDUINO_SAMD_NANO_33_IOT) || defined(ARDUINO_AVR_UNO_WIFI_REV2)
  #include <WiFiNINA.h>
#elif defined(ARDUINO_SAMD_MKR1000)
  #include <WiFi101.h>
#elif defined(ARDUINO_ARCH_ESP8266)
  #include <ESP8266WiFi.h>
#elif defined(ARDUINO_PORTENTA_H7_M7) || defined(ARDUINO_NICLA_VISION) || defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_GIGA)
  #include <WiFi.h>
#endif
#include "Adafruit_MLX90395.h"
#include <Wire.h>
Adafruit_MLX90395 sensor0 = Adafruit_MLX90395();
Adafruit_MLX90395 sensor1 = Adafruit_MLX90395();
Adafruit_MLX90395 sensor2 = Adafruit_MLX90395();
Adafruit_MLX90395 sensor3 = Adafruit_MLX90395();
#include "arduino_secrets.h"
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;    // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
char dx; // dx and dy for Newton Raphson
char dy;
bool NR = false; // varible to indicate if the user is putting the dx, dy inputs

// To connect with SSL/TLS:
// 1) Change WiFiClient to WiFiSSLClient.
// 2) Change port value from 1883 to 8883.
// 3) Change broker value to a server with a known SSL/TLS root certificate 
//    flashed in the WiFi module.

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = "test.mosquitto.org";
int        port     = 1883;
const char topic[]  = "arduino/simple/asm/prueba";

//set interval for sendig messages (milliseconds)
const long interval = 1000;
unsigned long previousMillis = 0;

//TCA9548A configuration
#define TCAADDR 0x70

void tcaselect(uint8_t i){
  if (i > 7) return;
 
  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();  
}

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // attempt to connect to WiFi network:
  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    // failed, retry
    Serial.print(".");
    delay(10000);
  }

  Serial.println("You're connected to the network");
  Serial.println();

  // You can provide a unique client ID, if not set the library uses Arduino-millis()
  // Each client must have a unique client ID
  // mqttClient.setId("clientId");

  // You can provide a username and password for authentication
  // mqttClient.setUsernamePassword("username", "password");

  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);

  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());

    while (1);
  }

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();

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

int num_media = 300; // número de medidas realizadas para obtener la media
float Bx_old;
float By_old;
float Bx_new;
float By_new;
float mx;
float my;

void loop() {
  // call poll() regularly to allow the library to send MQTT keep alives which
  // avoids being disconnected by the broker
  mqttClient.poll();

  // to avoid having delays in loop, we'll use the strategy from BlinkWithoutDelay
  // see: File -> Examples -> 02.Digital -> BlinkWithoutDelay for more info
  unsigned long currentMillis = millis();
  
  // Magnetic field operations
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
  
  for (int i=0; i<num_media; i++){
    /* Get a new sensor event, normalized to uTesla */
    sensors_event_t event;
    tcaselect(0); // El sensor 0 es aquel que está conectado al canal 0
    sensor0.getEvent(&event);
    Bx0 += event.magnetic.x;
    By0 += event.magnetic.y;
    Bz0 += event.magnetic.z;
    tcaselect(1); // El sensor 1 es aquel que está conectado al canal 1
    sensor1.getEvent(&event);
    Bx1 += event.magnetic.x;
    By1 += event.magnetic.y;
    Bz1 += event.magnetic.z;
    tcaselect(2); // El sensor 2 es aquel que está conectado al canal 2
    sensor2.getEvent(&event);
    Bx2 += event.magnetic.x;
    By2 += event.magnetic.y;
    Bz2 += event.magnetic.z;
    tcaselect(3); // El sensor 3 es aquel que está conectado al canal 3
    sensor3.getEvent(&event);
    Bx3 += event.magnetic.x;
    By3 += event.magnetic.y;
    Bz3 += event.magnetic.z;
    delay(0.1);
    }
  Bx0 = Bx0 / num_media;
  By0 = By0 / num_media;
  Bz0 = Bz0 / num_media;
  float B0 = sqrt(pow(Bx0,2)+pow(By0,2 )+pow(Bz0,2));

  Bx1 = Bx1 / num_media;
  By1 = By1 / num_media;
  Bz1 = Bz1 / num_media;
  float B1 = sqrt(pow(Bx1,2)+pow(By1,2 )+pow(Bz1,2));

  Bx2 = Bx2 / num_media;
  By2 = By2 / num_media;
  Bz2 = Bz2 / num_media;
  float B2 = sqrt(pow(Bx2,2)+pow(By2,2 )+pow(Bz2,2));

  Bx3 = Bx3 / num_media;
  By3 = By3 / num_media;
  Bz3 = Bz3 / num_media;
  float B3 = sqrt(pow(Bx3,2)+pow(By3,2 )+pow(Bz3,2));

  // Cuando el usuario haya introducido el desplazamiento que quiere realizar (dx, dy):
  //1) Guardamos el último dato de la diferecia adquirida en las direcciones en cuestión, es decir:
  //   (Bs0(x0) - Bs1(x0)) y (Bs2(y0) - Bs3(y0))
  //2) Esperamos a que el usuario presione ENTER, indicando que ya ha realizado el desplazamiento
  //3) Las nuevas diferencias adquiridas, es de decir:
  //   (Bs0(x0+dx) - Bs1(x0+dx)) y (Bs2(y0+dy) - Bs3(y0+dy))
  //   se utilizan para calcular dBx y dBy
  //4) Calculamos las nuevas estimaciones x1, y1
  // NOTA: En todos estos pasos la información relevante envía por MQTT para que el usuario pueda verla

  // 0) Recoger dx y dy
  if (strcmp(Serial.readString().c_str(), "START") == 0){
    Serial.print("Comienzo del método Newton-Raphson!");
    dx = Serial.parseFloat();
    Serial.print("dx: " );
    Serial.println(dx);

    while (!Serial.available()) {
    // Esperar hasta el segundo input (dy)
    }

    dy = Serial.parseFloat();
    Serial.print("dy: " );
    Serial.println(dy);

    // 1) Guardar (Bs0(x0) - Bs1(x0)) y (Bs2(y0) - Bs3(y0))
    Bx_old = B0 - B1;
    By_old = B2 - B3;
    Serial.print("Bx(x0): " ); Serial.println(Bx_old);
    Serial.print("By(y0): " ); Serial.println(By_old);
    // 2) Esperar a ENTER
    while (Serial.read() != '\n') {
      // Esperar hasta ENTER
    }
    NR = true;
    }
  
  if (currentMillis - previousMillis >= interval) {
    // save the last time a message was sent
    previousMillis = currentMillis;
    if (NR == false){
      /* Display the results (mean magnetic field is measured in uTesla) */
      Serial.print(Bx0);
      Serial.print("\t"); Serial.print(By0);
      Serial.print("\t"); Serial.print(Bz0);
      Serial.print("\t"); Serial.println(B0);

      Serial.print(Bx1);
      Serial.print("\t"); Serial.print(By1);
      Serial.print("\t"); Serial.print(Bz1);
      Serial.print("\t"); Serial.println(B1);

      Serial.print(Bx2);
      Serial.print("\t"); Serial.print(By2); 
      Serial.print("\t"); Serial.print(Bz2); 
      Serial.print("\t"); Serial.println(B2);

      Serial.print(Bx3);
      Serial.print("\t"); Serial.print(By3);
      Serial.print("\t"); Serial.print(Bz3);
      Serial.print("\t"); Serial.println(B3);
      Serial.print("Bx:\t");
      Serial.print(B0 - B1);
      Serial.print("\tBy:\t");
      Serial.println(B2 - B3);
      Serial.println("------------------------------------------------");
    } else {
      Bx_new = B0 - B1;
      By_new = B2 - B3;
      mx = - Bx_old / ((Bx_new - Bx_old)/dx);
      my = - By_old / ((By_new - By_old)/dy);
      Serial.print("Bx(x0+dx): " ); Serial.println(Bx_new);
      Serial.print("By(y0+dy): " ); Serial.println(By_new);
      Serial.print("Desplazamientos a realizar: ");
      Serial.print("mx: " ); Serial.println(mx);
      Serial.print("my: " ); Serial.println(my);

      // The displacement to be made is the only information to be sent
      mqttClient.beginMessage(topic);
      Serial.print("mx: " ); mqttClient.print(mx);
      mqttClient.print("\t");
      Serial.print("my: " ); mqttClient.print(my);
      mqttClient.endMessage();
    }
  }
}
