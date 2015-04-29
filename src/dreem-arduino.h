#ifndef Dreem_h
#define Dreem_h

#include <inttypes.h>
#include <string.h>
#include <arduino.h>

#include "SimpleList.h"

//#define DREEMDEBUG 
#define DREEMDEBUGSERIAL Serial

#define DREEMSERIAL Serial

#define DIGITALINPUTDEBOUNCECYCLES 20
#define DREEMSERIALLINETERMINATOR "\r\n"

extern "C" 
{
	typedef void(*DreemMethodCallback)(void);
	typedef void(*DreemStringAttributeCallback)(String, String );
	typedef void(*DreemFloatAttributeCallback)(String, String);
	typedef void(*DreemIntAttributeCallback)(String, String);
}

// contains the details to connect a dreem name to an analog input pin
class AnalogInputBinding
{
public:
	AnalogInputBinding(int _pin, String _name) : name(_name), pin(_pin)
	{
		lastValue = analogRead(_pin);
	}
	AnalogInputBinding(){ lastValue = 0; };

	String name;
	int pin;
	int lastValue;
};

// contains the details to connect a dreem name to an output pin. 
class OutputBinding
{
public:
	OutputBinding(int _pin, String _name, bool _analog = false) : name(_name), pin(_pin), analog(_analog)
	{
		pinMode(_pin, OUTPUT);
		digitalWrite(_pin, LOW);
	}

	OutputBinding(){};

	String name;
	int pin;
	bool analog;
};


// contains the details to connect a dreem name to a digital input pin. 
// the input is debounced for a few cycles.
class DigitalInputBinding
{
public:
	DigitalInputBinding(int _pin, String _name, bool pullup = false) : name(_name), pin(_pin)
	{
		pinMode(_pin, INPUT);
		if (pullup)
		{
			digitalWrite(_pin, HIGH);
		}
		else
		{
			digitalWrite(_pin, LOW);
		}
		lastValue = digitalRead(_pin);
		stablecount = 0;
	}

	DigitalInputBinding()
	{ 
		lastValue = 0; 
		stablecount = 0;
	};

	String name;
	int pin;
	int lastValue;
	long stablecount;
};

class AttributeBinding
{
public:
	AttributeBinding(){};
	AttributeBinding(String name, void(*function)(AttributeBinding *, String), void *userdata) : name(name), function(function), userdata(userdata){}
	String name;
	void(*function)(AttributeBinding *, String);
	void *userdata;
};

class MethodRegistration
{
public:
	MethodRegistration(){};
	MethodRegistration(String name, String (*function)(void *), void *userdata) : name(name), function(function), userdata(userdata){}
	String name;
	String (*function)(void*);
	void *userdata;
};

// available modes for operating the library. 
enum
{
	Dreem_Serial,
	Dreem_Wifi,
	Dreem_Ethernet
};

class DreemInterface: public Print
{
public:
	DreemInterface(){ lastHeartBeat = millis(); }

	void begin();
	void initComplete();
	void bindAttribute(String name, void(*function)(AttributeBinding*, String), void* userdata = NULL);
	
	void setAttribute(String name, int value);
	void setAttribute(String name, float value);
	void setAttribute(String name, String value);

	void bindAnalogInput(int pin, String name);
	void bindDigitalInput(int pin, String name, bool pullup = false);
	void bindDigitalOutput(int pin, String name);
	void bindAnalogOutput(int pin, String name);
	void registerMethod(String name, String(*function)(void *), void* userdata = NULL);
	void update();

	// method called from remote
	void executeMethod(String name);
	
	// call a method on the host
	void callMethod(String name);
	void sendBoundItems();
	
	void updateAttribute(String name, String value);
	 
	// for Print class overload:
	virtual size_t write(uint8_t);
private:
	SimpleList<AnalogInputBinding> BoundAnalogInputs;
	SimpleList<DigitalInputBinding> BoundDigitalInputs;
	SimpleList<OutputBinding> BoundDigitalOutputs;
	SimpleList<OutputBinding> BoundAnalogOutputs;
	SimpleList<AttributeBinding> BoundAttributes;
	SimpleList<MethodRegistration> RegisteredMethods;

	void updateAnalogInputs();
	void updateDigitalInputs();
	
	void readIncomingSerialCommands();

	int dreemMode;
	uint32_t lastHeartBeat;
};

extern DreemInterface Dreem;

#endif
