#include <SPI.h>
#include <Ethernet.h>
#include <LiquidCrystal.h>

String texto = "";

//Define os pinos que serão utilizados para ligação ao display (Utilizar estes pinos com Ethernet shield pois se utilizar os padrões irá dar conflito)
LiquidCrystal lcd(9, 8, 7, 6, 5, 4);

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
IPAddress server(xxx,xxx,xx,xxx); // Put here the IP of machine API

// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192, 168, xx, xx);
IPAddress myDns(192, 168, xx, x);

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;

// Variables to measure the speed
unsigned long beginMicros, endMicros;
unsigned long byteCount = 0;
bool printWebData = true;  // set to false for better speed measurement

void _get(){
  // if you get a connection, report back via serial:
  if (client.connect(server, 5000)) {
    Serial.print("connected to ");
    Serial.println(client.remoteIP());
    // Make a HTTP request:
    client.println("GET /msg HTTP/1.0");
    client.println("Host: 192.168.xx.xx"); // Put here the IP of machine API
    client.println("Connecthion: close");
    client.println();
  } else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }
}

void setup() {
  //Define o número de colunas e linhas do LCD
  lcd.begin(16, 2);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Status:");
  lcd.setCursor(0, 1);
  lcd.print("Carregando...");

  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // start the Ethernet connection:
  Serial.println("Initialize Ethernet with DHCP:");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
      while (true) {
        delay(1); // do nothing, no point running without Ethernet hardware
      }
    }
    if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet cable is not connected.");
    }
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip, myDns);
  } else {
    Serial.print("  DHCP assigned IP ");
    Serial.println(Ethernet.localIP());
  }
  // give the Ethernet shield a second to initialize:
  delay(1000);
  Serial.print("connecting to ");
  Serial.print(server);
  Serial.println("...");

  _get();

  beginMicros = micros();
}

String GetStringBetweenStrings(String input, String firstdel, String enddel){
  int posfrom = input.indexOf(firstdel) + firstdel.length();
  int posto   = input.indexOf(enddel);
  return input.substring(posfrom, posto);
}

void loop() {

  texto = "";

  while(client.connected() && !client.available()) delay(1); //waits for data
  while (client.connected() || client.available()) { //connected or data available
    char c = client.read(); //gets byte from ethernet buffer
    texto += (char)c;
  }

  String stringOne = GetStringBetweenStrings(texto, "<", ">");
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print(stringOne);

  client.stop(); //stop client

  // Refresh interval
  delay(30000);

  _get();

}
