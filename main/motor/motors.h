#ifndef MOTORS_H
#define MOTORS_H

void motor_break_A();
void motor_break_B();
void motor_forward_A();
void motor_backward_A();
void motor_forward_B();
void motor_backward_B();
void motor_set_speed_A();
void motor_set_speed_B();
void init_mcpwm();
// void motor_task_init();
#endif