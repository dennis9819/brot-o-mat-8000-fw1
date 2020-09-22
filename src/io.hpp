#define addr_controller1 0x20

#define MCP23017_IODIRA 0x00   //!< I/O direction register A
#define MCP23017_IPOLA 0x02    //!< Input polarity port register A
#define MCP23017_GPINTENA 0x04 //!< Interrupt-on-change pins A
#define MCP23017_DEFVALA 0x06  //!< Default value register A
#define MCP23017_INTCONA 0x08  //!< Interrupt-on-change control register A
#define MCP23017_IOCONA 0x0A   //!< I/O expander configuration register A
#define MCP23017_GPPUA 0x0C    //!< GPIO pull-up resistor register A
#define MCP23017_INTFA 0x0E    //!< Interrupt flag register A
#define MCP23017_INTCAPA 0x10  //!< Interrupt captured value for port register A
#define MCP23017_GPIOA 0x12    //!< General purpose I/O port register A
#define MCP23017_OLATA 0x14    //!< Output latch register 0 A

#define MCP23017_IODIRB 0x01   //!< I/O direction register B
#define MCP23017_IPOLB 0x03    //!< Input polarity port register B
#define MCP23017_GPINTENB 0x05 //!< Interrupt-on-change pins B
#define MCP23017_DEFVALB 0x07  //!< Default value register B
#define MCP23017_INTCONB 0x09  //!< Interrupt-on-change control register B
#define MCP23017_IOCONB 0x0B   //!< I/O expander configuration register B
#define MCP23017_GPPUB 0x0D    //!< GPIO pull-up resistor register B
#define MCP23017_INTFB 0x0F    //!< Interrupt flag register B
#define MCP23017_INTCAPB 0x11  //!< Interrupt captured value for port register B
#define MCP23017_GPIOB 0x13    //!< General purpose I/O port register B
#define MCP23017_OLATB 0x15    //!< Output latch register 0 B

#define LCD_BATBAL 0x01  
#define LCD_BATPERC 0x02  
#define LCD_ROTS 0x03 
#define LCD_SPEED 0x04  
#define LCD_DIRECTION 0x05  
#define LCD_MOTOR 0x06  

#define pin_sda 4
#define pin_scl 5

//pin definitions
// PORTA
#define pa_dir 0x00
#define pa_m_s3 0x01 //active low
#define pa_sw_stop 0x02
#define pa_sw_reset 0x04
#define pa_sw_forward 0x08
#define pa_sw_backward 0x10
#define pa_res1 0x20
#define pa_res2 0x40
#define pa_res3 0x80

#define pb_dir 0b00011110
#define pb_led_stop 0x80
#define pb_led_reset 0x40
#define pb_led_forward 0x20
#define pb_led_backward 0x10
#define pb_m_dir 0x08
#define pb_m_en 0x04
#define pb_m_s0 0x02
#define pb_m_s1 0x01

#define lamp_blink 500

#define lmpode_off 0x02
#define lmpode_blink 0x01
#define lmpode_on 0x00




static int lampmodes[4];
static int lampstate[4];
static uint32_t lamplast[4];
void setLamp(int id, int mode);


void setupI2C();
void sendI2Cmessage(char msg[], int bytes);
void updateOut();
void outLamp(int id, int val);
void updateIO();
void updateLamps();
void sendDisplay(byte addr, byte data);
void setMotorTimer(uint16_t value);