/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

#include <project.h>
#include <stdint.h>
#include "o_dmni.h"

/// @brief Send I2C data
/// @param p Pointer to structure holding the data about the module
/// @param cwp pwm number
/// @param pwm pwm value
void odmni_sendi2cdata(struct o_dmni *p,uint8_t cwp, uint16_t *pwm){
    int i=0;
    uint8 pwmbyte[9];
    uint8 *pwmptr = (uint8*)pwm;
    pwmbyte[0] = cwp;
    if (pwm){
        for (i = 0; i< 8; i++)
            pwmbyte[i+1] = pwmptr[i];
        I2CM_I2CMasterSendStart(p->I2Caddr, I2CM_I2C_WRITE_XFER_MODE, 1);
        for(i = 0; i<9; i++)
            I2CM_I2CMasterWriteByte(pwmbyte[i], 1);
        I2CM_I2CMasterSendStop(1);
    }
    else
        I2CM_I2CMasterWriteBuf(p->I2Caddr,&cwp,1,I2CM_I2C_MODE_COMPLETE_XFER);    
}

/// @brief Receive I2C data
/// @param p Pointer to structure holding the data about the module
/// @param pwmNum pwm number
/// @return 
uint32_t odmni_receivei2cdata(struct o_dmni *p,uint8_t pwmNum){
    uint8_t ptr_byte[4];
    p->cmd_with_pwmNum |= pwmNum;

    odmni_sendi2cdata(p,p->cmd_with_pwmNum, NULL);
    
    I2CM_I2CMasterReadBuf(p->I2Caddr,ptr_byte,4,I2CM_I2C_MODE_COMPLETE_XFER);
    uint32_t *ptr4byte = (uint32_t*)ptr_byte;
    return *ptr4byte;
}

/// @brief Initialize omni motor driver
/// @param p Pointer to structure holding the data about the module
/// @param I2C_address I2C address of the omni motor driver
void odmni_init(struct o_dmni *p, uint8_t I2C_address){
    p->I2Caddr = I2C_address;
    p->period = 1023;
    p->period2 = 511;
}

/// @brief Set frequency of the PWM
/// @param p Pointer to structure holding the data about the module
/// @param mode 0 - 10 KHz, 1 - 20 KHz
void odmni_chgfreq(struct o_dmni *p, uint8 mode){
    uint16_t pwm[4] = {0,0,0,0};
    if (mode == 0){
        pwm[0] = 1;
        p->period = 331;
        p->period2 = 165;
    }
    if (mode == 1){
        pwm[0] = 2;
        p->period = 1023;
        p->period2 = 511;
    }
    odmni_sendi2cdata(p,4, pwm);

}

/// @brief Stop the motors
/// @param p Pointer to structure holding the data about the module
/// @param motornum motor number which has to be stoped
void odmni_stopMotor(struct o_dmni *p, uint8_t motornum){
    uint16_t pwm[4] = {0,0,0,0};
    if (motornum & M1) pwm[0] = 1;
    if (motornum & M2) pwm[1] = 1;
    odmni_sendi2cdata(p,0, pwm);
}

/// @brief Start the motors
/// @param p Pointer to structure holding the data about the module
/// @param motornum motor number which has to be started
void odmni_startMotor(struct o_dmni *p, uint8_t motornum){
    uint16_t pwm[4] = {0,0,0,0};
    if (motornum & M1) pwm[0] = 1;
    if (motornum & M2) pwm[1] = 1;
    odmni_sendi2cdata(p,1, pwm);
}

/// @brief Set the motor pwm
/// @param p Pointer to structure holding the data about the module
/// @param motorNum motor number
/// @param speed pwm value
void odmni_setDCspeed_lin(struct o_dmni *p,uint8_t motorNum, float speed){
    uint16_t pwm[4];
    uint16_t duty;
    duty = ((p->period2/100.0*(speed))+p->period2);
    duty = (speed<-99.9)?0:duty;
    
    switch(motorNum)
    {
        case M1:  p->cmd_with_pwmNum = 0B10001001;  
                 pwm[0] = duty; break;
        case M2:  p->cmd_with_pwmNum = 0B10001010;
                 pwm[1] = duty; break;
    }
    odmni_sendi2cdata(p,p->cmd_with_pwmNum, pwm);
}

/// @brief Set the both motor speeds simultaneously
/// @param p Pointer to structure holding the data about the module
/// @param speed0 PWM value of first motor
/// @param speed1 PWM value of second motor
void odmni_setDCspeed2_lin(struct o_dmni *p,float speed0, float speed1){
    uint16_t pwm[4];
    p->cmd_with_pwmNum = 0B10001000;
    
    pwm[0] = ((p->period2/100.0*(speed0))+p->period2);
    pwm[1] = ((p->period2/100.0*(speed1))+p->period2);
 
    pwm[0] = (speed0<-99.9)?0:pwm[0];
    pwm[1] = (speed1<-99.9)?0:pwm[1];
    
    odmni_sendi2cdata(p,p->cmd_with_pwmNum, pwm);
}

/// @brief Set pwm in heater configuration
/// @param p Pointer to structure holding the data about the module
/// @param motorNum Heater number
/// @param duty PWM value for the heater
void odmni_setHpwm_lin(struct o_dmni *p,uint8_t motorNum, float duty){
    uint16_t pwm[4];
    uint8_t cmd=0;
    duty = ((duty>100)?duty/100.0:1) * p->period;
    duty = duty < 0? 0:duty;
    
    switch(motorNum)
    {
        case M1:  cmd = 0B10001001;  
                 pwm[0] = duty; break;
        case M2:  cmd = 0B10001010;
                 pwm[1] = duty; break;
    }
    odmni_sendi2cdata(p,cmd, pwm);
}

/// @brief Set heater value for both heaters simultaneously
/// @param p Pointer to structure holding the data about the module
/// @param duty0 duty cycle of first heater
/// @param duty1 duty cycle of second heater
void odmni_setHpwm2_lin(struct o_dmni *p,float duty0, float duty1){
    uint16_t pwm[4];
    uint8_t cmd = 0B10001000;
    
    pwm[0] = (duty0<100?duty0/100.0:1)*p->period;
    pwm[1] = (duty1<100?duty1/100.0:1)*p->period;

    pwm[0] = pwm[0]<0?0:pwm[0];    
    pwm[1] = pwm[1]<0?0:pwm[1];
    
    odmni_sendi2cdata(p,cmd, pwm);
}

/// @brief Set Transision rate for PWM between 0 and 100%
/// @param p Pointer to structure holding the data about the module
/// @param rate0 rate for first motor
/// @param rate1 rate for second motor
void odmni_setRate(struct o_dmni *p,uint16_t rate0, uint16_t rate1){   
    p->cmd_with_pwmNum = 3;

    p->present_rate[0] = rate0 * 10;
    p->present_rate[1] = rate1 * 10;
    p->present_rate[2] = rate0 * 10;
    p->present_rate[3] = rate1 * 10;
    
    odmni_sendi2cdata(p, 3, p->present_rate);
}

/// @brief Reset the driver
/// @param p Pointer to structure holding the data about the module
void odmni_picReset(struct o_dmni *p){
  uint16_t pwm[4] = {0,0,0,0};
  odmni_sendi2cdata(p,0xF0, pwm);
}

/// @brief Set I2C address for the module
/// @param p Pointer to structure holding the data about the module
/// @param addr address
void odmni_setI2Caddr(struct o_dmni *p,uint8_t addr){
  UART_AMOSI_PutChar(0B10000000);
  UART_AMOSI_PutChar(0B11110110);
  UART_AMOSI_PutChar(0B10000001);
  UART_AMOSI_PutChar(0xCC);
  UART_AMOSI_PutChar(addr);
  p->I2Caddr = addr;
}

/// @brief Read speed of the motor
/// @param p Pointer to structure holding the data about the module
/// @param motorNum motor number
/// @return pwm value
float odmni_readDCspeed(struct o_dmni *p,uint8_t motorNum){
    uint16_t duty;
    float speed;
    uint8 cmd;
    uint8_t ptr_byte[4];

    cmd = 0B10010000 | motorNum;

    odmni_sendi2cdata(p,cmd, NULL);
    
    I2CM_I2CMasterReadBuf(p->I2Caddr,ptr_byte,4,I2CM_I2C_MODE_COMPLETE_XFER);
    uint32_t *ptr4byte = (uint32_t*)ptr_byte;
    duty = *ptr4byte;
    speed = ((duty-511.0)/5.12);

    return speed;
}


/// @brief Read PWM rate
/// @param p Pointer to structure holding the data about the module
/// @param motorNum motor number
/// @return pwm value
uint16_t odmni_readRate(struct o_dmni *p,uint8_t motorNum){
    uint16_t rate;
    uint8 cmd;
    uint8_t ptr_byte[4];
    
    cmd = 0B10011000 | motorNum;
    odmni_sendi2cdata(p,cmd, NULL);
    
    I2CM_I2CMasterReadBuf(p->I2Caddr,ptr_byte,4,I2CM_I2C_MODE_COMPLETE_XFER);
    uint32_t *ptr4byte = (uint32_t*)ptr_byte;

    rate = *ptr4byte/10.0;

    return rate;
}

void odmni_holdDCSpeed(struct o_dmni *p,float speed0, float speed1){
    uint8 cmd = 0B10001111;
    uint16_t pwm[4] = {0,0,0,0};
    
    pwm[0] = ((5.12*(speed0))+511);
    pwm[1] = ((5.12*(speed1))+511);


    pwm[0] = (speed0<-99.9)?0:pwm[0];
    pwm[1] = (speed1<-99.9)?0:pwm[1];
    
    odmni_sendi2cdata(p, cmd, pwm);
}

void odmni_executeHold(struct o_dmni *p){
    uint8 cmd = 0B10000111;
    uint16_t pwm[4] = {0,0,0,0};
    
    odmni_sendi2cdata(p,cmd, pwm);
}

/// @brief Set stepper motor mode
/// @param p Pointer to structure holding the data about the module
/// @param mode Stepper motor mode 0 - 127
void odmni_STPmotormode(struct o_dmni *p,uint8_t mode){
    uint8_t cmd= 0B10101000;
    uint16_t pwm[4];
    
    pwm[0] = mode; 
    pwm[1] = 1;
    pwm[2] = 0; 
    pwm[3] = 0;
    
    odmni_sendi2cdata(p,cmd, pwm);
}

void odmni_STPsetmode(struct o_dmni *p,uint8_t mode){
    uint16_t pwm[4];
    uint8_t cmd = 0B10101000;
    
    pwm[0] = mode; 
    pwm[1] = 1;
    
    odmni_sendi2cdata(p,cmd, pwm);
}

/// @brief Read stepper motor PWM
/// @param p Pointer to structure holding the data about the module
/// @param motorNum motor number
/// @return 
float odmni_STPreadPWM(struct o_dmni *p,uint8_t motorNum){
    uint8 cmd;
    uint16_t duty;
    float speed;
    uint8_t ptr_byte[4];

    //motorNum = (motorNum!=1)?3:motorNum;
    cmd = 0B10010000 | motorNum;
    odmni_sendi2cdata(p,cmd, NULL);
    
    I2CM_I2CMasterReadBuf(p->I2Caddr,ptr_byte,4,I2CM_I2C_MODE_COMPLETE_XFER);
    uint32_t *ptr4byte = (uint32_t*)ptr_byte;
    duty = *ptr4byte;
    speed = ((duty-p->period2)/(p->period2/100));

    return speed;
}

/// @brief Set stepper motor position 
/// @param p Pointer to structure holding the data about the module
/// @param pos position of the stepper motor
/// @param speed speed of the motor
void odmni_STPsetPos(struct o_dmni *p,uint8_t pos, float speed){
    uint8_t cmd = 0xAC;
    uint16_t pwm[4], duty = ((speed>=0)?speed:-speed)/100 * p->period2;
    pwm[0] = pos;
    pwm[1] = duty;
    pwm[2] = 0xFFFF;
    pwm[3] = 0;
    
    odmni_sendi2cdata(p,cmd, pwm);
}


/* [] END OF FILE */