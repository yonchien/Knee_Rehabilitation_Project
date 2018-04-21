/**
*****************************************************************************
**
**  File        : syscalls.c
**
**  Abstract    : Atollic TrueSTUDIO Minimal System calls file
**
** 		          For more information about which c-functions
**                need which of these lowlevel functions
**                please consult the Newlib libc-manual
**
**  Environment : Atollic TrueSTUDIO
**
**  Distribution: The file is distributed “as is,” without any warranty
**                of any kind.
**
**  (c)Copyright Atollic AB.
**  You may use this file as-is or modify it according to the needs of your
**  project. This file may only be built (assembled or compiled and linked)
**  using the Atollic TrueSTUDIO(R) product. The use of this file together
**  with other tools than Atollic TrueSTUDIO(R) is not permitted.
**
*****************************************************************************
*/

/* Includes */
#include <stdint.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>
#include <stdint.h>
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"

#include "deca_port.h"

#ifdef USE_USB_VCP_LOG
#include "usbd_cdc_vcp.h"
#else
/*#include "serial.h"*/
#endif


/* Variables */
#undef errno
extern int32_t errno;

register uint8_t * stack_ptr asm("sp");


char *__env[1] = { 0 };
char **environ = __env;


/* Functions */
void initialise_monitor_handles()
{
}

int _getpid(void)
{
	return 1;
}

int _kill(int32_t pid, int32_t sig)
{
	errno = EINVAL;
	return -1;
}

void _exit (int status)
{
	_kill(status, -1);
	while (1) {}		/* Make sure we hang here */
}


int _write(int32_t file, uint8_t *ptr, int32_t len)
{
#ifdef USE_USB_VCP_LOG
	VCP_send_buffer((uint8_t*)ptr, len);
#else
	/* Implement your write code here, this is used by puts and printf for example */
	int32_t i = 0;
	while(i < len)
	{
		SendByte_UART2(*ptr++);
		i++;
	}
#endif

	return len;
}


caddr_t _sbrk(int32_t incr)
{
	extern uint8_t end asm("end");
	static uint8_t *heap_end;
	uint8_t *prev_heap_end;

	if (heap_end == 0)
		heap_end = &end;

	prev_heap_end = heap_end;
	if (heap_end + incr > stack_ptr)
	{
//		write(1, "Heap and stack collision\n", 25);
//		abort();
		errno = ENOMEM;
		return (caddr_t) -1;
	}

	heap_end += incr;

	return (caddr_t) prev_heap_end;
}

int _close(int32_t file)
{
	return -1;
}


int _fstat(int32_t file, struct stat *st)
{
	st->st_mode = S_IFCHR;
	return 0;
}

int _isatty(int32_t file)
{
	return 1;
}

int _lseek(int32_t file, int32_t ptr, int32_t dir)
{
	return 0;
}

#if 0
int _read_r(int32_t file, uint8_t *ptr, int32_t len)
{

	int32_t count = 0;
	while(len > 0)
	{
		if(!IsBufferEmpty())
		{
			uint8_t ch = GetRcvElement();
			if(ch == '\r' || ch == '\n')
			{
				break;
			}
			else
			{
				*ptr++ = ch;
				count++;
				len--;
			}
		}
	}
	return ((count > 0) ? count : -1);

}
#endif


int _read(int32_t file, uint8_t *ptr, int32_t len)
{
#ifdef USE_USB_VCP_LOG
	if (file != 0) {
		return 0;
	}
	// Use USB CDC Port for stdin
	while(!VCP_get_char((uint8_t*)ptr)){};
	// Echo typed characters
	VCP_put_char((uint8_t)*ptr);
	return 1;
#else
	uint8_t ch = 0x00;
	int32_t count = 0;
	while(len-- > 0)
	{
		//if((ch = GetByte_UART2()) == 0x0A)
		if((ch = GetRcvElement()) == 0x0A)
		{
			break;
		}
		else
		{
			*ptr++ = ch;
			count++;
		}
	}
	return ((count > 0) ? count : -1);
#endif
}



int _open(const uint8_t *path, int32_t flags, int32_t mode)
{
	/* Pretend like we always fail */
	return -1;
}

int _wait(int32_t *status)
{
	errno = ECHILD;
	return -1;
}

int _unlink(const uint8_t *name)
{
	errno = ENOENT;
	return -1;
}

int _times(struct tms *buf)
{
	return -1;
}

/* Return a clock that ticks at CLOCKS_PER_SEC Hz.  */
clock_t _times_r(void *reent, struct tms * tp)
{
  clock_t timeval = portGetTickCount(); //time32_incr;

  if (tp)
    {
      tp->tms_utime  = timeval;	/* user time */
      tp->tms_stime  = 0;	/* system time */
      tp->tms_cutime = 0;	/* user time, children */
      tp->tms_cstime = 0;	/* system time, children */
    }

  return timeval;
};

int _stat(const uint8_t *file, struct stat *st)
{
	st->st_mode = S_IFCHR;
	return 0;
}

int _link(const uint8_t *old, const uint8_t *new)
{
	errno = EMLINK;
	return -1;
}

int _fork(void)
{
	errno = EAGAIN;
	return -1;
}

int _execve(const uint8_t *name, uint8_t * const *argv, uint8_t * const *env)
{
	errno = ENOMEM;
	return -1;
}

