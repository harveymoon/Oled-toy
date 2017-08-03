
void routeSound(OSCMessage &msg, int addrOffset ) {
  Serial.print("do sound: ");
  //  SS2();
  int type =   msg.getInt(0);
  playSound(type);

}


void playSound(int index) {
  Serial.println(index);
  switch (index) {
    case 1:
      SS1();
      break;
    case 2:
      SS2();
      break;
    case 3:
      SS3();
      break;
    case 4:
      SS4();
      break;
    case 5:
      tickSound();
      break;
    case 6:
      tickSound2();
      break;
    case 7:
      churpSound2();
      break;
    case 8:
      churpSound3();
      break;
    case 9:
      chirp();
      break;
    case 10:
      meow();
      break;
    case 11:
      meow2();
      break;
    case 12:
      mew();
      break;
    case 13:
      ruff();
      break;
    case 14:
      arf();
      break;
  }
  //
  Serial.println("sound done");
  delay(10);

}



void SS1() {
  churpSound(1600, 2);
  delay(1);
  churpSound(2600, 2);
  delay(1);
  churpSound(3200, 2);
  delay(1);
  churpSound(4000, 2);
  delay(1);
  churpSound(1000, 5);
}

void SS2() {
  for (int i = 0; i < 10; i++)
  {
    churpSound(random(3000) + 2000, 2);
    delay(1);
  }
}

void SS3() {
  churpSound(4800, 1);
  delay(1);
  churpSound(4300, 1);
  delay(1);
  churpSound(4000, 1);
  delay(1);
  churpSound(3500, 1);
  delay(1);
  churpSound(3000, 1);
}

void SS4() {

  churpSound(3000, 1);
  delay(1);
  churpSound(3500, 1);
  delay(1);
  churpSound(4000, 1);
  delay(1);
  churpSound(4300, 1);
  delay(1);
  churpSound(4800, 1);
}

void tickSound() {
  digitalWrite(SPEAKERPIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1);
  digitalWrite(SPEAKERPIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1);
  digitalWrite(SPEAKERPIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(100);
  digitalWrite(SPEAKERPIN, LOW);    // turn the LED off by making the voltage LOW
}

void tickSound2() {
  digitalWrite(SPEAKERPIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1);
  digitalWrite(SPEAKERPIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1);

}

void churpSound(int inF, int num) {
  for (int i = 0; i < num; i++)
  {
    tone(SPEAKERPIN, inF, 30);
    delay(60);
  }
  noTone(SPEAKERPIN);
}

void churpSound2() {
  int crickets = random(1000);
  int chirp = 50;
  analogWrite(SPEAKERPIN, crickets);
  delayMicroseconds(chirp);
  analogWrite(SPEAKERPIN, 0);
  delayMicroseconds(chirp);
}

void churpSound3() {
  for (int i = 0; i < 20; i += 2)
  {
    delay(i);
    digitalWrite(SPEAKERPIN, LOW);
    delay(20 - i);
    digitalWrite(SPEAKERPIN, HIGH);
  }
}

void chirp() {  // Bird chirp
  for (uint8_t i = 200; i > 180; i--)
    playTone(i, 9);
}

void meow() {  // cat meow (emphasis ow "me")
  uint16_t i;
  playTone(5100, 50);       // "m" (short)
  playTone(394, 180);       // "eee" (long)
  for (i = 990; i < 1022; i += 2) // vary "ooo" down
    playTone(i, 8);
  playTone(5100, 40);       // "w" (short)
}

void meow2() {  // cat meow (emphasis on "ow")
  uint16_t i;
  playTone(5100, 55);      // "m" (short)
  playTone(394, 170);      // "eee" (long)
  delay(30);               // wait a tiny bit
  for (i = 330; i < 360; i += 2) // vary "ooo" down
    playTone(i, 10);
  playTone(5100, 40);      // "w" (short)
}

void mew() {  // cat mew
  uint16_t i;
  playTone(5100, 55);      // "m"   (short)
  playTone(394, 130);      // "eee" (long)
  playTone(384, 35);       // "eee" (up a tiny bit on end)
  playTone(5100, 40);      // "w"   (short)
}

void ruff() {   // dog ruff
  uint16_t i;
  for (i = 890; i < 910; i += 2) // "rrr"  (vary down)
    playTone(i, 3);
  playTone(1664, 150);        // "uuu" (hard to do)
  playTone(12200, 70);        // "ff"  (long, hard to do)
}

void arf() {    // dog arf
  uint16_t i;
  playTone(890, 25);         // "a"    (short)
  for (i = 890; i < 910; i += 2) // "rrr"  (vary down)
    playTone(i, 5);
  playTone(4545, 80);        // intermediate
  playTone(12200, 70);       // "ff"   (shorter, hard to do)
}


void routeTone(OSCMessage &msg, int addrOffset ) {
  Serial.print("do tone: ");
  int toneN =   msg.getInt(0);
  int toneLen =   msg.getInt(1);
  playTone(toneN, toneLen);
}

// play tone on a piezo speaker: tone shorter values produce higher frequencies
//  which is opposite beep() but avoids some math delay - similar to code by Erin Robotgrrl

void playTone(uint16_t tone1, uint16_t duration) {
  if (tone1 < 50 || tone1 > 15000) return; // these do not play on a piezo
  for (long i = 0; i < duration * 1000L; i += tone1 * 2) {
    digitalWrite(SPEAKERPIN, HIGH);
    delayMicroseconds(tone1);
    digitalWrite(SPEAKERPIN, LOW);
    delayMicroseconds(tone1);
  }
}

void routeNote(OSCMessage &msg, int addrOffset ) {
  Serial.println("do note: ");
  Serial.println(msg.getType(0));

  int lengthStr = msg.getDataLength(0);
  char strIn[lengthStr];
  msg.getString(0, strIn, lengthStr);

  char note =   strIn[0];
  int duration =   msg.getInt(1);
  playNote(note, duration);
}

void playNote(char note, int duration) {
  char names[] = { 'c', 'd', 'e', 'f', 'g', 'x', 'a', 'z', 'b', 'C', 'y', 'D', 'w', 'E', 'F', 'q', 'G', 'i' };
  // c=C4, C = C5. These values have been tuned.
  int tones[] = { 1898, 1690, 1500, 1420, 1265, 1194, 1126, 1063, 1001, 947, 893, 843, 795, 749, 710, 668, 630, 594 };

  // play the tone corresponding to the note name
  for (int i = 0; i < 18; i++) {
    if (names[i] == note) {
      playTone(tones[i], duration);
    }
  }
}

void routeBeep(OSCMessage &msg, int addrOffset ) {
  Serial.print("do beep: ");
  int freq =   msg.getInt(0);
  int toneMil =   msg.getInt(1);
  beep(freq, toneMil);
}

// another sound producing function similar to http://web.media.mit.edu/~leah/LilyPad/07_sound_code.html
void beep (int16_t frequencyInHertz, long timeInMilliseconds) {
  long x;
  long delayAmount = (long)(1000000 / frequencyInHertz);
  long loopTime = (long)((timeInMilliseconds * 1000) / (delayAmount * 2));
  for (x = 0; x < loopTime; x++) {
    digitalWrite(SPEAKERPIN, HIGH);
    delayMicroseconds(delayAmount);
    digitalWrite(SPEAKERPIN, LOW);
    delayMicroseconds(delayAmount);
  }
}




