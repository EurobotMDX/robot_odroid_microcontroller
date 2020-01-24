
/*
 ultrasonic.h - Library for controlling ping sensors
 Created by Chibuike I.P. Okpaluba October 29, 2015.
 ***************************************************
 To be Used by Chibuike, Haun, and Fredick for a
 robotic car project.
 
 This code can only be used by Middlesex Engineering Staffs
 and Students
 
 For more information e-mail Chibuike at <chibuikepraise@gmail.com>
*/


// Basically, this prevents problems if someone
// accidently #include's your library twice.
#pragma once

//the class definition goes here
class Ultrasonic
{
  public:
    Ultrasonic();
    initialize(const int trigPin, const int echoPin);
    float getDistance();
    
  private:
    float microSecondsToMeters();
    int _trigPin;
    int _echoPin;
    float _duration;

    long _last_called_time;
    float _last_distance;

    float _max_change_delta;
};

Ultrasonic::Ultrasonic()
{
}

Ultrasonic::initialize(const int trigPin, const int echoPin)
{
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  _trigPin = trigPin;
  _echoPin = echoPin;
  _duration = 0;

  _last_called_time = 0;
  _last_distance = 0;

  _max_change_delta = 3.0;
}

float Ultrasonic::getDistance()
{
  digitalWrite(_trigPin, LOW);
  delayMicroseconds(50);
  digitalWrite(_trigPin, HIGH);
  delayMicroseconds(30);
  digitalWrite(_trigPin, LOW);
  
  _duration = pulseIn(_echoPin, HIGH, 11765);
  _duration /= 2;

  // The speed of sound is 2.94117647 u_sec/mm
  float distance_m = _duration / (2941.17647);

  float time_diff = (float)(millis() - _last_called_time);
  float distance_delta = abs((distance_m - _last_distance) / time_diff);

  if (distance_delta < _max_change_delta || time_diff < 0)
  {
    _last_called_time = millis();

    if (distance_m < 0 || _duration <= 0)
    {
      distance_m = -1;
    }
    else
    {
      _last_distance = distance_m;
      return distance_m;
    } 
  }

  return _last_distance;
}






