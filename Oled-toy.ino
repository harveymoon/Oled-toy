#include <mem.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include <OSCBundle.h>

#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager

#include <PubSubClient.h>

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

const char* DeviceName = "QQ";
//"ESP_toy_01";
const char* mqtt_server = "iot.unanything.com";
long lastReconnectAttempt = 0;
long buttonTimer = 0;


//WiFiServer server(80);
char SerialContent[30];

ESP8266WebServer server(80);

// a instance of UDP  to send and receive packets (udp, oviously!;)
WiFiUDP Udp;
IPAddress outIp(6, 6, 6, 6);    //ip address of the receiving host
//IPAddress outIp(192,168,1,26);
const unsigned int outPort = 9000;              //host port
const unsigned int localPort = 9999;        // local port to listen for UDP packets (here's where we send the packets)

#include <Wire.h>  // Include Wire if you're using I2C
#include <SPI.h>  // Include SPI if you're using SPI
#include <SFE_MicroOLED.h>  // Include the SFE_MicroOLED library

#include <ArduinoJson.h>

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
//MicroOLED oled(PIN_RESET, PIN_DC, PIN_CS); // SPI declaration
MicroOLED oled(PIN_RESET, DC_JUMPER);    // I2C declaration

#define SPEAKERPIN        D4 // Pin with the piezo speaker attached

#include <FS.h>
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

  SS4();

  oled.begin();    // Initialize the OLED

  oled.clear(ALL); // Clear the display's internal memory
  oled.clear(PAGE); // Clear the buffer.
  flipScreen(false, true); 
  
  oled.setFontType(0);
  oled.setCursor(0, 0);
  String str = "";
  for (int i = 0; i < 4; i++)
    str += i  ? "." + String(WiFi.localIP()[i]) : String(WiFi.localIP()[i]);
  str += ":";
  str += localPort;

  oled.print(str);
  oled.setCursor(0, 20);
  oled.print(String(DeviceName) + ".local");
  oled.display();
  delay(100);
  oled.display();  // Display what's in the buffer (splashscreen)
  delay(5000);

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


  delay(500);
  ESP.wdtFeed();
  delay(500);
  ESP.wdtFeed();
  delay(500);
  ESP.wdtFeed();
  delay(500);
  ESP.wdtFeed();
  delay(500);
  ESP.wdtFeed();
  delay(500);
  ESP.wdtFeed();
  delay(500);
  ESP.wdtFeed();
  delay(500);
  ESP.wdtFeed();
  delay(500);
  ESP.wdtFeed();
  delay(500);
  ESP.wdtFeed();
  delay(500);
  ESP.wdtFeed();
  delay(500);
  ESP.wdtFeed();


  //  getBTCDisplay();
  //  drawBender();

  drawBalloon();

  //  slowText("this is what slow text looks like when you use it to type");
}


void sendMessage(char* serialMessage) {
  OSCMessage msg("/serial");
  msg.add(serialMessage);

  Udp.beginPacket(outIp, outPort);
  msg.send(Udp); // send the bytes to the SLIP stream
  Udp.endPacket(); // mark the end of the OSC Packet
  msg.empty(); // free space occupied by message
  delay(1);
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


void routeCommand(OSCMessage &msg, int addrOffset ) {
  char boo[25];

  msg.getString(0, boo, 25);

  Serial.println(boo);

  delay(10);
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
        Serial.println("sending a boop");
        String  outMessage = "{'m':'-', 'd':'heart'}";
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
  OSCMessage msgIN;
  int size;
  if ( (size = Udp.parsePacket()) > 0)
  {
    while (size--)
      msgIN.fill(Udp.read());
    if (!msgIN.hasError()) {

      msgIN.route("/pixel", routePixel);
      msgIN.route("/clear", routeClear);
      msgIN.route("/line", routeLine);
      msgIN.route("/invert", routeInvert);
      msgIN.route("/rect", routeRect);
      msgIN.route("/circle", routeCircle);
      msgIN.route("/text", routeText);
      msgIN.route("/scroll", routeScroll);
      msgIN.route("/scrollStop", routeScrollStop);
      msgIN.route("/flip", routeFlip);
      msgIN.route("/countdown", routeCountdown);
      msgIN.route("/sound", routeSound);
      msgIN.route("/tone", routeTone);
      msgIN.route("/beep", routeBeep);
      msgIN.route("/note", routeNote);

      msgIN.route("/crypto", routeCrypto);

      msgIN.route("/bender", routeBender);
      msgIN.route("/heart", routeHeart);
      msgIN.route("/balloon", routeBalloon);
      msgIN.route("/cat", routeCat);

    }
    outIp = Udp.remoteIP();
  }


  server.handleClient();

  mqttRun();

}
