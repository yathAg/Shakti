#include "platform.h"
#include "gpio.h"
int led_word = 0b0;


void off_led(){
    write_word(GPIO_DATA_REG, 0x00);
    printf("GPIO Data Register Value: %x\n",GPIO_DATA_REG);
}

void set_led(int8_t x,int8_t y){
    switch(y){
        case 0:
            led_word = 0b1110;
           
            break;
        case 1:
            led_word = 0b1101;
            
            break;
        case 2:
            led_word = 0b1011;
           
            break;
        case 3:
            led_word = 0b0111;
            
            break;

    }

    switch(x){
        case 0:
            led_word = (led_word<<6)|0b000100;
            
            break;
        case 1:
            led_word = (led_word<<6)|0b001000;
            
            break;
        case 2:
            led_word = (led_word<<6)|0b010000;
            
            break;
        case 3:
            led_word = (led_word<<6)|0b100000;
            
            break;

    }
    write_word(GPIO_DATA_REG,led_word);
}


void main(){
    write_word(GPIO_DIRECTION_CNTRL_REG, 0x00000FFF);

    off_led();
    
    while(1){
        for(int i =0; i<4 ;i++){
            for (int j=0; j<4; j++){
                set_led(i,j);
                printf("%d %d \n", i , j);
                delay_loop(1000,1000);
            }
        }
    }
    
    
    
}

