#include <SPI.h>
#include <Ethernet.h>
#include <VirtualWire.h>
char msg[4];
EthernetClient client;
boolean actions[] = {false,false,false}; // Notifications array
float temp;
// MAC address for the controller
byte mac[] = { 0x9E, 0xAD, 0xBA, 0xEF, 0xFE, 0xED };
char server[] = "www.edged.xyz";    // name address for server (using DNS), here put your URL
const int tempPin = 1; // Temperature pin -> Arduino analog pin 1
// Set the static IP address, dnServer, subnet, gateway
IPAddress ip(192, 168, 1, 95);
IPAddress dnServer(208,67,222,222);
IPAddress subnet(255,255,255,0);
IPAddress gateway(192,168,1,1);

void setup() {
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect
  }
  vw_set_ptt_inverted(true); // Required for DR3100
  vw_set_rx_pin(8);
  vw_setup(4000);  // Bits per sec
  vw_rx_start();
  job();
  // start the Ethernet connection:
  Ethernet.begin(mac, ip, dnServer, gateway, subnet);
  delay(4000);
  //Call the function that will handle the connection:
  conn();
}

void loop() {
  // if there are incoming bytes available
  // from the server, read them and print them:
  if (client.available()) {
    char c = client.read();
    Serial.print(c);
  }

  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting.");
    Serial.println("");
    client.stop();

    delay(4000);
    job(); //call job function
    delay(1000);
    Serial.println("Reconnecting to server");
  // then connect again
    conn();
  }
}

void conn() {
  // print the Ethernet board/shield's IP address:
  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());

  Serial.println("connecting...");
  if (client.connect(server, 80)) {
    Serial.println("connected");
    // Make a HTTP GET request including the temperature and the notifications' values for smoke, motion and door:
    client.println("GET /connect/submitdata.php?temp=" + String((int)temp) + "&motion=" + String(actions[1]) + "&smoke=" + String(actions[0]) + "&door=" + String(actions[2]) + " HTTP/1.0");
    client.println("Host: www.edged.xyz"); // here put your URL
    client.println("User-Agent: arduino-ethernet");
    client.println("Connection: close");
    client.println();
  } else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }
  }
  // Function job gets transmitted message and calculates the state regarding the enabled notifications
  void job(){
    int i;
    uint8_t buf[VW_MAX_MESSAGE_LEN];
    uint8_t buflen = VW_MAX_MESSAGE_LEN;
    while (!vw_get_message(buf, &buflen)){ // waits until it receives a message from RF transmitter
    delay(500);}
    for (i=0;i<buflen;i++){
     msg[i] = char(buf[i]); // add each received byte to the char array msg
    }
    msg[buflen] = '\0'; // append the "terminate string" character
    int state = atoi(msg); // convert char array to integer
    Serial.println(state);
    // Export current notifications from state variable:
    if (state >= 4){
      actions[2]=true;
      Serial.print("Door Opened");
      state-=4;
    }else{
      actions[2]=false;
    }
    if (state%2) {
      actions[0]=true;
      Serial.print("Gas");
      state-=1;
    }else{
      actions[0]=false;
    }
    if (state == 2){
      actions[1]=true;
      Serial.print("Motion trigered");
    }else{
      actions[1]=false;
    }
    //Calculate temperature from voltage reading:
    int reading = analogRead(tempPin);
    float voltage=reading*3.3; // Vcc=3.3V
    voltage/=1024.0;
    temp = (voltage-0.3)*100;
  }
