#include <NewPing.h>
#include <VirtualWire.h>
#define TRIGGER_PIN  13  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     12  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters).
#define SENS 16  // How sensitive the alarm is in the change of distance detected by the utrasonic (distance) sensor.
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
char controller[4];
const int gasPin = A0; //GAS sensor output pin to Arduino analog A0 pin
const int buzzPin = 9; //Buzzer control pin to Arduino digital 9 pin
const int doorPin = 11; // Door switch pin to Arduino digital 11 pin
const int transPin = 5; // RF transmitter Data pin to Arduino digital 5 pin
const int loops[] = {20,10,8};// For how many loops the notification for detected gas, motion and door respectively will last
int loopcounter[] = {0,0,0};
boolean gastrigger = false; // Gas notification (initialized to false)
boolean motiontrigger = false; // Motion notification (initialized to false)
boolean doortrigger = false; // Door notification (initialized to false)
int prev = -1; //Previous value of distance sensor
int sendevery = 0;

void setup()
{
	Serial.begin(9600); //Initialize serial port - 9600 bps
        pinMode(buzzPin,OUTPUT); //Set buzzPin to OUTPUT
        pinMode(doorPin,INPUT);
        digitalWrite(buzzPin,HIGH); //Initialization of buzzPin to HIGH
        vw_set_ptt_inverted(true);
        vw_set_tx_pin(transPin); // The pin where the data are sent in order to be transmitted (RF)
        vw_setup(4000);// speed of data transfer Kbps
}
void read(){
  int avg = 0;
  int i=0;
  unsigned int uS;
  while (i<5){ //Get 5 non-zero values
    delay(25);
   uS = sonar.ping(); //Get distance value
   if (uS > 0){
     avg += uS/US_ROUNDTRIP_CM;  // Convert ping time to distance in cm and add it to the avg
     i++;
   }
  }
  avg /= 5; //Gets average distance
  Serial.print("Ping: ");
  Serial.print(avg);
  Serial.println("cm");
  if (prev == -1){prev = avg;} // If first measurement
  int temp = abs(prev - avg); // Get the distance difference
  if (temp > SENS){ // If distance difference bigger than sensitivity
    motiontrigger = true;loopcounter[1] = 0; // Enable motion notification and initialize its counter
    Serial.println("Movement");
  }
  prev = avg;
}

void loop()
{       read();
        int svalue = analogRead(gasPin); //Get gas value in V
        Serial.print("Smoke :");
	Serial.println(svalue);
        int dvalue = digitalRead(doorPin); //Get door value (0 or 1)
        if ( dvalue == LOW){
          Serial.println("Door Alert");
          doortrigger = true; loopcounter[2] = 0;
        }
	delay(300); // Print value every 1 sec.
        if (svalue > 205){ // Enable gas notification if gas value exceeds 205
           Serial.println("Gas Alert");
           gastrigger = true; loopcounter[0] = 0;
        }
        unsigned int state = 0;
				// Keep notifications for #loops
				// Calculate our current state depending on which notifications are enabled
        if (gastrigger){
         loopcounter[0]++;
         state= state +1;
        }
        if (motiontrigger){
         loopcounter[1]++;
          state = state +2;
        }
        if (doortrigger){
         loopcounter[2]++;
         state = state +4;
        }
        if (loopcounter[0] == loops[0]){gastrigger=false;}
        if (loopcounter[1] == loops[1]){motiontrigger=false;}
        if (loopcounter[2] == loops[2]){doortrigger=false;}

        if(gastrigger || motiontrigger || doortrigger){ //For any enabled notification
          digitalWrite(buzzPin, LOW); // trigger the buzzer
        }else{
          digitalWrite(buzzPin,HIGH);
        }
				// Print our current notification state
        Serial.print("gas: ");Serial.print(gastrigger);Serial.print(", motion: ");Serial.print(motiontrigger);Serial.print(", door :");Serial.println(doortrigger);
        if (sendevery == 4){ // Every 4 loops send (RF) the current state
        sendevery=0;
        itoa(state,controller,10); // Convert integer to char array
        Serial.println("Sending");
        vw_send((uint8_t *)controller,4); // Transmitt
        vw_wait_tx(); // Wait until the whole message is gone
        Serial.println("Sent");
        }
        sendevery++;
}
