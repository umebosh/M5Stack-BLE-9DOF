#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

#include <M5Stack.h>

#define BNO055_SAMPLERATE_DELAY_MS (100)

Adafruit_BNO055 bno = Adafruit_BNO055();

BLECharacteristic *pCharacteristic;
bool deviceConnected = false;
uint8_t value = 0;

uint8_t pitchData[2];
uint16_t pitchValue;
uint8_t rollData[2];
uint16_t rollValue;
uint8_t yawData[2];
uint16_t yawValue;
uint8_t allData[6];

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};


void setup()
{
  M5.begin();
  Wire.begin();

  if (!bno.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while (1);
  }
  int8_t temp = bno.getTemp();
  Serial.print("Current Temperature: ");
  Serial.print(temp);
  Serial.println(" C");
  Serial.println("");
  bno.setExtCrystalUse(true);
  Serial.println("Calibration status values: 0=uncalibrated, 3=fully calibrated");


  M5.Lcd.setTextSize(1);
  M5.Lcd.setTextColor(GREEN , BLACK);
  M5.Lcd.fillScreen(BLACK);
  BLEDevice::init("MyESP32");
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );

  // https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.descriptor.gatt.client_characteristic_configuration.xml
  // Create a BLE Descriptor
  pCharacteristic->addDescriptor(new BLE2902());

  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");
}

void loop() {
  imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);

  /* Display the floating point data */
  Serial.print("X: ");
  Serial.print(euler.x());
  Serial.print(" Y: ");
  Serial.print(euler.y());
  Serial.print(" Z: ");
  Serial.print(euler.z());
  Serial.println("\t\t");

  M5.Lcd.setTextColor(GREEN , BLACK);
  M5.Lcd.setTextSize(3);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.print("X: ");
  M5.Lcd.println(euler.x());
  M5.Lcd.print("Y: ");
  M5.Lcd.println(euler.y());
  M5.Lcd.print("Z: ");
  M5.Lcd.println(euler.z());
  M5.Lcd.println("\t\t");
  /*
    // Quaternion data
    imu::Quaternion quat = bno.getQuat();
    Serial.print("qW: ");
    Serial.print(quat.w(), 4);
    Serial.print(" qX: ");
    Serial.print(quat.y(), 4);
    Serial.print(" qY: ");
    Serial.print(quat.x(), 4);
    Serial.print(" qZ: ");
    Serial.print(quat.z(), 4);
    Serial.print("\t\t");
  */

  /* Display calibration status for each sensor. */
  uint8_t system, gyro, accel, mag = 0;
  bno.getCalibration(&system, &gyro, &accel, &mag);

  pitchValue = (uint16_t)(euler.x() * 100);
  rollValue = (uint16_t)(euler.y() * 100);
  yawValue = (uint16_t)(euler.z() * 100);
  pitchData[0] = pitchValue >> 8;
  pitchData[1] = pitchValue;
  rollData[0] = rollValue >> 8;
  rollData[1] = rollValue;
  yawData[0] = yawValue >> 8;
  yawData[1] = yawValue;
  allData[0] = pitchData[0];
  allData[1] = pitchData[1];
  allData[2] = rollData[0];
  allData[3] = rollData[1];
  allData[4] = yawData[0];
  allData[5] = yawData[1];

  if (deviceConnected) {
    Serial.printf("*** NOTIFY: %d ***\n", value);
    pCharacteristic->setValue(allData, 6);
    pCharacteristic->notify();
    //pCharacteristic->indicate();
    value++;
    //    c++;
  }
  delay(100);
}
