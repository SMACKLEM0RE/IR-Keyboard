#include <HashMap.h>
#include <boarddefs.h>
#include <IRremote.h>
#include <IRremoteInt.h>
#include <ir_Lego_PF_BitStreamEncoder.h>
#include <IRremote.h>

// Macros for use of modifiers, and often used keys.
#define ENTER 0x28
#define WIN 0xE3
#define LEFT_SHIFT 0x02
#define LEFT_CTRL 0x01
#define LEFT_ALT 0x04
#define TAB 0x2B

//define the max size of the hashtable
const byte HASH_SIZE = 38;
//storage
HashType<char, int> hashRawArray[HASH_SIZE];
//handles the storage [search,retrieve,insert]
HashMap<char, int> hashMap = HashMap<char, int>(hashRawArray, HASH_SIZE);

// Keyboard report buffer
uint8_t buf[8] = {
    0}; 

// States to allow additional functionality for each program/subroutine
enum current_state
{
  netflix,
  chrome,
  xbox,
  base,
  logon
};

// Setup for Autologin functionality and IR functionality
int RECV_PIN = 11;
int onPin = 10;
IRrecv irrecv(RECV_PIN);
decode_results results;
irrecv.enableIRIn();
pinMode(onPin, OUTPUT);
current_state state = base; // change to logon for boot testing

// Hashmap to store HID values for characters used.
void setup()
{
  hashMap[0]('a', 4);
  hashMap[1]('b', 5);
  hashMap[2]('c', 6);
  hashMap[3]('d', 7);
  hashMap[4]('e', 8);
  hashMap[5]('f', 9);
  hashMap[6]('g', 10);
  hashMap[7]('h', 11);
  hashMap[8]('i', 12);
  hashMap[9]('j', 13);
  hashMap[10]('k', 14);
  hashMap[11]('l', 15);
  hashMap[12]('m', 16);
  hashMap[13]('n', 17);
  hashMap[14]('o', 18);
  hashMap[15]('p', 19);
  hashMap[16]('q', 20);
  hashMap[17]('r', 21);
  hashMap[18]('s', 22);
  hashMap[19]('t', 23);
  hashMap[20]('u', 24);
  hashMap[21]('v', 25);
  hashMap[22]('w', 26);
  hashMap[23]('x', 27);
  hashMap[24]('y', 28);
  hashMap[25]('z', 29);
  hashMap[26]('1', 30);
  hashMap[27]('2', 31);
  hashMap[28]('3', 32);
  hashMap[29]('4', 33);
  hashMap[30]('5', 34);
  hashMap[31]('6', 35);
  hashMap[32]('7', 36);
  hashMap[33]('8', 37);
  hashMap[34]('9', 38);
  hashMap[35]('0', 39);
  hashMap[36](' ', 44);
  hashMap[37]('.', 55);

  Serial.begin(9600);
}

void loop()
{
  // If IR signal recieved
  if (irrecv.decode(&results))
  {
    // Commands here override state control.
    if (results.value == 0xFD40BF)
    { // Func (Alt-F4)
      buf[0] = LEFT_ALT;
      buf[2] = 0x3D;
      Serial.write(buf, 8);
      releaseKey();
      state = base;
    }
    else if (results.value == 0xFD10EF)
    { // Down (Alt-Tab)
      buf[0] = LEFT_ALT + LEFT_SHIFT;
      buf[2] = 0x2B;
      Serial.write(buf, 8);
      releaseKey();
    }
    else if (results.value == 0xFD50AF)
    { // Up (Alt-Shift-Tab)
      buf[0] = LEFT_ALT;
      buf[2] = 0x2B;
      Serial.write(buf, 8);
      releaseKey();
    }
    //     Serial.println(results.value, HEX); // Outputs Signal recieved in "FD50AF" format
    else
    {
      // Pass signal recieved to current states handler
      switch (state)
      {
      case netflix:
        netflixHandler(results.value);
        break;
      case chrome:
        chromeHandler(results.value);
        break;
      case xbox:
        xboxHandler(results.value);
        break;
      case base:
        baseHandler(results.value);
        break;
      case logon:
        logonHandler(results.value);
        break;
      default:
        // if nothing else matches, do the default
        // default is optional
        break;
      }
    }
    // Resume loop waiting for input.
    irrecv.resume();
  }
}

// Type string argument out in sequence
void sendChars(const char *string)
{
  for (int i = 0; string[i] != '\0'; i++)
  {
    buf[2] = hashMap.getValueOf(string[i]);
    Serial.write(buf, 8);
    releaseKey();
  }
}

// Press Windows key (GUI Key) and then type string and press ENTER
void winRun(const char *string)
{
  buf[2] = WIN;
  Serial.write(buf, 8);
  releaseKey();
  delay(250);
  sendChars(string);
  buf[2] = ENTER;
  Serial.write(buf, 8);
  releaseKey();
}

// Clear buffer and send to signal no key being pressed.
void releaseKey()
{
  buf[0] = 0;
  buf[2] = 0;
  Serial.write(buf, 8); // Release key
}

// Sends signal to turn on computer, waits for boot and then logs in. 
void logonHandler(int ir_code)
{
  digitalWrite(onPin, HIGH);
  delay(100);
  digitalWrite(onPin, LOW);
  delay(5000); // Delay for long enough for computer to reach login screen
  buf[2] = ENTER;
  Serial.write(buf, 8);
  releaseKey();
  delay(200);
  sendChars("0000"); // Password or Pin example
  state = base;
}

// Base state handler for deciding which state to go into.
void baseHandler(int ir_code)
{ 
  switch (ir_code)
  {
  case 0xFD00FF: // Power
    state = logon;
    break;
  case 0xFD30CF: // 0
    break;
  case 0xFD08F7: // 1
    state = chrome;
    break;
  case 0xFD8877: // 2
    state = netflix;
    netflixHandler(1);
    break;
  case 0xFD48B7: // 3
    break;
  case 0xFD28D7: // 4
    break;
  case 0xFDA857: // 5
    break;
  case 0xFD6897: // 6
    break;
  case 0xFD18E7: // 7
    break;
  case 0xFD9867: // 8
    break;
  case 0xFD58A7: // 9
    break;
  case 0xFD807F: // Vol+
    break;
  case 0xFD906F: // Vol-
    break;
  case 0xFDB04F: // EQ
    break;
  case 0xFDA05F: // play/pause
    break;
  }
}

// Handle Chrome commands.
void chromeHandler(int ir_code)
{
  winRun("chrome");
  state = base;
}

// Handle xboxApp commands.
void xboxHandler(int ir_code)
{
  winRun("xbox");
  state = base;
}

// Handle Netflix through edge.
void netflixHandler(int ir_code)
{
  switch (ir_code)
  {
  case 1: // Run when coming from base state, open edge and go to netflix.com
    winRun("edge");
    delay(2000);
    sendChars("netflix");
    delay(500);
    buf[2] = ENTER;
    Serial.write(buf, 8);
    releaseKey();
    break;
  case 0xFD30CF: // 0 - Tab
    buf[2] = TAB;
    Serial.write(buf, 8);
    releaseKey();
    break;
  case 0xFD08F7: // 1 - Tab and login (in browser)
    for (int i = 0; i < 3; i++)
    {
      buf[2] = TAB;
      Serial.write(buf, 8);
      releaseKey();
      delay(200);
    }
    buf[2] = ENTER;
    Serial.write(buf, 8);
    releaseKey();
    break;

  case 0xFD8877: // 2 - Last Watched homepage different sometimes, will need to be changed
    for (int i = 0; i < 7; i++)
    {
      buf[2] = TAB;
      Serial.write(buf, 8);
      releaseKey();
      delay(100);
    }
    buf[2] = ENTER;
    Serial.write(buf, 8);
    releaseKey();
    delay(1000);
    buf[2] = TAB;
    Serial.write(buf, 8);
    releaseKey();
    delay(500);
    buf[2] = ENTER;
    Serial.write(buf, 8);
    releaseKey();
    delay(2000);
    buf[2] = 0x09;
    Serial.write(buf, 8);
    releaseKey();
    break;
  case 0xFDB04F: // EQ - Enter
    buf[2] = ENTER;
    Serial.write(buf, 8);
    releaseKey();
    break;
  case 0xFD708F: // ST - Shift Tab
    buf[0] = LEFT_SHIFT;
    buf[2] = TAB;
    Serial.write(buf, 8);
    releaseKey();
    break;
  case 0xFDA05F: // Play - Space
    buf[2] = 0x2C;
    Serial.write(buf, 8);
    releaseKey();
    break;
  case 0xFD906F: // Vol-
    buf[2] = 0x51;
    Serial.write(buf, 8);
    releaseKey();
    break;
  case 0xFD807F: // Vol+
    buf[2] = 0x52;
    Serial.write(buf, 8);
    releaseKey();
    break;
  case 0xFD20DF: // Rew
    buf[0] = LEFT_SHIFT;
    buf[2] = 0x50;
    Serial.write(buf, 8);
    releaseKey();
    break;
  case 0xFD609F: // For
    buf[0] = LEFT_SHIFT;
    buf[2] = 0x4F;
    Serial.write(buf, 8);
    releaseKey();
    break;
  case 0xFD48B7: // 3 - F
    buf[2] = 0x09;
    Serial.write(buf, 8);
    releaseKey();
    break;
  }
}