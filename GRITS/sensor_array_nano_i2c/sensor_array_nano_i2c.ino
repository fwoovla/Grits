//nano i2c sensor array

// ---------------------------------------------------------------------------
//i2c setup
#include <Wire.h>

#define SDA_PIN 4
#define SCL_PIN 5

const int16_t SENSOR_ADDR = 0x16; // 0x08 LEFT   0x09 RIGHT;

const int VEC_MAX = 4;
float data_msg[VEC_MAX] = {0,0,0,0};

void on_request() {
  Wire.write((uint8_t*) data_msg, sizeof(data_msg));  
}


// ---------------------------------------------------------------------------

#include <NewPing.h>

#define TRIGGER_PIN_1  7  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN_1     7  // Arduino pin tied to echo pin on the ultrasonic sensor.

#define TRIGGER_PIN_2  8  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN_2     8  // Arduino pin tied to echo pin on the ultrasonic sensor.

#define TRIGGER_PIN_3  9  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN_3     9  // Arduino pin tied to echo pin on the ultrasonic sensor.

#define ECHO_PIN_4     10  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define TRIGGER_PIN_4  10  // Arduino pin tied to trigger pin on the ultrasonic sensor.

#define MAX_DISTANCE 100 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

NewPing sonar1(TRIGGER_PIN_1, ECHO_PIN_1, MAX_DISTANCE); // NewPing setup of pins and maximum distance.
NewPing sonar2(TRIGGER_PIN_2, ECHO_PIN_2, MAX_DISTANCE); 
NewPing sonar3(TRIGGER_PIN_3, ECHO_PIN_3, MAX_DISTANCE); 
NewPing sonar4(TRIGGER_PIN_4, ECHO_PIN_4, MAX_DISTANCE); 


int num_sensors = 4;

float dist_1 = 0;
float dist_2 = 0;
float dist_3 = 0;
float dist_4 = 0;


unsigned long last_ping_time= 0;
int ping_delay = 200;

void setup() {
  Serial.begin(115200);

  Wire.begin(SENSOR_ADDR);        // join i2c bus with address #8
  Wire.onRequest(on_request); // register event 
  //Wire.onReceive(on_receive);
}


void loop() {

  if(millis() - last_ping_time > ping_delay) {
      dist_1 = sonar1.ping_cm(MAX_DISTANCE); 
      if(dist_1 == 0) {
        dist_1 = MAX_DISTANCE;
      }
      
      data_msg[0] = dist_1;
      //Serial.print("Ping 1: ");
      Serial.print(dist_1); 
      Serial.print("cm\t\t");
      
      delay(20); 
      dist_2 = sonar2.ping_cm(MAX_DISTANCE);
      if(dist_2 == 0) {
        dist_2 = MAX_DISTANCE;
      }      
      data_msg[1] = dist_2;
      //Serial.print("Ping 2: ");
      Serial.print(dist_2); 
      Serial.print("cm\t\t");
      
      delay(20);         
      dist_3 = sonar3.ping_cm(MAX_DISTANCE);
      if(dist_3 == 0) {
        dist_3 = MAX_DISTANCE;
      }
      data_msg[2] = dist_3;
      //Serial.print("Ping 3: ");
      Serial.print(dist_3); 
      Serial.print("cm\t\t");

      delay(20);         
      dist_4 = sonar4.ping_cm(MAX_DISTANCE);
      if(dist_4 == 0) {
        dist_4 = MAX_DISTANCE;
      }  
      data_msg[3] = dist_4;
      //Serial.print("Ping 3: ");
      Serial.print(dist_4); 
      Serial.println("cm\t\t");      
      
      last_ping_time = millis();
  }
}
