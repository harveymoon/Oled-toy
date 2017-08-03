
// API server

void routeCrypto(OSCMessage &msg, int addrOffset ) {
  getBTCDisplay();
}

// Previous Bitcoin value & threshold
float previousValue = 0.0;
float threshold = 0.05;

String getURL(const char* host, int httpPort) {
  // Use WiFiClient class to create TCP connections
  WiFiClient client;

  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return "";
  }
  // We now create a URI for the request
  String url = "/v1/ticker/?limit=2";
  // String url = "/stan";

  Serial.print("Requesting URL: ");
  Serial.println(host + url);
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return "";
    }
  }
  String answer;
  // Read all the lines of the reply from server and print them to Serial
  while (client.available()) {
    String line = client.readStringUntil('\r');
    /// Serial.print(line);
    answer += line;
  }
  //Serial.println(answer);
  return answer;
}

void getBTCDisplay() {

  oled.clear(ALL); // Clear the display's internal memory
  oled.clear(PAGE); // Clear the buffer.
  //const char* host = "api.coindesk.com";
  const char* host = "api.coinmarketcap.com";
  const int httpPort = 80;
  String answer = getURL(host, httpPort);
  // Convert to JSON
  String jsonAnswer;
  int jsonIndexStart;
  int jsonIndexEnd;
  for (int i = 0; i < answer.length(); i++) {
    if (answer[i] == '[') {
      jsonIndexStart = i;
      break;
    }
  }

  for (int i = 0; i < answer.length(); i++) {
    if (answer[i] == ']') {
      jsonIndexEnd = i;
      break;
    }
  }

  //
  // Get JSON data
  jsonAnswer = answer.substring(jsonIndexStart, jsonIndexEnd + 1);
  jsonAnswer.trim();
  //jsonAnswer = "{ \"items\" : " + jsonAnswer + " }";
  jsonAnswer.replace('\n', ' ');
  jsonAnswer.replace(" ", "");
  Serial.println();
  Serial.println("JSON answer: ");
  Serial.print('*');
  Serial.print(jsonAnswer);
  Serial.println('*');
  Serial.println(jsonAnswer.length());
  char jsonA[900];
  jsonAnswer.toCharArray(jsonA, 900);
  Serial.print('*');
  Serial.print(jsonA);
  Serial.println('*');
  StaticJsonBuffer<800> jsonBuffer;
  JsonArray& root = jsonBuffer.parseArray(jsonA);
  if (!root.success())
  {
    Serial.println("parseObject() failed");
    delay(2000);
    //return;
  }
  const  char* sym1 = root[0]["symbol"];
  float price1 = root[0]["price_usd"];
  float change1 = root[0]["percent_change_1h"];
  Serial.println(sym1);
  Serial.println(price1);
  Serial.println(change1);
  const char* sym2 = root[1]["symbol"];
  float price2 = root[1]["price_usd"];
  float change2 = root[1]["percent_change_1h"];
  Serial.println(sym2);
  Serial.println(price2);
  Serial.println(change2);
  oled.setCursor(0, 0);
  oled.setFontType(0);  // Set font to type 1
  oled.print(sym1);
  oled.print('\n');

  oled.setFontType(1);  // Set font to type 1
  oled.print(price1);

  int x0 = 25; int y0 = 1;   //
  int x1 = 36; int y1 = 5; //

  if (change1 > 0) {
    oled.line(x0, y0, x1, y1);
  } else {
    oled.line(x0, y1, x1, y0);
  }
  oled.setFontType(0);  // Set font to type 1
  oled.print(sym2);
  oled.print('\n');

  oled.setFontType(1);  // Set font to type 1
  oled.print(price2);

  y0 += 24;
  y1 += 24;

  if (change2 > 0) {
    oled.line(x0, y0, x1, y1);  // Draw a line from (x0,y0) to (x1,y1);
  } else {
    oled.line(x0, y1, x1, y0);  // Draw a line from (x0,y0) to (x1,y1);
  }

  oled.display();       // Refresh the display

}
