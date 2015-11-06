#include <SPI.h>
#include <UIPEthernet.h>
#include <Wire.h>
#include "DHT.h"

#define DHTPIN 2     // what digital pin we're connected to
#define DHTTYPE DHT11   // DHT 11

EthernetClient client;
IPAddress ip(192,168,1,66);
signed long next;
int interval = 10000; 
char server[] = "192.168.1.30";
int photocellPin = 14;     // the cell and 10K pulldown are connected to a0
int photocellReading;     // the analog reading from the sensor divider
//int LEDpin = 11;          // connect Red LED to pin 11 (PWM pin)
//int LEDbrightness;        // 
int co2Addr = 0x68; // This is the default address of the CO2 sensor, 7bits shifted left.
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  Serial.print(F("This is a test to see if the setup block prints to serial..  "));
  while (!Serial) {
    ; 
  }
  Wire.begin ();
  pinMode(13, OUTPUT);
  //pinMode(LEDpin, OUTPUT);
  dht.begin();
  
  uint8_t mac[6] = {0x00,0x01,0x02,0x03,0x04,0x05};
  Ethernet.begin(mac, ip);
  Serial.print(F("localIP: "));
  Serial.println(Ethernet.localIP());
  Serial.print(F("subnetMask: "));
  Serial.println(Ethernet.subnetMask());
  Serial.print(F("gatewayIP: "));
  Serial.println(Ethernet.gatewayIP());
  Serial.print(F("dnsServerIP: "));
  Serial.println(Ethernet.dnsServerIP());
  next = 0;
}

int readCO2()
{
 int co2_value = 0;
 // We will store the CO2 value inside this variable.
 digitalWrite(13, HIGH);

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
 digitalWrite(13, LOW);
 return co2_value;
 }
 else
 {
 // Failure!
 
 digitalWrite(13, LOW);
 return 0;
 }
}


void loop() {
  
  int co2Value = readCO2();
  if(co2Value > 0)
  {
  //Serial.print("CO2 Value: ");
  //Serial.println(co2Value);
  }
  else
  {
  Serial.println(F("Checksum failed / Communication failure"));
  }
  delay(500);
  
  photocellReading = analogRead(photocellPin);  
  //digitalWrite(LEDpin, LOW);
  delay(500);
  float h = dht.readHumidity();
  float f = dht.readTemperature(true);
  delay(500);
  if (isnan(h) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  //digitalWrite(LEDpin, HIGH);
  delay(100);
  if (client.connect(server, 80)) {
    client.print(F( "GET /add_data.php?"));
    client.print(F("fahrenheit="));
    client.print( f );
    client.print(F("&"));
    client.print(F("humidity="));
    client.print( h );
    client.print(F("&"));
    client.print(F("co2="));
    client.print( co2Value );
    client.print(F("&"));
    client.print(F("light="));
    client.print( photocellReading );
    client.println(F( " HTTP/1.1"));
    client.print(F( "Host: " ));
    client.println(server);
    client.println(F( "Connection: close" ));
    client.println();
    client.println();
    client.stop();
    Serial.print(F("Temperature: "));
    Serial.print(f);
    Serial.println(F(" *F\t"));
    Serial.print(F("Humidity: "));
    Serial.print(h);
    Serial.println(F(" %\t"));
    Serial.print(F("Co2Value: "));
    Serial.println(co2Value);
    Serial.print(F("Light reading = "));
    Serial.println(photocellReading);
    Serial.println("---------------------------------");
  }
  else {
    Serial.println(F("connection failed"));
  }
  delay(interval);
  
}

