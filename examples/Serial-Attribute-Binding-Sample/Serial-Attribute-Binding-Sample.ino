//// these 4 includes are needed to convince Arduino IDE to reference the correct libraries:
#include <SimpleList.h>
#include <SerialCommand.h>
#include <SoftwareSerial.h>
#include <Dreem.h>

String QuickCountdown(void *userdata);

void setup()
{
  Dreem.begin();

  // bind the first 2 analog inputs to dreem attributes knob1 and knob2
  Dreem.bindAnalogInput(3, "knob1");
  Dreem.bindAnalogInput(4, "knob2");

  Dreem.bindDigitalInput(11, "button1", true);
  Dreem.bindDigitalInput(12, "button2", true);

  Dreem.bindAttribute("number", writeNumber);

  Dreem.bindDigitalOutput(2, "seg0");
  Dreem.bindDigitalOutput(3, "seg1");
  Dreem.bindDigitalOutput(4, "seg2");
  Dreem.bindDigitalOutput(5, "seg3");
  Dreem.bindDigitalOutput(6, "seg4");
  Dreem.bindDigitalOutput(7, "seg5");
  Dreem.bindDigitalOutput(8, "seg6");
  Dreem.bindDigitalOutput(9, "seg7");

  Dreem.registerMethod("countdown" , QuickCountdown);
  QuickCountdown((void*)0);

  Dreem.callMethod("initsequencedone");
}

void loop()
{
  // calling Dreem.update() makes sure all the serial commands get processed and all the bound inputs/outputs get updated/sent.
  Dreem.update();
}

String QuickCountdown(void *userdata)
{
  for (int i = 9; i > 0; i--)
  {
    Dreem.updateAttribute("number", String(i));

    delay(100);
  }
  Dreem.updateAttribute("number", "0");
  delay(100);

  Dreem.updateAttribute("number", "off");
  return "done";
}


////// some fun with the 7seg display:

////
//    777
//   1   6
//   1   6
//   1   6
//    000
//   2   5
//   2   5
//   2   5
//    333   4

byte digits[10][8] =
{
  {0, 1, 1, 1, 0, 1, 1, 1 }  , //0
  {0, 0, 0, 0, 0, 1, 1, 0 }  , //1
  {1, 0, 1, 1, 0, 0, 1, 1 }  , //2
  {1, 0, 0, 1, 0, 1, 1, 1 }  , //3
  {1, 1, 0, 0, 0, 1, 1, 0 }  , //4
  {1, 1, 0, 1, 0, 1, 0, 1 }  , //5
  {1, 1, 1, 1, 0, 1, 0, 1 }  , //6
  {0, 0, 0, 0, 0, 1, 1, 1 }  , //7
  {1, 1, 1, 1, 0, 1, 1, 1 }  , //8
  {1, 1, 0, 1, 0, 1, 1, 1 }  , //9
};

void writeNumber(AttributeBinding* A, String newval)
{
  if (newval == "off")
  {
    for (int i = 0 ; i < 8; i++)
    {
      String segname = "seg" + String(i);
      Dreem.updateAttribute(segname, "0");
    }
    return;
  }
  
  int N = newval.toInt();
  N = N % 10;
  
  for (int i = 0 ; i < 8; i++)
  {
    String segname = "seg" + String(i);
    if (digits[N][i] > 0)
    {
      Dreem.updateAttribute(segname, "1");
    }
    else
    {
      Dreem.updateAttribute(segname, "0");
    };
  };
};
