#include "osapi.h"
#include "user_interface.h"

#include "button.h"

static uint32 elapsed_debounce = 0;

static bool debounce_ok(void)
{
	uint32 now = elapsed;
	if (elapsed_debounce == 0 || now - elapsed_debounce >= DEBOUNCE_MS) {
		elapsed_debounce = now;
		return true;
	}
	return false;
}

static void ICACHE_RAM_ATTR handle_gpio(void *arg)
{
	uint32 status = GPIO_REG_READ(GPIO_STATUS_ADDRESS);
	GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, status);
	while (status) {
		int pin = __builtin_ffs(status) - 1;
		status &= ~(1 << pin);
		if (debounce_ok())
			button_pressed(pin);
	}
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
	if (pin < 0)
		return;
	os_printf("Button %d\r\n", pin);
}
