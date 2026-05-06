#include "mem.h"
#include "osapi.h"
#include "user_interface.h"
#include "uart.h"

#include "main.h"
#include "button.h"

static os_timer_t os_timer;
volatile int elapsed = 0;

static void on_timer_main(void *arg)
{
	++elapsed;
	os_timer_arm(&os_timer, 1, 0);
}

void ICACHE_FLASH_ATTR user_init(void)
{
	uart_init(BIT_RATE_115200, BIT_RATE_115200);

	button_init();

	os_timer_disarm(&os_timer);
	os_timer_setfn(&os_timer, &on_timer_main, (void *)NULL);
	os_timer_arm(&os_timer, 1, 0);
}

// user_pre_init is required from SDK v3.0.0 onwards
// It is used to register the parition map with the SDK, primarily to allow the
// app to use the SDK's OTA capability. We don't make use of that in otb-iot
// and therefore the only info we provide is the mandatory stuff:
// - RF calibration data
// - Physical data
// - System parameter
// The location and length of these are from the 2A SDK getting started guide
void ICACHE_FLASH_ATTR user_pre_init(void)
{
	bool rc = false;
	static const partition_item_t part_table[] = 
	{
		{SYSTEM_PARTITION_RF_CAL,
			0xfb000,
			0x1000},
		{SYSTEM_PARTITION_PHY_DATA,
			0xfc000,
			0x1000},
		{SYSTEM_PARTITION_SYSTEM_PARAMETER,
			0xfd000,
			0x3000},
	};

	// This isn't an ideal approach but there's not much point moving on
	// unless or until this has succeeded cos otherwise the SDK will just
	// barf and refuse to call user_init()
	while (!rc)
		rc = system_partition_table_regist(part_table,
				sizeof(part_table)/sizeof(part_table[0]),
				2);

	return;
}
