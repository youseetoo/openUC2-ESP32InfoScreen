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

void setup()
{
  Serial.begin(115200); /* prepare for possible serial debug */
  Serial.setTimeout(100); 
  Serial.setTxBufferSize(1024);
  Serial.println("");
  Serial.println("ESP32 Info Screen - Serial Mode");
  esp_log_level_set("*", ESP_LOG_NONE);
  Serial.setDebugOutput(false);

  esp_err_t err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
  {
    Serial.println("nvs init err");
  }

  SerialApi::init();
  lvgl_controller::initlgvl();
  serial_controller::init();

  // Set up UI event handlers for serial communication
  uc2ui_ledpage::setColorChangedListner(SerialApi::updateColors);
  uc2ui_motorpage::setUpdateMotorSpeedListner(SerialApi::driveMotorForever);
  uc2ui_motorpage::setMoveMotorStepsListner(SerialApi::moveMotorSteps);
  uc2ui_ledpage::setenableLedListner(SerialApi::setLedOn);
  uc2ui_motorpage::setDriveXYMotorListner(SerialApi::driveMotorXYForever);
  uc2ui_motorpage::setGoToPositionListener(SerialApi::goToPosition);
  
  // Set up new objective page handlers
  uc2ui_objectivepage::setOnSlotChangeListener(SerialApi::setObjectiveSlot);


  // Set up PWM/Lasers page handler
  uc2ui_laserspage::setPwmUpdateCallback(SerialApi::setPwmValue);

  // Set up sample map click handler
  uc2ui_samplemappage::setSampleMapClickCallback(SerialApi::onSampleMapClick);

  // Set up acquisition page capture button handler  
  uc2ui_acquisitionpage::setCaptureButtonCallback(SerialApi::snapImage);

  // Start the serial message task
  SerialApi::startSerialTask();
  
  Serial.println("Serial interface ready");
}

void loop()
{
  SerialApi::loop();
  serial_controller::loop();
  delay(5);
}
