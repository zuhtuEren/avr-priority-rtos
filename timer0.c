#include "timer0.h"
#include "task.h"
#include <avr/interrupt.h>

volatile unsigned long system_time_ms = 0;

/* * Configures Timer0 to generate an interrupt every 1 millisecond.
 * Uses CTC (Clear Timer on Compare Match) mode.
 */
void timer0_init(void) {
    TCCR0A |= (1 << WGM01);              
    TCCR0B |= (1 << CS00) | (1 << CS01); 
    OCR0A = 249;
    TIMSK0 |= (1 << OCIE0A);
}

/* * Macro to push all CPU registers to the current task's stack.
 * Must be executed exactly as an ISR starts.
 */
#define SAVE_CONTEXT() \
    asm volatile ( \
        "push r0 \n\t" \
        "in r0, %0 \n\t" \
        "push r0 \n\t" \
        "clr r1 \n\t" \
        "push r1 \n\t" \
        "push r2 \n\t" "push r3 \n\t" "push r4 \n\t" "push r5 \n\t" \
        "push r6 \n\t" "push r7 \n\t" "push r8 \n\t" "push r9 \n\t" \
        "push r10 \n\t" "push r11 \n\t" "push r12 \n\t" "push r13 \n\t" \
        "push r14 \n\t" "push r15 \n\t" "push r16 \n\t" "push r17 \n\t" \
        "push r18 \n\t" "push r19 \n\t" "push r20 \n\t" "push r21 \n\t" \
        "push r22 \n\t" "push r23 \n\t" "push r24 \n\t" "push r25 \n\t" \
        "push r26 \n\t" "push r27 \n\t" "push r28 \n\t" "push r29 \n\t" \
        "push r30 \n\t" "push r31 \n\t" \
        :: "I" (_SFR_IO_ADDR(SREG)) \
    )

/* * Macro to pop all CPU registers from the new task's stack.
 * Must be executed right before exiting the ISR to resume the new task.
 */
#define RESTORE_CONTEXT() \
    asm volatile ( \
        "pop r31 \n\t" "pop r30 \n\t" "pop r29 \n\t" "pop r28 \n\t" \
        "pop r27 \n\t" "pop r26 \n\t" "pop r25 \n\t" "pop r24 \n\t" \
        "pop r23 \n\t" "pop r22 \n\t" "pop r21 \n\t" "pop r20 \n\t" \
        "pop r19 \n\t" "pop r18 \n\t" "pop r17 \n\t" "pop r16 \n\t" \
        "pop r15 \n\t" "pop r14 \n\t" "pop r13 \n\t" "pop r12 \n\t" \
        "pop r11 \n\t" "pop r10 \n\t" "pop r9 \n\t" "pop r8 \n\t" \
        "pop r7 \n\t" "pop r6 \n\t" "pop r5 \n\t" "pop r4 \n\t" \
        "pop r3 \n\t" "pop r2 \n\t" "pop r1 \n\t" "pop r0 \n\t" \
        "out %0, r0 \n\t" \
        "pop r0 \n\t" \
        "reti \n\t" \
        :: "I" (_SFR_IO_ADDR(SREG)) \
    )

/* * The Core Dispatcher / Scheduler.
 * Fires every 1ms. Naked attribute prevents compiler interference.
 */
ISR(TIMER0_COMPA_vect, ISR_NAKED) {
    /* 1. Freeze the current task and store its stack pointer */
    SAVE_CONTEXT();
    tcb_list[current_task].stack_pointer = (volatile uint8_t*)SP;

    /* 2. Increment global uptime counter */
    system_time_ms++;

    /* 3. Alarm Clock Mechanism: Decrement sleep ticks for blocked tasks */
    for (int i = 0; i < MAX_TASKS; i++) {
        if (tcb_list[i].task_state == TASK_BLOCKED) { 
            if (tcb_list[i].sleep_ticks > 0) {
                tcb_list[i].sleep_ticks--;       
            }
            if (tcb_list[i].sleep_ticks == 0) {
                tcb_list[i].task_state = TASK_READY;  /* Wake up the task! */
            }
        }
    }

    /* 4. Priority-Based Preemptive Scheduler */
    uint8_t highest_priority_task_id = 0;  
    int16_t highest_priority = -1;  

    /* Find the highest priority task that is currently READY */
    for (int i = 0; i < MAX_TASKS; i++) {    
        if (tcb_list[i].task_state == TASK_READY && (int16_t)tcb_list[i].priority > highest_priority) {
            highest_priority_task_id = i;         
            highest_priority = tcb_list[i].priority;  
        }
    }
    
    /* 5. Switch context to the winner and resume execution */
    current_task = highest_priority_task_id;
    SP = (uint16_t)tcb_list[current_task].stack_pointer;
    RESTORE_CONTEXT();
}