/*
 * Copyright (c) 2024, Your Name
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief Pin control SoC-level definitions for TI Tiva C Series
 *
 * Defines the pinctrl_soc_pin_t type and the macros that Zephyr's
 * pinctrl subsystem uses to extract pin configuration from the
 * device tree.
 */

#ifndef _SOC_TIVA_C_PINCTRL_SOC_H_
#define _SOC_TIVA_C_PINCTRL_SOC_H_

#include <zephyr/devicetree.h>
#include <zephyr/types.h>

/* Pin-type values matching the DTS ti,pin-type integers directly */
#define TIVA_C_PIN_TYPE_GPIO      0
#define TIVA_C_PIN_TYPE_UART      1
#define TIVA_C_PIN_TYPE_I2C       2
#define TIVA_C_PIN_TYPE_I2C_SCL   3
#define TIVA_C_PIN_TYPE_SSI       4
#define TIVA_C_PIN_TYPE_CAN       5
#define TIVA_C_PIN_TYPE_PWM       6

typedef struct pinctrl_soc_pin {
	/** TivaWare GPIOPinConfigure() value (e.g. GPIO_PA0_U0RX) */
	uint32_t pinmux;
	/** GPIO port index: 0=A .. 5=F */
	uint8_t port;
	/** GPIO pin bit-mask (GPIO_PIN_0 .. GPIO_PIN_7) */
	uint8_t pin;
	/** Pin type flags (TIVA_C_PIN_TYPE_*) */
	uint8_t flags;
} pinctrl_soc_pin_t;

/*
 * Extract port index and pin bit-mask from a TivaWare pinmux value.
 * Encoding (see GPIOPinConfigure in driverlib/gpio.c):
 *   bits [23:16] = port index (0=A, 1=B, ... 5=F)
 *   bits [15:8]  = pin shift  (pin_number * 4)
 */
#define TIVA_C_PINMUX_PORT(pinmux) (((pinmux) >> 16) & 0xFF)
#define TIVA_C_PINMUX_PIN(pinmux)  (1U << ((((pinmux) >> 8) & 0xFF) / 4))

#define Z_PINCTRL_STATE_PIN_INIT(node_id, prop, idx)			\
	{								\
		.pinmux = DT_PROP(DT_PROP_BY_IDX(node_id, prop, idx),	\
				  pinmux),				\
		.port   = TIVA_C_PINMUX_PORT(				\
			DT_PROP(DT_PROP_BY_IDX(node_id, prop, idx),	\
				pinmux)),				\
		.pin    = TIVA_C_PINMUX_PIN(				\
			DT_PROP(DT_PROP_BY_IDX(node_id, prop, idx),	\
				pinmux)),				\
		.flags  = DT_PROP(DT_PROP_BY_IDX(node_id, prop, idx),	\
				  ti_pin_type),				\
	},

#define Z_PINCTRL_STATE_PINS_INIT(node_id, prop)			\
	{								\
		DT_FOREACH_PROP_ELEM(node_id, prop,			\
				     Z_PINCTRL_STATE_PIN_INIT)		\
	}

#endif /* _SOC_TIVA_C_PINCTRL_SOC_H_ */

