//ESP GRITS BRAIN


#include <Wire.h>
//#include <PolledTimeout.h>

#define SDA_PIN 4
#define SCL_PIN 6

const int16_t I2C_MASTER = 0x42;
const int16_t L_MOTOR_ADDR = 0x08;
const int16_t R_MOTOR_ADDR = 0x09;
const int16_t SENSOR_ADDR = 0x16;

const int WHEEL_DATA_MAX = 6;
const int SENSOR_DATA_MAX = 4;

float left_wheel_data[WHEEL_DATA_MAX]; //
float right_wheel_data[WHEEL_DATA_MAX]; //

float sensor_data[SENSOR_DATA_MAX];
float last_sensor_data[SENSOR_DATA_MAX];

//vars for ease of use
float l_side_sensor_range;
float l_sensor_range;
float r_side_sensor_range;
float r_sensor_range;


int l_dir_cmd = 1; // 1  = fwd, 255 = rev
int r_dir_cmd = 1;
int l_speed_cmd = 250;  // curently 0-255
int r_speed_cmd = 250;


void read_sensors() {  
  uint8_t* data_pointer = (uint8_t*) sensor_data;
  Wire.requestFrom(SENSOR_ADDR, sizeof(sensor_data));

  while (Wire.available()) {
    *data_pointer++ = Wire.read();
  }
  
  Serial.println("new sensor data");

  for (int i = 0; i < SENSOR_DATA_MAX; i++) {
    
    if(sensor_data[i] == 0) {
      
      sensor_data[i] = last_sensor_data[i];  
    }
    
    last_sensor_data[i] = sensor_data[i];

    Serial.println(sensor_data[i]); 
  }
  
  l_side_sensor_range = sensor_data[0];  //use these vars for ease
  l_sensor_range = sensor_data[1];
  r_sensor_range = sensor_data[2];
  r_side_sensor_range = sensor_data[3];

}

void read_left_wheel() {

  uint8_t* data_pointer = (uint8_t*) left_wheel_data;

  Wire.requestFrom(L_MOTOR_ADDR, sizeof(left_wheel_data));
  while (Wire.available()) {
    *data_pointer++ = Wire.read();
  }
}


void read_right_wheel() {
  
  uint8_t* data_pointer = (uint8_t*) right_wheel_data;

  Wire.requestFrom(R_MOTOR_ADDR, sizeof(right_wheel_data));
  while (Wire.available()) {
    *data_pointer++ = Wire.read();
  }
}

void command_l_wheel(int _dir, int _sp) {
  
  Wire.beginTransmission(L_MOTOR_ADDR); // transmit to device #8
  Wire.write(_dir);
  Wire.write(_sp);
  Wire.endTransmission();    // stop transmitting  
//  Serial.print(_dir);
//  Serial.print("\t");
//  Serial.println(_sp);
}


void command_r_wheel(int _dir, int _sp) {
  Wire.beginTransmission(R_MOTOR_ADDR); // transmit to device #8
  Wire.write(_dir);
  Wire.write(_sp);
  Wire.endTransmission();    // stop transmitting  
//  Serial.print(_dir);
//  Serial.print("\t");
//  Serial.println(_sp);
}

unsigned long last_sensor_check = 0;
int sensor_check_delay = 100;

unsigned long last_wheel_check = 0;
int wheel_check_delay = 300;

unsigned long last_control_cmd = 0;
int control_cmd_delay = 300;

bool can_go;

void setup() {
  Serial.begin(115200);  // start serial for output
  Wire.begin();        // join i2c bus (address optional for master)

  can_go = true;
}

/////////////////////////////////////////////////////////
void loop() {

  if(millis() - last_control_cmd > control_cmd_delay) {

    update_wheel_cmd();
    
  }
  // check wheels
  if(millis() - last_wheel_check > wheel_check_delay) {
    read_right_wheel();
    read_left_wheel();
    
    last_wheel_check = millis();
  }
  
//chek sensors
  if(millis() - last_sensor_check > sensor_check_delay) {

    read_sensors();
    
    last_sensor_check = millis();  
 }
}
//////////////////////////////////////////////////////////////

void update_wheel_cmd() {
  
  int side_react_zone = 10;  // 3
  int front_react_zone = 10;

  int side_monitor_zone = 30;  //2
  int front_monitor_zone = 30;

  int side_notify_zone = 50;  //1
  int front_notify_zone = 50;
  
  int obj_left_side = 0;  //default 0
  int obj_left_front = 0;
  int obj_right_front = 0;
  int obj_right_side = 0;
  
//reset vars
  l_dir_cmd = 1;
  r_dir_cmd = 1;
  l_speed_cmd = 250;
  r_speed_cmd = 250;

////check for objects
//
  //react
  if(l_side_sensor_range < side_react_zone) { //object is left
    obj_left_side = 3;
  }
  if(l_sensor_range < front_react_zone) { //object is center left
    obj_left_front = 3;    
  }
  if(r_sensor_range < front_react_zone) { //object is center right
    obj_right_front = 3;
  }
  if(r_side_sensor_range < side_react_zone) { //object is center right
    obj_right_side = 3;
  }

//////  mid
  if(l_side_sensor_range < side_monitor_zone) { //object is left
    obj_left_side = 2;
  }
  if(l_sensor_range < front_monitor_zone) { //object is center left
    obj_left_front = 2;    
  }
  if(r_sensor_range < front_monitor_zone) { //object is center right
    obj_right_front = 2;
  }
  if(r_side_sensor_range < side_monitor_zone) { //object is center right
    obj_right_side = 2;
  }  

//////  low
  if(l_side_sensor_range < side_notify_zone) { //object is left
    obj_left_side = 1;
  }
  if(l_sensor_range < front_notify_zone) { //object is center left
    obj_left_front = 1;    
  }
  if(r_sensor_range < front_notify_zone) { //object is center right
    obj_right_front = 1;
  }
  if(r_side_sensor_range < side_notify_zone) { //object is center right
    obj_right_side = 1;
  }   
  //////

//main algorithm to chose path

  int left_weight = l_side_sensor_range + l_sensor_range;//obj_left_side + obj_left_front;
  int right_weight = r_side_sensor_range + r_sensor_range;//obj_right_side + obj_right_front;

  if(l_sensor_range < 30 || r_sensor_range < 30) {
    
    if(l_side_sensor_range < r_side_sensor_range) {
      l_dir_cmd = 1;
      r_dir_cmd = 255;
    }
    else {
      l_dir_cmd = 255;
      r_dir_cmd = 1;
    }
  }
//
//  if(left_weight || right_weight < 50) {
//    if(left_weight > right_weight) {
//      l_dir_cmd = 255;
//      r_dir_cmd = 1;
//    }
//    if(right_weight > left_weight) {
//      l_dir_cmd = 1;
//      r_dir_cmd = 255;
//    }
//    if(right_weight == left_weight) {
//      l_dir_cmd = 255;
//      r_dir_cmd = 255;
//    }
//end path algorithm

////send wheel commands  
  command_r_wheel(r_dir_cmd, r_speed_cmd);
  command_l_wheel(l_dir_cmd, l_speed_cmd);  
}

//disply motor data to serial
void output_motor_data() {
  
  Serial.print("dir\t\t");
  Serial.print("speed cm/s\t\t");  
  Serial.print("pwm\t\t");
  Serial.print("rps\t\t");
  Serial.print("ticks\t\t ");
  Serial.print("delta ticks\n");
      
    Serial.println("LEFT");
    for(int i = 0; i < WHEEL_DATA_MAX; i++) {
      Serial.print(left_wheel_data[i]);
      Serial.print("\t\t");
    }    
    Serial.println("\nRIGHT");
    
    for(int i = 0; i < WHEEL_DATA_MAX; i++) {
      Serial.print(right_wheel_data[i]);
      Serial.print("\t\t");
    }    
    Serial.println();  
}
