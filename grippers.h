#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

class Gripper
{
  public:
    Gripper();

    void initialize(Adafruit_PWMServoDriver *pwm_driver, int enable_pin);
    void destroy();

    float convert_to_radians(const float angle_deg) const; // receives degrees
    bool set_joint_positions(const float angle_1, const float angle_2, const float angle_3); // uses radians
    bool set_joint_position(const int joint_id, const float angle); // uses radians

    void enable_motors(bool state);

  private:
    int _enable_pin;
    
    Adafruit_PWMServoDriver *_pwm_driver;
    uint16_t _SERVO_MIN;
    uint16_t _SERVO_MAX;

    uint16_t _calculate_pulselen(const float angle) const; // usese radians
};

Gripper::Gripper()
{
  _SERVO_MIN = 150;
  _SERVO_MAX = 600;
}

void Gripper::initialize(Adafruit_PWMServoDriver *pwm_driver, int enable_pin)
{
  _pwm_driver = pwm_driver;
  
  _pwm_driver->begin();
  _pwm_driver->setPWMFreq(60);

  _enable_pin = enable_pin;
  pinMode(_enable_pin, OUTPUT);
}

void Gripper::destroy()
{
}

/**
 * {@angle_1, @angle_2, @angle_3 uses radians}
 * 
 * remember to wait for the servos to reach the specified degree
 */
bool Gripper::set_joint_positions(const float angle_1, const float angle_2, const float angle_3)
{
  set_joint_position(0, angle_1);
  set_joint_position(1, angle_2);
  set_joint_position(2, angle_3);

  return true;
}

/**
 * @ joint_id: defines the joint to be moved
 * {@angle uses radians}
 * 
 * remember to wait for the servos to reach the specified degree
 */
bool Gripper::set_joint_position(const int joint_id, const float angle)
{
  float a = constrain(angle, 0, PI);

  if (joint_id == 2)
  {
//    Serial.print("a: ");
//    Serial.println(a);

    a = PI - a;

//    Serial.print("a: ");
//    Serial.println(a);
  }

  _pwm_driver->setPWM(joint_id, 0, _calculate_pulselen(a));
  return true;
}

void Gripper::enable_motors(bool state)
{
  digitalWrite(_enable_pin, !state);
}

/**
 * {@angle uses radians}
 */
uint16_t Gripper::_calculate_pulselen(const float angle) const
{
  return ((angle / PI) * (_SERVO_MAX)) + _SERVO_MIN;
}

float Gripper::convert_to_radians(const float angle_deg) const // receives degrees
{
  return (angle_deg / PI) * 180.0;
}





