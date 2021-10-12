//nano i2c wheel motor controller


#include <Wire.h>

#define SDA_PIN 4
#define SCL_PIN 5

const int16_t I2C_MASTER = 0x42;
//const int16_t MOTOR_ADDR = 0x08; // 0x08 LEFT   0x09 RIGHT;
const int16_t MOTOR_ADDR = 0x09;


#define FULL_TURN 3960

#define ENC      2 
#define OUT_A    3
#define OUT_B    4
#define OUT_PWM  6

//typedef struct wheel_message { //4 bytes each
//    float dir;
//    float pwm;
//    float rpm;
//    
//} wheel_message;
//
//wheel_message wheel_data;

float pwm = 0;
float ticks = 0;  // 1980 ticks = 2pi radians of rotation
float last_ticks = 0;
float dir = 0;   //-1,0,1
float rps = 0;
float speed = 0;

float wheel_circ = 31.415; // d * pi

void setup() {
  // Init Serial Monitor
  Serial.begin(9600);

  Wire.begin(MOTOR_ADDR);        // join i2c bus with address #8
  Wire.onRequest(on_request); // register event  
  Wire.onReceive(on_receive);

  init_pins();

  attachInterrupt(digitalPinToInterrupt(ENC), on_encoder, CHANGE);  

  write_wheel(0, 0);

}

long last_check = 0;
int check_time = 200;

const int VEC_MAX = 5;
float data_msg[VEC_MAX] = {0,0,0,0,0};

void loop() {

  if(millis() - last_check > check_time) {
    last_check = millis();
    get_data();
  }
  
}


// function that executes whenever data is requested by master
// this function is registered as an event, see setup()

void on_request() {
  Wire.write((uint8_t*) data_msg, sizeof(data_msg));
}



void on_receive(size_t howMany) {
  
  (void) howMany;
//  while (1 < Wire.available()) { // loop through all but the last
//    char c = Wire.read(); // receive byte as a character
//    Serial.print(c);         // print the character
//  }
  int _dir = Wire.read();    // receive byte as an integer
  int _sp = Wire.read();
  //Serial.println(x);         // print the integer
  write_wheel(_dir, _sp);
//  Serial.println(_dir);
//  Serial.println(_sp);
}  



void get_data() {
  float delta_ticks = ticks - last_ticks;
  float rev = delta_ticks / FULL_TURN;
  speed = (rev * wheel_circ) * (1000 / check_time);
  last_ticks = ticks;
  rps = (rev * 5);

//  Serial.print("dir");
//  Serial.print("speed cm/s");  
//  Serial.print("pwm");
//  //Serial.print("rev\t\t");
//  Serial.print("rps\t\t");
//  Serial.print("ticks\t\t ");
//  Serial.print("delta ticks\n ");
//
//  
//  Serial.print(dir);
//  Serial.print("\t\t");
//  Serial.print(speed);
//  Serial.print("\t\t");
//  Serial.print(pwm);
//  Serial.print("\t\t");  
//  Serial.println(rps);
//  Serial.print("\t\t");  
//  Serial.print(ticks);  
//  Serial.print("\t\t");  
//  Serial.println(delta_ticks);  
  
  data_msg[0] = dir;
  data_msg[1] = speed;
  data_msg[3] = pwm;
  data_msg[2] = rps;
  data_msg[4] = ticks;
  data_msg[5] = delta_ticks;


}


void write_wheel(int _dir, int _speed) {
  
  dir = _dir;
  pwm = _speed;
  if(dir == 255) {
    dir = -1;
  }
  
  analogWrite(OUT_PWM, pwm);    //set speed
  
  if(dir == 1) {                //fwd
    digitalWrite(OUT_A, HIGH);
    digitalWrite(OUT_B, LOW);
  }
  
  if(dir == -1) {                //rev
    digitalWrite(OUT_A, LOW);
    digitalWrite(OUT_B, HIGH);
  }
  
  if(dir == 0) {                //stop
    digitalWrite(OUT_A, LOW);
    digitalWrite(OUT_B, LOW);
  }  
}


void init_pins() {
  pinMode(OUT_A, OUTPUT);
  pinMode(OUT_B, OUTPUT);
  pinMode(OUT_PWM, OUTPUT);
  pinMode(ENC, INPUT);  
}

//ICACHE_RAM_ATTR 
void on_encoder() {
  if (dir == 1) { //going fwd
    ticks++;
  }
  if (dir == -1) {  //going rev
    ticks--;
  }
}
