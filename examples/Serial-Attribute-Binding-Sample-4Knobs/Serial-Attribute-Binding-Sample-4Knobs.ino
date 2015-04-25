//// these 4 includes are needed to convince Arduino IDE to reference the correct libraries:
#include <SimpleList.h>
#include <SerialCommand.h>
#include <SoftwareSerial.h>
#include <Dreem-Arduino.h>


void setup()
{
  Dreem.begin();

  // bind the first 2 analog inputs to dreem attributes knob1 and knob2
  Dreem.bindAnalogInput(0, "knob1");
   Dreem.bindAnalogInput(1, "knob2");
  Dreem.bindAnalogInput(2, "knob3");
  Dreem.bindAnalogInput(3, "knob4");

  Dreem.callMethod("initsequencedone");
}

void loop()
{
  // calling Dreem.update() makes sure all the serial commands get processed and all the bound inputs/outputs get updated/sent.
  Dreem.update();
  delay(20);
}


