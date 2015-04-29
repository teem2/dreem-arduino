/*
 The MIT License (see LICENSE)
 Copyright (C) 2014-2015 Teem2 LLC
*/


//// these 3 includes are needed to convince Arduino IDE to reference the correct libraries:
#include <SimpleList.h>
#include <SerialCommand.h>
#include <dreem-arduino.h>

void setup()
{
	// start the serial connection to the dreem server
	Dreem.begin();

	// bind the first 2 analog inputs to dreem attributes knob1 and knob2
	Dreem.bindAnalogInput(0, "knob1");
	Dreem.bindAnalogInput(1, "knob2");
	Dreem.bindAnalogInput(2, "knob3");
	Dreem.bindAnalogInput(3, "knob4");
	
	Dreem.bindAnalogOutput(4, "led4");
	Dreem.bindAnalogOutput(5, "led3");
	Dreem.bindAnalogOutput(6, "led2");
	Dreem.bindAnalogOutput(7, "led1");

	Dreem.initComplete();
}

void loop()
{
	// calling Dreem.update() makes sure all the serial commands get processed and all the bound inputs/outputs get updated/sent.
	Dreem.update();
	
	// limit the update rate to 50hz to avoid needless data updates
	delay(20);
}


