/*
This code was written by Jan Koppatscheck (DO6JAN) and Hendrik Lüth (DO9XE).
 The main-structure of the code comes from: http://blog.bouni.de/blog/2012/06/25/a-arduino-telnet-server/
 For Information please go to http://www.labor19.de/ or write to kontakt@lueth-labs.de.
 If you have any suggestions how to make the code more effective and smaller please write us.
 Thank you for using our code :)
 */
#include <SPI.h>
#include <Ethernet.h>
#define MAX_CMD_LENGTH   25


byte mac[] = { 
  0x90, 0xA2, 0xDA, 0x00, 0xE3, 0x5B };


IPAddress ip(192, 168, 55, 223);
IPAddress gateway(192, 168, 55, 1);
IPAddress subnet(255, 255, 255, 0);


EthernetServer server = EthernetServer(23);
EthernetClient client;
boolean connected = false;


String cmd, senden, senden2, senden3, senden4; 
int StatDB[10] = {
  0,0,0,0,0,0,0,0,0,0};
int StatAna[16] = {
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; 

void setup()
{
  Ethernet.begin(mac, ip, gateway, subnet);
  server.begin();
  for (int i=22;i<38;i++) // definiert die Pins 22-37 als output
  {
    pinMode(i, OUTPUT);
    digitalWrite(i, HIGH);
  }


  for (int x=38;x<48;x++) // definiert die Pins 38-47 als input
  {
    pinMode(x, INPUT);
  }


}


void loop()
{
  while(1){
    client = server.available();


    if (client == true) {
      if (!connected) {
        client.flush();
        connected = true;
      }


      if (client.available() > 0) {
        readTelnetCommand(client.read());
      }
    }


    // check for inputs


    for (int k=38;k<48;k++)
    {
      int oNR2 = k - 37;
      int oNR = k - 38;
      if (oNR2 < 10){
        senden = "0" + String(oNR2) + ".";
      }
      else{
        senden = String(oNR2) + ".";
      }


      if(digitalRead(k) != StatDB[oNR]) {
        StatDB[oNR] = !StatDB[oNR];
        server.println(senden + String(StatDB[oNR]));
      }
    }
    //check for analog inputs
    for (int z=8;z<12;z++){
      int Analog = analogRead(z);
      int dif = Analog - StatAna[z];
      if (-10 < dif < 10){
        Analog = 0;
      }
      else{
        server.println("A" + String(z) + "." + Analog);
        StatAna[z] = Analog;      
      }
    } 
  }
}


void readTelnetCommand(char c) {


  if(cmd.length() == MAX_CMD_LENGTH) {
    cmd = "";
  }


  cmd += c;


  if(c == '\n') {
    if(cmd.length() > 2) {
      // remove \r and \n from the string
      cmd = cmd.substring(0,cmd.length() - 2);
      parseCommand();
    }
  }
}


void parseCommand() {
  int ort=cmd.indexOf(".");
  String CMDstat;
  int cmdNR;
  String cmdNRtest = String (cmd.substring(0, ort));
  cmdNR = cmdNRtest.toInt() + 21;
  
  if(ort == 0){
  return;
  }


  int lange = cmd.length();
  CMDstat=cmd.substring(lange, lange-1);

  if(cmd.equals("kill")){
    for (int x=22;x<38;x++) 
    {
      digitalWrite(x, HIGH);
    }
  }
  else if(cmd.equals("help")) {
    server.println("-=* GPIO Server Help/Info *=-");
    server.println("This GPIO Server is based on a Arduino Mega 2560");
    server.println("The Programm was written by DO6JAN an DO9XE");
    server.println("For Questions please go to www.labor19.net");
    server.println("Copyright 2014 Jan Koppatscheck, Hendrik Lüth");
    server.println("For privat use only!"); 
  }  
  else if(cmd.equals("get-all")){
    server.println("ID00001");


    for (int g=0;g<10;g++){
      int oNR3 = g + 1;
      if (oNR3 < 10){
        senden3 = "0" + String(oNR3) + ".";
      }
      else{
        senden3 = String(oNR3) + ".";
      }
      senden4 = String(StatDB[g]);
      server.println(senden3 + senden4);
    }
    for (int l=8;l<12;l++){
      int Analog = analogRead(l);
      server.println("A" + String(l) + "." + Analog);
    }
  }
  else if(21 < cmdNR < 37){    
    if(CMDstat.equals("1")){
      digitalWrite(cmdNR, LOW);  
    }
    else if(CMDstat.equals("0")){
      digitalWrite(cmdNR, HIGH);
    }
  }  
  cmd = "";
}
