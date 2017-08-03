void sendMessage(char* serialMessage) {
  OSCMessage msg("/serial");
  msg.add(serialMessage);

  Udp.beginPacket(outIp, outPort);
  msg.send(Udp); // send the bytes to the SLIP stream
  Udp.endPacket(); // mark the end of the OSC Packet
  msg.empty(); // free space occupied by message
  delay(1);
}

void handleOSC() {
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
}
