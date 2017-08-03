#include <mem.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include <OSCBundle.h>

#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager

#include <FS.h> // filesystem 

#include <PubSubClient.h>  // mqtt control library

#include <Wire.h>  // Include Wire if you're using I2C
#include <SPI.h>  // Include SPI if you're using SPI
#include <SFE_MicroOLED.h>  // Include the SFE_MicroOLED library
#include <ArduinoJson.h>  // json messages

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

const char* DeviceName = "QQ";
const char* mqtt_server = "iot.unanything.com";

ESP8266WebServer server(80);

// a instance of UDP  to send and receive packets (udp, oviously!;)
WiFiUDP Udp;
IPAddress outIp(6, 6, 6, 6);    //ip address of the receiving host
const unsigned int outPort = 9000; //host port 
const unsigned int localPort = 9999; // local port to listen for UDP packets

//////////////////////////
// MicroOLED Definition //
//////////////////////////
#define PIN_RESET 255  // Connect RST to pin 9
#define PIN_DC    8  // Connect DC to pin 8
#define PIN_CS    10 // Connect CS to pin 10
#define DC_JUMPER 0

//////////////////////////////////
// MicroOLED Object Declaration //
//////////////////////////////////
MicroOLED oled(PIN_RESET, DC_JUMPER);    // I2C declaration

#define SPEAKERPIN D4 // Pin with the piezo speaker attached

#define DBG_OUTPUT_PORT Serial

const int buttonPin = D8;     // the number of the pushbutton pin

void setup() {
  pinMode(buttonPin, INPUT);
  ESP.wdtFeed();
  Serial.begin(115200);
  WiFiManager wifiManager;
  wifiManager.autoConnect(DeviceName);
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Udp.begin(localPort);
  Serial.println(Udp.localPort());
  ESP.wdtFeed();
  if (!MDNS.begin(DeviceName)) {
    // Serial.println("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }
  // Add service to MDNS-SD
  MDNS.addService("http", "tcp", 80);
  setupMqtt();

  SS4(); // a little sound to trigger good connection at this state

  oled.begin();    // Initialize the OLED
  flipScreen(false, true); // do something here if the screen is mounted upside down
oled_showIP();

  SPIFFS.begin();
  {
    Dir dir = SPIFFS.openDir("/");
    while (dir.next()) {
      String fileName = dir.fileName();
      size_t fileSize = dir.fileSize();
      DBG_OUTPUT_PORT.printf("FS File: %s, size: %s\n", fileName.c_str(), formatBytes(fileSize).c_str());
    }
    DBG_OUTPUT_PORT.printf("\n");
  }

  //SERVER INIT
  //list directory
  server.on("/list", HTTP_GET, handleFileList);

  //load editor
  server.on("/edit", HTTP_GET, []() {
    if (!handleFileRead("/edit.htm")) server.send(404, "text/plain", "FileNotFound");
  });
  //create file
  server.on("/edit", HTTP_PUT, handleFileCreate);
  //delete file
  server.on("/edit", HTTP_DELETE, handleFileDelete);
  //first callback is called after the request has ended with all parsed arguments
  //second callback handles file uploads at that location
  server.on("/edit", HTTP_POST, []() {
    server.send(200, "text/plain", "");
  }, handleFileUpload);

  server.on("/sendSecret", HTTP_GET, handleSecretMessage);
  server.on("/cmd", HTTP_GET, routeWebCmd);
  server.on("/drawbitmap",  HTTP_GET, []() {
    String Mtype = server.arg("type");
    server.send(200, "text/plain", "");
  });

  server.on("/info", HTTP_GET, []() {
    String json = "{";
    String str = "";
    for (int i = 0; i < 4; i++) {
      str += i  ? "." + String(WiFi.localIP()[i]) : String(WiFi.localIP()[i]);
    }
    json += "\"IP\":" + str;
    json += ", \"Hostname\":" + String(DeviceName) + ".local" ;
    json += ", \"OSCport\":" + String(localPort);
    json += "}";
    server.send(200, "text/json", json);
    json = String();
  });

  //called when the url is not defined here
  //use it to load content from SPIFFS
  server.onNotFound([]() {
    if (!handleFileRead(server.uri()))
      server.send(404, "text/plain", "FileNotFound");
  });

  //get heap status, analog input value and all GPIO statuses in one json call
  server.on("/all", HTTP_GET, []() {
    String json = "{";
    json += "\"heap\":" + String(ESP.getFreeHeap());
    json += ", \"analog\":" + String(analogRead(A0));
    json += ", \"gpio\":" + String((uint32_t)(((GPI | GPO) & 0xFFFF) | ((GP16I & 0x01) << 16)));
    json += "}";
    server.send(200, "text/json", json);
    json = String();
  });

  server.begin();
  DBG_OUTPUT_PORT.println("HTTP server started");

  //  getBTCDisplay();
  //  drawBender();

  drawBalloon();
}



int readline(int readch, char *buffer, int len)
{
  static int pos = 0;
  int rpos;

  if (readch > 0) {
    switch (readch) {
      case '\n': // Ignore new-lines
        break;
      case '\r': // Return on CR
        rpos = pos;
        pos = 0;  // Reset position index ready for next time
        return rpos;
      default:
        if (pos < len - 1) {
          buffer[pos++] = readch;
          buffer[pos] = 0;
        }
    }
  }
  // No end of line has been found, so return -1.
  return -1;
}

int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

void loop() {
  int reading = digitalRead(buttonPin);
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == HIGH) {
        String  outMessage = "{'m':'-', 'd':'heart'}"; // constructs a json string with message and dest
        char charBuf[ outMessage.length() ];
        outMessage.toCharArray(charBuf,  outMessage.length() + 5 );
        client.publish("postMessage", charBuf );
      }
    }
  }
  lastButtonState = reading;



  if (Serial.available()) {
    static char buffer[80];
    while (Serial.available()) {
      if (readline(Serial.read(), buffer, 80) > 0) {
        sendMessage(buffer);
      }
    }
  }

  ESP.wdtFeed();

  handleOSC();

  server.handleClient();
  mqttRun();
}
