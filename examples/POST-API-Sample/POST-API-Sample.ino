/*
 The MIT License (see LICENSE)
 Copyright (C) 2014-2015 Teem2 LLC
*/

#include <Bridge.h>
#include <HttpClient.h>

void setup() 
{
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  Bridge.begin();
  Serial.begin(115200);
}

void loop() 
{
  // blink the LED at the beginning of the cycle
  digitalWrite(13, HIGH);
  delay(100);
  digitalWrite(13, LOW);
  
  // perform a POST to the server.
  HttpClient client;
  char  Post[] = "{\"type\":\"method\",\"rpcid\": \"screens\",\"method\":\"test\",\"args\": [\"hello from arduino!\"]}";
  client.post("http://10.0.1.3:8080/posttest", Post);

  //read back the answer from the server and dump to Serial
  while (client.available()) 
  {
    char c = client.read();
    Serial.print(c);
  }
  Serial.flush();
  
  // blink the LED at the end of the cycle
  digitalWrite(13, HIGH);
  delay(100);
  digitalWrite(13, LOW);
  
  delay(1000);
}
