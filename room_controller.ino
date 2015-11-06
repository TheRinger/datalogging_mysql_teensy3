#include <SPI.h>
#include <UIPEthernet.h>
#include <Wire.h>
#include "DHT.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#define DHTPIN 2     // what digital pin we're connected to
#define DHTTYPE DHT11   // DHT 11
#define ONE_WIRE_BUS 6
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress insideThermometer = { 0x28, 0xF3, 0xB3, 0xB3, 0x4, 0x0, 0x0, 0xB3 };

EthernetClient client;
IPAddress ip(192,168,1,66);
signed long next;
int interval = 8000; 
char server[] = "192.168.1.30";
int co2_start = 600;
int co2_stop = 1000;
int maxTemp = 90;
int maxHum = 80;
int photocellPin = 14;  
int photocellReading;   
int LEDpin = 3;       
int LEDpin2 = 4;
int TMP = 6;
int tmp = 0;
int co2Addr = 0x68; // This is the default address of the CO2 sensor, 7bits shifted left.
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  sensors.begin();
  //while (!Serial) {
  //  ; 
  //}
  Wire.begin ();
  pinMode(13, OUTPUT);
  pinMode(LEDpin, OUTPUT);
  pinMode(LEDpin2, OUTPUT);
  pinMode(TMP, INPUT);
  dht.begin();
  uint8_t mac[6] = {0x00,0x01,0x02,0x03,0x04,0x05};
  Ethernet.begin(mac, ip);
  Serial.print(F("localIP: "));
}

///////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

void loop() {
  int co2;
  int co2Value = readCO2();
  if(co2Value != 0) {
    co2 = co2Value; 
  }
  else
  {
  Serial.println(F("Co2 Failed"));
  }
  photocellReading = analogRead(photocellPin);
  sensors.requestTemperatures();
  float tempC = sensors.getTempC(insideThermometer);
  float tempF = DallasTemperature::toFahrenheit(tempC);
  float h = dht.readHumidity();
  delay(1000);
  float f = dht.readTemperature(true);
   if (isnan(h) || isnan(f)) {
    Serial.println(F("DHT Failed!"));
    return;
  }




  if(co2<=co2_start && photocellReading>500) {
      digitalWrite(LEDpin, LOW);
      Serial.println("SETTING PIN ON");
  }
  if(co2>=co2_stop || photocellReading<=500) {
      digitalWrite(LEDpin, HIGH);
      Serial.println("SETTING PIN OFF");
  }
  if(h>=maxHum || f>=maxTemp) {
    digitalWrite(LEDpin2, LOW);
  }



    

  if (client.connect(server, 80)) {
    client.print(F( "GET /add_data.php?"));
    client.print(F("fahrenheit="));
    client.print( f );
    client.print(F("&"));
    client.print(F("humidity="));
    client.print( h );
    client.print(F("&"));
    client.print(F("co2="));
    client.print( co2 );
    client.print(F("&"));
    client.print(F("light="));
    client.print( photocellReading );
    client.print(F("&"));
    client.print(F("temp2="));
    client.print( tempF );
    client.println(F( " HTTP/1.1"));
    client.print(F( "Host: " ));
    client.println(server);
    client.println(F( "Connection: close" ));
    client.println();
    client.println();
    client.stop();
    Serial.print(F("Temp2: "));
    Serial.println(tempF);
    Serial.print(F("Temperature: "));
    Serial.print(f);
    Serial.println(F(" *F\t"));
    Serial.print(F("Humidity: "));
    Serial.print(h);
    Serial.println(F(" %\t"));
    Serial.print(F("Co2Value: "));
    Serial.println(co2);
    Serial.print(F("Light reading = "));
    Serial.println(photocellReading);
    Serial.println("---------------------------------");
  }
  else {
    Serial.println(F("connection to Database failed"));
  }
  delay(interval); 
}

void printTemperature(DeviceAddress deviceAddress)
{
  float tempC = sensors.getTempC(deviceAddress);
  Serial.print(F("Temp C: "));
  Serial.print(tempC);
  Serial.print(F(" Temp F: "));
  Serial.println(DallasTemperature::toFahrenheit(tempC)); // Converts tempC to Fahrenheit
}

int readCO2()
{
 int co2_value = 0;
 Wire.beginTransmission(co2Addr);
 Wire.send(0x22);
 Wire.send(0x00);
 Wire.send(0x08);
 Wire.send(0x2A);
Wire.endTransmission();
 delay(10);
 Wire.requestFrom(co2Addr, 4);
 byte i = 0;
 byte buffer[4] = {0, 0, 0, 0};
 while(Wire.available())
 {
 buffer[i] = Wire.receive();
 i++;
 }
 co2_value = 0;
 co2_value |= buffer[1] & 0xFF;
 co2_value = co2_value << 8;
 co2_value |= buffer[2] & 0xFF;
 byte sum = 0; //Checksum Byte
 sum = buffer[0] + buffer[1] + buffer[2]; //Byte addition utilizes overflow
 if(sum == buffer[3])
 {
 // Success!
 return co2_value;
 }
 else
 {
 // Failure!
 return 0;
 }
}

