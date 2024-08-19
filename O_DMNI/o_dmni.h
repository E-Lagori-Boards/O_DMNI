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

#ifndef O_DMNI_H
#define O_DMNI_H
    
#include <stdint.h>

#ifndef M1
#define M1 0b1
#endif

#ifndef M2
#define M2 0b10
#endif

struct o_dmni{
        uint8_t cmd_with_pwmNum;
        uint16_t present_rate[4];
        uint8_t I2Caddr;
        uint16_t period;
        uint16_t period2;
};

void odmni_picReset(struct o_dmni *);   
void odmni_init(struct o_dmni *, uint8_t I2C_address);
void odmni_chgfreq(struct o_dmni *p,uint8_t mode);
void odmni_startMotor(struct o_dmni *, uint8_t motornum);
void odmni_stopMotor(struct o_dmni *, uint8_t motornum);
void odmni_setDCspeed_lin(struct o_dmni *,uint8_t, float);
void odmni_setDCspeed2_lin(struct o_dmni *,float, float);
void odmni_setHpwm_lin(struct o_dmni *,uint8_t, float);
void odmni_setHpwm2_lin(struct o_dmni *,float, float);
void odmni_setRate(struct o_dmni *,uint16_t, uint16_t);
void odmni_setI2Caddr(struct o_dmni *,uint8_t);
float odmni_readDCspeed(struct o_dmni *,uint8_t);
uint16_t odmni_readRate(struct o_dmni *,uint8_t);
void odmni_holdDCSpeed(struct o_dmni *, float, float);
void odmni_executeHold(struct o_dmni *);
void odmni_STPsetPWM(struct o_dmni *p,float speed);
float odmni_STPreadPWM(struct o_dmni *p,uint8_t motorNum);
void odmni_STPholdPWM(struct o_dmni *p,float speed);
void odmni_STPexecuteHold(struct o_dmni *p);
void odmni_STPmotormode(struct o_dmni *p,uint8_t mode);
void odmni_STPsetmode(struct o_dmni *p,uint8_t mode);
void odmni_STPsetPos(struct o_dmni *p,uint8_t pos, float speed);
/* [] END OF FILE */
#endif