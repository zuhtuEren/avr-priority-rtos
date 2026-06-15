#ifndef TASK_H_
#define TASK_H_

#include <stdint.h>

/* System configuration limits */
#define STACK_SIZE 128
#define MAX_TASKS  3

/* Task lifecycle states defined via typedef for strict type safety */
typedef enum { 
    TASK_READY, 
    TASK_RUNNING, 
    TASK_BLOCKED 
} TaskState_t;

/* * Task Control Block (TCB)
 * Holds the execution context, state, and scheduling parameters of a task.
 */
typedef struct {
    volatile uint8_t* stack_pointer; 
    volatile TaskState_t task_state; /* DÜZELTİLDİ: volatile eklendi */
    uint8_t priority;                
    volatile uint16_t sleep_ticks;   
} TCB_t;

/* Global RTOS variables exposed for the Context Switcher */
extern TCB_t tcb_list[MAX_TASKS];
extern volatile uint8_t current_task;

/* RTOS Core API */
void task_create(uint8_t task_id, void (*task_code)(void), TaskState_t initial_state, uint8_t priority);
void os_start(void);
void os_delay(uint16_t ticks);

#endif
