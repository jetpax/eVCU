/* ======================================================================
 * Demonstrates a switch and a listening device
 */

#include <Arduino.h>
#include <Parameter.h>
#include <VehicleController.h>
#include <Device.h>
#include <Switch.h>


#define ISR_PIN GPIO_NUM_0

#include "WS2812_RMT.h"

#define LEDS_COUNT  1
#define LEDS_PIN	48
#define CHANNEL		0

WS2812_RMT strip = WS2812_RMT(LEDS_COUNT, LEDS_PIN, CHANNEL, TYPE_GRB);


// Instantiate the VehicleController and the vehicle's Parameters
VehicleController vc;
ParameterBool paramSwitch(0);
ParameterInt paramCount(1);
  

// Define the Device child class
class DeviceListener : public Device
{
public:
  DeviceListener(VehicleController* vc) : Device(vc) {};
  void begin() {
    Serial.println("Started DeviceListener");
    this->startTasks(8000);
    
    // Register Device for other Parameters
    this->registerForValueChanged(0);

    // Start input observation
    // nothing to observe
  };
  
  void shutdown() {
    this->unregisterForValueChanged(0);
  };
  
private:
  void onValueChanged(Parameter* pParamWithNewValue) override {
    if(pParamWithNewValue) {
      switch(pParamWithNewValue->getId()) {
        case 0:
          Serial.println("DeviceListener saw a new value of the switch: "
            +String(paramSwitch.getVal()));
          this->setIntegerValue(&paramCount, paramCount.getVal() + 1);
          break;
        default:
          break;
      }
    }
  };
};


// Instantiate Devices
Switch devOne(&vc, ISR_PIN, INPUT_PULLUP, &paramSwitch, 30);
DeviceListener devTwo(&vc);


void setup() {
  // Preparations
  Serial.begin(115200); // Start the Serial monitor
  
  Serial.println("\n===== Starting up the devices =====\n");
  
  devTwo.begin();
  devOne.begin();
  
  Serial.println("\n===== Starting the test =====\n");
  Serial.println("Device two reacts to the input observation of Device one \n"+
                 String("and changes a Parameter, to which Device one reacts.\n"));
  strip.begin();
  strip.setBrightness(20);  

  while(1) { // don't leave the scope where the Device instances live
    for (int j = 0; j < 255; j += 2) {
      for (int i = 0; i < LEDS_COUNT; i++) {
        strip.setLedColorData(i, strip.Wheel((i * 256 / LEDS_COUNT + j) & 255));
      }
      strip.show();
      vTaskDelay(10);
    }  
  }

}

void loop()
{
  vTaskDelete(NULL); // We don't need that loop
}
