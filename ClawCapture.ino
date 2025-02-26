#include <MeMegaPi.h>

const int CLAW_SPEED = 60;
const int MOTOR_SPEED = 100;
const int ARM_SPEED = 50;
const int INTERMEDIATE_DELAY = 1000;

MeUltrasonicSensor ultraSensor(PORT_8);
MeEncoderOnBoard Encoder_1(SLOT1);
MeEncoderOnBoard Encoder_2(SLOT2);
MeEncoderOnBoard Encoder_3(SLOT3);
MeMegaPiDCMotor claw(PORT4B);

void isr_process_encoder1(void)
{
  if(digitalRead(Encoder_1.getPortB()) == 0)
  {
    Encoder_1.pulsePosMinus();
  }
  else
  {
    Encoder_1.pulsePosPlus();
  }
}

void isr_process_encoder2(void)
{
  if(digitalRead(Encoder_2.getPortB()) == 0)
  {
    Encoder_2.pulsePosMinus();
  }
  else
  {
    Encoder_2.pulsePosPlus();
  }
}

void isr_process_encoder3(void)
{
  if(digitalRead(Encoder_3.getPortB()) == 0)
  {
    Encoder_3.pulsePosMinus();
  }
  else
  {
    Encoder_3.pulsePosPlus();
  }
}

void openClaw()
{
  claw.run(CLAW_SPEED);
  delay(6000);
  claw.stop();
}

void closeClaw()
{
  claw.run(-CLAW_SPEED);
  delay(6000);
  claw.stop();
}

void raiseClaw()
{
  Encoder_3.setMotorPwm(-ARM_SPEED);
  delay(3000);
  Encoder_3.setMotorPwm(0);
}

void lowerClaw()
{
  Encoder_3.setMotorPwm(ARM_SPEED);
  delay(2700);
  Encoder_3.setMotorPwm(0);
}

void moveStraight(bool back)
{
  if(!back)
  {
    Encoder_1.setMotorPwm(-MOTOR_SPEED);
    Encoder_2.setMotorPwm(MOTOR_SPEED);
    delay(1500);
    Encoder_1.setMotorPwm(0);
    Encoder_2.setMotorPwm(0);
  }
  else
  {
    Encoder_1.setMotorPwm(MOTOR_SPEED);
    Encoder_2.setMotorPwm(-MOTOR_SPEED);
    delay(1500);
    Encoder_1.setMotorPwm(0);
    Encoder_2.setMotorPwm(0);
  }
}

void rotate(bool back)
{
  if(!back)
  {
    Encoder_1.setMotorPwm(MOTOR_SPEED);
    Encoder_2.setMotorPwm(MOTOR_SPEED);
    delay(3000);
    Encoder_1.setMotorPwm(0);
    Encoder_2.setMotorPwm(0);
  }
  else
  {
    Encoder_1.setMotorPwm(-MOTOR_SPEED);
    Encoder_2.setMotorPwm(-MOTOR_SPEED);
    delay(3000);
    Encoder_1.setMotorPwm(0);
    Encoder_2.setMotorPwm(0);
  }
}

void setup()
{
  attachInterrupt(Encoder_1.getIntNum(), isr_process_encoder1, RISING);
  attachInterrupt(Encoder_2.getIntNum(), isr_process_encoder2, RISING);
  attachInterrupt(Encoder_3.getIntNum(), isr_process_encoder3, RISING);
  Serial.begin(115200);
  
  //Set PWM 8KHz
  TCCR1A = _BV(WGM10);
  TCCR1B = _BV(CS11) | _BV(WGM12);

  TCCR2A = _BV(WGM21) | _BV(WGM20);
  TCCR2B = _BV(CS21);

  Encoder_1.setPulse(7);
  Encoder_2.setPulse(7);
  Encoder_1.setRatio(26.9);
  Encoder_2.setRatio(26.9);
  Encoder_1.setPosPid(1.8, 0, 1.2);
  Encoder_2.setPosPid(1.8, 0, 1.2);
  Encoder_1.setSpeedPid(0.18, 0, 0);
  Encoder_2.setSpeedPid(0.18, 0, 0);
}

bool hasGrabbed = false;

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print("Distance : ");
  Serial.print(ultraSensor.distanceCm());
  Serial.println(" cm");
  delay(100);
  openClaw();
  while(true)
  {
    if(ultraSensor.distanceCm() < 10)
    {
      closeClaw();
      delay(INTERMEDIATE_DELAY);
      raiseClaw();
      delay(INTERMEDIATE_DELAY);
      moveStraight(false);
      delay(INTERMEDIATE_DELAY);
      rotate(true);
      delay(INTERMEDIATE_DELAY);
      lowerClaw();
      delay(INTERMEDIATE_DELAY);
      openClaw();
      delay(INTERMEDIATE_DELAY);
      rotate(false);
      delay(INTERMEDIATE_DELAY);
      moveStraight(true);
      delay(INTERMEDIATE_DELAY);
    }
  }
}
