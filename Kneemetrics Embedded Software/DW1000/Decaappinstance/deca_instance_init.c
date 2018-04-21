#include "deca_instance_init.h"
#include "stm32f4xx_spi.h"
#include "compiler.h"
#include "uart.h"
#include "instance.h"

#define DW1000_UWB_Reset_Port		GPIOC
#define	DW1000_UWB_Reset_Pin		GPIO_Pin_7

#define FIXED_REPLY_DELAY_MULTI				25

uint64 forwardingAddress[1] =
{
     0xDECA030000000001
} ;
//Anchor address list
uint64 anchorAddressList[ANCHOR_LIST_SIZE] =
{
     0xDECA020000000001 ,       // First anchor
     0xDECA020100000002 ,       // Second anchor
     0xDECA020200000003 ,       // Third anchor
     0xDECA020300000004 ,       // Fourth anchor
     0xDECA020400000005 ,       // Fifth anchor
     0xDECA020500000006 ,       // Sixth anchor
     0xDECA020600000007 ,       // Seventh anchor
     0xDECA020700000008 ,       // Eighth anchor
     0xDECA020800000009 ,       // Ninth anchor
     0xDECA020900000010 ,       // Tenth anchor
     0xDECA021000000011         // Eleventh anchor
} ;



int instance_anchaddr; //0 = 0xDECA020000000001; 1 = 0xDECA020000000002; 2 = 0xDECA020000000003 ......
int dr_mode = 0;

//if instance_mode = TAG_TDOA then the device cannot be selected as anchor
int instance_mode = ANCHOR;//TAG

int paused = 0;

double antennaDelay; // This is system effect on RTD subtracted from local calculation.

char reset_request;

//Configuration for DecaRanging Modes (8 default use cases selectable by the switch S1 on EVK)
chConfig_t chConfig[8] = {
//mode 1  - S1: 5 off, 6 off, 7 off
		{ 2, // channel
				DWT_PRF_16M, // prf
				DWT_BR_110K, // datarate
				3, // preambleCode
				DWT_PLEN_1024, // preambleLength
				DWT_PAC32, // pacSize
				1, // non-standard SFD
				(1025 + 64 - 32) //SFD timeout
		},
		//mode 2 - S1: 5 on, 6 off, 7 off
		{ 2, // channel
				DWT_PRF_16M, // prf
				DWT_BR_6M8, // datarate
				3, // preambleCode
				DWT_PLEN_128, // preambleLength
				DWT_PAC8, // pacSize
				0, // non-standard SFD
				(129 + 8 - 8) //SFD timeout
		},
		//mode 3 - S1: 5 of, 6 on, 7 off
		{ 2, // channel
				DWT_PRF_64M, // prf
				DWT_BR_110K, // datarate
				9, // preambleCode
				DWT_PLEN_1024, // preambleLength
				DWT_PAC32, // pacSize
				1, // non-standard SFD
				(1025 + 64 - 32) //SFD timeout
		},
		//mode 4 - S1: 5 on, 6 on, 7 off
		{ 2, // channel
				DWT_PRF_64M, // prf
				DWT_BR_6M8, // datarate
				9, // preambleCode
				DWT_PLEN_128, // preambleLength
				DWT_PAC8, // pacSize
				0, // non-standard SFD
				(129 + 8 - 8) //SFD timeout
		},
		//mode 5 - S1: 5 off, 6 off, 7 on
		{ 5, // channel
				DWT_PRF_16M, // prf
				DWT_BR_110K, // datarate
				3, // preambleCode
				DWT_PLEN_1024, // preambleLength
				DWT_PAC32, // pacSize
				1, // non-standard SFD
				(1025 + 64 - 32) //SFD timeout
		},
		//mode 6 - S1: 5 on, 6 off, 7 on
		{ 5, // channel
				DWT_PRF_16M, // prf
				DWT_BR_6M8, // datarate
				3, // preambleCode
				DWT_PLEN_128, // preambleLength
				DWT_PAC8, // pacSize
				0, // non-standard SFD
				(129 + 8 - 8) //SFD timeout
		},
		//mode 7 - S1: 5 off, 6 on, 7 on
		{ 5, // channel
				DWT_PRF_64M, // prf
				DWT_BR_110K, // datarate
				9, // preambleCode
				DWT_PLEN_1024, // preambleLength
				DWT_PAC32, // pacSize
				1, // non-standard SFD
				(1025 + 64 - 32) //SFD timeout
		},
		//mode 8 - S1: 5 on, 6 on, 7 on
		{ 5, // channel
				DWT_PRF_64M, // prf
				DWT_BR_6M8, // datarate
				9, // preambleCode
				DWT_PLEN_128, // preambleLength
				DWT_PAC8, // pacSize
				0, // non-standard SFD
				(129 + 8 - 8) //SFD timeout
		} };

uint8 getanchoraddress(void)
{
    uint8 mode = 0;


    if(TR_SW2_5)
    {
        mode = 1;
    }

    if(TR_SW2_6)
    {
        mode = mode + 2;
    }

    if(TR_SW2_7)
    {
        mode = mode + 4;
    }

    if(TR_SW2_8)
    {
        mode = mode + 8;
    }


    return mode;
}

// ======================================================
//
//  Configure instance tag/anchor/etc... addresses
//

void addressconfigure(void)
{
	instanceAddressConfig_t ipc;

	ipc.forwardToFRAddress = forwardingAddress[0];
	instance_anchaddr = 0;//getanchoraddress();
	ipc.anchorAddress = anchorAddressList[instance_anchaddr];
	ipc.anchorAddressList = anchorAddressList;
	ipc.anchorListSize = ANCHOR_LIST_SIZE;
	ipc.anchorPollMask = getanchoraddress();              // anchor poll mask

	ipc.sendReport = 1;//1 => anchor sends TOF report to tag
	//ipc.sendReport = 2 ;  //2 => anchor sends TOF report to listener

	instancesetaddresses(&ipc);
}


// Restart and re-configure
void restartinstance(void) {
	instance_close(); //shut down instance, PHY, SPI close, etc.

	SPI_ConfigFastRate(SPI_BaudRatePrescaler_32);

	inittestapplication(); //re-initialise instance/device
} // end restartinstance()

int decarangingmode(void) {
	int mode = 0;

	if (is_switch_on(TA_SW1_5)) {
		mode = 1;
	}

	if (is_switch_on(TA_SW1_6)) {
		mode = mode + 2;
	}

	if (is_switch_on(TA_SW1_7)) {
		mode = mode + 4;
	}

	return mode;
}

uint32 inittestapplication(void) {
	uint32 devID;
	instanceConfig_t instConfig;
	int i, result;
	char addrr[100];

	i = 10;

	//this is called here to wake up the device (i.e. if it was in sleep mode before the restart)
	devID = instancereaddeviceid();
	if (DWT_DEVICE_ID != devID) //if the read of device ID fails, the DW1000 could be asleep
			{
		port_SPIx_clear_chip_select();
		//CS low

		Sleep(1); //200 us to wake up then waits 5ms for DW1000 XTAL to stabilise
		port_SPIx_set_chip_select();
		//CS high
		Sleep(7);
		devID = instancereaddeviceid();
		// SPI not working or Unsupported Device ID
		if (DWT_DEVICE_ID != devID) {

			return (-1);
		}
		//clear the sleep bit - so that after the hard reset below the DW does not go into sleep
		dwt_softreset();
	}

	//reset the DW1000 by driving the RSTn line low
	reset_DW1000();
	result = instance_init();

	if (0 > result) {

		return (-1); // Some failure has occurred
	}
	SPI_ConfigFastRate(SPI_BaudRatePrescaler_4); //increase SPI to max SPI_BaudRatePrescaler_4
	devID = instancereaddeviceid();



	if (DWT_DEVICE_ID != devID) // Means it is NOT MP device
			{
		// SPI not working or Unsupported Device ID
		return (-1);
	}

	if (port_IS_TAG_pressed() == 0) {
		instance_mode = TAG;
	} else {
		instance_mode = ANCHOR;
	}

	instancesetrole(instance_mode); // Set this instance role

	if (is_fastrng_on() == S1_SWITCH_ON) //if fast ranging then initialise instance for fast ranging application
	{
		instance_init_f(instance_mode); //initialise Fast 2WR specific data
		//when using fast ranging the channel config is either mode 2 or mode 6
		//default is mode 2
		dr_mode = decarangingmode();

		if ((dr_mode & 0x1) == 0)
			dr_mode = 1;
	} else {
		instance_init_s(instance_mode);
		dr_mode = decarangingmode();
	}

	instConfig.channelNumber = chConfig[dr_mode].channel;
	instConfig.preambleCode = chConfig[dr_mode].preambleCode;
	instConfig.pulseRepFreq = chConfig[dr_mode].prf;
	instConfig.pacSize = chConfig[dr_mode].pacSize;
	instConfig.nsSFD = chConfig[dr_mode].nsSFD;
	instConfig.sfdTO = chConfig[dr_mode].sfdTO;
	instConfig.dataRate = chConfig[dr_mode].datarate;
	instConfig.preambleLen = chConfig[dr_mode].preambleLength;

	instance_config(&instConfig); // Set operating channel etc


	addressconfigure(); // set up initial payload configuration

	instancesettagsleepdelay(POLL_SLEEP_DELAY, BLINK_SLEEP_DELAY); //set the Tag sleep time

	//if TA_SW1_2 is on use fast ranging (fast 2wr)
	if (is_fastrng_on() == S1_SWITCH_ON) {
		//Fast 2WR specific config
		//configure the delays/timeouts
		instance_config_f();
	} else //use default ranging modes
	{
		// NOTE: this is the delay between receiving the blink and sending the ranging init message
		// The anchor ranging init response delay has to match the delay the tag expects
		// the tag will then use the ranging response delay as specified in the ranging init message
		// use this to set the long blink response delay (e.g. when ranging with a PC anchor that wants to use the long response times != 150ms)
#if (DR_DISCOVERY == 1)
		if (is_switch_on(TA_SW1_8) == S1_SWITCH_ON) {
			instancesetblinkreplydelay(FIXED_LONG_BLINK_RESPONSE_DELAY);
		} else //this is for ARM to ARM tag/anchor (using normal response times 150ms)
#endif
		{
			instancesetblinkreplydelay(FIXED_REPLY_DELAY);
		}

#if (DR_DISCOVERY == 1)
        //set the default response delays
        instancesetreplydelay(FIXED_REPLY_DELAY, 0);
#else
        //set the default response delays
        instancesetreplydelay(FIXED_REPLY_DELAY_MULTI, 0);
#endif
	}

	return devID;
}

void reset_DW1000(void) {
GPIO_ResetBits(DW1000_UWB_Reset_Port, DW1000_UWB_Reset_Pin);
DelayMilliSec(200);
GPIO_SetBits(DW1000_UWB_Reset_Port, DW1000_UWB_Reset_Pin);
}


