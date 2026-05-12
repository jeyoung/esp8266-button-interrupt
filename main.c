#include "mem.h"
#include "osapi.h"
#include "user_interface.h"
#include "uart.h"

#include "button.h"

static os_timer_t os_timer;
static uint32 heartbeat = 0;

static void on_timer_main(void *arg)
{
	button_tick();
	if (++heartbeat % 1000 == 0)
		os_printf(".");
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
#define RF_CAL_ADDR     0xfb000
#define RF_CAL_SIZE     0x1000
#define PHY_DATA_ADDR   0xfc000
#define PHY_DATA_SIZE   0x1000
#define SYS_PARAM_ADDR  0xfd000
#define SYS_PARAM_SIZE  0x3000

void ICACHE_FLASH_ATTR user_pre_init(void)
{
	bool rc = false;
	static const partition_item_t part_table[] = 
	{
		{SYSTEM_PARTITION_RF_CAL,
			RF_CAL_ADDR,
			RF_CAL_SIZE},
		{SYSTEM_PARTITION_PHY_DATA,
			PHY_DATA_ADDR,
			PHY_DATA_SIZE},
		{SYSTEM_PARTITION_SYSTEM_PARAMETER,
			SYS_PARAM_ADDR,
			SYS_PARAM_SIZE},
	};

	int retries = 10;
	while (!rc && retries--) {
		rc = system_partition_table_regist(part_table,
				sizeof(part_table)/sizeof(part_table[0]),
				2);
		if (!rc)
			os_printf("Failed to register partition table, retrying...\r\n");
	}
	if (!rc)
		system_restart();

	return;
}
