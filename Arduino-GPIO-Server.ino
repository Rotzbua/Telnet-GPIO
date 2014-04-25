/*
This code was written by Jan Koppatscheck (DO6JAN) and Hendrik Lüth (DO9XE).
 The main-structure of the code comes from: http://blog.bouni.de/blog/2012/06/25/a-arduino-telnet-server/
 For Information please go to http://www.labor19.de/ or write to kontakt@lueth-labs.de.
 If you have any suggestions how to make the code more effective and smaller please write us.
 Thank you for using our code 
 */
#include <SPI.h>
#include <Ethernet.h>
#define MAX_CMD_LENGTH 25


byte mac[] = {
  0x90, 0xA2, 0xDA, 0x00, 0xE3, 0x5B };


IPAddress ip(192, 168, 55, 223);
IPAddress gateway(192, 168, 55, 10);
IPAddress subnet(255, 255, 255, 0);


EthernetServer server = EthernetServer(23);
EthernetClient client;
boolean connected = false;

//deklarieren der einzelnen Variablentypen
String cmd, senden, senden2, senden3, senden4, CMDstat, cmdNRstr;
int ort, cmdNR;

//Deklarieren der Arrays, die hier als Datenbank misbraucht werden
int StatIn[10] = {
  0,0,0,0,0,0,0,0,0,0};
int StatAna[16] = {
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int StatOut[16] = {
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


      if(digitalRead(k) != StatIn[oNR]) {
        StatIn[oNR] = !StatIn[oNR];
        server.println(senden + String(StatIn[oNR]));
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

  ort=cmd.indexOf(".");
  cmdNRstr = String (cmd.substring(0, ort));
  cmdNR = cmdNRstr.toInt() + 21;

  int lange = cmd.length();
  CMDstat=cmd.substring(lange, lange-1);

  if (ort==-1){
    ControllCommand;
  }
  else{
    SwitchCommand;
  }


  cmd = "";
}

//Die Folgende Funktion wertet die Schaltkommandos aus
void SwitchCommand(){

  if(21 < cmdNR < 37){
    if(CMDstat.equals("1")){
      digitalWrite(cmdNR, LOW);
      StatOut[cmdNRstr.toInt()]=1;
    }
    else if(CMDstat.equals("0")){
      digitalWrite(cmdNR, HIGH);
      StatOut[cmdNRstr.toInt()] = 0;
    }
  }
}


//Die Folgende Funktion wertet die Steuerbefehle aus
void ControllCommand(){
  if(cmd.equals("kill")){
    for (int x=22;x<38;x++)
    {
      digitalWrite(x, HIGH);
    }
  }
  else if(cmd.equals("help")||cmd.equals("--help")||cmd.equals("-h")||cmd.equals("--h")) {
    server.println("-=* GPIO Server Help/Info *=-");
    server.println("This GPIO Server is based on a Arduino Mega 2560");
    server.println("The Programm was written by DO6JAN an DO9XE");
    server.println("For Questions please go to www.labor19.net");
    server.println("Copyright 2014 Jan Koppatscheck, Hendrik Lüth");
    server.println("\n\rCommands:");
    server.println("kill      switch all Outputs off");
    server.println("XX.1/0    Output XX On 1 / Off 0");
    server.println("get-all   displays all inputs, outputs and sever ID");
    server.println("help      this (also: --help , -h , --h)");

  }
  else if(cmd.equals("get-all")){
    server.println("ID00001");

    for (int g=0;g<10;g++){
      int oNR3 = g + 1;
      if (oNR3 < 10){
        senden3 = "I0" + String(oNR3) + ".";
      }
      else{
        senden3 = "I" + String(oNR3) + ".";
      }
      server.println(senden3 + String(StatIn[g]));
    }
    for (int l=8;l<12;l++){
      int Analog = analogRead(l);
      server.println("A" + String(l) + "." + Analog);
    }
    for (int q=0;q<16;q++){
      int oNR4 = q + 1;
      if (oNR4 < 10){
        senden4 = "O0" + String(oNR4) + ".";
      }
      else{
        senden4 = "O" + String(oNR4) + ".";
      }
      server.println(senden4 + String(StatOut[q]));
    }    
  }
}
