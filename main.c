#include "mem.h"
#include "osapi.h"
#include "user_interface.h"
#include "uart.h"

#include "main.h"

#define PIN_BUTTON 2
#define PIN_LED 0

static os_timer_t os_timer;
volatile static int elapsed = 0, elapsed_debounce = 0;

void button_pressed(int button_id);

/* Main timer tick handler
 */
static void on_timer_main(void *arg)
{
	if (++elapsed % 1000 == 0) 
		os_printf("%d >> \n", elapsed);
	os_timer_arm(&os_timer, 1, 0);
}

/* Pin interrupt handler
 */
static void handle_gpio(void *arg)
{
	uint32 status = GPIO_REG_READ(GPIO_STATUS_ADDRESS);
	GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, status);
	if (elapsed_debounce == 0 || elapsed - elapsed_debounce >= 30) {
		elapsed_debounce = elapsed;
		button_pressed(status);
	}
}

void button_pressed(int button_id)
{
	os_printf("Button %d\r\n", button_id);
}

/* User-defined intialisation
 */
void ICACHE_FLASH_ATTR user_init(void)
{
	uart_init(BIT_RATE_115200, BIT_RATE_115200);

	gpio_init();
	gpio_pin_intr_state_set(GPIO_ID_PIN(PIN_BUTTON), GPIO_PIN_INTR_NEGEDGE);
	gpio_pin_intr_state_set(GPIO_ID_PIN(PIN_LED), GPIO_PIN_INTR_NEGEDGE);
	ETS_GPIO_INTR_ATTACH(&handle_gpio, (void *)NULL);
	ETS_GPIO_INTR_ENABLE();
	GPIO_DIS_OUTPUT(PIN_BUTTON);

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
