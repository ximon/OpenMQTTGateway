/*  
  OpenMQTTGateway Addon  - ESP8266 or Arduino program for home automation 
   Act as a wifi or ethernet gateway between your 433mhz/infrared IR signal  and a MQTT broker 
   Send and receiving command by MQTT
 
    GPIO Input derived from HC SR-501 reading Addon and https://www.arduino.cc/en/Tutorial/Debounce

    This reads a high (open) or low (closed) through a circuit (switch, float sensor, etc.) connected to ground.

    Copyright: (c)Florian ROBERT
    
    Contributors:
    - 1technophile
    - QuagmireMan
  
    This file is part of OpenMQTTGateway.
    
    OpenMQTTGateway is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    OpenMQTTGateway is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "User_config.h"

#define GPIO_Count 2

int input_GPIOs[GPIO_COUNT] = [INPUT_GPIO, 3];
char* input_GPIO_names[GPIO_COUNT] = ["gpio1","gpio2"];

#ifdef ZsensorGPIOInput
#  if defined(TRIGGER_GPIO) && INPUT_GPIO == TRIGGER_GPIO
unsigned long resetTimes[GPIO_Count] = [0,0];
#  endif

unsigned long lastDebounceTimes[GPIO_Count] = [0,0];
int InputStates[GPIO_Count] = [3,3]; // Set to 3 so that it reads on startup
int lastInputStates[GPIO_Count] = [3,3];

void setupGPIOInput() {

  for (int i = 0; i < GPIO_Count; i++) {
    Log.notice(F("Reading GPIO at pin: %d" CR), input_GPIOs[i];
    pinMode(input_GPIOs[i], INPUT_PULLUP); // declare GPIOInput pin as input_pullup to prevent floating. Pin will be high when not connected to ground
  }
}


void MeasureGPIOInput(gpioIndex) {
  int reading = digitalRead(input_GPIOs[gpioIndex]);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (readings[gpioIndex] != lastInputStates[gpioIndex]) {
    // reset the debouncing timer
    lastDebounceTimes[gpioIndex] = millis();
  }

  if (reading != lastInputStates[gpioIndex]) {
    // reset the debouncing timer
    lastDebounceTimes[gpioIndex] = millis();
  }


  if ((millis() - lastDebounceTimes[gpioIndex]) > GPIOInputDebounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:
#  if defined(ESP8266) || defined(ESP32)
    yield();
#  endif

#  if defined(TRIGGER_GPIO) && input_GPIO == TRIGGER_GPIO
    if (reading == LOW) {
      if (resetTimes[gpioIndex] == 0) {
        resetTimes[gpioIndex] = millis();
      } else if ((millis() - resetTimes[gpioIndex]) > 10000) {
        Log.trace(F("Button Held" CR));
        Log.notice(F("Erasing ESP Config, restarting" CR));
        setup_wifimanager(true);
      }
    } else {
      resetTime[gpioIndex] = 0;
    }
#  endif
    // if the Input state has changed:
    if (reading != InputStates[gpioIndex]) {
      InputStates[gpioIndex] = reading;
      Log.trace(F("Creating GPIOInput buffer" CR));
      const int JSON_MSG_CALC_BUFFER = JSON_OBJECT_SIZE(1);
      StaticJsonDocument<JSON_MSG_CALC_BUFFER> jsonBuffer;
      JsonObject GPIOdata = jsonBuffer.to<JsonObject>();
      if (InputState == HIGH) {
        GPIOdata[input_GPIO_names[gpioIndex]] = "HIGH";
      }
      if (InputState == LOW) {
        GPIOdata[input_GPIO_names[gpioIndex]] = "LOW";
      }
      if (GPIOdata.size() > 0)
        pub(subjectGPIOInputtoMQTT, GPIOdata);

#  ifdef ZactuatorONOFF
      if (InputState_1 == ACTUATOR_BUTTON_TRIGGER_LEVEL) {
        ActuatorButtonTrigger();
      }
#  endif
    }
  }

  // save the reading. Next time through the loop, it'll be the lastInputState:
  lastInputStates[gpioIndex] = reading;
}
#endif
