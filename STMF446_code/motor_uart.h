#include "mbed.h"

PwmOut pwm1(D7);
PwmOut pwm1n(D11);
PwmOut pwm2(D8);
PwmOut pwm2n(A3);

//Motor1 sensor
InterruptIn HallA_1(A1);
InterruptIn HallB_1(A2);

//Motor2 sensor
InterruptIn HallA_2(D13);
InterruptIn HallB_2(D12);

int HALLA_1 = 0, HALLB_1 = 0, HALLA_2 = 0, HALLB_2 = 0;

int new_state_1 = 0, old_state_1 = 0, new_state_2 = 0, old_state_2 = 0;

float iSpeedCount_1 = 0, iSpeedCount_2 = 0;

float Velocity_1 = 0, Velocity_2 = 0;
float SPEED_CAL = 100/12*60/29;              //RPM

// PI variables
float VelocityError_1 = 0;
float VelocityError_2 = 0;
float VelocityErrorOld_1 = 0;
float VelocityErrorOld_2 = 0;
float PIOut_1 = 0;
float PIOut_2 = 0;
float PIOutOld_1 = 0;
float PIOutOld_2 = 0;
float T = 0.01;                 // sampling time
float Kp = 0.01;
float Ki = 0.10;

// velocity command
float VelocityRef_1 = 0;
float VelocityRef_2 = 0;

void init_PWM(void);
void CN_interrupt(void);
void init_CN(void);
void motor_PI(void);

void motor_PI(void)
{
    Velocity_1 = iSpeedCount_1 * SPEED_CAL;          
    iSpeedCount_1 = 0;
    
    VelocityError_1 = VelocityRef_1 - Velocity_1;     
    PIOut_1 = PIOutOld_1 + Kp*VelocityError_1 + (Ki*T-Kp)*VelocityErrorOld_1; 
    
    if(PIOut_1 >= 0.5f)
        PIOut_1 = 0.5f;
    else if(PIOut_1 <= -0.5f)
        PIOut_1 = -0.5f;
    
    pwm1.write(PIOut_1 + 0.5f);
    TIM1->CCER |= 0x4;
    
    PIOutOld_1 = PIOut_1;
    VelocityErrorOld_1 = VelocityError_1;
    
    Velocity_2 = iSpeedCount_2 * SPEED_CAL;          
    iSpeedCount_2 = 0;  

    VelocityError_2 = VelocityRef_2 - Velocity_2;    
    PIOut_2 = PIOutOld_2 + Kp*VelocityError_2 + (Ki*T-Kp)*VelocityErrorOld_2; 
    
    if(PIOut_2 >= 0.5f)
        PIOut_2 = 0.5f;
    else if(PIOut_2 <= -0.5f)
        PIOut_2 = -0.5f;
    
    pwm2.write(PIOut_2 + 0.5f);
    TIM1->CCER |= 0x40;
    
    PIOutOld_2 = PIOut_2;
    VelocityErrorOld_2 = VelocityError_2;
}

void CN_interrupt(void)
{
    // Motor 1
    HALLA_1 = HallA_1.read();
    HALLB_1 = HallB_1.read();
    
    if(HALLA_1==0 && HALLB_1==0)
        new_state_1 = 1;
    else if(HALLA_1==0 && HALLB_1==1)
        new_state_1 = 2;
    else if(HALLA_1==1 && HALLB_1==1)
        new_state_1 = 3;
    else if(HALLA_1==1 && HALLB_1==0)
        new_state_1 = 4;
    
    if(new_state_1-old_state_1== 1 || new_state_1-old_state_1== -3)        
    {
        iSpeedCount_1 = iSpeedCount_1 + 1;
        old_state_1 = new_state_1;
    }
    else if(new_state_1-old_state_1== -1 || new_state_1-old_state_1== 3)   
    {
        iSpeedCount_1 = iSpeedCount_1 - 1;
        old_state_1 = new_state_1;
    }
    
    //////////////////////////////////
    
    //Motor 2
    HALLA_2 = HallA_2.read();
    HALLB_2 = HallB_2.read();
    
    if(HALLA_2==0 && HALLB_2==0)
        new_state_2 = 1;
    else if(HALLA_2==0 && HALLB_2==1)
        new_state_2 = 2;
    else if(HALLA_2==1 && HALLB_2==1)
        new_state_2 = 3;
    else if(HALLA_2==1 && HALLB_2==0)
        new_state_2 = 4;
    
    if(new_state_2-old_state_2==1 || new_state_2-old_state_2==-3)          
    {
        iSpeedCount_2 = iSpeedCount_2 + 1;
        old_state_2 = new_state_2;
    }
    else if(new_state_2-old_state_2==-1 || new_state_2-old_state_2==3)    
    {
        iSpeedCount_2 = iSpeedCount_2 - 1;
        old_state_2 = new_state_2;
    }
}

void init_PWM(void)
{
    pwm1.period_us(50);
    pwm1.write(0.5);
    TIM1->CCER |= 0x4;
    
    pwm2.period_us(50);
    pwm2.write(0.5);
    TIM1->CCER |= 0x40;
}
 
void init_CN(void)
{
    HallA_1.rise(&CN_interrupt);
    HallA_1.fall(&CN_interrupt);
    HallB_1.rise(&CN_interrupt);
    HallB_1.fall(&CN_interrupt);
    
    HallA_2.rise(&CN_interrupt);
    HallA_2.fall(&CN_interrupt);
    HallB_2.rise(&CN_interrupt);
    HallB_2.fall(&CN_interrupt);
    
    HALLA_1 = HallA_1.read();
    HALLB_1 = HallB_1.read();
    HALLA_2 = HallA_2.read();
    HALLB_2 = HallB_2.read();
}

