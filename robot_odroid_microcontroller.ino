#include <Cth.h>
#include <stdlib.h>

#include "grippers.h"
#include "ultrasonic.h"
#include "command_interpreter.h"

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// ultrasonic parameters
int number_of_sensors = 0;
Ultrasonic *sensors_array;

const int status_led_pin = 13;
const int trigger_pins[] = {12, 10, 8, 6};
const int echo_pins[]    = {11,  9, 7, 5};

const int pull_to_start_pin = 4;
const int selected_side_pin = A2;


// grippers pwm parameters
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
Gripper my_gripper = Gripper();
CommandInterpreter cmd_interperter = CommandInterpreter();

// TODO: fix crashes
// TODO: fix { {, issues with sequence parsing
// TODO: use status_led_pin

void initialize_ultrasonic_sensors()
{
  // count the number of pins defined
  number_of_sensors = min(sizeof(trigger_pins), sizeof(echo_pins)) / sizeof(int);

  // create a buffer to store the sensor objects
  sensors_array = new Ultrasonic[number_of_sensors];

  // initialize sensors
  for (int i = 0; i < number_of_sensors; i++)
  {
    sensors_array[i].initialize(
      trigger_pins[i],
      echo_pins[i]
    );
  }
}

void __debug()
{
  Serial.println("Enter your command?");
}

void setup()
{
  Serial.begin(115200);
  Serial.setTimeout(1);

  pinMode(status_led_pin, OUTPUT);
  pinMode(pull_to_start_pin, INPUT_PULLUP);
  pinMode(selected_side_pin, INPUT_PULLUP);
  
  my_gripper.initialize(&pwm, A0);
  enable_grippers();

  initialize_ultrasonic_sensors();
  cmd_interperter.initialize('{', '}', ',');

  Scheduler.startLoop(command_execution_handler);
  Scheduler.startLoop(data_feedback_handler);
}

void loop()
{
  digitalWrite(status_led_pin, HIGH);
  delay(1000);
  digitalWrite(status_led_pin, LOW);
  delay(1000);
}

void command_execution_handler()
{
  if (Serial.available() > 0)
  {
    cmd_interperter.parse_sequence(Serial.readString());
//    Serial.println("Enter your command?");
  }

  cmd_interperter.execute_next_instruction();
  Scheduler.delay(100);
}

void data_feedback_handler()
{
  Serial.println("{" + get_distances_as_string_array() + "," + get_auxillary_switches_as_json_str() + "}");
  Scheduler.delay(100);
}

void ultrasonic_sensor_handler()
{
  Serial.println("{" + get_distances_as_string_array() + "}");
  Scheduler.delay(100);
}

void move_gripper_jaw(const int jaw_id, const float new_angle)
{
//  Serial.print("Moving jaw ");
//  Serial.print(jaw_id);
//  Serial.print(" to ");
//  Serial.print(new_angle);
//  Serial.println(" rad");

  my_gripper.set_joint_position(jaw_id, new_angle);
}

void enable_grippers()
{
  my_gripper.enable_motors(true);
}

void disable_grippers()
{
  my_gripper.enable_motors(false);
}


String get_distances_as_string_array()
{
  char buff[10];
  String msg = "";
  
  for (int i=0; i<number_of_sensors; i++)
  {
    dtostrf((float) sensors_array[i].getDistance(), 4, 5, buff);
    
    if (i != 0)
    {
      msg = msg + "," + String(buff);
    }
    else
    {
      msg = msg + String(buff);
    }
  }
  
  return "\"d\":[" + msg + "]";
}

String get_auxillary_switches_as_json_str()
{
  return "\"p2s\":" + String(digitalRead(pull_to_start_pin)); // + ",\"side\":" + String(digitalRead(selected_side_pin));
}

