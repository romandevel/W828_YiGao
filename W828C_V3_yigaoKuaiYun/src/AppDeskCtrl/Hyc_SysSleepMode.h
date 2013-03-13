#ifndef HYC_SYS_SLEEP_MODE
#define HYC_SYS_SLEEP_MODE

#include "hyTypes.h"

#include "hyhwTcc7901.h"
#include "hyhwGpio.h"


#define SDIO_WAKEUP			BIT0
#define KEY_WAKEUP			BIT1
#define UART1_WAKEUP		BIT2
#define TIME_30S_WAKEUP		BIT3
#define LOW_POWER_WAKEUP	BIT4
#define USB_VBUS_WAKEUP		BIT5
#define POWER_OFF_WAKEUP	BIT6
#define G_KEY_WAKEUP	    BIT7
#define W_KEY_WAKEUP	    BIT8
#define ALARM_WAKEUP        BIT9

typedef struct _tCTR_WAKE_KEY
{
     volatile U32  *pOutPort;
     volatile U32  outGpio;
     volatile U32  *pInPort;
     volatile U32  inGpio;
     U32  valid;
     U32  gpioStatus;
     U32  detectCnt;
     U32  maxKeyTotal;
     U32  wakeType;
          
}tCTR_WAKE_KEY;


//#define TEST_GPIO_SLEEPENABLE

//#define TEST_USE_GPIO
#ifdef TEST_USE_GPIO
	#define TEST_PORT		(PORT_XA)
	#define TEST_GPIO		(GPIO_31)
#endif

#endif