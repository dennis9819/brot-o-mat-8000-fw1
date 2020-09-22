#include <Wire.h>
#include <Arduino.h>
#include "io.hpp"
#include "motion.hpp"

char pa = 0x00;
char pb = 0x00;

uint32_t dly_1 = 0;
uint32_t dly_2 = 0;
uint32_t dly_3 = 0;
uint32_t dly_4 = 0;

byte inputs_last = 0;

extern struct Motion motion;

void updateLamps(){
    if (motion.direction == 1){
        setLamp(1,0);
        setLamp(2,2);
        setLamp(3,0);
    }else if (motion.direction == 2){
        setLamp(2,0);
        setLamp(3,0);
        setLamp(1, (motion.speed == 10) + 1);
    }else if (motion.direction == 0){
        setLamp(1,0);
        setLamp(2,0);
        setLamp(3, (motion.speed == 10) + 1);
    }
}

ICACHE_RAM_ATTR void panelPress() {
    noInterrupts();
    //Serial.println("BUTTON DETECTED!!!");
    Wire.beginTransmission(0x20);
    Wire.write(0x13); // set MCP23017 memory pointer to GPIOB address
    Wire.endTransmission();
    Wire.requestFrom(0x20, 1); // request one byte of data from MCP20317
    byte inputs = Wire.read();
    delay(20); // for debounce
    //change
    if (inputs_last != inputs){
        byte changes = inputs^inputs_last;
        inputs_last = inputs;
        if ((changes & 0x02) > 0){
            Serial.println("Button 1");
            if (motion.direction == 1){
                motion.direction = 0;
                changeDir();
            } else if (motion.direction == 0){
                if ((inputs & 0x02) > 0 && motion.speed < 10){
                    motion.speed++;
                    changeSpeed();
                }
            } else if (motion.direction == 2){
                if ((inputs & 0x02) > 0 && motion.speed > 1){
                    motion.speed--;
                    changeSpeed();
                }
            } 
        }
        if ((changes & 0x04) > 0){
            Serial.println("Button 2");
            motion.direction = 1;
            changeDir();
        }
        if ((changes & 0x08) > 0){
            Serial.println("Button 3");
            if (motion.direction == 1){
                motion.direction = 2;
                changeDir();
            } else if (motion.direction == 0){
                if ((inputs & 0x08) > 0 && motion.speed > 1){
                    motion.speed--;
                    changeSpeed();
                }
            } else if (motion.direction == 2){
                if ((inputs & 0x08) > 0 && motion.speed < 10){
                    motion.speed++;
                    changeSpeed();
                }
            }
        }
        if ((changes & 0x10) > 0){
            Serial.println("Button 4");
            
        }
    }
    interrupts();
    
}

void sendI2Cmessage(char msg[], int bytes){
    Wire.beginTransmission(addr_controller1);
    for (int i = 0; i<bytes; i++){
        Wire.write(msg[i]); 
    }
    Wire.endTransmission(); 
}

void setupI2C(){
    Wire.begin(pin_sda,pin_scl);
    //test if connected
    Wire.beginTransmission(addr_controller1);
    int error = Wire.endTransmission();
    if (error != 0)
    {
        Serial.println("IO Controller not found!");
        ESP.reset();
    }
    //setup registers
    char mcp_io_a[] = {MCP23017_IODIRA,0x00};
    sendI2Cmessage(mcp_io_a,2);
    char mcp_io_b[] = {MCP23017_IODIRB,0b00011110};
    sendI2Cmessage(mcp_io_b,2);
    char mcp_gppu_a[] = {MCP23017_GPPUA,0xFF};
    sendI2Cmessage(mcp_gppu_a,2);
    char mcp_gppu_b[] = {MCP23017_GPPUB,0b00011110};
    sendI2Cmessage(mcp_gppu_b,2);

    //enable interrupt for front panel buttons
    char mcp_ipol_b[] = {MCP23017_IPOLB,0b00011110};
    sendI2Cmessage(mcp_ipol_b,2);

    char mcp_gpinten_b[] = {MCP23017_GPINTENB,0b00011110};
    sendI2Cmessage(mcp_gpinten_b,2);

    char mcp_iocon[] = {MCP23017_IOCONB ,0b00000010};
    sendI2Cmessage(mcp_iocon,2);

    //set motor speed
    char mot[] = {LCD_MOTOR,0xFF,0x00};
    sendI2Cmessage(mot,3);

    pb |= pb_m_en;
    updateOut();

    for(int i =0; i < 4; i++){
        lampmodes[i] = 0;
        lampstate[i] = 0;
        lamplast[i] = 0;
    }

    attachInterrupt(digitalPinToInterrupt(14), panelPress, CHANGE);
    panelPress();
}

void updateOut(){
    char mcp_gpio_a[] = {0x12,pb};
    sendI2Cmessage(mcp_gpio_a,2);
    char mcp_gpio_b[] = {0x13,pa};
    sendI2Cmessage(mcp_gpio_b,2);
    //Serial.println(String(mcp_gpio_a[0], BIN));
    Serial.println(String(pb, BIN));

}

void setLamp(int id, int mode){
    lampmodes[id] = mode;
    lampstate[id] = (mode == 1 || mode == 2);
    if (mode == 1){
        lamplast[id] == millis();
    }
    outLamp(id,lampstate[id]);
    char mcp_gpio_a[] = {0x12,pb};
    sendI2Cmessage(mcp_gpio_a,2);
}

void updateIO(){
    bool changed = false;
    for(int i =0; i < 4; i++){
        if (lampmodes[i] == 1 && (lamplast[i] + lamp_blink) < millis()){
            lampstate[i] = (~lampstate[i]) & 0x01;
            outLamp(i,lampstate[i]);
            changed = true;
            lamplast[i] = millis();
        }
    }
    if(changed){
        char mcp_gpio_a[] = {0x12,pb};
        sendI2Cmessage(mcp_gpio_a,2);
    }
    
}

void outLamp(int id, int val){
    if (val == 1){
        pb |= (1<<(id+4));
    }else{
        pb &= ~(1<<(id+4));
    }
}


void sendDisplay(byte addr, byte data){
    Wire.beginTransmission(0x08);
    Wire.write(addr); 
    Wire.write(data); 
    Wire.endTransmission(); 
}

void setMotorTimer(uint16_t value){
    char mot[] = {LCD_MOTOR,(uint8_t)value & 0xff ,(uint8_t)(value >> 8) };
    Wire.beginTransmission(0x08);
    for (int i = 0; i<3; i++){
        Wire.write(mot[i]); 
    }
    Wire.endTransmission(); 
}