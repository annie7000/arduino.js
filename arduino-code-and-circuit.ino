/*----------------------------------------------------------------*/
// NOTES
// will need to modify to eventually accept on 
// and off signal from mobile app
//
/*------------------------------------------*/
// CIRCUIT 
/*------LCD Display--------*/
// - LCD D4 to digital pin 26
// - LCD D5 to digital pin 28
// - LCD D6 to digital pin 30
// - LCD D7 to digital pin 32
// - LCD A to 5v input (with a 220ohm resistor)
// - LCD K to ground
// - LCD E to digital pin 24
// - LCD RW to ground
// - LCD RS to digital pin 22
// - LCD VO to output of potentiometer (this will control the brightness of the display) with VDD and VSS to input
// - LCD VDD to 5v input
// - LCD VSS to ground
// - button connected to pin 34 and ground (w/ INPUT_PULLUP on the pin)
//
/*--------------Relays-------------*/
// just connect the big strip to GND, VCC, and output pins 5-12
//
/*-------ESP8266 WiFi Module-------*/
//Arduino Mega + ESP8266-01 using HTTP GET/POST requests to a node.js server 
//
// - ESP8266 3.3V to Arduino 3.3V (with 100uF capacitor)
// - ESP8266 EN to 3.3V via 10k resistor
// - ESP8266 RST to 3.3V via 10k resistor  
// - ESP8266 TX to Arduino 15 (RX3)
// - ESP8266 RX to voltage divider: Arduino 14 (TX3) -> 1k -> ESP8266 RX, and 2.2k from ESP8266 RX to GND
// - ESP8266 GND to Arduino GND
// - GPIO0 and GPIO2: leave floating (internal pull-up)
//
// notes 
//   - esp8266 can't directly connect and communicate to firebase, 
//     so set up inbetween server w/ node.js (run locally now eventually run server via cloud)
//
/*------AC voltage sensor module ZMPT101B-------*/
// - sensor GND to GND
// - sensor VCC to 5V
// - sensor OUT to Analog in pins (A0-A2)

/*---------------------------------------------------------------*/

  #include <LiquidCrystal.h>
  #include <Wire.h>
  #define ESP8266 Serial3

// modify this portion before running code (for wifi connection)

  String SSID = ""; // wifi ssid
  String PASS = ""; // wifi password
  String HOST = "";  // your PC IP if the server to ran locally

  bool wifiConnected = false;

  LiquidCrystal lcd_1(22, 24, 26, 28, 30, 32); // the pins the LCD is connected to

  int seconds = 0; // button/menu setup for LCD display to switch through different sensor readings, 3 in total
  int buttonPin = 34;
  int buttonState;
  int lastbuttonState = HIGH;
  int menu_num = 0;
  int lastMenu_num = -1;

  unsigned long lastDebounceTime= 0;
  const long debounceDelay = 50;

  String message = "";
  char c_data;

  int relayPins[8] = {5,6,7,8,9,10,11,12};

void setup() { 
  /*-------------------------------RELAY SETUP-----------------------------------*/ 

  Serial.begin(115200); // output pin setup for relay control, connect relay to these pins

  for (int i = 0; i < 8; i++) {
    pinMode(relayPins[i], OUTPUT);
    digitalWrite(relayPins[i], HIGH); // default ON for all
  }
  /*-------------------------------LCD SETUP-----------------------------------*/ 

  lcd_1.begin(16, 2); // sets up the amount of columns and rows on the display
  pinMode(buttonPin, INPUT_PULLUP);

  lcd_1.print("LCD ready");
  delay(1000); // display "LCD ready" for 1 sec
  lcd_1.clear();

  lcd_1.print("Press button to");
  lcd_1.setCursor(0, 1);
  lcd_1.print("change menu");
  delay(2000); 
  lcd_1.clear();

  /*----------------------------ESP8266 WIFI SETUP-------------------------------*/ 

  ESP8266.begin(9600);

  delay(2000);

  sendAT("AT", 2000);
  sendAT("AT+RST", 3000);
  sendAT("AT+CWMODE=1", 2000);

} 

void loop() {

  /*-------------------------------RELAY CODE-----------------------------------*/ 

  String response = getCommands(); // GETS CMD FROM NODE.JS SERVER
  applyCommands(response);
  delay(2500);

/* OLD TESTER RELAY CMD FOR WORKING RELAY WITH SERIAL MONITOR 


  if (Serial.available() > 0) { 

    c_data = Serial.read();

    if (c_data != '\n') { 

      message = message + c_data;

    } else { // when an input to entered in from serial monitor, will have to eventally take input from the app

      if (message == "ON 1") {

        digitalWrite(5, LOW); // Sending 0 volts ot pin 5
        Serial.println("relay to pin 5 is on"); 
        message = "";

      } else if (message == "OFF 1") {

        digitalWrite(5, HIGH);
        Serial.println("relay to pin 5 off"); 
        message = "";

      } else if (message == "ON 2") { // signal to pin 6

        digitalWrite(6, LOW);
        Serial.println("relay to pin 6 is on"); 
        message = ""; 

      } else if (message == "OFF 2") {

        digitalWrite(6, HIGH);
        Serial.println("relay to pin 6 off");
        message = "";

      } else if (message == "ON 3") { // signal to pin 7

        digitalWrite(7, LOW);
        Serial.println("relay to pin 7 is on"); 
        message = ""; 

      } else if (message == "OFF 3") {

        digitalWrite(7, HIGH);
        Serial.println("relay to pin 7 off");
        message = "";

      } else if (message == "ON 4") { // signal to pin 8

        digitalWrite(8, LOW);
        Serial.println("relay to pin 8 is on"); 
        message = ""; 

      } else if (message == "OFF 4") {

        digitalWrite(8, HIGH);
        Serial.println("relay to pin 8 off");
        message = "";

      } else if (message == "ON 5") { // signal to pin 9

        digitalWrite(9, LOW);
        Serial.println("relay to pin 9 is on"); 
        message = ""; 

      } else if (message == "OFF 5") {

        digitalWrite(9, HIGH);
        Serial.println("relay to pin 9 off");
        message = "";

      } else if (message == "ON 6") { // signal to pin 10

        digitalWrite(10, LOW);
        Serial.println("relay to pin 10 is on"); 
        message = ""; 

      } else if (message == "OFF 6") {

        digitalWrite(10, HIGH);
        Serial.println("relay to pin 10 off");
        message = "";

      } else if (message == "ON 7") { // signal to pin 11

        digitalWrite(11, LOW);
        Serial.println("relay to pin 11 is on"); 
        message = ""; 

      } else if (message == "OFF 7") {

        digitalWrite(11, HIGH);
        Serial.println("relay to pin 11 off");
        message = "";

      } else if (message == "ON 8") { // signal to pin 12

        digitalWrite(12, LOW);
        Serial.println("relay to pin 12 is on"); 
        message = ""; 

      } else if (message == "OFF 8") {

        digitalWrite(12, HIGH);
        Serial.println("relay to pin 12 off");
        message = "";

      } else {

        Serial.println("Send ON or OFF and a number 1-8");
        message = ""; 

      }
    }
  }
*/ 
 /*---------------------------ESP8266 WIFI CONNECTION-----------------------------*/ 

  if (!wifiConnected) {
    wifiConnected = connectWiFi();

    if (wifiConnected){
      Serial.println("WiFi connected!");

    } else if (!wifiConnected) {
      Serial.println("WiFi failed");
      delay(5000);
      return;
    }
  }

 /*-------------------------------SENSOR CODE------------------------------------*/ 

  long sum1 = 0, sum2 = 0, sum3 = 0;
  int samples = 500;

  for (int i = 0; i < samples; i++) {
    int value1 = analogRead(A0) - 512; // account for offset form voltage sensors
    sum1 += value1 * value1;

    int value2 = analogRead(A1) - 512; 
    sum2 += value2 * value2;

    int value3 = analogRead(A2) - 512;
    sum3 += value3 * value3;
  }

  float rms1 = sqrt(sum1 / samples);
  float rms2 = sqrt(sum2 / samples);
  float rms3 = sqrt(sum3 / samples);
  
  float voltage1 = rms1 * (5.0 / 1023.0) * 100.0;
  float voltage2 = rms2 * (5.0 / 1023.0) * 100.0;;
  float voltage3 = rms3 * (5.0 / 1023.0) * 100.0;; 

  String data = "voltage1=" + String(voltage1);
  data += "&voltage2=" + String(voltage2);
  data += "&voltage3=" + String(voltage3);

  sendHTTP(data);

  delay(2000);

 /*---------------------------------LCD CODE-------------------------------------*/ 

  buttonState = digitalRead(buttonPin);

  if (buttonState != lastbuttonState){

    lastDebounceTime = millis();

  }
  
    if ((millis() - lastDebounceTime) > debounceDelay) {
    if (buttonState == LOW) { // Button pressed
      
      Serial.print("Button Pressed, yay");

      menu_num++;

      if (menu_num >= 4) {

        menu_num = 0;

      }
      
      lcd_1.clear();
      
      lcd_1.setCursor(0, 0);
      lcd_1.print("Menu ");
      lcd_1.print(menu_num + 1);
      delay(200);
      lcd_1.clear();
    }
  }

  lastbuttonState = buttonState;

  if (menu_num == 0){ // for voltage sensor 1 display
  
    lcd_1.setCursor(0, 0);
    lcd_1.print("S1: ");
    lcd_1.print(voltage1, 2); // 2 decimal points
    lcd_1.print("v  "); // voltage at the end

  } else if (menu_num == 1){ // for sensor 2 display

    lcd_1.setCursor(0, 0);
    lcd_1.print("S2: ");
    lcd_1.print(voltage2, 2);
    lcd_1.print("v  ");

  } else if (menu_num == 2){ // for sensor 3 display

    lcd_1.setCursor(0, 0);
    lcd_1.print("S3: ");
    lcd_1.print(voltage3, 2);
    lcd_1.print("v  ");

  } else if (menu_num == 3){ // total sensors combined

    lcd_1.setCursor(0, 0);
    lcd_1.print("Total: ");
    lcd_1.print(voltage1 + voltage2 + voltage3, 2);
    lcd_1.print("v  ");

  }
  
  delay(200); // rate screen refreshes at

}

 /*-----------------------ESP8266 WIFI CONNECTION HELPER FUNCTIONS---------------------------*/ 

void sendHTTP(String data) {
  sendAT("AT+CIPMUX=0", 2000);
  Serial.println("Sending data:");
  Serial.println(data);

  ESP8266.println("AT+CIPSTART=\"TCP\",\"" + HOST + "\",3000");
  delay(2000);

  String req = "POST /api/data HTTP/1.1\r\n";
  req += "Host: " + HOST + "\r\n";
  req += "Content-Type: application/x-www-form-urlencoded\r\n";
  req += "Content-Length: " + String(data.length()) + "\r\n";
  req += "Connection: close\r\n\r\n";
  req += data;

  ESP8266.println("AT+CIPSEND=" + String(req.length()));

  long start = millis();
  while (millis() - start < 5000) {
    if (ESP8266.find(">")) break;
  }

  ESP8266.print(req);

  delay(3000);

  while (ESP8266.available()) {
    Serial.write(ESP8266.read());
  }

  ESP8266.println("AT+CIPCLOSE");
}

bool connectWiFi() { 
  
  Serial.println("Connecting to WiFi..."); 
  String cmd = "AT+CWJAP=\"" + SSID + "\",\"" + PASS + "\"";
  ESP8266.println(cmd); long start = millis(); 

  while (millis() - start < 15000) { 

    if (ESP8266.available()) { 
      String res = ESP8266.readString(); 
      Serial.println(res); 

      if (res.indexOf("WIFI CONNECTED") != -1 || res.indexOf("OK") != -1) { 
        return true; 
        } 
      } 
    } 
    return false; 
  }

void sendAT(String cmd, int timeout) { 
  
  ESP8266.println(cmd); long start = millis(); 

  while (millis() - start < timeout) { 
    while (ESP8266.available()) { 

      Serial.write(ESP8266.read()); 
    } 
  } 
}

/*--------------------RELAY STATE HELPER FUNC---------------------*/

String getCommands() {

  ESP8266.println("AT+CIPSTART=\"TCP\",\"" + HOST + "\",3000");
  delay(2000);

  String req = "GET /api/commands HTTP/1.1\r\n";
  req += "Host: " + HOST + "\r\n";
  req += "Connection: close\r\n\r\n";

  ESP8266.println("AT+CIPSEND=" + String(req.length()));

  if (ESP8266.find(">")) {
    ESP8266.print(req);
  }

  delay(3000);

  String response = "";

  while (ESP8266.available()) {
    response += char(ESP8266.read());
  }

  Serial.println(response);

  // RELAYS ON/OFF STUFF PINS 5-12
  /* if (response.indexOf("relay1\":\"ON") != -1) {
    digitalWrite(5, HIGH);
  } else if (response.indexOf("relay1\":\"OFF") != -1) {
    digitalWrite(5, LOW);
  } else if (response.indexOf("relay2\":\"ON") != -1) {
    digitalWrite(6, HIGH);
  } else if (response.indexOf("relay2\":\"OFF") != -1) {
    digitalWrite(6, LOW);
  } else if (response.indexOf("relay3\":\"ON") != -1) {
    digitalWrite(7, HIGH);
  } else if (response.indexOf("relay3\":\"OFF") != -1) {
    digitalWrite(7, LOW);
  } else if (response.indexOf("relay4\":\"ON") != -1) {
    digitalWrite(8, HIGH);
  } else if (response.indexOf("relay4\":\"OFF") != -1) {
    digitalWrite(8, LOW);
  } else if (response.indexOf("relay5\":\"ON") != -1) {
    digitalWrite(9, HIGH);
  } else if (response.indexOf("relay5\":\"OFF") != -1) {
    digitalWrite(9, LOW);
  } else if (response.indexOf("relay6\":\"ON") != -1) {
    digitalWrite(10, HIGH);
  } else if (response.indexOf("relay6\":\"OFF") != -1) {
    digitalWrite(10, LOW);
  } else if (response.indexOf("relay7\":\"ON") != -1) {
    digitalWrite(11, HIGH);
  } else if (response.indexOf("relay7\":\"OFF") != -1) {
    digitalWrite(11, LOW);
  } else if (response.indexOf("relay8\":\"ON") != -1) {
    digitalWrite(12, HIGH);
  } else if (response.indexOf("relay8\":\"OFF") != -1) {
    digitalWrite(12, LOW);
  } */

  ESP8266.println("AT+CIPCLOSE");
  return response;
}

void applyCommands(String response) {

  for (int i = 1; i <= 8; i++) {

    String key = "relay" + String(i) + "\":\"ON";

    if (response.indexOf(key) != -1) {
      digitalWrite(relayPins[i-1], HIGH);
    } else {
      digitalWrite(relayPins[i-1], LOW);
    }
  }
}