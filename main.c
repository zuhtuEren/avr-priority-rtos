#include "timer0.h"
#include "driver.h"
#include "task.h"

void task_green(void) {
    while(1) {
        if (button_read(BTN_GREEN)) {
            led_on(LED_GREEN);
            os_delay(3000); 
            led_off(LED_GREEN);
        }
        // Eğer butona basılmadıysa 10 ms uyu.
        os_delay(10); 
    }
}

void task_yellow(void) {
    while(1) {
        if (button_read(BTN_YELLOW)) {
            led_on(LED_YELLOW);
            os_delay(3000);     
            led_off(LED_YELLOW);
        }
        // Subay, Er'in nefes alması için işlemciyi devrediyor.
        os_delay(10); 
    }
}

void task_red(void) {
    while(1) {
        if (button_read(BTN_RED)) {
            led_on(LED_RED);
            os_delay(3000);     
            led_off(LED_RED);
        }
        // General, alt rütbelilerin butonlarını okuması için 10ms uyuyor.
        os_delay(10); 
    }
}

int main(void) {
    driver_init();    
    timer0_init();

    task_create(0, task_green, TASK_READY, 1);   
    task_create(1, task_yellow, TASK_READY, 2);  
    task_create(2, task_red, TASK_READY, 3);     

    os_start();

    return 0; 
}