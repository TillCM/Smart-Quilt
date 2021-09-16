#include <WiFiEsp.h>
#include <WiFiEspClient.h>
#include <WiFiEspServer.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <aREST.h>
#include <SPI.h>
#include <WiFiEspUdp.h>
#define RX 10
#define TX 11 
SoftwareSerial esp8266(RX,TX);


char ssid[] = "SmartQuilt"; //  your network SSID (name)
char pass[] = "CID3208till";    // your network password (use for WPA, or use as key for WEP)

int status = WL_IDLE_STATUS;
int temperature;
int humidity;
int val;
int sensorVal;
int touchPin=9;
int reqCount = 0; 



WiFiEspServer server(80);

RingBuffer buf(8);


void setup() 
{
  Serial.begin(9600);
  esp8266.begin(115200);

  // Init variables and expose them to REST API
  temperature = 24;
  humidity = 40;

    WiFi.init(&esp8266);    // initialize ESP module
   while (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    delay(1000);
  }

  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    while (true); // don't continue
  }

  Serial.print("Attempting to start AP ");
  Serial.println(ssid);

  // uncomment these two lines if you want to set the IP address of the AP
  IPAddress localIp(192, 168, 8, 1);
 WiFi.configAP(localIp);
  
  // start access point
  status = WiFi.beginAP(ssid, 10, pass, ENC_TYPE_WPA2_PSK);

  Serial.println("Access point started");
  printWifiStatus();
  
  // start the web server on port 80
  server.begin();
  Serial.println("Server started");
  
}

void loop() 
{
  sensorVal - getSensorData();
 WiFiEspClient client = server.available();  // listen for incoming clients

  if (client) {                               // if you get a client,
    Serial.println("New client");             // print a message out the serial port
    buf.init();                               // initialize the circular buffer
    while (client.connected()) {              // loop while the client's connected
      if (client.available()) {               // if there's bytes to read from the client,
        Serial.println("available");
        char c = client.read();               // read a byte, then
        buf.push(c);                          // push it to the ring buffer

        // you got two newline characters in a row
        // that's the end of the HTTP request, so send a response
        if (buf.endsWith("\r\n")) {
          delay(1000);
          sendHttpResponse(client);
          break;
        }
      }
    }
    
    // give the web browser time to receive the data
    delay(10);

    // close the connection
    client.stop();
    Serial.println("Client disconnected");
  }

  }  



int getSensorData(){
  val = digitalRead(touchPin); 
  if(val ==1){
     Serial.println("Touched");
  }
     return val;
}

void printWifiStatus()
{
  // print your WiFi shield's IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print where to go in the browser
  Serial.println();
  Serial.print("To see this page in action, connect to ");
  Serial.print(ssid);
  Serial.print(" and open a browser to http://");
  Serial.println(ip);
  Serial.println();
}

void sendHttpResponse(WiFiEspClient client)
{
  client.print(
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/json\r\n"
    "Connection: close\r\n"  // the connection will be closed after completion of the response
    "Refresh: 20\r\n"        // refresh the page automatically every 20 sec
    "\r\n");
  client.print("{\"name\": \"Hello from Quilt\"}");
  client.print(++reqCount);
  client.print(sensorVal);
  client.print("Analog input A0: ");
  client.print(analogRead(0));
 
}
     
