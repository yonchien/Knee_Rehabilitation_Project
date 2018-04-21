#ifndef _DECA_INSTANCE_INIT_H_
#define _DECA_INSTANCE_INIT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "deca_port.h"
#include "instance.h"

#define SOFTWARE_VER_STRING    "Version 2.26    " //

typedef struct {
	uint8 channel;
	uint8 prf;
	uint8 datarate;
	uint8 preambleCode;
	uint8 preambleLength;
	uint8 pacSize;
	uint8 nsSFD;
	uint16 sfdTO;
} chConfig_t;


/*
#if (DR_DISCOVERY == 0)
//Tag address list
uint64 tagAddressList[3] =
{
	0xDECA010000001001, // First tag
	0xDECA010000000002,// Second tag
	0xDECA010000000003// Third tag
};

//Anchor address list
uint64 anchorAddressList[ANCHOR_LIST_SIZE] =
{
	0xDECA020000000001 , // First anchor
	0xDECA020000000002 ,// Second anchor
	0xDECA020000000003 ,// Third anchor
	0xDECA020000000004// Fourth anchor
};

//ToF Report Forwarding Address
uint64 forwardingAddress[1] =
{
	0xDECA030000000001
};

// ======================================================
//
//  Configure instance tag/anchor/etc... addresses
//
void addressconfigure(void);
#endif
*/

uint32 inittestapplication(void);

// Restart and re-configure
void restartinstance(void);

void reset_DW1000(void);

int decarangingmode(void);



#ifdef __cplusplus
}
#endif

#endif
