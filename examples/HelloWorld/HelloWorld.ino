/*
 The MIT License (see LICENSE)
 Copyright (C) 2014-2015 Teem2 LLC
*/


#include <dreem-arduino.h>
#include <SimpleList.h>
#include <SerialCommand.h>

void setup() 
{
	// start the serial connection to the dreem server
	Dreem.begin();
	
	// register any bindings here
	
	
	// tell the dreem server we are ready for an inquery about the bind properties
    Dreem.initComplete();
}

void loop() 
{
	// call Dreem.update() regularly to update the bindings
	Dreem.update();
}
