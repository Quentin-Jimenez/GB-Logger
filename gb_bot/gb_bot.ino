
#include <LiquidCrystal.h>
#include "ESP8266WiFi.h" 
#include "PubSubClient.h"

#include <Discord_WebHook.h>

// LCD Pins
const int rs = 16, en = 5, d4 = 4, d5 = 0, d6 = 14, d7 = 12;

//
String selections = {"Take a GB", "Change Water", "Change Bottle"}
int selectpos = 0;

// GB Takers
String names[] = {"Quentin", "Kev", "Tyler", "Aleks", "Seb", "Kam", "Liam", "Jack", "Someone Else"  "Jared", "Mike", "David", "Werner", "Jonah", "Tristan", "Theo", "Brian", "Restart"};
int namepos = 0;
#define NUMITEMS(arg) ((int) (sizeof (arg) / sizeof (arg [0])))
// Sizes
String sizes[] = {"Extra-Small", "Small", "Medium", "Large", "Extra Large", "To The Brim", "Overflowing", "Restart"};
int sizepos = 0;
// Rooms
String rooms[] = {"Room 8", "Room 7", "Other", "Restart"};
int roompos = 0;
// Confirm
String confirm[] = {"Confirm", "Restart"};
int confirmpos = 0;


// Custom Chars
byte weedTopL[] = {0b00001, 0b00001, 0b00011, 0b10011, 0b11011, 0b11011, 0b11111, 0b01111}; // Weed top
byte weedBottomL[] = {0b00111, 0b10111, 0b11111, 0b01111, 0b00011, 0b00111, 0b01001, 0b00001}; // Weed bottom
byte weedTopR[] = {0b10000, 0b10000, 0b11000, 0b11001, 0b11011, 0b11011, 0b11111, 0b11110}; // Weed top
byte weedBottomR[] = {0b11100, 0b11101, 0b11111, 0b11110, 0b11000, 0b11100, 0b10010, 0b10000}; // Weed bottom

// Bottom Text Controller
int bottom = 15;
int scroll = 0;


// Controls State of Entry
int state = 0; // 0 = Option Selector (Default), 1 = Names, 1 == Size, 2 == Room, 3 == Confirm


// WiFi
const char* ssid = "Stop\ Eating\ Meat";
const char* wifi_password="amivegan";

// MQTT
const char* mqtt_server = "192.168.1.6";  // IP of the MQTT broker (lodgebot raspi) 
const char* gb_topic = "Lodge/Room8/GBData"; 
const char* mqtt_username = "brothers1883"; // MQTT username
const char* mqtt_password = "brothers1883"; // MQTT password
const char* clientID = "client_room8"; // MQTT client ID

// Idle
int idlecount = 0;


LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
WiFiClient wifiClient;
PubSubClient client(mqtt_server, 1883, wifiClient); 

Discord_Webhook discord;
String DISCORD_WEBHOOK = "https://discord.com/api/webhooks/1047655465280094278/_R7-ScBWAK99Ym5BUOMoS5JMYx18i5Y-jEQw5nKK47AwOzCYRiH55uUXzTbakHWv5Zgn";


void(* resetFunc) (void) = 0;


// Custom function to connect to the MQTT broker via WiFi
void connect_MQTT(){
  Serial.print("Connecting to ");
  Serial.println(ssid);

  // Connect to the WiFi
  WiFi.begin(ssid, wifi_password);

  // Wait until the connection has been confirmed before continuing
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Debugging - Output the IP Address of the ESP8266
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Connect to MQTT Broker
  // client.connect returns a boolean value to let us know if the connection was successful.
  // If the connection is failing, make sure you are using the correct MQTT Username and Password (Setup Earlier in the Instructable)
  if (client.connect(clientID, mqtt_username, mqtt_password)) {
    Serial.println("Connected to MQTT Broker!");
  }
  else {
    Serial.println("Connection to MQTT Broker failed...");
  }
}


 void setup() {
  // Start LCD
  lcd.begin(16, 2);

  // Connect to the WiFi
  WiFi.begin(ssid, wifi_password);

  // Discord Init
  discord.begin(DISCORD_WEBHOOK);
  discord.addWiFi(ssid, wifi_password);
  discord.connectWiFi();

  // Initialize Custom Chars
  lcd.createChar(0, weedTopL);
  lcd.createChar(1, weedBottomL);
  lcd.createChar(2, weedTopR);
  lcd.createChar(3, weedBottomR);
  
  Serial.begin(9600); //initialize serial communication at 9600 bits per second
}


void loop() {

  // Read Button Inputs From Analog, 1 100K Resistor and 2K resistors in series
  int value = analogRead(A0);

  //Serial.println(value);
  //Serial.println(pos);
  //Serial.println(state);

  if(idlecount > 10*90 && value > 1000){ // Idle after 90 seconds of no input until a button is pressed
    // Do idle thing, text scrolling
    lcd.clear();
    lcd.print("IDLE");
  }else{ 
    // Not idle anymore
  
  
    if (value > 1020 && value < 1030){
      idlecount++;
      
    } else if (value > 0 && value < 25){
      
      // Right Button
      if(state==0){
        selectpos++;
      }else if(state==1){
        namepos++;
      }else if(state==2){
        sizepos++;
      }else if(state==3){
        roompos++;
      }else if(state==4){
        confirmpos++;
      }
      delay(250);
      
    }else if (value > 250 && value < 300){
      
      // Center Button
      if(state==0){
        if(selectpos==0){ // Initial menu, take gb selected
          state++;
        }else if(selectpos==1){ // Change Water
  
          lcd.clear();
          lcd.print("Transmitting...");
          
          connect_MQTT();
          delay(10);
          
          discord.send("The GB water was changed");
          
          if(client.publish(gb_topic, String("water").c_str())){
            Serial.println("Successfully sent!");
            lcd.clear();
            lcd.print("Success!");
          }
          delay(10);
          client.disconnect();
          resetFunc(); 
          
        }else if(selectpos==2){ // Change Bottle
  
          lcd.clear();
          lcd.print("Transmitting...");
          
          connect_MQTT();
          delay(10);
          
          discord.send("The GB bottle was changed");
          
          if(client.publish(gb_topic, String("bottle").c_str())){
            Serial.println("Successfully sent!");
            lcd.clear();
            lcd.print("Success!");
          }
          delay(10);
          client.disconnect();
          resetFunc(); 
          
        }
      }
      if(state==1){
        if(sizepos==NUMITEMS(names)-1){ // Restart
          resetFunc(); 
        }else{
          state++;
        }
      }else if(state==2){
        if(sizepos==NUMITEMS(sizes)-1){ // Restart
          resetFunc(); 
        }else{
          state++;
        }
      }else if(state==3){
        if(roompos==NUMITEMS(rooms)-1){ // Restart
          resetFunc(); 
        }else{
          state++;
        }
      }else if(state==4){ // Confirm State
        if(confirmpos==NUMITEMS(confirm)-1){ // Restart
          resetFunc(); 
        }else{
          // Confirmed Entry
          lcd.clear();
          lcd.print("Transmitting...");
          
          connect_MQTT();
          delay(10);
          
          discord.send(names[namepos] + " took a " + sizes[sizepos] + " GB in " + rooms[roompos]);
          
          if(client.publish(gb_topic, String(names[namepos] + "," + sizes[sizepos] + "," + rooms[roompos]).c_str())){
            Serial.println("Successfully sent!");
            lcd.clear();
            lcd.print("Success!");
          }
          delay(10);
          client.disconnect();
          delay(1500);
          
          lcd.clear();
          lcd.print("Enjoy Your GB!");
          delay(1500);
  
          // Print Weed
          lcd.clear();
          lcd.setCursor(7, 0);
          lcd.write(byte(0));
          lcd.setCursor(8, 0);
          lcd.write(byte(2));
          lcd.setCursor(7, 1);
          lcd.write(byte(1));
          lcd.setCursor(8, 1);
          lcd.write(byte(3));
          delay(1500);
          
          resetFunc(); 
        }
      }
      delay(250);
      
    }else if (value > 430 && value < 480){
      
      // Left Button
      if(state==0){
        selectpos--;
      }else if(state==1){
        namepos--;
      }else if(state==2){
        sizepos--;
      }else if(state==3){
        roompos--;
      }else if(state==4){
        confirmpos--;
      }
      delay(250);
      
    }
    
    // Loop Positions in circle
    if(state == 0){
      if(selectpos > NUMITEMS(selections)-1){
        selectpos = 0;
      }else if(selectpos < 0){
        selectpos = NUMITEMS(selections)-1;
      }
    }else if(state == 1){
      if(namepos > NUMITEMS(names)-1){
        namepos = 0;
      }else if(namepos < 0){
        namepos = NUMITEMS(names)-1;
      }
    }else if(state == 2){
      if(sizepos > NUMITEMS(sizes)-1){
        sizepos = 0;
      }else if(sizepos < 0){
        sizepos = NUMITEMS(sizes)-1;
      }
    }else if(state == 3){
      if(roompos > NUMITEMS(rooms)-1){
        roompos = 0;
      }else if(roompos < 0){
        roompos = NUMITEMS(rooms)-1;
      }
    }else if(state == 4){
      if(confirmpos > NUMITEMS(confirm)-1){
        confirmpos = 0;
      }else if(confirmpos < 0){
        confirmpos = NUMITEMS(confirm)-1;
      }
    }
  
    // State Printer
    if(state==0){
      lcd.clear();
      lcd.print(selections[selectpos]); //names[pos]
    }else if(state==1){
      // Serial.println(value); //names[pos]
      lcd.clear();
      lcd.print(names[namepos]); //names[pos]
    }else if (state==2){
      lcd.clear();
      lcd.print(sizes[sizepos]);
    }else if (state==3){
      lcd.clear();
      lcd.print(rooms[roompos]);
    }else if (state==4){
      lcd.clear();
      lcd.print(confirm[confirmpos]);
    }
  
    /*
    lcd.setCursor(bottom, 1);
    lcd.print("She Call Me Gerb");
    scroll++;
  
    Serial.println(bottom);
    if(bottom == -1 * String("She Call Me Gerb").length()){
      bottom = 15;
    }
    if(scroll % 4 == 0){
      bottom--;
    }
    */
    delay(100); 
  }
}
