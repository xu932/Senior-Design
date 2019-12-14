
#define XY_1_PWM               (4)
#define XY_1_DIR               (8)

#define XY_2_PWM               (5)
#define XY_2_DIR               (9)

#define XY_EN                  (25)

#define LIQ_PWM                (19)
#define LIQ_EN1                (12)
#define LIQ_EN2                (13)
#define LIQ_EN3                (14)
#define LIQ_DIR                (2)

#define PWM_CH_NUM             (3)

// 8192 - 100%
#define LEDC_TEST_DUTY         (4096)


void motor_xy_init();
void motor_xy_home();
void motor_xy_move(uint32_t newX, uint32_t newY);

void pwm_init();
void _testmove(int i);
void _testpump(int p);