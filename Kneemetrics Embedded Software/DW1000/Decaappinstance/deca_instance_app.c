#include "deca_instance_app.h"
#include "instance.h"
#include "deca_port.h"
#include "compiler.h"
#include "uart.h"//
#include <stdio.h>

extern int instance_mode;
static int toggle = 1;
static int ranging = 0;
float range_result = 0;
static double avg_result = 0;

deca_rangeData_t deca_val;

void process_deca_irq(void)
{
    do{

        instance_process_irq(0);

    }while(port_CheckEXT_IRQ() == 1); //while IRS line active (ARM can only do edge sensitive interrupts)

}


void deca_app(void) {

	char stri[50];
	char stt[100];
	//int c;
	if (port_IS_TAG_pressed() == 0) {
		instance_mode = TAG;
	} else {
		instance_mode = ANCHOR;
	}

	port_EnableEXT_IRQ(); //enable ScenSor IRQ before starting

	while(1)
	{

		instance_run();

		if(instancenewrange())
		{
			//blink green LED when get new range
			if (GetLedState(LED_GRE) == LED_ON)
							SwitchOffLED(LED_GRE);
						else
							SwitchOnLED(LED_GRE);

#if ENABLE_PRINTF
			printf("\r\n Got New Range");
#endif
			ranging = 1;
			range_result = instance_get_idist();
			avg_result = instance_get_adist();

			int d1 = range_result;
			float f2 = range_result - d1;
			int d2 = trunc(f2 * 100);

			//switch on yellow LED if distance is > 1 m
			if (range_result>1)
				SwitchOnLED(LED_YEL);
			else
				SwitchOffLED(LED_YEL);

			//prepare string to be sent through uart to Rasperry PI
		//	sprintf(stri, "LAST RESULT: %d.%02d m\n",d1,d2);//prepare string to be sent through uart
		//	USART_puts(USART6,stri);
			//return;//
			//sleep(20);

			//put function for IMU
		}

		/*if (c==2){//
			c=0;//
			return;//
		}*/

/*		if(ranging == 0)
		{
			if(instance_mode != ANCHOR)
			{
				if(instancesleeping())
				{
					if(toggle)
					{
						toggle = 0;
					}
					else
					{
						toggle = 1;
					}
				}

				if(instanceanchorwaiting() == 2)
				{
					ranging = 1;
				}
				//c++;//
			}
			else
			{
				if(instanceanchorwaiting())
				{
					toggle+=2;

					if(toggle > 300000)
					{
						if(toggle & 0x1)
						{
							toggle = 0;
						}
						else
						{
							toggle = 1;
						}
					}

				}
				//c++;//
			}
		}*/
	}

}


void update_range(deca_rangeData_t* deca_rang)
{
	deca_rang->iRange = 34361; //deca_val.iRange;
	deca_rang->avgRange = 768985; //deca_val.avgRange;
}
