/* pinctrl_tiva_c.c - TI Tiva C Series pin controller driver */

#define DT_DRV_COMPAT ti_tiva_c_pinctrl

/*
 * Copyright (c) 2024, Your Name
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/drivers/pinctrl.h>

/* TivaWare HAL */
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "inc/hw_memmap.h"

/*
 * GPIO port base addresses (APB bus) indexed 0-5 for ports A-F.
 * Used to enable the GPIO clock for each port before configuring pins.
 */
static const uint32_t gpio_port_base[] = {
	GPIO_PORTA_BASE,  /* 0 = Port A */
	GPIO_PORTB_BASE,  /* 1 = Port B */
	GPIO_PORTC_BASE,  /* 2 = Port C */
	GPIO_PORTD_BASE,  /* 3 = Port D */
	GPIO_PORTE_BASE,  /* 4 = Port E */
	GPIO_PORTF_BASE,  /* 5 = Port F */
};

static const uint32_t gpio_port_periph[] = {
	SYSCTL_PERIPH_GPIOA,
	SYSCTL_PERIPH_GPIOB,
	SYSCTL_PERIPH_GPIOC,
	SYSCTL_PERIPH_GPIOD,
	SYSCTL_PERIPH_GPIOE,
	SYSCTL_PERIPH_GPIOF,
};

/*
 * pinctrl_soc_pin_t layout (from pinctrl_soc.h):
 *   .pinmux  = TivaWare GPIO_Pxy_FUNC value (e.g. GPIO_PA0_U0RX = 0x00000001)
 *   .port    = port index (0=A, 1=B, ... 5=F)
 *   .pin     = pin bit mask (GPIO_PIN_0 .. GPIO_PIN_7)
 *   .flags   = pin type flags from DTS
 */
int pinctrl_configure_pins(const pinctrl_soc_pin_t *pins,
		uint8_t pin_cnt,
		uintptr_t reg)
{
	ARG_UNUSED(reg);

	for (uint8_t i = 0; i < pin_cnt; i++) {
		uint8_t port_idx = pins[i].port;
		uint8_t pin_mask = pins[i].pin;
		uint32_t pinmux = pins[i].pinmux;

		/* Enable GPIO port clock */
		SysCtlPeripheralEnable(gpio_port_periph[port_idx]);
		while (!SysCtlPeripheralReady(gpio_port_periph[port_idx])) {
		}

		/* Apply the alternate-function mux setting via TivaWare */
		GPIOPinConfigure(pinmux);

		/* Set pin type based on flags from DTS */
		uint32_t port_base = gpio_port_base[port_idx];

		switch (pins[i].flags) {
			case TIVA_C_PIN_TYPE_UART:
				GPIOPinTypeUART(port_base, pin_mask);
				break;
			case TIVA_C_PIN_TYPE_I2C:
				GPIOPinTypeI2C(port_base, pin_mask);
				break;
			case TIVA_C_PIN_TYPE_I2C_SCL:
				GPIOPinTypeI2CSCL(port_base, pin_mask);
				break;
			case TIVA_C_PIN_TYPE_SSI:
				GPIOPinTypeSSI(port_base, pin_mask);
				break;
			case TIVA_C_PIN_TYPE_CAN:
				GPIOPinTypeCAN(port_base, pin_mask);
				break;
			case TIVA_C_PIN_TYPE_PWM:
				GPIOPinTypePWM(port_base, pin_mask);
				break;
			default:
				/* Default: standard digital I/O */
				GPIOPadConfigSet(port_base, pin_mask,
						GPIO_STRENGTH_2MA,
						GPIO_PIN_TYPE_STD);
				break;
		}
	}

	return 0;
}

