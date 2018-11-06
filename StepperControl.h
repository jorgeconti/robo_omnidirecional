
// ensure this library description is only included once
#ifndef StepperControl_h
#define StepperControl_h

// library interface description
class StepperControl {
  public:
    // constructors:
    StepperControl(int number_of_steps, int motor_pin_1, int motor_pin_3,
                                 int motor_pin_2, int motor_pin_4);

	void checkStepper(void);


    // speed setter method:
    void setSpeed(long whatSpeed);

    // mover method:
    void step(int steps_to_move);
	
	void quantas_bobinas_ativadas(int num);  
	
    void stopMotor(void);  

    int statusMotor(void);  
	
    int version(void);

  private:
    void stepMotor(int this_step);

    int direction;            // Direction of rotation
    unsigned long step_delay; // delay between steps, in ms, based on speed
    int number_of_steps;      // total number of steps this motor can take
    int Bobinas_acionadas;            // how many pins are in use.
    int step_number;          // which step the motor is on

    // motor pin numbers:
    int motor_pin_1;
    int motor_pin_2;
    int motor_pin_3;
    int motor_pin_4;

    int motor_ativo;

    unsigned long last_step_time; // time stamp in us of when the last step was taken
};

#endif

