
#include "Dreem.h"
#include "Arduino.h"
#include "SerialCommand.h"

#ifdef DREEMDEBUG	
	#define LOG(x) DREEMDEBUGSERIAL.print(x)	
	#define LOGLN(x) DREEMDEBUGSERIAL.println(x)
#else
	#define LOG(x)	
	#define LOGLN(x)
#endif

SerialCommand SerialCommandInterface;   

void sendBoundItems()
{
	Dreem.sendBoundItems();
}

void updateAttribute()
{
	char *arg1 = SerialCommandInterface.next();
	char *arg2 = SerialCommandInterface.next();
	if (arg1 != NULL && arg2 != NULL)
	{
		Dreem.updateAttribute(String(arg1), String(arg2));
	}
}

void executeMethod()
{
	char *arg1 = SerialCommandInterface.next();
	if (arg1 != NULL)
	{
		Dreem.executeMethod(String(arg1));
	}
}

void unknownCommand()
{
	Serial.println("I don't understand that command..");
}

// start Dreem using the default serial connection.
void DreemInterface::begin()
{
	dreemMode = Dreem_Serial;
	DREEMSERIAL.begin(115200);

#ifdef DREEMDEBUG

#if DREEMDEBUGSERIAL!=DREEMSERIAL 
	DREEMDEBUGSERIAL.begin(115200);	

	#endif
	LOGLN("DreemInterface::begin()");
#endif

	SerialCommandInterface.addCommand("inq", ::sendBoundItems); 
	SerialCommandInterface.addCommand("atr", ::updateAttribute);
	SerialCommandInterface.addCommand("mtd", ::executeMethod);	

	SerialCommandInterface.addDefaultHandler(::unknownCommand);

}

void DreemInterface::setAttribute(String name, int Value)
{
	print("{\"atr\":\"");
	print(name);
	print("\",\"type\":\"i\",\"value\":");
	print(Value);
	print("}");
	print(DREEMSERIALLINETERMINATOR);
}

void DreemInterface::setAttribute(String name, String Value)
{
	print("{\"atr\":\"");
	print(name);
	print("\",\"type\":\"s\",\"value\":\"");
	print(Value);
	print("\"}");
	print(DREEMSERIALLINETERMINATOR);
}

void DreemInterface::setAttribute(String name, float Value)
{
	print("{\"atr\":\"");
	print(name);
	print("\",\"type\":\"f\",\"value\":");
	print(Value);
	print("}");
	print(DREEMSERIALLINETERMINATOR);	
}

void DreemInterface::registerMethod(String name, String(*function)(void *), void* userdata)
{
	RegisteredMethods.push_back(MethodRegistration(name, function, userdata));
}

void DreemInterface::update()
{
	updateAnalogInputs();
	updateDigitalInputs();
	readIncomingSerialCommands();
	if (millis() - lastHeartBeat > 1000)
	{
		lastHeartBeat = millis();
		
		callMethod("heartbeat");
	}
}

void DreemInterface::bindAnalogInput(int pin, String name)
{
	BoundAnalogInputs.push_back(AnalogInputBinding(pin, name));
}

void UpdateOutputPin(AttributeBinding *binding, String newvalue)
{
	if (newvalue == "1" || newvalue == "HIGH" || newvalue == "ON" || newvalue == "on" || newvalue == "high")
	{
		digitalWrite((int)binding->userdata, HIGH);
	}
	else
	{
		digitalWrite((int)binding->userdata, LOW);
	}
}

void DreemInterface::bindDigitalOutput(int pin, String name)
{
	BoundDigitalOutputs.push_back(DigitalOutputBinding(pin, name));
	bindAttribute(name, UpdateOutputPin,(void*)pin);
}

void DreemInterface::bindDigitalInput(int pin, String name, bool pullup)
{
	BoundDigitalInputs.push_back(DigitalInputBinding(pin, name, pullup));
}

void DreemInterface::bindAttribute(String name, void(*function)(AttributeBinding*, String), void *userdata)
{
	BoundAttributes.push_back(AttributeBinding(name, function, userdata));
}

void DreemInterface::updateAttribute(String name, String value)
{
	for (SimpleList<AttributeBinding>::iterator itr = BoundAttributes.begin(); itr != BoundAttributes.end(); ++itr)
	{
		if (itr->name == name)
		{
			itr->function(&(*itr), value);
		}
	}
}

void DreemInterface::updateAnalogInputs()
{
	//Serial.println("DreemInterface::updateAnalogInputs()");
	for (SimpleList<AnalogInputBinding>::iterator itr = BoundAnalogInputs.begin(); itr != BoundAnalogInputs.end(); ++itr)
	{
		int AnalogVal = analogRead(itr->pin) / 10; // is this thresholding needed?
		if (itr->lastValue != AnalogVal)
		{
			float Value = AnalogVal / 102.4f;
			setAttribute(itr->name, Value);
			itr->lastValue= AnalogVal;			
		}
	}
}

void DreemInterface::updateDigitalInputs()
{
	for (SimpleList<DigitalInputBinding>::iterator itr = BoundDigitalInputs.begin(); itr != BoundDigitalInputs.end(); ++itr)
	{
		int newval = digitalRead(itr->pin);
		if (itr->lastValue != newval)
		{
			itr->stablecount = 0;			
			itr->lastValue = newval;
		}
		else
		{
			if (itr->stablecount < (DIGITALINPUTDEBOUNCECYCLES + 4))
			{
				itr->stablecount++;
				if (itr->stablecount == DIGITALINPUTDEBOUNCECYCLES)
				{
					setAttribute(itr->name, newval);
				}
			}
		}
	}
}

void DreemInterface::readIncomingSerialCommands()
{
	SerialCommandInterface.readSerial();     
}

void DreemInterface::callMethod(String name)
{
	println("{\"mtd\":\"" + name + "\"}");
}

void DreemInterface::executeMethod(String name)
{
	for (SimpleList<MethodRegistration>::iterator itr = RegisteredMethods.begin(); itr != RegisteredMethods.end(); ++itr)
	{
		if (itr->name == name)
		{
			
			println("{\"ret\":\"" + itr->function(itr->userdata) + "\"}");
			return;
		}
	}	
}

void DreemInterface::sendBoundItems()
{
	print("{\"inq\":{");
	print("\"digitalInputs\":[");
	int c = 0;
	for (SimpleList<DigitalInputBinding>::iterator itr = BoundDigitalInputs.begin(); itr != BoundDigitalInputs.end(); ++itr)
	{
		print("\"");
		print(itr->name);	
		print("\"");
		if (c < BoundDigitalInputs.size() - 1)
		{
			print(",");
		}
		c++;
	}
	print("],");
	print("\"digitalOutputs\":[");
	c = 0;
	for (SimpleList<DigitalOutputBinding>::iterator itr = BoundDigitalOutputs.begin(); itr != BoundDigitalOutputs.end(); ++itr)
	{
		print("\"");
		print(itr->name);
		print("\"");
		if (c < BoundDigitalOutputs.size() - 1)
		{
			print(",");
		}
		c++;
	}
	
	print("],");
	print("\"attributeBindings\":[");
	c = 0;
	for (SimpleList<AttributeBinding>::iterator itr = BoundAttributes.begin(); itr != BoundAttributes.end(); ++itr)
	{
		print("\"");
		print(itr->name);
		print("\"");
		if (c < BoundAttributes.size() - 1)
		{
			print(",");
		}
		c++;
	}

	print("],");
	print("\"analogInputs\":[");
	c = 0;
	for (SimpleList<AnalogInputBinding>::iterator itr = BoundAnalogInputs.begin(); itr != BoundAnalogInputs.end(); ++itr)
	{
		print("\"");
		print(itr->name);
		print("\"");

		if (c < BoundAnalogInputs.size() - 1)
		{
			print(",");
		}
		c++;
	}
	print("]");
	println ("}}");
}

size_t DreemInterface::write(uint8_t character)
{
	switch (dreemMode)
	{
	case Dreem_Serial:
			DREEMSERIAL.write(character);
		break;

	case Dreem_Wifi:
			// todo
		break;

	case Dreem_Ethernet:
			// todo
		break;
	}
}

// The actual instance
DreemInterface Dreem;