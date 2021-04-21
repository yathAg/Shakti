
#include "uart.h" //Includes the definitions of uart communication protocol//
#include "string.h"
#include "pinmux.h"
#include "i2c.h"

#define ESP_UART uart_instance[1]// UART 1 to communicate to ESP8266



/**
  * @fn int write_to_esp8266(char *data,uart_num sel,int baudrate)
  * @brief sends data to esp8266 using UART
  * @param data data that has to be sent to could
  * @param sel Uart that is connected to esp8266
  * @param baudrate Baudrate that is used to communicate with esp8266
*/
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



/**
  * @fn int write_to_esp8266(char *data,uart_num sel,int baudrate)
  * @brief sends carriage return and new line charector to esp8266
  * @detail sends carriage return and new line charector to esp8266
  *         this method is neeed to indicate end to data transmission
  * @param data data that has to be sent to could
  * @param sel Uart that is connected to esp8266
  * @param baudrate Baudrate that is used to communicate with esp8266
*/
int write_enter_to_esp8266()
{
	write_uart_character(ESP_UART, '\r');
	write_uart_character(ESP_UART, '\n');
}



/**
  * @fn int read_from_esp8266(char *data,uart_num sel,int baudrate)
  * @brief Reads data sent by esp8266
  * @details Reads data sent by esp8266 until one of the key word is found
  * @param data responses read from esp8266, mainly used for logging
  * @param sel Uart that is connected to esp8266
  * @param baudrate Baudrate that is used to communicate with esp8266
*/
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



void at_command(char *input , int wait , char *data)
{
	print_char(input);
	write_to_esp8266(input);
	delay_loop(wait,wait);
	read_from_esp8266(data);
}



void print_char(char *data)
{
	while (*data != '\0')
	{
		printf("%c", *data);
		data++;
	}
	printf("\n");
}



int find_data(char *start ,char *input)
{
	if (strstr(start, input) != NULL)
	{ 	
		start = strstr(start, input);
		print_char(start);
		printf(".");
		return 1;				
	}
	else return 0;
}



/**
  * @fn void main()
  * @brief Entry point for the program
*/
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


	while (1)
	{
		printf("-");
		char *ptr = find;
		char *str = find; 

		for(int i =0 ; i <30 ;i++)
		{
			read_uart_character(ESP_UART, &ch1);
			*str = ch1;
			str++;
			*str= '\0';

			if (find_data(ptr,"led=ON")==1) 
			{
				break;
			}
			
			if (find_data(ptr,"led=OFF")==1) 
			{
				break;
			}
		
		}	

		// delay(1000,1000);
		*str ='\0';
		
	}
}
