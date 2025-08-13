# 1 "/var/folders/4w/k4yhf14j7xsbp2jd85yk555r0000gn/T/tmp60vmfhkr"
#include <Arduino.h>
# 1 "/Users/bene/Downloads/openUC2-ESP32InfoScreen/src/main.ino"
#include <Arduino.h>
#include "nvs_flash.h"
#include "lvgl_controller.h"
#include "serial_controller.h"
#include "uc2ui_wifipage.h"
#include "SerialApi.h"
#include "uc2ui_ledpage.h"
#include "uc2ui_motorpage.h"
#include "uc2ui_objectivepage.h"
#include "uc2ui_laserspage.h"
#include "uc2ui_samplemappage.h"
#include "uc2ui_acquisitionpage.h"
void setup();
void loop();
#line 14 "/Users/bene/Downloads/openUC2-ESP32InfoScreen/src/main.ino"
void setup()
{
  Serial.begin(115200);
  Serial.setTimeout(50);
  Serial.println("");
  Serial.println("ESP32 Info Screen - Serial Mode");

  esp_err_t err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
  {
    Serial.println("nvs init err");
  }

  SerialApi::init();
  lvgl_controller::initlgvl();
  serial_controller::init();


  uc2ui_ledpage::setColorChangedListner(SerialApi::updateColors);
  uc2ui_motorpage::setUpdateMotorSpeedListner(SerialApi::driveMotorForever);
  uc2ui_motorpage::setMoveMotorStepsListner(SerialApi::moveMotorSteps);
  uc2ui_ledpage::setenableLedListner(SerialApi::setLedOn);
  uc2ui_motorpage::setDriveXYMotorListner(SerialApi::driveMotorXYForever);


  uc2ui_objectivepage::setOnSlotChangeListener(SerialApi::setObjectiveSlot);
  uc2ui_objectivepage::setOnSnapButtonListener(SerialApi::snapImage);


  uc2ui_laserspage::setPwmUpdateCallback(SerialApi::setPwmValue);


  uc2ui_samplemappage::setSampleMapClickCallback(SerialApi::onSampleMapClick);


  uc2ui_acquisitionpage::setCaptureButtonCallback(SerialApi::snapImage);


  SerialApi::startSerialTask();

  Serial.println("Serial interface ready");
}

void loop()
{
  SerialApi::loop();
  serial_controller::loop();
  delay(5);
}