#include "osapi.h"
#include "user_interface.h"

#include "button.h"

static int elapsed_debounce = 0;

static bool ICACHE_FLASH_ATTR debounce_ok(void)
{
	if (elapsed_debounce == 0 || elapsed - elapsed_debounce >= DEBOUNCE_MS) {
		elapsed_debounce = elapsed;
		return true;
	}
	return false;
}

static void handle_gpio(void *arg)
{
	uint32 status = GPIO_REG_READ(GPIO_STATUS_ADDRESS);
	GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, status);
	int pin = status ? (__builtin_ffs(status) - 1) : -1;
	if (debounce_ok())
		button_pressed(pin);
}

void ICACHE_FLASH_ATTR button_init(void)
{
	gpio_init();
	gpio_pin_intr_state_set(GPIO_ID_PIN(PIN_BUTTON), GPIO_PIN_INTR_NEGEDGE);
	gpio_pin_intr_state_set(GPIO_ID_PIN(PIN_LED), GPIO_PIN_INTR_NEGEDGE);
	ETS_GPIO_INTR_ATTACH(&handle_gpio, (void *)NULL);
	ETS_GPIO_INTR_ENABLE();
	GPIO_DIS_OUTPUT(PIN_BUTTON);
}

void button_pressed(int pin)
{
	os_printf("Button %d\r\n", pin);
}
