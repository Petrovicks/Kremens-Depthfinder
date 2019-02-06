//NMEA
#include <SoftwareSerial.h>

//SD libraries
#include <SPI.h>

//NOTE: SD library was modified to have SD card reader work with the MEGA2560 board.
//For portability the library is included in an src directory with this sketch. This is catered for the Arduino 1.8.6 IDE.
#include "src/SD.h" 
SoftwareSerial depthFinder(62, 63, true); //Inversion occurs in software on the digital pin. Used for depthfinder.

const int chipSelect = 8; // The Sparkfun microSD shield uses pin 8 for CS.
File myFile;

//Packets to capture NMEA messages from devices.
String gps_packet;
String depth_packet;

//Flags used for flow control when parsing full NMEA messages.
bool gpsSentence = false;
bool gpsValid = false;
bool depthSentence = false;
bool depthValid = false;
bool bothValid = false; //Check if packets are ready to be written to SD card.

String parse_gps_packet(String);
String parse_depth_packet(String);

void setup()
{
  Serial.begin(9600); //USB debugging.
  Serial1.begin(4800); //GPS
  depthFinder.begin(4800);

  pinMode(10, OUTPUT); //Hardware SS pin must be set to OUTPUT for SD library to work.
  if (!SD.begin(10, 11, 12, 13)) { //Note the extra arguments. These pins are what's used to emulate SPI in software via the modified SD library.
    Serial.println("Card failed to mount, or not present.");
    while (1); //Stuck
  }
  Serial.println("Card initialized.");
}

void loop()
{
  if (Serial1.available() && !gpsValid) {
    char c = Serial1.read() ;

    //All NMEA messages begin with $.
    if (c == '$') {
      gpsSentence = true;
      gps_packet = "";
    }

    //Look for carriage return.
    if (gpsSentence && c != '\r') {
      gps_packet += c;
    }

    //GPRMC packet contains all we need.
    else if (gps_packet.substring(3, 6) == "RMC") {
      gpsSentence = false;
      gpsValid = true;
      Serial.println(gps_packet);
    }
    else {
      gps_packet = "";
    }
  }

  if (depthFinder.available() && !depthValid) {
    char c = depthFinder.read();

    //All NMEA messages start with $.
    if (c == '$') {
      depthSentence = true;
      depth_packet = "";
    }

    //Look for carriage return.
    if (depthSentence && c != '\r') {
      depth_packet += c;
    }

    //This is the depth header.
    else if (depth_packet.substring(0, 6) == "$SDDPT") {
      depthSentence = false;
      depthValid = true;
      Serial.println(depth_packet);
    }

    //Depth finder will occassionally send out hardware debugging related message, which we don't need.
    else {
      depth_packet = "";
    }
  }

  if (gpsValid && depthValid) {
    myFile = SD.open("test.txt", FILE_WRITE);
    if (myFile) {
      Serial.println("Writing to SD..");

      String full_gps = parse_gps_packet(gps_packet);
      String depth = parse_depth_packet(depth_packet);

      myFile.println();
      myFile.println(full_gps + "," + depth);
      myFile.close();
      depthValid = false;
      gpsValid = false;
    }
    else {
      Serial.println("File not open..");
    }
  }
}

//This is basic string parsing. tinyGPS library could have been used, but this seemed like a simple enough solution.
//Uses commas as delimiters to divide full messages into their respective sections. 
//http://aprs.gids.nl/nmea/ - NMEA standard reference that was used for this. 
String parse_gps_packet(String packet) {
  int comma_array[11];
  int comma_index = 0;
  for (int i = 0; i < packet.length(); i++) {
    if (packet[i] == ',') {
      comma_array[comma_index] = i;
      comma_index++;
    }
  }
  String gps_lat;
  String gps_long;
  String gps_hour = packet.substring(comma_array[0] + 1, comma_array[1]);
  String gps_date = packet.substring(comma_array[8] + 1, comma_array[9]);
  gps_lat += packet[comma_array[3] + 1];
  gps_lat += packet.substring(comma_array[2] + 1, comma_array[3]);
  gps_long += packet[comma_array[5] + 1];
  gps_long += packet.substring(comma_array[4] + 1, comma_array[5]);

  return gps_date + "," + gps_hour + "," + gps_lat + "," + gps_long;
}

String parse_depth_packet(String packet) {
  int comma_array[3];
  int comma_index = 0;
  for (int i = 0; i < packet.length(); i++) {
    if (depth_packet[i] == ',') {
      comma_array[comma_index] = i;
      comma_index++;
    }
  }

  String depth;
  depth += packet[comma_array[0] + 1];
  if (depth == ",") {
    return "0";
  }
  else {
    return depth;
  }
}
