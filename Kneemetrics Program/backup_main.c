#include "quaternion.h"
#include "leds.h"
#include "rtc_clock.h"
#include "stm32f4xx.h"
#include "stm32f4xx_spi.h"
#include "irq.h"
#include "battery_charger_management.h"
#include "spi.h"
#include "compiler.h"
#include "deca_device_api.h"
#include "deca_instance_init.h"
#include "deca_instance_app.h"
#include "uart.h"
#include "stm32f4xx_sdio.h"
#include "stm32f4_discovery.h"
#include "stm32f4_discovery_sdio_sd.h"
#include "bme280.h"

#include "ff.h"
#include "diskio.h"

FRESULT res;
FRESULT euler_res;
FRESULT test_res;
FILINFO fno;
FIL fil;
FIL euler_fil;
FIL test_fil;
DIR dir;
FATFS fs32;
char* path;

long datamag[3] = { 0 };
long imutemp = 0;

#if _USE_LFN
static char lfn[_MAX_LFN + 1];
fno.lfname = lfn;
fno.lfsize = sizeof lfn;
#endif

//******************************************************************************

char *dec32(unsigned long i)
{
	static char str[16];
	char *s = str + sizeof(str);

	*--s = 0;

	do
	{
		*--s = '0' + (char)(i % 10);
		i /= 10;
	} while (i);

	return(s);
}




/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "stdio.h"

#include "uart.h"
#include "i2c.h"
#include "gpio.h"
#include "main.h"
#include "board-st_discovery.h"

#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "invensense.h"
#include "invensense_adv.h"
#include "eMPL_outputs.h"
#include "mltypes.h"
#include "mpu.h"
#include "log.h"
#include "packet.h"
/* Private typedef -----------------------------------------------------------*/
/* Data read from MPL. */

#define FIFO_POCKET_SIZE (32)		// (same as MAX_PACKET_LENGTH in inv_mpu_dmp_motion_driver.c)

#define PRINT_ACCEL     (0x01)
#define PRINT_GYRO      (0x02)
#define PRINT_QUAT      (0x04)
#define PRINT_COMPASS   (0x08)
#define PRINT_EULER     (0x10)
#define PRINT_ROT_MAT   (0x20)
#define PRINT_HEADING   (0x40)
#define PRINT_PEDO      (0x80)
#define PRINT_LINEAR_ACCEL (0x100)
#define PRINT_GRAVITY_VECTOR (0x200)

volatile uint32_t hal_timestamp = 0;
#define ACCEL_ON        (0x01)
#define GYRO_ON         (0x02)
#define COMPASS_ON      (0x04)

#define MOTION          (0)
#define NO_MOTION       (1)

/* Starting sampling rate. */
#define DEFAULT_MPU_HZ  (20)

#define FLASH_SIZE      (512)
#define FLASH_MEM_START ((void*)0x1800)

#define PEDO_READ_MS    (1000)
#define TEMP_READ_MS    (500)
#define COMPASS_READ_MS (100)
struct rx_s {
	unsigned char header[3];
	unsigned char cmd;
};
struct hal_s {
	unsigned char lp_accel_mode;
	unsigned char sensors;
	unsigned char dmp_on;
	unsigned char wait_for_tap;
	volatile unsigned char new_gyro;
	unsigned char motion_int_mode;
	unsigned long no_dmp_hz;
	unsigned long next_pedo_ms;
	unsigned long next_temp_ms;
	unsigned long next_compass_ms;
	unsigned int report;
	unsigned short dmp_features;
	struct rx_s rx;
};
static struct hal_s hal = { 0 };

/* USB RX binary semaphore. Actually, it's just a flag. Not included in struct
* because it's declared extern elsewhere.
*/
volatile unsigned char rx_new;

unsigned char *mpl_key = (unsigned char*)"eMPL 5.1";

/* Platform-specific information. Kinda like a boardfile. */
struct platform_data_s {
	signed char orientation[9];
};

/* The sensors can be mounted onto the board in any orientation. The mounting
* matrix seen below tells the MPL how to rotate the raw data from the
* driver(s).
* TODO: The following matrices refer to the configuration on internal test
* boards at Invensense. If needed, please modify the matrices to match the
* chip-to-body matrix for your particular set up.
*/
static struct platform_data_s gyro_pdata = {
	.orientation = { 1, 0, 0,
	0, 1, 0,
	0, 0, 1 }
};

#if defined MPU9150 || defined MPU9250
static struct platform_data_s compass_pdata = {
	.orientation = { 0, 1, 0,
	1, 0, 0,
	0, 0, -1 }
};
#define COMPASS_ENABLED 1
#elif defined AK8975_SECONDARY
static struct platform_data_s compass_pdata = {
	.orientation = { -1, 0, 0,
	0, 1, 0,
	0, 0,-1 }
};
#define COMPASS_ENABLED 1
#elif defined AK8963_SECONDARY
static struct platform_data_s compass_pdata = {
	.orientation = { -1, 0, 0,
	0,-1, 0,
	0, 0, 1 }
};
#define COMPASS_ENABLED 1
#endif


/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* ---------------------------------------------------------------------------*/
/* Get data from MPL.
* TODO: Add return values to the inv_get_sensor_type_xxx APIs to differentiate
* between new and stale data.
*/
static void read_from_mpl(void)
{
	long msg, data[9];
	int8_t accuracy;
	unsigned long timestamp;
	float float_data[3] = { 0 };

	if (inv_get_sensor_type_quat(data, &accuracy, (inv_time_t*)&timestamp)) {
		/* Sends a quaternion packet to the PC. Since this is used by the Python
		* test app to visually represent a 3D quaternion, it's sent each time
		* the MPL has new data.
		*/
		eMPL_send_quat(data);

		/* Specific data packets can be sent or suppressed using USB commands. */
		if (hal.report & PRINT_QUAT)
			eMPL_send_data(PACKET_DATA_QUAT, data);
	}

	if (hal.report & PRINT_ACCEL) {//hal.report & PRINT_ACCEL
		if (inv_get_sensor_type_accel(data, &accuracy,
			(inv_time_t*)&timestamp))
			eMPL_send_data(PACKET_DATA_ACCEL, data);
	}
	if (1) {//hal.report & PRINT_GYRO
		if (inv_get_sensor_type_gyro(data, &accuracy,
			(inv_time_t*)&timestamp))
			eMPL_send_data(PACKET_DATA_GYRO, data);
	}
#ifdef COMPASS_ENABLED
	if (hal.report & PRINT_COMPASS) {//hal.report & PRINT_COMPASS
		if (inv_get_sensor_type_compass(data, &accuracy,
			(inv_time_t*)&timestamp))
			eMPL_send_data(PACKET_DATA_COMPASS, data);
	}
#endif
	if (hal.report & PRINT_EULER) {//hal.report & PRINT_EULER
		if (inv_get_sensor_type_euler(data, &accuracy,
			(inv_time_t*)&timestamp))
			eMPL_send_data(PACKET_DATA_EULER, data);
	}
	if (hal.report & PRINT_ROT_MAT) {
		if (inv_get_sensor_type_rot_mat(data, &accuracy,
			(inv_time_t*)&timestamp))
			eMPL_send_data(PACKET_DATA_ROT, data);
	}
	if (hal.report & PRINT_HEADING) {
		if (inv_get_sensor_type_heading(data, &accuracy,
			(inv_time_t*)&timestamp))
			eMPL_send_data(PACKET_DATA_HEADING, data);
	}
	if (hal.report & PRINT_LINEAR_ACCEL) {
		if (inv_get_sensor_type_linear_acceleration(float_data, &accuracy, (inv_time_t*)&timestamp)) {
			MPL_LOGI("Linear Accel: %7.5f %7.5f %7.5f\r\n",
				float_data[0], float_data[1], float_data[2]);
		}
	}
	if (hal.report & PRINT_GRAVITY_VECTOR) {
		if (inv_get_sensor_type_gravity(float_data, &accuracy,
			(inv_time_t*)&timestamp))
			MPL_LOGI("Gravity Vector: %7.5f %7.5f %7.5f\r\n",
				float_data[0], float_data[1], float_data[2]);
	}
	if (hal.report & PRINT_PEDO) {
		unsigned long timestamp;
		get_tick_count(&timestamp);
		if (timestamp > hal.next_pedo_ms) {
			hal.next_pedo_ms = timestamp + PEDO_READ_MS;
			unsigned long step_count, walk_time;
			dmp_get_pedometer_step_count(&step_count);
			dmp_get_pedometer_walk_time(&walk_time);
			MPL_LOGI("Walked %ld steps over %ld milliseconds..\n", step_count,
				walk_time);
		}
	}

	/* Whenever the MPL detects a change in motion state, the application can
	* be notified. For this example, we use an LED to represent the current
	* motion state.
	*/
	msg = inv_get_message_level_0(INV_MSG_MOTION_EVENT |
		INV_MSG_NO_MOTION_EVENT);
	if (msg) {
		if (msg & INV_MSG_MOTION_EVENT) {
			MPL_LOGI("Motion!\n");
		}
		else if (msg & INV_MSG_NO_MOTION_EVENT) {
			MPL_LOGI("No motion!\n");
		}
	}
}

#ifdef COMPASS_ENABLED
void send_status_compass() {
	char stri[100];
	UINT BytesWritten;
	DWORD size;
	//	long datamag[3] = { 0 };
	int8_t accuracy = { 3 };
	unsigned long timestamp;
	inv_get_compass_set(datamag, &accuracy, (inv_time_t*)&timestamp);
	unsigned long timerValue = GetTickCount();
	//MPL_LOGI("Compass: %7.4f %7.4f %7.4f ",
	//		data[0]/65536.f, data[1]/65536.f, data[2]/65536.f);
	//	MPL_LOGI("Accuracy= %d\r\n", accuracy);
	/*sprintf(stri, "%li  %li  %li  \r\n",data[0],data[1],data[2]);
	//  sprintf(stri, out);//prepare string to be sent through uart
	//   	USART_puts(USART6,out);
	res = f_open(&fil,"mag.txt",FA_WRITE | FA_OPEN_ALWAYS);

	size = (&fil)->fsize;

	res = f_lseek(&fil,size);

	res = f_write(&fil,stri, strlen(stri),&BytesWritten);

	f_close(&fil);*/

}
#endif

/* Handle sensor on/off combinations. */
static void setup_gyro(void)
{
	unsigned char mask = 0, lp_accel_was_on = 0;
	if (hal.sensors & ACCEL_ON)
		mask |= INV_XYZ_ACCEL;
	if (hal.sensors & GYRO_ON) {
		mask |= INV_XYZ_GYRO;
		lp_accel_was_on |= hal.lp_accel_mode;
	}
#ifdef COMPASS_ENABLED
	if (hal.sensors & COMPASS_ON) {
		mask |= INV_XYZ_COMPASS;
		lp_accel_was_on |= hal.lp_accel_mode;
	}
#endif
	/* If you need a power transition, this function should be called with a
	* mask of the sensors still enabled. The driver turns off any sensors
	* excluded from this mask.
	*/
	mpu_set_sensors(mask);
	mpu_configure_fifo(mask);
	if (lp_accel_was_on) {
		unsigned short rate;
		hal.lp_accel_mode = 0;
		/* Switching out of LP accel, notify MPL of new accel sampling rate. */
		mpu_get_sample_rate(&rate);
		inv_set_accel_sample_rate(1000000L / rate);
	}
}

static void tap_cb(unsigned char direction, unsigned char count)
{
	switch (direction) {
	case TAP_X_UP:
		MPL_LOGI("Tap X+ ");
		break;
	case TAP_X_DOWN:
		MPL_LOGI("Tap X- ");
		break;
	case TAP_Y_UP:
		MPL_LOGI("Tap Y+ ");
		break;
	case TAP_Y_DOWN:
		MPL_LOGI("Tap Y- ");
		break;
	case TAP_Z_UP:
		MPL_LOGI("Tap Z+ ");
		break;
	case TAP_Z_DOWN:
		MPL_LOGI("Tap Z- ");
		break;
	default:
		return;
	}
	MPL_LOGI("x%d\n", count);
	return;
}

static void android_orient_cb(unsigned char orientation)
{
	switch (orientation) {
	case ANDROID_ORIENT_PORTRAIT:
		MPL_LOGI("Portrait\n");
		break;
	case ANDROID_ORIENT_LANDSCAPE:
		MPL_LOGI("Landscape\n");
		break;
	case ANDROID_ORIENT_REVERSE_PORTRAIT:
		MPL_LOGI("Reverse Portrait\n");
		break;
	case ANDROID_ORIENT_REVERSE_LANDSCAPE:
		MPL_LOGI("Reverse Landscape\n");
		break;
	default:
		return;
	}
}


static inline void run_self_test(void)
{
	int result;
	long gyro[3], accel[3];
	char stri[100];
	DWORD size;
	UINT BytesWritten;

#if defined (MPU6500) || defined (MPU9250)
	result = mpu_run_6500_self_test(gyro, accel, 0);
#elif defined (MPU6050) || defined (MPU9150)
	result = mpu_run_self_test(gyro, accel);
#endif
	if (result == 0x7) {

		MPL_LOGI("Passed!\n");


		MPL_LOGI("accel: %d %d %d\n",
			accel[0],
			accel[1],
			accel[2]);
		MPL_LOGI("gyro: %d %d %d\n",
			gyro[0],
			gyro[1],
			gyro[2]);

		sprintf(stri, "%li  %li  %li\r\n\r\n%hi %hi %hi\r\n", gyro[0], gyro[1], gyro[2], accel[0], accel[1], accel[2]);
		res = f_open(&fil, "selft2.txt", FA_WRITE | FA_OPEN_ALWAYS);

		size = (&fil)->fsize;

		res = f_lseek(&fil, size);

		res = f_write(&fil, stri, strlen(stri), &BytesWritten);

		f_close(&fil);
		/* Test passed. We can trust the gyro data here, so now we need to update calibrated data*/

#ifdef USE_CAL_HW_REGISTERS
		/*
		* This portion of the code uses the HW offset registers that are in the MPUxxxx devices
		* instead of pushing the cal data to the MPL software library
		*/
		unsigned char i = 0;

		for (i = 0; i<3; i++) {
			gyro[i] = (long)(gyro[i] * 32.8f); //convert to +-1000dps
			accel[i] *= 2048.f; //convert to +-16G
			accel[i] = accel[i] >> 16;
			gyro[i] = (long)(gyro[i] >> 16);
		}

		mpu_set_gyro_bias_reg(gyro);

#if defined (MPU6500) || defined (MPU9250)
		mpu_set_accel_bias_6500_reg(accel);


		sprintf(stri, "%ld  %ld  %ld\r\n\r\n%ld %ld %ld\r\n", gyro[0], gyro[1], gyro[2], accel[0], accel[1], accel[2]);
		res = f_open(&fil, "selft3.txt", FA_WRITE | FA_OPEN_ALWAYS);

		size = (&fil)->fsize;

		res = f_lseek(&fil, size);

		res = f_write(&fil, stri, strlen(stri), &BytesWritten);

		f_close(&fil);
#elif defined (MPU6050) || defined (MPU9150)
		mpu_set_accel_bias_6050_reg(accel);
#endif
#else
		/* Push the calibrated data to the MPL library.
		*
		* MPL expects biases in hardware units << 16, but self test returns
		* biases in g's << 16.
		*/
		unsigned short accel_sens;
		float gyro_sens;

		mpu_get_accel_sens(&accel_sens);
		accel[0] *= accel_sens;
		accel[1] *= accel_sens;
		accel[2] *= accel_sens;
		inv_set_accel_bias(accel, 3);
		//	dmp_set_accel_bias(gyro);
		mpu_get_gyro_sens(&gyro_sens);
		gyro[0] = (long)(gyro[0] * gyro_sens);
		gyro[1] = (long)(gyro[1] * gyro_sens);
		gyro[2] = (long)(gyro[2] * gyro_sens);
		inv_set_gyro_bias(gyro, 3);
		//		dmp_set_gyro_bias(gyro);



		sprintf(stri, "%li  %li  %li\r\n\r\n%hi %hi %hi\r\n", gyro[0], gyro[1], gyro[2], accel[0], accel[1], accel[2]);
		res = f_open(&fil, "selftest.txt", FA_WRITE | FA_OPEN_ALWAYS);

		size = (&fil)->fsize;

		res = f_lseek(&fil, size);

		res = f_write(&fil, stri, strlen(stri), &BytesWritten);

		f_close(&fil);
#endif
	}
	else {
		if (!(result & 0x1))
			MPL_LOGE("Gyro failed.\n");
		if (!(result & 0x2))
			MPL_LOGE("Accel failed.\n");
		if (!(result & 0x4))
			MPL_LOGE("Compass failed.\n");
	}

}

static void handle_input(void)
{

	char c = 't';//USART_ReceiveData(USART2);

	switch (c) {
		/* These commands turn off individual sensors. */
	case '8':
		hal.sensors ^= ACCEL_ON;
		setup_gyro();
		if (!(hal.sensors & ACCEL_ON))
			inv_accel_was_turned_off();
		break;
	case '9':
		hal.sensors ^= GYRO_ON;
		setup_gyro();
		if (!(hal.sensors & GYRO_ON))
			inv_gyro_was_turned_off();
		break;
#ifdef COMPASS_ENABLED
	case '0':
		hal.sensors ^= COMPASS_ON;
		setup_gyro();
		if (!(hal.sensors & COMPASS_ON))
			inv_compass_was_turned_off();
		break;
#endif
		/* The commands send individual sensor data or fused data to the PC. */
	case 'a':
		hal.report ^= PRINT_ACCEL;
		break;
	case 'g':
		hal.report ^= PRINT_GYRO;
		break;
#ifdef COMPASS_ENABLED
	case 'c':
		hal.report ^= PRINT_COMPASS;
		break;
#endif
	case 'e':
		hal.report ^= PRINT_EULER;
		break;
	case 'r':
		hal.report ^= PRINT_ROT_MAT;
		break;
	case 'q':
		hal.report ^= PRINT_QUAT;
		break;
	case 'h':
		hal.report ^= PRINT_HEADING;
		break;
	case 'i':
		hal.report ^= PRINT_LINEAR_ACCEL;
		break;
	case 'o':
		hal.report ^= PRINT_GRAVITY_VECTOR;
		break;
#ifdef COMPASS_ENABLED
	case 'w':
		send_status_compass();
		break;
#endif
		/* This command prints out the value of each gyro register for debugging.
		* If logging is disabled, this function has no effect.
		*/
	case 'd':
		mpu_reg_dump();
		break;
		/* Test out low-power accel mode. */
	case 'p':
		if (hal.dmp_on)
			/* LP accel is not compatible with the DMP. */
			break;
		mpu_lp_accel_mode(20);
		/* When LP accel mode is enabled, the driver automatically configures
		* the hardware for latched interrupts. However, the MCU sometimes
		* misses the rising/falling edge, and the hal.new_gyro flag is never
		* set. To avoid getting locked in this state, we're overriding the
		* driver's configuration and sticking to unlatched interrupt mode.
		*
		* TODO: The MCU supports level-triggered interrupts.
		*/
		mpu_set_int_latched(0);
		hal.sensors &= ~(GYRO_ON | COMPASS_ON);
		hal.sensors |= ACCEL_ON;
		hal.lp_accel_mode = 1;
		inv_gyro_was_turned_off();
		inv_compass_was_turned_off();
		break;
		/* The hardware self test can be run without any interaction with the
		* MPL since it's completely localized in the gyro driver. Logging is
		* assumed to be enabled; otherwise, a couple LEDs could probably be used
		* here to display the test results.
		*/
	case 't':
		run_self_test();
		/* Let MPL know that contiguity was broken. */
		inv_accel_was_turned_off();
		inv_gyro_was_turned_off();
		inv_compass_was_turned_off();
		break;
		/* Depending on your application, sensor data may be needed at a faster or
		* slower rate. These commands can speed up or slow down the rate at which
		* the sensor data is pushed to the MPL.
		*
		* In this example, the compass rate is never changed.
		*/
	case '1':
		if (hal.dmp_on) {
			dmp_set_fifo_rate(10);
			inv_set_quat_sample_rate(100000L);
		}
		else
			mpu_set_sample_rate(10);
		inv_set_gyro_sample_rate(100000L);
		inv_set_accel_sample_rate(100000L);
		break;
	case '2':
		if (hal.dmp_on) {
			dmp_set_fifo_rate(20);
			inv_set_quat_sample_rate(50000L);
		}
		else
			mpu_set_sample_rate(20);
		inv_set_gyro_sample_rate(50000L);
		inv_set_accel_sample_rate(50000L);
		break;
	case '3':
		if (hal.dmp_on) {
			dmp_set_fifo_rate(40);
			inv_set_quat_sample_rate(25000L);
		}
		else
			mpu_set_sample_rate(40);
		inv_set_gyro_sample_rate(25000L);
		inv_set_accel_sample_rate(25000L);
		break;
	case '4':
		if (hal.dmp_on) {
			dmp_set_fifo_rate(50);
			inv_set_quat_sample_rate(20000L);
		}
		else
			mpu_set_sample_rate(50);
		inv_set_gyro_sample_rate(20000L);
		inv_set_accel_sample_rate(20000L);
		break;
	case '5':
		if (hal.dmp_on) {
			dmp_set_fifo_rate(100);
			inv_set_quat_sample_rate(10000L);
		}
		else
			mpu_set_sample_rate(100);
		inv_set_gyro_sample_rate(10000L);
		inv_set_accel_sample_rate(10000L);
		break;
	case ',':
		/* Set hardware to interrupt on gesture event only. This feature is
		* useful for keeping the MCU asleep until the DMP detects as a tap or
		* orientation event.
		*/
		dmp_set_interrupt_mode(DMP_INT_GESTURE);
		break;
	case '.':
		/* Set hardware to interrupt periodically. */
		dmp_set_interrupt_mode(DMP_INT_CONTINUOUS);
		break;
	case '6':
		/* Toggle pedometer display. */
		hal.report ^= PRINT_PEDO;
		break;
	case '7':
		/* Reset pedometer. */
		dmp_set_pedometer_step_count(0);
		dmp_set_pedometer_walk_time(0);
		break;
	case 'f':
		if (hal.lp_accel_mode)
			/* LP accel is not compatible with the DMP. */
			return;
		/* Toggle DMP. */
		if (hal.dmp_on) {
			unsigned short dmp_rate;
			unsigned char mask = 0;
			hal.dmp_on = 0;
			mpu_set_dmp_state(0);
			/* Restore FIFO settings. */
			if (hal.sensors & ACCEL_ON)
				mask |= INV_XYZ_ACCEL;
			if (hal.sensors & GYRO_ON)
				mask |= INV_XYZ_GYRO;
			if (hal.sensors & COMPASS_ON)
				mask |= INV_XYZ_COMPASS;
			mpu_configure_fifo(mask);
			/* When the DMP is used, the hardware sampling rate is fixed at
			* 200Hz, and the DMP is configured to downsample the FIFO output
			* using the function dmp_set_fifo_rate. However, when the DMP is
			* turned off, the sampling rate remains at 200Hz. This could be
			* handled in inv_mpu.c, but it would need to know that
			* inv_mpu_dmp_motion_driver.c exists. To avoid this, we'll just
			* put the extra logic in the application layer.
			*/
			dmp_get_fifo_rate(&dmp_rate);
			mpu_set_sample_rate(dmp_rate);
			inv_quaternion_sensor_was_turned_off();
			MPL_LOGI("DMP disabled.\n");
		}
		else {
			unsigned short sample_rate;
			hal.dmp_on = 1;
			/* Preserve current FIFO rate. */
			mpu_get_sample_rate(&sample_rate);
			dmp_set_fifo_rate(sample_rate);
			inv_set_quat_sample_rate(1000000L / sample_rate);
			mpu_set_dmp_state(1);
			MPL_LOGI("DMP enabled.\n");
		}
		break;
	case 'm':
		/* Test the motion interrupt hardware feature. */
#ifndef MPU6050 // not enabled for 6050 product
		hal.motion_int_mode = 1;
#endif
		break;

	case 'v':
		/* Toggle LP quaternion.
		* The DMP features can be enabled/disabled at runtime. Use this same
		* approach for other features.
		*/
		hal.dmp_features ^= DMP_FEATURE_6X_LP_QUAT;
		dmp_enable_feature(hal.dmp_features);
		if (!(hal.dmp_features & DMP_FEATURE_6X_LP_QUAT)) {
			inv_quaternion_sensor_was_turned_off();
			MPL_LOGI("LP quaternion disabled.\n");
		}
		else
			MPL_LOGI("LP quaternion enabled.\n");
		break;
	default:
		break;
	}
	hal.rx.cmd = 0;
}

/* Every time new gyro data is available, this function is called in an
* ISR context. In this example, it sets a flag protecting the FIFO read
* function.
*/
void gyro_data_ready_cb(void)
{
	hal.new_gyro = 1;
}









void NVIC_Configuration(void);
uint8_t outbuf[5120];
uint8_t inbuf[5120];

int main(void)
{
	int i = 0;
	char stri[500];
	char stri5[2];
	int error;
	uint8_t letto;
	UINT BytesWritten;
	DWORD size;
	uint8_t detection = 0;
	uint8_t received_data[2];
	UINT Total = 0;

	inv_error_t result;
	unsigned char accel_fsr, new_temp = 0;
	unsigned short gyro_rate, gyro_fsr;
	unsigned long timestamp;
	short gyro[3], accel_short[3], sensors;
	struct int_param_s int_param;

#ifdef COMPASS_ENABLED
	unsigned char new_compass = 0;
	unsigned short compass_fsr;
#endif



	RCC_HSEConfig(RCC_HSE_ON);
	while (!RCC_WaitForHSEStartUp()) {
	}

	InitializeDecaIRQ();
	SysTick_Init();
	port_DisableEXT_IRQ();
	Sleep(100);
	SD_enable();
	InitializeLEDS();

	//	Button_Inter_init();
	SwitchOnLED(LED_RED);
	//	InitializeTimer();
	Sleep(100);
	SD_disable();
	Sleep(100);
	SD_enable();
	Sleep(100);

	//	SD_LowLevel_Init(); //Initialize PINS, vector table and SDIO interface


	//Initialize the SD
	InitExtUART();
	//InitBLEUART();
	//	sprintf(stri, "Ciao");//prepare string to be sent through uart
	//			USART_puts(USART6,stri);
	//switch off BLE
	//Initial();
	//GPIO_WriteBit(GPIOE, GPIO_Pin_0, Bit_RESET);
	BLE_enable();
	Sleep(100);
	BLE_disable();
	Sleep(100);
	//	BLE_enable();
	//	Sleep(100);

	/*//PROGRAMMING BLE START
	Initial();
	//	GPIO_WriteBit(GPIOE, GPIO_Pin_0, Bit_RESET);
	//	sleep(1);

	//switch on BLE
	//	GPIO_WriteBit(GPIOE, GPIO_Pin_0, Bit_SET);

	//	reset BLE
	GPIO_WriteBit(GPIOE, GPIO_Pin_4, Bit_RESET);
	GPIO_WriteBit(GPIOB, GPIO_Pin_6, Bit_RESET);
	SwitchOnLED(LED_RED);
	Sleep(300);
	GPIO_WriteBit(GPIOE, GPIO_Pin_4, Bit_SET);

	InitBLEUART_PROG();
	while(1){

	} //PROGRAMMING BLE END*/
	//GPIO_WriteBit(GPIOB, GPIO_Pin_6, Bit_RESET);
	//switch on BLE
	//		GPIO_WriteBit(GPIOE, GPIO_Pin_0, Bit_SET);
	//		sleep(1);
	//GPIO_WriteBit(GPIOB, GPIO_Pin_7, Bit_RESET);
	//reset BLE
	//			GPIO_WriteBit(GPIOE, GPIO_Pin_4, Bit_RESET);
	//	GPIO_WriteBit(GPIOB, GPIO_Pin_6, Bit_RESET);
	//	SwitchOnLED(LED_RED);
	//Sleep(300);
	//		GPIO_WriteBit(GPIOE, GPIO_Pin_4, Bit_SET);
	//Sleep(20);
	//GPIO_WriteBit(GPIOB, GPIO_Pin_6, Bit_SET);
	//		sleep(2);

	//			InitBLEUART();
	//		while(1){

	//		}

	/*// Start BLE APP
	GPIO_WriteBit(GPIOE, GPIO_Pin_4, Bit_RESET);

	Sleep(300);
	GPIO_WriteBit(GPIOE, GPIO_Pin_4, Bit_SET);
	//End BLE APP start*/

	board_init();

	BME_enable();
	Sleep(100);
	BME_disable();
	Sleep(100);
	//	 BME_enable();
	// Sleep(100);
	FuelG_alert_init();

	unsigned char data[2];

	char batt[2];
	char aler[2];
	char aler2[2];
	char conf[2];

	//fuel Gauge init

	Sleep(100);
	data[0] = 0x00;
	data[1] = 0x00;
	uint8_t id;

	//ALERT if Vbatt < 2.88 Volts
	HAL_FuelGaugeConfig(25);
	Sleep(5);
	Fuel_I2C_WriteRegister(0x36, 0x1A, 2, &data);



	NVIC_Configuration();

	memset(&fs32, 0, sizeof(FATFS));

	res = f_mount(0, &fs32);

	if (res == FR_OK) {
		/*if (GetLedState(LED_RED) == LED_ON)
		SwitchOffLED(LED_RED);
		else
		SwitchOnLED(LED_RED);*/
	}



	/*		 //init ext SPI and bmp280

	SPIBMEInit();
	id=read_8bit_serial_data(0xD0);
	if (id==0x58)
	SwitchOnLED(LED_YEL);
	else
	SwitchOnLED(LED_GRE);

	read_coefficients2();

	uint8_t dataToWrite = 0; //Temporary variable
	send_8bit_serial_data(BME280_CTRL_MEAS_REG,0x00);
	Sleep(5);
	id=read_8bit_serial_data(BME280_CTRL_MEAS_REG);
	Sleep(5);
	sprintf(stri, "%x\r\n",id); //this value has to be divided by 100
	res = f_open(&fil,"read.txt",FA_WRITE | FA_OPEN_ALWAYS);

	size = (&fil)->fsize;

	res = f_lseek(&fil,size);

	res = f_write(&fil,stri, strlen(stri),&BytesWritten);

	f_close(&fil);


	dataToWrite = (0x00 << 0x5);// & 0xE0;
	dataToWrite += (0x0 << 0x02);// & 0x1C;

	send_8bit_serial_data(BME280_CONFIG_REG,dataToWrite);
	Sleep(5);
	id=read_8bit_serial_data(BME280_CTRL_MEAS_REG);
	Sleep(5);
	sprintf(stri, "%x\r\n",id); //this value has to be divided by 100
	res = f_open(&fil,"read.txt",FA_WRITE | FA_OPEN_ALWAYS);

	size = (&fil)->fsize;

	res = f_lseek(&fil,size);

	res = f_write(&fil,stri, strlen(stri),&BytesWritten);

	f_close(&fil);



	//set ctrl_meas
	//First, set temp oversampling
	dataToWrite = (0x01 << 0x5);// & 0xE0;
	//Next, pressure oversampling
	dataToWrite += (0x01 << 0x02);// & 0x1C;
	//Last, set mode
	dataToWrite += 0x03;// & 0x03;

	send_8bit_serial_data(BME280_CTRL_MEAS_REG,dataToWrite);
	Sleep(5);
	id=read_8bit_serial_data(BME280_CTRL_MEAS_REG);
	Sleep(5);
	sprintf(stri, "%x\r\n",id); //this value has to be divided by 100
	res = f_open(&fil,"read.txt",FA_WRITE | FA_OPEN_ALWAYS);

	size = (&fil)->fsize;

	res = f_lseek(&fil,size);

	res = f_write(&fil,stri, strlen(stri),&BytesWritten);

	f_close(&fil);*/






	//BME init

	/*uint8_t dataToWrite = 0; //Temporary variable
	init_SPI1();
	//init_SPIEXT();
	//SPIBMEInit(SPI_BaudRatePrescaler_32);
	Sleep(20);
	id=read_8bit_serial_data(0xD0);
	if (id==0x60)
	SwitchOnLED(LED_GRE);
	sleep(2);
	SwitchOffLED(LED_GRE);

	//letto=read_8bit_serial_data(0xD0);
	letto=read_8bit_serial_data(0xD0);
	if (letto==0x60){
	//	SwitchOnLED(LED_YEL);

	}


	Sleep(20);
	read_coefficients();

	//while(1);
	Sleep(10);
	// 16x ovesampling p, 2x oversampling t normal mode
	GPIO_ResetBits(GPIOA, GPIO_Pin_4); //A4
	//send_8bit_serial_reg(BME280_CTRL_MEAS_REG,0x57);
	send_8bit_serial_data(BME280_CTRL_MEAS_REG,0x00); //sleep mode to config


	//mySPI_SendData(BME280_CTRL_MEAS_REG,ctrlm);
	GPIO_ResetBits(GPIOA, GPIO_Pin_5); //A5
	GPIO_SetBits(GPIOA, GPIO_Pin_4); //A4
	GPIO_ResetBits(GPIOA, GPIO_Pin_6); //A6

	Sleep(5);

	id=read_8bit_serial_data(BME280_CTRL_MEAS_REG);
	Sleep(5);
	sprintf(stri, "%x\r\n",id); //this value has to be divided by 100
	res = f_open(&fil,"read.txt",FA_WRITE | FA_OPEN_ALWAYS);

	size = (&fil)->fsize;

	res = f_lseek(&fil,size);

	res = f_write(&fil,stri, strlen(stri),&BytesWritten);

	f_close(&fil);


	//set humidity oversampling
	dataToWrite = 1 & 0x07;
	GPIO_ResetBits(GPIOA, GPIO_Pin_4); //A4
	//  send_8bit_serial_reg(BME280_CONFIG_REG,0x10);
	//   send_8bit_serial_data(BME280_CONFIG_REG,0x08); //ab
	send_8bit_serial_data(BME280_CTRL_HUMIDITY_REG,dataToWrite); //Set ctrl_hum first, then ctrl_meas to activate ctrl_hum
	//mySPI_SendData(BME280_CTRL_MEAS_REG,ctrlm);
	GPIO_ResetBits(GPIOA, GPIO_Pin_5); //A5
	GPIO_SetBits(GPIOA, GPIO_Pin_4); //A4
	GPIO_ResetBits(GPIOA, GPIO_Pin_6); //A6

	//again 0x00

	GPIO_ResetBits(GPIOA, GPIO_Pin_4); //A4
	//send_8bit_serial_reg(BME280_CTRL_MEAS_REG,0x57);
	send_8bit_serial_data(BME280_CTRL_MEAS_REG,0x00); //sleep mode to config


	//mySPI_SendData(BME280_CTRL_MEAS_REG,ctrlm);
	GPIO_ResetBits(GPIOA, GPIO_Pin_5); //A5
	GPIO_SetBits(GPIOA, GPIO_Pin_4); //A4
	GPIO_ResetBits(GPIOA, GPIO_Pin_6); //A6

	dataToWrite = (0 << 0x5) & 0xE0;
	dataToWrite |= (0 << 0x02) & 0x1C;
	GPIO_ResetBits(GPIOA, GPIO_Pin_4); //A4
	//  send_8bit_serial_reg(BME280_CONFIG_REG,0x10);
	//   send_8bit_serial_data(BME280_CONFIG_REG,0x08); //ab
	send_8bit_serial_data(BME280_CONFIG_REG,dataToWrite); //Set the config word
	//mySPI_SendData(BME280_CTRL_MEAS_REG,ctrlm);
	GPIO_ResetBits(GPIOA, GPIO_Pin_5); //A5
	GPIO_SetBits(GPIOA, GPIO_Pin_4); //A4
	GPIO_ResetBits(GPIOA, GPIO_Pin_6); //A6


	id=read_8bit_serial_data(BME280_CONFIG_REG);
	Sleep(5);
	sprintf(stri, "%x\r\n",id); //this value has to be divided by 100
	res = f_open(&fil,"read.txt",FA_WRITE | FA_OPEN_ALWAYS);

	size = (&fil)->fsize;

	res = f_lseek(&fil,size);

	res = f_write(&fil,stri, strlen(stri),&BytesWritten);

	f_close(&fil);




	//set ctrl_meas
	//First, set temp oversampling
	dataToWrite = (0x01 << 0x5) & 0xE0;
	//Next, pressure oversampling
	dataToWrite |= (0x01 << 0x02) & 0x1C;
	//Last, set mode
	dataToWrite |= 0x03 & 0x03;
	GPIO_ResetBits(GPIOA, GPIO_Pin_4); //A4
	//  send_8bit_serial_reg(BME280_CONFIG_REG,0x10);
	//   send_8bit_serial_data(BME280_CONFIG_REG,0x08); //ab
	send_8bit_serial_data(BME280_CTRL_MEAS_REG,dataToWrite); //Set the config word
	//mySPI_SendData(BME280_CTRL_MEAS_REG,ctrlm);
	GPIO_ResetBits(GPIOA, GPIO_Pin_5); //A5
	GPIO_SetBits(GPIOA, GPIO_Pin_4); //A4
	GPIO_ResetBits(GPIOA, GPIO_Pin_6); //A6

	id=read_8bit_serial_data(BME280_CTRL_MEAS_REG);
	Sleep(5);
	sprintf(stri, "%x\r\n",id); //this value has to be divided by 100
	res = f_open(&fil,"read.txt",FA_WRITE | FA_OPEN_ALWAYS);

	size = (&fil)->fsize;

	res = f_lseek(&fil,size);

	res = f_write(&fil,stri, strlen(stri),&BytesWritten);

	f_close(&fil);*/

	/* while(1){
	readT();

	readP();
	Sleep(25);
	}*/


	InitializeM0M1();


	//InitializeBattChargerIRQ();rr
	//EnableACOKIRQ();

	SetMxLow(M0);
	SetMxHigh(M1);

	SPIUWBInit(SPI_BaudRatePrescaler_32);
	dwt_configuresleep(DWT_LOADUCODE | DWT_PRESRV_SLEEP, DWT_WAKE_CS);
	dwt_entersleep();
	//	mySPI_SendData(BME280_CTRL_MEAS_REG,0xB7);
	//mySPI_SendData(BME280_CONFIG_REG,0x14);
	/*while(1){
	readTemperature();
	Sleep(5);
	readPressure();
	Sleep(100);
	}*/
	//				 	if (res == FR_OK){
	//				 		sprintf(stri, "error= %d",res);//prepare string to be sent through uart
	//				 						 		  			USART_puts(USART6,stri);
	//
	//				 	}

	// 	sleep(1);
	//uint32_t timerValue = GetTickCount();//TIM_GetCounter(TIM2);



	//		  Write a single block to the SD
	//	  for (i=0;i<512;i++) {
	//		     outbuf[i]=0x09;
	//		   }

	/*		   res = f_open(&fil, "HELLO.TXT", FA_CREATE_ALWAYS | FA_WRITE);

	sprintf(stri, "error= %d",res);//prepare string to be sent through uart
	USART_puts(USART6,stri);

	if (res == FR_OK)
	{

	UINT BytesWritten;
	char crlf[] = "\r\n";
	char *s = "Hello how are you?";

	res = f_write(&fil, s, strlen(s), &BytesWritten);

	res = f_write(&fil, crlf, strlen(crlf), &BytesWritten);

	res = f_close(&fil); // LENGTH.TXT

	//  		if (res != FR_OK)
	//	  		printf("res = %d f_close LENGTH.TXT\n", res);
	}*/




	result = mpu_init(&int_param);
	if (result) {

		//		      MPL_LOGE("Could not initialize gyro.\n");
	}


	/* If you're not using an MPU9150 AND you're not using DMP features, this
	* function will place all slaves on the primary bus.
	* mpu_set_bypass(1);
	*/

	result = inv_init_mpl();
	if (result) {

		//		      MPL_LOGE("Could not initialize MPL.\n");
	}
	// Sleep(100);

	inv_enable_quaternion();

	inv_enable_9x_sensor_fusion();
	/* Update gyro biases when not in motion.
	* WARNING: These algorithms are mutually exclusive.
	*/
	inv_enable_fast_nomot();
	/* inv_enable_motion_no_motion(); */
	/* inv_set_no_motion_time(1000); */

	/* Update gyro biases when temperature changes. */
	inv_enable_gyro_tc();
	/* This algorithm updates the accel biases when in motion. A more accurate
	* bias measurement can be made when running the self-test (see case 't' in
	* handle_input), but this algorithm can be enabled if the self-test can't
	* be executed in your application.
	*
	* inv_enable_in_use_auto_calibration();
	*/

	// inv_enable_in_use_auto_calibration();
#ifdef COMPASS_ENABLED
	/* Compass calibration algorithms. */
	inv_enable_vector_compass_cal();
	inv_enable_magnetic_disturbance();
#endif
	/* If you need to estimate your heading before the compass is calibrated,
	* enable this algorithm. It becomes useless after a good figure-eight is
	* detected, so we'll just leave it out to save memory.
	* inv_enable_heading_from_gyro();
	*/
	//  inv_enable_heading_from_gyro();

	/* Allows use of the MPL APIs in read_from_mpl. */
	inv_enable_eMPL_outputs();

	result = inv_start_mpl();





	if (result == INV_ERROR_NOT_AUTHORIZED) {
		while (1) {




			//   MPL_LOGE("Not authorized.\n");
		}
	}
	if (result) {

		//  MPL_LOGE("Could not start the MPL.\n");
	}
	/* Get/set hardware configuration. Start gyro. */
	/* Wake up all sensors. */
#ifdef COMPASS_ENABLED
	mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL | INV_XYZ_COMPASS);
#else
	mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL);
#endif
	/* Push both gyro and accel data into the FIFO. */
	mpu_configure_fifo(INV_XYZ_GYRO | INV_XYZ_ACCEL);
	mpu_set_sample_rate(1000);//DEFAULT_MPU_HZ

#ifdef COMPASS_ENABLED
							  /* The compass sampling rate can be less than the gyro/accel sampling rate.
							  * Use this function for proper power management.
							  */
	mpu_set_compass_sample_rate(1000 / COMPASS_READ_MS);
#endif


	/* Read back configuration in case it was set improperly. */
	mpu_get_sample_rate(&gyro_rate);
	mpu_get_gyro_fsr(&gyro_fsr);
	mpu_get_accel_fsr(&accel_fsr);
#ifdef COMPASS_ENABLED
	mpu_get_compass_fsr(&compass_fsr);
#endif
	/* Sync driver configuration with MPL. */
	/* Sample rate expected in microseconds. */
	inv_set_gyro_sample_rate(10);//(1000000L / gyro_rate);
	inv_set_accel_sample_rate(10);//(1000000L / gyro_rate);
#ifdef COMPASS_ENABLED
								  /* The compass rate is independent of the gyro and accel rates. As long as
								  * inv_set_compass_sample_rate is called with the correct value, the 9-axis
								  * fusion algorithm's compass correction gain will work properly.
								  */
	inv_set_compass_sample_rate(10);//(COMPASS_READ_MS * 1000L);
#endif
									/* Set chip-to-body orientation matrix.
									* Set hardware units to dps/g's/degrees scaling factor.
									*/
	inv_set_gyro_orientation_and_scale(
		inv_orientation_matrix_to_scalar(gyro_pdata.orientation),
		(long)gyro_fsr << 15);
	inv_set_accel_orientation_and_scale(
		inv_orientation_matrix_to_scalar(gyro_pdata.orientation),
		(long)accel_fsr << 15);
#ifdef COMPASS_ENABLED
	inv_set_compass_orientation_and_scale(
		inv_orientation_matrix_to_scalar(compass_pdata.orientation),
		(long)compass_fsr << 15);
#endif
	/* Initialize HAL state variables. */
#ifdef COMPASS_ENABLED
	hal.sensors = ACCEL_ON | GYRO_ON | COMPASS_ON;
#else
	hal.sensors = ACCEL_ON | GYRO_ON;
#endif
	hal.dmp_on = 0;
	hal.report = 0;
	hal.rx.cmd = 0;
	hal.next_pedo_ms = 0;
	hal.next_compass_ms = 0;
	hal.next_temp_ms = 0;
	/* Compass reads are handled by scheduler. */

	// get_tick_count(&timestamp);

	/* To initialize the DMP:
	* 1. Call dmp_load_motion_driver_firmware(). This pushes the DMP image in
	*    inv_mpu_dmp_motion_driver.h into the MPU memory.
	* 2. Push the gyro and accel orientation matrix to the DMP.
	* 3. Register gesture callbacks. Don't worry, these callbacks won't be
	*    executed unless the corresponding feature is enabled.
	* 4. Call dmp_enable_feature(mask) to enable different features.
	* 5. Call dmp_set_fifo_rate(freq) to select a DMP output rate.
	* 6. Call any feature-specific control functions.
	*
	* To enable the DMP, just call mpu_set_dmp_state(1). This function can
	* be called repeatedly to enable and disable the DMP at runtime.
	*
	* The following is a short summary of the features supported in the DMP
	* image provided in inv_mpu_dmp_motion_driver.c:
	* DMP_FEATURE_LP_QUAT: Generate a gyro-only quaternion on the DMP at
	* 200Hz. Integrating the gyro data at higher rates reduces numerical
	* errors (compared to integration on the MCU at a lower sampling rate).
	* DMP_FEATURE_6X_LP_QUAT: Generate a gyro/accel quaternion on the DMP at
	* 200Hz. Cannot be used in combination with DMP_FEATURE_LP_QUAT.
	* DMP_FEATURE_TAP: Detect taps along the X, Y, and Z axes.
	* DMP_FEATURE_ANDROID_ORIENT: Google's screen rotation algorithm. Triggers
	* an event at the four orientations where the screen should rotate.
	* DMP_FEATURE_GYRO_CAL: Calibrates the gyro data after eight seconds of
	* no motion.
	* DMP_FEATURE_SEND_RAW_ACCEL: Add raw accelerometer data to the FIFO.
	* DMP_FEATURE_SEND_RAW_GYRO: Add raw gyro data to the FIFO.
	* DMP_FEATURE_SEND_CAL_GYRO: Add calibrated gyro data to the FIFO. Cannot
	* be used in combination with DMP_FEATURE_SEND_RAW_GYRO.
	*/
	dmp_load_motion_driver_firmware();
	//   dmp_set_orientation(inv_orientation_matrix_to_scalar(gyro_pdata.orientation));
	//dmp_register_tap_cb(tap_cb);
	// dmp_register_android_orient_cb(android_orient_cb);
	/*
	* Known Bug -
	* DMP when enabled will sample sensor data at 200Hz and output to FIFO at the rate
	* specified in the dmp_set_fifo_rate API. The DMP will then sent an interrupt once
	* a sample has been put into the FIFO. Therefore if the dmp_set_fifo_rate is at 25Hz
	* there will be a 25Hz interrupt from the MPU device.
	*
	* There is a known issue in which if you do not enable DMP_FEATURE_TAP
	* then the interrupts will be at 200Hz even if fifo rate
	* is set at a different rate. To avoid this issue include the DMP_FEATURE_TAP
	*
	* DMP sensor fusion works only with gyro at +-2000dps and accel +-2G
	*/
	//hal.dmp_features = DMP_FEATURE_6X_LP_QUAT | DMP_FEATURE_TAP |
	//    DMP_FEATURE_ANDROID_ORIENT | DMP_FEATURE_SEND_RAW_ACCEL | DMP_FEATURE_SEND_CAL_GYRO |
	//     DMP_FEATURE_GYRO_CAL;
	hal.dmp_features = DMP_FEATURE_SEND_RAW_ACCEL | DMP_FEATURE_SEND_CAL_GYRO | DMP_FEATURE_GYRO_CAL;
	dmp_enable_feature(hal.dmp_features);
	dmp_set_fifo_rate(1000);//DEFAULT_MPU_HZ
	mpu_set_dmp_state(1);
	hal.dmp_on = 1;
	hal.new_gyro = 0;
	//    handle_input();

	/* START of Lukes code	*/
#define SCALE_ON 1
#define SCALE_OFF 0
#define INIT_CAP 10000
#define NC 3
#define ND 3
	double acc_norm = 0;
	double acc_filt[33] = { 0 };
	uint64_t acc_prev_time = 0;
	int acc_size = 0;
	UINT EulerBytesWritten;
	UINT TestBytesWritten;
	DWORD euler_size;
	DWORD test_size;
	double qosserv_prev[4] = { 0 };
	qosserv_prev[0] = 1;
	double qfilt_prev[4] = { 0 };
	double accf[3] = { 0 };
	double euler_angles[3] = { 0 };
	char euler_stri[1000];
	char test_stri[500];

	double dcof[ND] = { 1.0, -1.778631777916921, 0.80080264670800094 };
	double ccof[NC] = { 0.0055427171987608272, 0.011085434397521654, 0.0055427171987608272 };
	double acc_z_x[ND] = { 0 };
	double acc_z_y[ND] = { 0 };
	double acc_z_z[ND] = { 0 };
	double gyr_z_x[ND] = { 0 };
	double gyr_z_y[ND] = { 0 };
	double gyr_z_z[ND] = { 0 };
	/* END of Lukes code	*/

	sprintf(stri, "\r\n\r\n---------------------------------------------------\r\n\r\n");
	res = f_open(&fil, "sens.txt", FA_WRITE | FA_OPEN_ALWAYS);
	euler_res = f_open(&euler_fil, "euler.txt", FA_WRITE | FA_OPEN_ALWAYS);
	test_res = f_open(&test_fil, "test.txt", FA_WRITE | FA_OPEN_ALWAYS);

	size = (&fil)->fsize;
	euler_size = (&euler_fil)->fsize;
	test_size = (&test_fil)->fsize;

	res = f_lseek(&fil, size);
	euler_res = f_lseek(&euler_fil, euler_size);
	test_res = f_lseek(&test_fil, test_size);

	res = f_write(&fil, stri, strlen(stri), &BytesWritten);
	euler_res = f_write(&euler_fil, stri, strlen(stri), &EulerBytesWritten);
	test_res = f_write(&test_fil, stri, strlen(stri), &TestBytesWritten);

	f_close(&fil);
	f_close(&euler_fil);
	f_close(&test_fil);

	SwitchOnLED(LED_GRE);
	SwitchOffLED(LED_RED);
	while (1) {

		unsigned long sensor_timestamp;
		int new_data = 0;
		short int_status;
		short fifoCount;
		char stri3[50];
		uint64_t timerValue = 0;
		char batt[2];

		hal.new_gyro = 1;		// mar, set new_gyro to indicate new FIFO data is ready for processing

		timerValue = GetTickCount();

		//     readP();
		//     readHumidity();
		new_compass = 1;
		new_temp = 0;

		if (!hal.sensors || !hal.new_gyro) {
			continue;
		}

		if (hal.new_gyro && hal.lp_accel_mode) {
			short accel_short[3];
			long accel[3];
			mpu_get_accel_reg(accel_short, &sensor_timestamp);
			accel[0] = (long)accel_short[0];
			accel[1] = (long)accel_short[1];
			accel[2] = (long)accel_short[2];
			inv_build_accel(accel, 0, sensor_timestamp);
			new_data = 1;
			hal.new_gyro = 0;
		}
		else if (hal.new_gyro && hal.dmp_on) {
			//  short gyro[3], accel_short[3], sensors;
			unsigned char more;

			long accel[3], quat[4], temperature;
			UINT BytesWritten;
			long data[3] = { 0 };

			int8_t accuracy = { 3 };
			unsigned long timestamp2;
			char stri[100];
			char stri2[100];

			DWORD size;

			sensors = INV_XYZ_GYRO | INV_XYZ_ACCEL | INV_XYZ_COMPASS;

			if (sensors & INV_XYZ_GYRO) {
				mpu_get_gyro_reg(gyro, &sensor_timestamp);

				new_data = 1;

				inv_build_gyro(gyro, sensor_timestamp);

				if (new_temp) {
					new_temp = 0;
					/* Temperature only used for gyro temp comp. */
					mpu_get_temperature(&temperature, &sensor_timestamp);

					inv_build_temp(temperature, sensor_timestamp);
					imutemp = temperature;
				}
			}
			if (sensors & INV_XYZ_ACCEL) {
				mpu_get_accel_reg(accel_short, &sensor_timestamp);

				new_data = 1;
			}

		}
		else if (hal.new_gyro) {
			short gyro[3], accel_short[3], sensors;
			unsigned char more;
			long accel[3], quat[4], temperature;
			/* This function gets new data from the FIFO. The FIFO can contain
			* gyro, accel, both, or neither. The sensors parameter tells the
			* caller which data fields were actually populated with new data.
			* For example, if sensors == INV_XYZ_GYRO, then the FIFO isn't
			* being filled with accel data. The more parameter is non-zero if
			* there are leftover packets in the FIFO. The HAL can use this
			* information to increase the frequency at which this function is
			* called.
			*/

			hal.new_gyro = 0;
			mpu_read_fifo(gyro, accel_short, &sensor_timestamp,
				&sensors, &more);
			if (more)
				hal.new_gyro = 1;
			if (sensors & INV_XYZ_GYRO) {
				/* Push the new data to the MPL. */
				inv_build_gyro(gyro, sensor_timestamp);
				new_data = 1;
				if (new_temp) {
					new_temp = 0;
					/* Temperature only used for gyro temp comp. */
					mpu_get_temperature(&temperature, &sensor_timestamp);
					inv_build_temp(temperature, sensor_timestamp);
				}
			}
			if (sensors & INV_XYZ_ACCEL) {
				accel[0] = (long)accel_short[0];
				accel[1] = (long)accel_short[1];
				accel[2] = (long)accel_short[2];
				inv_build_accel(accel, INV_CALIBRATED | 3, sensor_timestamp);
				new_data = 1;
			}
		}
#ifdef COMPASS_ENABLED
		if (new_compass) {
			short compass_short[3];
			long compass[3];
			new_compass = 0;
			/* For any MPU device with an AKM on the auxiliary I2C bus, the raw
			* magnetometer registers are copied to special gyro registers.
			*/
			if (!mpu_get_compass_reg(compass_short, &sensor_timestamp)) {
				compass[0] = (long)compass_short[0];
				compass[1] = (long)compass_short[1];
				compass[2] = (long)compass_short[2];
				/* NOTE: If using a third-party compass calibration library,
				* pass in the compass data in uT * 2^16 and set the second
				* parameter to INV_CALIBRATED | acc, where acc is the
				* accuracy from 0 to 3.
				*/
				inv_build_compass(compass, INV_CALIBRATED | 3, sensor_timestamp);
			}
			// send_status_compass();
			int8_t accuracy = { 3 };
			unsigned long timestamp;
			inv_get_compass_set(datamag, &accuracy, (inv_time_t*)&timestamp);
			new_data = 1;

			/*	START of Lukes code	*/

			/* Perform the JCS conversion in real time and converts values */
			double acc_double_x = ((double)accel_short[2]) * 2000 / 32768; // (mg)
			double acc_double_y = -((double)accel_short[0]) * 2000 / 32768; // (mg)
			double acc_double_z = -((double)accel_short[1]) * 2000 / 32768; // (mg)

			double gyr_double_x = ((double)gyro[2]) * 2000 / 32768; // (deg/s)
			double gyr_double_y = -((double)gyro[0]) * 2000 / 32768; // (deg/s)
			double gyr_double_z = -((double)gyro[1]) * 2000 / 32768; // (deg/s)

																	 /* Filter the new data */
			double new_acc_double_x = ccof[0] * acc_double_x + acc_z_x[0];
			double new_acc_double_y = ccof[0] * acc_double_y + acc_z_y[0];
			double new_acc_double_z = ccof[0] * acc_double_z + acc_z_z[0];
			double new_gyr_double_x = ccof[0] * gyr_double_x + gyr_z_x[0];
			double new_gyr_double_y = ccof[0] * gyr_double_y + gyr_z_y[0];
			double new_gyr_double_z = ccof[0] * gyr_double_z + gyr_z_z[0];
			for (i = 1; i < ND; i++) {
				acc_z_x[i - 1] = ccof[i] * acc_double_x + acc_z_x[i] - dcof[i] * new_acc_double_x;
				acc_z_y[i - 1] = ccof[i] * acc_double_y + acc_z_y[i] - dcof[i] * new_acc_double_y;
				acc_z_z[i - 1] = ccof[i] * acc_double_z + acc_z_z[i] - dcof[i] * new_acc_double_z;
				gyr_z_x[i - 1] = ccof[i] * gyr_double_x + gyr_z_x[i] - dcof[i] * new_gyr_double_x;
				gyr_z_y[i - 1] = ccof[i] * gyr_double_y + gyr_z_y[i] - dcof[i] * new_gyr_double_y;
				gyr_z_z[i - 1] = ccof[i] * gyr_double_z + gyr_z_z[i] - dcof[i] * new_gyr_double_z;
			}
			acc_double_x = new_acc_double_x;
			acc_double_y = new_acc_double_y;
			acc_double_z = new_acc_double_z;
			gyr_double_x = new_gyr_double_x;
			gyr_double_y = new_gyr_double_y;
			gyr_double_z = new_gyr_double_z;


			sprintf(test_stri, "%li\t%li\t%li\t%li\t%li\t%li\t%llu\r\n", (long)acc_double_x, (long)acc_double_y, (long)acc_double_z, (long)gyr_double_x, (long)gyr_double_y, (long)gyr_double_z, timerValue);
			//sprintf(euler_stri, "%i\r\n", acc_size);
			test_res = f_open(&test_fil, "test.txt", FA_WRITE | FA_OPEN_ALWAYS);
			test_size = (&test_fil)->fsize;
			test_res = f_lseek(&test_fil, test_size);
			test_res = f_write(&test_fil, test_stri, strlen(test_stri), &TestBytesWritten);
			f_close(&test_fil);




			/* Normalize the new accel data */
			acc_norm = pow(acc_double_x, 2) + pow(acc_double_y, 2) + pow(acc_double_z, 2);
			acc_norm = sqrt(acc_norm);

			/* Fill in first 11 rows of normalized accel data */
			if (acc_size <= ACC_LENGTH - 3) {
				if (acc_norm == 0)
					acc_norm = 1;
				acc_filt[acc_size] = acc_double_x / acc_norm;
				acc_filt[acc_size + 1] = acc_double_y / acc_norm;
				acc_filt[acc_size + 2] = acc_double_z / acc_norm;
				acc_size = acc_size + 2;
			}
			else if (acc_size >= 35) {
				int j = 0;
				int index = acc_size - 23;
				/* Move all elements down three to make room for new data points */
				for (j = 0; j < ACC_LENGTH - 3; j++) {
					acc_filt[j] = acc_filt[j + 3];
				}
				/* Fill in the next row of accel data (last three positions in array) */
				acc_filt[ACC_LENGTH - 3] = acc_double_x / acc_norm;
				acc_filt[ACC_LENGTH - 2] = acc_double_y / acc_norm;
				acc_filt[ACC_LENGTH - 1] = acc_double_z / acc_norm;
				double dt = ((double)timerValue - (double)acc_prev_time) / 1000; // change timer value to seconds
				double gyro_vec[4] = { 0,gyr_double_x, gyr_double_y, gyr_double_z };
				quaternion_calc(acc_filt, gyro_vec, dt, index, euler_angles, qosserv_prev, qfilt_prev, accf); // Perform quaternion calculation

																											  //            	            	  /* Print results to "EULER" text file */
				sprintf(euler_stri, "%li\t%li\t%li\t%llu\r\n", (long)euler_angles[0], (long)euler_angles[1], (long)euler_angles[2], timerValue, acc_size);
				euler_res = f_open(&euler_fil, "euler.txt", FA_WRITE | FA_OPEN_ALWAYS);
				euler_size = (&euler_fil)->fsize;
				euler_res = f_lseek(&euler_fil, euler_size);
				euler_res = f_write(&euler_fil, euler_stri, strlen(euler_stri), &EulerBytesWritten);
				f_close(&euler_fil);
			}
			acc_size++;
			acc_prev_time = timerValue; // Update previous time stamp for next sample
			short x_tmp = accel_short[0];
			short y_tmp = accel_short[1];
			accel_short[0] = accel_short[2] * 2000 / 32768;
			accel_short[1] = -x_tmp * 2000 / 32768;
			accel_short[2] = -y_tmp * 2000 / 32768;
			short x_tmp_g = gyro[0];
			short y_tmp_g = gyro[1];
			gyro[0] = gyro[2] * 2000 / 32768;
			gyro[1] = -x_tmp_g * 2000 / 32768;
			gyro[2] = -y_tmp_g * 2000 / 32768;

			/*	END of Lukes code	*/



			//      sprintf(stri, "%hi  %hi  %hi  %hi  %hi  %hi  %li  %li  %li  %hi  %d  %d  %li  %llu\r\n",accel_short[0],accel_short[1],accel_short[2],gyro[0],gyro[1],gyro[2],datamag[0],datamag[1],datamag[2],imutemp,bartemp,barpress,adc_h,timerValue);
			sprintf(stri, "%hi\t%hi\t%hi\t%hi\t%hi\t%hi\t%llu\r\n", accel_short[0], accel_short[1], accel_short[2], gyro[0], gyro[1], gyro[2], timerValue);
			res = f_open(&fil, "sens.txt", FA_WRITE | FA_OPEN_ALWAYS);

			size = (&fil)->fsize;

			res = f_lseek(&fil, size);

			res = f_write(&fil, stri, strlen(stri), &BytesWritten);

			f_close(&fil);

		}
#endif
		if (new_data) {
			inv_execute_on_data();
			/* This function reads bias-compensated sensor data and sensor
			* fusion outputs from the MPL. The outputs are formatted as seen
			* in eMPL_outputs.c. This function only needs to be called at the
			* rate requested by the host.
			*/
			//           read_from_mpl();

			//Sleep(100);
		}

	}

	//  free(q);



	//		printf("res = %d f_mount\n", res);





	//	error=  SD_Init();  //After return from this function sd is in transfer mode.
	//	sprintf(stri, "error= %d",error);//prepare string to be sent through uart
	//		 		  			USART_puts(USART6,stri);

	//			if (error!=0){
	//

	//	}

	//Single Block Test
	/* error= SD_WriteMultiBlocksFIXED(outbuf,0,512,1);
	//   Status = SD_WriteMultiBlocksFIXED(buff, sector, BLOCK_SIZE, count); // 4GB Compliant
	sprintf(stri, "error= %d",error);//prepare string to be sent through uart
	USART_puts(USART6,stri);
	if (error == SD_OK)
	{
	SDTransferState State;

	error = SD_WaitWriteOperation(); // Check if the Transfer is finished

	while((State = SD_GetStatus()) == SD_TRANSFER_BUSY); // BUSY, OK (DONE), ERROR (FAIL)

	if ((State == SD_TRANSFER_ERROR) || (error!= SD_OK)){
	sprintf(stri, "RES_ERROR = %d",error);//prepare string to be sent through uart
	USART_puts(USART6,stri);
	}
	else
	{
	sprintf(stri, "RES_OK");//prepare string to be sent through uart
	USART_puts(USART6,stri);
	}
	}
	Sleep(500);

	error= SD_ReadMultiBlocksFIXED(inbuf,0,512,10);
	//   Status = SD_WriteMultiBlocksFIXED(buff, sector, BLOCK_SIZE, count); // 4GB Compliant
	sprintf(stri, "error= %d",error);//prepare string to be sent through uart
	USART_puts(USART6,stri);
	if (error == SD_OK)
	{
	SDTransferState State;

	error = SD_WaitReadOperation(); // Check if the Transfer is finished

	while((State = SD_GetStatus()) == SD_TRANSFER_BUSY); // BUSY, OK (DONE), ERROR (FAIL)

	if ((State == SD_TRANSFER_ERROR) || (error!= SD_OK)){
	sprintf(stri, "RES_ERROR = %d",error);//prepare string to be sent through uart
	USART_puts(USART6,stri);
	}
	else
	{
	sprintf(stri, "RES_OK");//prepare string to be sent through uart
	USART_puts(USART6,stri);
	}
	}

	//error=SD_WriteBlock(outbuf, 0,512);
	//SD_WaitWriteOperation();
	//while(SD_GetStatus() != SD_TRANSFER_OK);
	for(i=0;i<5120;i++){
	sprintf(stri5, "%x",inbuf[i]);//prepare string to be sent through uart
	USART_puts(USART6,stri5);
	}*/
	Sleep(1);
	if (GetLedState(LED_YEL) == LED_ON)
		SwitchOffLED(LED_YEL);
	else
		SwitchOnLED(LED_YEL);

	InitializeM0M1();


	//InitializeBattChargerIRQ();
	//EnableACOKIRQ();
	//Input current limit setting (USB 3.0 mode)
	SetMxLow(M0);
	SetMxHigh(M1);

	SPIUWBInit(SPI_BaudRatePrescaler_32);

	while (instancereaddeviceid() != DWT_DEVICE_ID) {
		Sleep(250);
		dwt_softreset();
		SPIUWBInit(SPI_BaudRatePrescaler_32);
		if (GetLedState(LED_RED) == LED_ON)
			SwitchOffLED(LED_RED);
		else
			SwitchOnLED(LED_RED);
	}
	Sleep(100);
	inittestapplication();
	port_EnableEXT_IRQ();

	while (1) {
		SwitchOffLED(LED_GRE);
		Sleep(50);
		deca_app();
		SwitchOnLED(LED_GRE);
	}//while

}

/**
* @brief  Configures SDIO IRQ channel.
* @param  None
* @retval None
*/
void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Configure the NVIC Preemption Priority Bits */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

	NVIC_InitStructure.NVIC_IRQChannel = SDIO_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
}
void SDIO_IRQHandler(void)
{
	SD_ProcessIRQSrc();
}
void SD_SDIO_DMA_IRQHANDLER(void)
{
	SD_ProcessDMAIRQ();
}
#ifdef  USE_FULL_ASSERT
/**
* @brief  Reports the name of the source file and the source line number
*         where the assert_param error has occurred.
* @param  file: pointer to the source file name
* @param  line: assert_param error line source number
* @retval None
*/
void assert_failed(uint8_t* file, uint32_t line)
{
	/* User can add his own implementation to report the file name and line number,
	ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	while (1); /* Infinite loop */
}
#endif
