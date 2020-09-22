#include <Arduino.h>
#include <motion.hpp>
#include "io.hpp"

extern struct Motion motion;
extern char pa;
extern char pb;

uint32_t lastSpeedCalc = 0;
long double vc = 0; //v current
long double vl = 0; //v current
long double vt = 0; //v target
long double vo = 0; //v origin
double a = 0;
double a_max = 20;
double dir = 1;
int phase = 10;
int lastState = 0; // 0: off //1: on

void setupDriver() {
    pinMode(pin_Step, OUTPUT); //Step pin as output
    digitalWrite(pin_Step, LOW); // Currently no stepper motor movement

    pb |= pb_m_en | pb_m_s0;
    pb &= ~(pb_m_dir | pb_m_s1);
    pa &= ~(pa_m_s3);

    updateOut();
    updateOut();

    t_last = 0;
    t_draw = 0;
    t_delay = 10;

    motion.speed = 5;
    motion.direction = 1;
    motion.steps = 0;
    motion.lastspeed = 0.0;
}

void tstep(){
    digitalWrite(pin_Step, HIGH);
    delayMicroseconds(5);
    digitalWrite(pin_Step, LOW);
}

double getTrigger(){
	double sr = ((double)a_max / accel);
	double t_vt = vt;
	double t_a = a_max;
	for (int i = 0; i < accel; i++) {
		t_a -= sr;
		t_vt -= t_a;
	}
	if (dir == -1) {
		t_vt = (vt - t_vt) + vt;
	}
	return t_vt;
}

void calcMaxAccel() {
	double v_target =((vt - vc) / 2.2);
	//bekannt dauer
	a_max = fabs(v_target / accel);
}

void updateDriver(){
    if(motion.direction == 1){
        vt = 0;
    }else{
        vt = motion.speed * 20;
    }
    if ((lastSpeedCalc + accl_dly) < millis()){
    lastSpeedCalc = millis();
    //calculate acceleration
        if (phase == 0) {
            double sr = ((double)a_max / accel); //inc per step
            a += sr;
            if (a >= a_max) {
                phase = 1;
            }
            vc += a * dir;
        }
        else if (phase == 1) {
        //constant
            vc += a * dir;
            if (vc >= getTrigger() && dir == 1) {
                phase = 2;
            } else if (vc <= getTrigger() && dir == -1) {
                phase = 2;
            }

        }
        else if (phase == 2) {
            double sr = ((double)a_max / accel); //inc per step
            a -= sr;
            vc += a * dir;
            if (a <= 0) {
                //stop 
                phase = 10;
                vc = vt;
                //end stop
            }
            if (dir == 1 && vc > vt) {
                vc = vt;
                phase = 10;
            }
            else if(dir == -1 && vc < vt) {
                vc = vt;
                phase = 10;
            }
        } 
        else if(phase == 10){
            if (vc != vt){
                calcMaxAccel();
                vo = vc;
                phase = 0;
                if (vc < vt) {
                    dir = 1;
                }
                else {
                    dir = -1;
                }
            }   
        }
        
    }
    if (vc > 200){
        vc = 200;
    }else if (vc < 0){
        vc = 0;
    }
   
    if(lastState == 0 && vc > 0){
        lastState = 1 ;
        pb &= ~(pb_m_en);
        updateOut();
    }else if(lastState == 1 && vc == 0){
        lastState = 0 ;
        pb |= pb_m_en;
        updateOut();
    }else{
        if (vc != vl && vc != 0){
            vl = vc;
            double ad = (1/vc);
            uint16_t tempSpd = ad*4000;
            Serial.print("ad: ");
            Serial.println(ad);
            Serial.print("tempSpd: ");
            Serial.println(tempSpd);
            setMotorTimer(tempSpd);
        }
    }


    if (t_sec + 1000 < millis()){
        t_sec = millis();
        double rev_pre_min = ((double)(motion.steps) / steps_per_rev)*60;
        motion.lastspeed = rev_pre_min;
        motion.steps = 0;
        sendDisplay(LCD_SPEED,(motion.speed) & 0xFF);
        sendDisplay(LCD_ROTS,(int)((double)motion.lastspeed * (double)10) & 0xFF);
    }
    
}

void changeDir(){
    switch(motion.direction){
      case 0:
          pb |= pb_m_dir;
          break;
      case 1:
          break;
      case 2:
          pb &= ~( pb_m_dir);
          break;
    }
    updateLamps();
    updateOut();
    sendDisplay(LCD_DIRECTION,(motion.direction) & 0xFF);
}

void changeSpeed(){  
    updateLamps();
}