#include "Arduino_NineAxesMotion.h"
#include <Wire.h>
#include <WiFiNINA.h>
#include <WiFiUdp.h>

NineAxesMotion mySensor;
IPAddress server(192,168,1,52);
WiFiUDP conn;
int16_t old_w = 0;
int16_t old_x = 0;
int16_t old_y = 0;
int16_t old_z = 0;

void setup()
{
  Serial.begin(115200);
  Wire.begin();
  mySensor.initSensor();
  mySensor.setOperationMode(OPERATION_MODE_NDOF);
  mySensor.setUpdateMode(MANUAL);
  mySensor.updateAccelConfig();
  WiFi.begin("aleph", "shibboleth");
  conn.begin(5004);
}

void loop()
{
  mySensor.updateQuat();
  mySensor.updateMag();
  uint8_t id = WiFi.localIP()[3];
  uint8_t action_flag = 0; // no buttons for now
  int16_t w = mySensor.readQuaternion(W_QUAT);
  int16_t x = mySensor.readQuaternion(X_QUAT);
  int16_t y = mySensor.readQuaternion(Y_QUAT);
  int16_t z = mySensor.readQuaternion(Z_QUAT);
  float mag_x = mySensor.readMagX();
  float mag_y = mySensor.readMagY();
  float mag_z = mySensor.readMagZ();
  if(old_w != w || old_x != x || old_y != y || old_z != z) {
    conn.beginPacket(server, 5005);
    conn.write(reinterpret_cast<char*>(&id), sizeof(id));
    auto time = millis();
    conn.write(reinterpret_cast<const char*>(&time), sizeof(time));
    conn.write(reinterpret_cast<char*>(&w), sizeof(w));
    conn.write(reinterpret_cast<char*>(&x), sizeof(x));
    conn.write(reinterpret_cast<char*>(&y), sizeof(y));
    conn.write(reinterpret_cast<char*>(&z), sizeof(z));
    conn.write(reinterpret_cast<char*>(&action_flag), sizeof(action_flag));
    conn.endPacket();
    old_w = w;
    old_x = x;
    old_y = y;
    old_z = z;
  }
}
