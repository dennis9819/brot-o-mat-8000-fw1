#define pin_Step 0 //GPIO0---D3 of Nodemcu--Step of  stepper motor driver

#define steps_per_rev 2566

#define accel 20
#define accl_dly 20


enum MotionDirection { backward, stop, forward };

static uint32_t t_last;
static uint32_t t_draw;
static uint32_t t_delay;

static uint32_t t_sec;

struct Motion {
    int direction;
    int speed;
    int steps;
    double lastspeed;
};

void setupDriver();
void updateDriver();
void tstep();

void changeDir();
void changeSpeed();