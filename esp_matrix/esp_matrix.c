
#include "uart.h" //Includes the definitions of uart communication protocol//
#include "string.h"
#include "pinmux.h"
#include "platform.h"
#include "gpio.h"
int led_word = 0b0; //for controlling the leds


#define ESP_UART uart_instance[1]// UART 1 to communicate to ESP8266


// function to wite data to esp
int write_to_esp8266(char *data)
{
	while (*data != '\0')
	{
		write_uart_character(ESP_UART, *data);
		data++;
	}
	write_uart_character(ESP_UART, '\r');
	write_uart_character(ESP_UART, '\n');
}


//function to read data from esp
int read_from_esp8266(char *data)
{
	int ch;
	char *str = data;
	char *test = data;
	char *start = data;

	for (int i = 0; i < 198; i++)
	{	
		read_uart_character(ESP_UART, &ch);
		*str = ch;
		str++;
		*str = '\0';
	
		if (strstr(test, "OK") != NULL)
		{
			 printf("from esp8266:%s\n", test);
			return;
		}
		if (strstr(test, "ERROR") != NULL)
		{
			printf("from esp8266:%s\n", test);
			return;
		}

	}
	*str = '\0';
	return 1;

}


//function to send at commands
void at_command(char *input , int wait , char *data)
{
	print_char(input);
	write_to_esp8266(input);
	delay_loop(wait,wait);
	read_from_esp8266(data);
}


// function to print strings from the given character array pointer
void print_char(char *data)
{
	while (*data != '\0')
	{
		printf("%c", *data);
		data++;
	}
	printf("\n");
}


//turn off all leds
void off_led()
{
    write_word(GPIO_DATA_REG, 0x00);
    printf("GPIO Data Register Value: %x\n",GPIO_DATA_REG);
}


// set led in matrix
void set_led(int x,int y){
    switch(y){
        case 0:
            led_word = 0b1110;
            // write_word(GPIO_DATA_REG , 0xE0);
            break;
        case 1:
            led_word = 0b1101;
            // write_word(GPIO_DATA_REG , 0xD0);
            break;
        case 2:
            led_word = 0b1011;
            // write_word(GPIO_DATA_REG , 0xB0);
            break;
        case 3:
            led_word = 0b0111;
            // write_word(GPIO_DATA_REG , 0x70);
            break;

    }

    switch(x){
        case 0:
            led_word = (led_word<<6)|0b000100;
            // write_word(GPIO_DATA_REG , 0x1);
            break;
        case 1:
            led_word = (led_word<<6)|0b001000;
            // write_word(GPIO_DATA_REG , 0x2);
            break;
        case 2:
            led_word = (led_word<<6)|0b010000;
            // write_word(GPIO_DATA_REG , 0x4);
            break;
        case 3:
            led_word = (led_word<<6)|0b100000;
            // write_word(GPIO_DATA_REG , 0x8);
            break;

    }
    write_word(GPIO_DATA_REG,led_word);
}



void main()
{
	int ch1;
	int baudrate = 115200;
	char data[200];
	char find[50];
	
	*pinmux_config_reg = 0x5;
	set_baud_rate(ESP_UART, baudrate);
	delay_loop(1000, 1000);
	
	at_command("AT",1000,data);
	at_command("ATE0",1000,data);
	// at_command("AT+RST",5000,data);
	
	at_command("AT+CWMODE=1",1000,data);
	at_command("AT+CWJAP=\"2.4GHZ\",\"Nibodh2805$\"",100,data);
	at_command("AT+CIPMUX=1",1000,data);
	at_command("AT+CIPSERVER=1,80",1000,data);

	write_word(GPIO_DIRECTION_CNTRL_REG, 0x00000FFF); //set direction I/O
	off_led();

	while (1)
	{
		printf("-\n");
		char *ptr = find;
		char *str = find; 
		char *x;
		char *y;
		int val_x;
		int val_y;

		for(int i =0 ; i <30 ;i++)
		{
			read_uart_character(ESP_UART, &ch1);
			*str = ch1;
			str++;
			*str= '\0';

			if (strstr(ptr, "xaxis") !=NULL)
			{
				for(int j =0 ;j<14; j++)
				{
					read_uart_character(ESP_UART, &ch1);
					*str = ch1;
					str++;
					*str= '\0';
				}
				ptr = (strstr(ptr, "xaxis"));
				
				x =ptr +6;
    			y= ptr+14;

				val_x = *x -48;
				val_y = *y -48;

    			printf("xaxis value is ---%d\n", val_x);
     			printf("yaxis value is ---%d\n", val_y);
				
				set_led(val_x,val_y);
				break;
			}
		}

		*str ='\0';
		
	}
}
