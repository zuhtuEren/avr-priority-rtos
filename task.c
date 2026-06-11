#include "task.h"
#include <avr/io.h>
#include <avr/interrupt.h>

/* Global storage for RTOS structures */
TCB_t tcb_list[MAX_TASKS];
uint8_t stack[MAX_TASKS][STACK_SIZE];
volatile uint8_t current_task = 0;

/* * Initializes a task's private stack frame so that it appears to the CPU
 * as if it was interrupted by the ISR. This allows the RESTORE_CONTEXT
 * macro to cleanly "resume" the task for the first time.
 */
void task_create(uint8_t task_id, void (*task_code)(void), TaskState_t initial_state, uint8_t priority) {
    /* Point to the top of this task's allocated stack space (grows downwards) */
    uint8_t *sp = &stack[task_id][STACK_SIZE - 1];

    /* 1. Push the entry point address (Program Counter) */
    uint16_t address = (uint16_t)task_code;
    *sp = (uint8_t)(address & 0xFF);        
    sp--;
    *sp = (uint8_t)((address >> 8) & 0xFF); 
    sp--;

    /* 2. Push initial Register 0 (R0) */
    *sp = 0x00; 
    sp--;
    
    /* 3. Push initial Status Register (SREG) with Global Interrupts Enabled (0x80) */
    *sp = 0x80; 
    sp--;

    /* 4. Push the remaining 31 general-purpose registers (R1 - R31), initialized to 0 */
    for (int i = 1; i <= 31; i++) {
        *sp = 0x00; 
        sp--;
    }

    /* 5. Save the final stack pointer and task configuration into the TCB */
    tcb_list[task_id].stack_pointer = sp;
    tcb_list[task_id].task_state = initial_state;
    tcb_list[task_id].priority = priority;
    tcb_list[task_id].sleep_ticks = 0;
}

/* * Ignites the RTOS engine.
 * Naked attribute prevents compiler-generated prologue/epilogue.
 * Loads the stack pointer of Task 0 and executes a RESTORE_CONTEXT logic.
 */
void os_start(void) __attribute__((naked));
void os_start(void) {
    SP = (uint16_t)tcb_list[0].stack_pointer;
    
    asm volatile (
        "pop r31 \n\t" "pop r30 \n\t" "pop r29 \n\t" "pop r28 \n\t"
        "pop r27 \n\t" "pop r26 \n\t" "pop r25 \n\t" "pop r24 \n\t"
        "pop r23 \n\t" "pop r22 \n\t" "pop r21 \n\t" "pop r20 \n\t"
        "pop r19 \n\t" "pop r18 \n\t" "pop r17 \n\t" "pop r16 \n\t"
        "pop r15 \n\t" "pop r14 \n\t" "pop r13 \n\t" "pop r12 \n\t"
        "pop r11 \n\t" "pop r10 \n\t" "pop r9 \n\t"  "pop r8 \n\t"
        "pop r7 \n\t"  "pop r6 \n\t"  "pop r5 \n\t"  "pop r4 \n\t"
        "pop r3 \n\t"  "pop r2 \n\t"  "pop r1 \n\t"  "pop r0 \n\t"
        "out %0, r0 \n\t" 
        "pop r0 \n\t"
        "reti \n\t"       
        :: "I" (_SFR_IO_ADDR(SREG))
    );
}

/* * Suspends the current task for a specified number of system ticks (milliseconds).
 * This yields the CPU to other ready tasks immediately upon the next timer tick.
 */
void os_delay(uint16_t ticks) {
    cli(); /* Disable interrupts to prevent race conditions during state update */
    
    tcb_list[current_task].sleep_ticks = ticks;  
    tcb_list[current_task].task_state = TASK_BLOCKED; 
    
    sei(); /* Re-enable interrupts */
    
    /* CPU idles here for this task, but the ISR will preempt and switch context */
    while (tcb_list[current_task].task_state == TASK_BLOCKED);  
}