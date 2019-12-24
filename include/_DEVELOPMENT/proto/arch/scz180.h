include(__link__.m4)

#ifndef __ARCH_SCZ180_H__
#define __ARCH_SCZ180_H__

#include <arch.h>

// IO MAPPED REGISTERS

#ifdef __CLANG

extern uint8_t io_dio;

extern uint8_t io_system;
extern uint8_t io_led_output;
extern uint8_t io_led_status;

extern uint8_t io_cf;

#else

__sfr __at __IO_DIO_PORT    io_dio;

__sfr __at __IO_SYSTEM      io_system;
__sfr __at __IO_LED_OUTPUT  io_led_output;
__sfr __at __IO_LED_STATUS  io_led_status;

__sfr __at __IO_CF_PORT     io_cf;

#endif /* !__ARCH_SCZ180_H__ */

#endif
