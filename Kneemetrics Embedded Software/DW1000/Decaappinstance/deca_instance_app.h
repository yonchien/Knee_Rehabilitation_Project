#ifndef _DECA_INSTANCE_APP_H_
#define _DECA_INSTANCE_APP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include<stdint.h>

//#define ANCHOR_COLOR 	LED_GREEN|LED_BLUE     // Magen
//#define TAG_COLOR		LED_GREEN|LED_RED     //Yellow For Tag

struct deca_rangeData
{
	uint32_t iRange;		//!< Instanteous range in Millimeters
	uint32_t avgRange; 		//!< Average range in Millimeters
};

typedef struct deca_rangeData deca_rangeData_t;

void deca_app(void);

void process_deca_irq(void);

//void deca_set_led_mode(void);

void deca_run(void);

void update_range(deca_rangeData_t*);

#ifdef __cplusplus
}
#endif

#endif
