#include "Arduino.h"
#include "StepperControl.h"


/*
 *   constructor for four-pin version
 *   Sets which wires should control the motor.
 */
StepperControl::StepperControl(int number_of_steps, int motor_pin_1, int motor_pin_3,
                                      int motor_pin_2, int motor_pin_4)
{
  this->motor_ativo=1;
  this->step_number = 0;    // which step the motor is on
  this->direction = 0;      // motor direction
  this->last_step_time = 0; // time stamp in us of the last step taken
  this->number_of_steps = number_of_steps; // total number of steps for this motor

  // Arduino pins for the motor control connection:
  this->motor_pin_1 = motor_pin_1;
  this->motor_pin_2 = motor_pin_2;
  this->motor_pin_3 = motor_pin_3;
  this->motor_pin_4 = motor_pin_4;

  // setup the pins on the microcontroller:
  pinMode(this->motor_pin_1, OUTPUT);
  pinMode(this->motor_pin_2, OUTPUT);
  pinMode(this->motor_pin_3, OUTPUT);
  pinMode(this->motor_pin_4, OUTPUT);

  // When there are 4 pins, set the others to 0:

  // Bobina_dupla is used by the stepMotor() method:
  this->Bobinas_acionadas = 1;
}

void StepperControl::checkStepper(void)
{
int  tempodelay=100;
    digitalWrite(this->motor_pin_1, HIGH);
    digitalWrite(this->motor_pin_2, LOW);
    digitalWrite(this->motor_pin_3, LOW);
    digitalWrite(this->motor_pin_4, LOW);
  delay(tempodelay);
  digitalWrite(this->motor_pin_1, LOW);
    digitalWrite(this->motor_pin_2, HIGH);
    digitalWrite(this->motor_pin_3, LOW);
    digitalWrite(this->motor_pin_4, LOW);
  delay(tempodelay);
  digitalWrite(this->motor_pin_1, LOW);
    digitalWrite(this->motor_pin_2, LOW);
    digitalWrite(this->motor_pin_3, HIGH);
    digitalWrite(this->motor_pin_4, LOW);
  delay(tempodelay);
  digitalWrite(this->motor_pin_1, LOW);
    digitalWrite(this->motor_pin_2, LOW);
    digitalWrite(this->motor_pin_3, LOW);
    digitalWrite(this->motor_pin_4, HIGH);
  delay(tempodelay);
  digitalWrite(this->motor_pin_1, LOW);
    digitalWrite(this->motor_pin_2, LOW);
    digitalWrite(this->motor_pin_3, LOW);
    digitalWrite(this->motor_pin_4, LOW);
}

int StepperControl::statusMotor(void)
{
  return this->motor_ativo;
}
/*
 * Sets the speed in revs per minute
 */
void StepperControl::setSpeed(long whatSpeed)
{
  this->step_delay = 60L * 1000L * 1000L / this->number_of_steps / whatSpeed;
}

void StepperControl::quantas_bobinas_ativadas(int num)
{
  this->Bobinas_acionadas = num;
}

/*
 * Moves the motor steps_to_move steps.  If the number is negative,
 * the motor moves in the reverse direction.
 */
void StepperControl::step(int steps_to_move)
{
  int steps_left = abs(steps_to_move);  // how many steps to take
unsigned long last_step_time=0;
// determine direction based on whether steps_to_mode is + or -:
  if (steps_to_move > 0) { this->direction = 1; }
  if (steps_to_move < 0) { this->direction = 0; }


  // decrement the number of steps, moving one step each time:
  
  while (steps_left > 0)
  {
    unsigned long now = micros();
        if ((now - last_step_time) >= 10)
    {
      // get the timeStamp of when you stepped:
      last_step_time = now;
      if (this->direction == 1)
      {
        this->step_number++;
        if (this->step_number == this->number_of_steps) {
          this->step_number = 0;
        }
      }
      else
      {
        if (this->step_number == 0) {
          this->step_number = this->number_of_steps;
        }
        this->step_number--;
      }
      // decrement the steps left:
      steps_left--;

        stepMotor(this->step_number % 4);    
  }

  }
}

/*
 * Moves the motor forward or backwards.
 */
void StepperControl::stepMotor(int thisStep)
{
  this->motor_ativo=1;
  
  if (this->Bobinas_acionadas == 1) {
    switch (thisStep) {
      case 0:  // 1000
        digitalWrite(this->motor_pin_1, HIGH);
        digitalWrite(this->motor_pin_2, LOW);
        digitalWrite(this->motor_pin_3, LOW);
        digitalWrite(this->motor_pin_4, LOW);
      break;
      case 1:  // 0010
        digitalWrite(this->motor_pin_1, LOW);
        digitalWrite(this->motor_pin_2, LOW);
        digitalWrite(this->motor_pin_3, HIGH);
        digitalWrite(this->motor_pin_4, LOW);
      break;
      case 2:  //0100
        digitalWrite(this->motor_pin_1, LOW);
        digitalWrite(this->motor_pin_2, HIGH);
        digitalWrite(this->motor_pin_3, LOW);
        digitalWrite(this->motor_pin_4, LOW);
      break;
      case 3:  //0001
        digitalWrite(this->motor_pin_1, LOW);
        digitalWrite(this->motor_pin_2, LOW);
        digitalWrite(this->motor_pin_3, LOW);
        digitalWrite(this->motor_pin_4, HIGH);
      break;
    }
  }
  
   if (this->Bobinas_acionadas == 2) {
    switch (thisStep) {
      case 0:  // 1010
        digitalWrite(this->motor_pin_1, HIGH);
        digitalWrite(this->motor_pin_2, LOW);
        digitalWrite(this->motor_pin_3, HIGH);
        digitalWrite(this->motor_pin_4, LOW);
      break;
      case 1:  // 0110
        digitalWrite(this->motor_pin_1, LOW);
        digitalWrite(this->motor_pin_2, HIGH);
        digitalWrite(this->motor_pin_3, HIGH);
        digitalWrite(this->motor_pin_4, LOW);
      break;
      case 2:  //0101
        digitalWrite(this->motor_pin_1, LOW);
        digitalWrite(this->motor_pin_2, HIGH);
        digitalWrite(this->motor_pin_3, LOW);
        digitalWrite(this->motor_pin_4, HIGH);
      break;
      case 3:  //1001
        digitalWrite(this->motor_pin_1, HIGH);
        digitalWrite(this->motor_pin_2, LOW);
        digitalWrite(this->motor_pin_3, LOW);
        digitalWrite(this->motor_pin_4, HIGH);
      break;
    }
  }
}

void StepperControl::stopMotor(void){
        this->motor_ativo=0;
        digitalWrite(this->motor_pin_1, LOW);
        digitalWrite(this->motor_pin_2, LOW);
        digitalWrite(this->motor_pin_3, LOW);
        digitalWrite(this->motor_pin_4, LOW);
}

/*
  version() returns the version of the library:
*/
int StepperControl::version(void)
{
  return 1;
}
