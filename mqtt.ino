
void setupMqtt() {
  client.setServer(mqtt_server, 5556);
  client.setCallback(callback);
}

int strcicmp(char const *a, char const *b)
{
  for (;; a++, b++) {
    int d = tolower(*a) - tolower(*b);
    if (d != 0 || !*a)
      return d;
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  ESP.wdtFeed();
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String s = "";
  for (int i = 0; i < length; i++) {
    //    Serial.print((char)payload[i]);
    s += (char)payload[i];
  }
  Serial.println(topic);
  Serial.println(s);

  if ( strcicmp(topic, DeviceName ) == 0 ) {
    meow2(); // extra happy

  } else if ( strcicmp(topic, "heart" ) == 0) {
    ESP.wdtFeed();
    drawHeart();
    oled.invert(true);
    SS4();
    delay(250);
    oled.invert(false);
    drawMoon();
    delay(250);
    SS4();
    delay(250);
    oled.invert(true);
    drawBalloon();
    delay(250);
    SS4();
    delay(250);
    oled.invert(false);

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

    oled.clear(PAGE);     // Clear the screen
    oled.display();
    delay(10);
    ESP.wdtFeed();
  }
  else {
    churpSound3();// msg in
  }

  oled.invert(true);
  delay(100);
  oled.invert(false);
  slowText(s);
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(DeviceName)) {
      Serial.println("connected");
      // Once connected, publish an announcement...

      // ... and resubscribe
      client.subscribe("heart");


      client.subscribe(DeviceName);
      client.subscribe("GroupChat");

    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void handleSecretMessage() {
  //  Serial.println("def handle secret");
  if (!server.hasArg("payload")) {
    server.send(500, "text/plain", "BAD ARGS");
    return;
  }
  String pMess = server.arg("payload");
  Serial.println("gotMessage :" + pMess  );
  server.send(200, "text/json", "[\"status\":\"sent\"]");
  String DestUsr = "*";

  if (server.hasArg("dst")) {
    DestUsr = server.arg("dst");
  }

  char charBuf[ pMess.length() ];
  pMess.toCharArray(charBuf, pMess.length());

  char destBuf[ DestUsr.length() ];
  DestUsr.toCharArray(destBuf, DestUsr.length());

  client.publish("postMessage",  charBuf);

  //  reconnect(); // mqtt reconnnector
  if (client.connect("User01")) {
    String  outMessage = "{'m':'" + pMess + "', 'd':'" + DestUsr + "'}";
    char charBuf[ outMessage.length() ];
    outMessage.toCharArray(charBuf,  outMessage.length() + 5 );
    client.publish("postMessage", charBuf );
  }
  //
}


void routeWebCmd() {
  if (!server.hasArg("type")) {
    server.send(500, "text/plain", "BAD ARGS");
    return;
  }
  String Mtype = server.arg("type");

  if ( Mtype.equals("pixel" )) {
    //local/cmd?type=pixel&x=5&y=35
    int x = server.arg("x").toInt();
    int y = server.arg("y").toInt();
    bool c = true;
    if (server.hasArg("c")) {
      c = (server.arg("c").toInt() == 1);
    }
    drawPixel(x, y, c);
    //
    server.send(200, "text/json", "[\"stat\":\"ok\"]");
  } else if ( Mtype.equals("clear")) {
    //    local/cmd?type=clear
    Serial.println("Clear");
    oled.clear(PAGE);     // Clear the screen
    oled.display();
    delay(10);
    server.send(200, "text/json", "[\"stat\":\"ok\"]");
  } else if ( Mtype.equals("line")) {
    //    .local/cmd?type=line&xy=5,45&xy2=50,20

    bool c = true;
    if (server.hasArg("c")) {
      c = (server.arg("c").toInt() == 1);
    }
    char * strtokIndx; // this is used by strtok() as an index
    char charBuf[50];
    server.arg("xy").toCharArray(charBuf, 50);
    strtokIndx = strtok(charBuf, ",");     // get the first part - the string
    int XX = atoi(strtokIndx); //
    strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
    int YY = atoi(strtokIndx);     //
    memset(charBuf, 0, 50);

    server.arg("xy2").toCharArray(charBuf, 50);
    strtokIndx = strtok(charBuf, ",");     // get the first part - the string
    int XX2 = atoi(strtokIndx); //
    strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
    int YY2 = atoi(strtokIndx);     //

    drawLineAt(XX, YY, XX2, YY2, c);
    server.send(200, "text/json", "[\"stat\":\"ok\"]");
  } else if (Mtype.equals("invert")) {
    oled.invert(server.arg("val").equals("1"));

    //  } else if ( Mtype.equals("rect")) {
    //    bool c = true;
    //    if (server.hasArg("c")) {
    //      c = (server.arg("c").toInt() == 1);
    //    }
    //    bool filled = true;
    //    if (server.hasArg("filled")) {
    //      filled = (server.arg("filled").toInt() == 1);
    //    }
    //    char * strtokIndx; // this is used by strtok() as an index
    //    char charBuf[50];
    //    server.arg("xy").toCharArray(charBuf, 50);
    //    strtokIndx = strtok(charBuf, ",");     // get the first part - the string
    //    int XX = atoi(strtokIndx); //
    //    strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
    //    int YY = atoi(strtokIndx);     //
    //    memset(charBuf, 0, 50);
    //
    //    server.arg("wh").toCharArray(charBuf, 50);
    //    strtokIndx = strtok(charBuf, ",");     // get the first part - the string
    //    int WW = atoi(strtokIndx); //
    //    strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
    //    int HH = atoi(strtokIndx);     //
    //    Serial.println("drawRect");

    //    drawRectAt(XX, YY, WW, HH, c, filled);
    //} else if ( Mtype.equals("circle")) {
    
    server.send(200, "text/json", "[\"stat\":\"ok\"]");
  } else if ( Mtype.equals("text")) {
    ///cmd?type=text&text=booooop&font=1
    int font = 0;
    if (server.hasArg("font")) {
      font = server.arg("font").toInt();
    }
    printText( server.arg("text"), font );
    server.send(200, "text/json", "[\"stat\":\"ok\"]");
  } else if ( Mtype.equals("flip")) {
    ///cmd?type=flip&H=1&V=1
    boolean Hval = server.arg("H").equals("0");
    boolean Vval = server.arg("V").equals("0");
    flipScreen(Hval, Vval);
    server.send(200, "text/json", "[\"stat\":\"ok\"]");
  } else if (  Mtype.equals("countdown")) {
    ///cmd?type=countdown&seconds=10
    doCountdown(server.arg("seconds").toInt());
    server.send(200, "text/json", "[\"stat\":\"ok\"]");
  } else  if ( Mtype.equals("sound")) {
    //.local/cmd?type=sound&index=3
    playSound(server.arg("index").toInt());
    server.send(200, "text/json", "[\"stat\":\"ok\"]");
  } else if (  Mtype.equals("tone") ) {
    ///cmd?type=tone&tone=1000&duration=1000
    int toneN =   server.arg("tone").toInt();
    int toneMil =   server.arg("duration").toInt();
    playTone(toneN, toneMil);
    server.send(200, "text/json", "[\" tone\":\"sent\"]");
  } else if ( Mtype.equals("note") ) {
    // looks like /cmd?type=note&note=C&duration=1000
    char note = (char)server.arg("note")[0];
    int duration = server.arg("duration").toInt();
    playNote(note, duration);
    server.send(200, "text/json", "[\"note\":\"sent\"]");
  } else if ( Mtype.equals("crypto")) {
    // looks like .local/cmd?type=crypto
    getBTCDisplay();
    server.send(200, "text/json", "[\"btc\":\"view\"]");
  }
}

void  mqttRun() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}






