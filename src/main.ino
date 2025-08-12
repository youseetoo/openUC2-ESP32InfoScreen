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

void setup()
{
  Serial.begin(115200); /* prepare for possible serial debug */
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

  // Set up UI event handlers for serial communication
  uc2ui_ledpage::setColorChangedListner(SerialApi::updateColors);
  uc2ui_motorpage::setUpdateMotorSpeedListner(SerialApi::driveMotorForever);
  uc2ui_ledpage::setenableLedListner(SerialApi::setLedOn);
  uc2ui_motorpage::setDriveXYMotorListner(SerialApi::driveMotorXYForever);
  
  // Set up new objective page handlers
  uc2ui_objectivepage::setOnSlotChangeListener(SerialApi::setObjectiveSlot);
  uc2ui_objectivepage::setOnSnapButtonListener(SerialApi::snapImage);

  // Set up PWM/Lasers page handler
  uc2ui_laserspage::setPwmUpdateCallback(SerialApi::setPwmValue);

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
