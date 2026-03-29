/*
 *----------------------------------------------------------------------
 *    micro T-Kernel 3.0 BSP
 *
 *    Copyright (C) 2022 by Ken Sakamura.
 *    This software is distributed under the T-License 2.2.
 *----------------------------------------------------------------------
 *
 *    Released by TRON Forum(http://www.tron.org) at 2022/11.
 *
 *----------------------------------------------------------------------
 */

#include <sys/machine.h>
#ifdef PICO_RP2040

/*
 *	devinit.c (RaspberryPi Pico)
 *	Device-Dependent Initialization
 */

#include <sys/sysdef.h>
#include <tm/tmonitor.h>
#include <tk/device.h>

#include <kernel.h>
#include "sysdepend.h"

#if DEVCNF_USE_USBHID
#include "../../../device/include/dev_usb_hid.h"
#endif
#if DEVCNF_USE_W5100S
#include "../../../device/include/dev_w5100s.h"
#endif

/* ------------------------------------------------------------------------ */

/*
 * Initialization before micro T-Kernel starts
 */

EXPORT ER knl_init_device( void )
{
	return E_OK;
}

/* ------------------------------------------------------------------------ */
/*
 * Start processing after T-Kernel starts
 *	Called from the initial task contexts.
 */
EXPORT ER knl_start_device( void )
{
#if USE_SDEV_DRV	// Use sample driver
	ER	err;

	/* A/D Converter unit.0 "adca" */
	#if DEVCNF_USE_ADC
		err = dev_init_adc(0);
		if(err < E_OK) return err;
	#endif

	/* I2C unit.0 "iica" — master mode only.
	 * When KB_IS_MASTER=0 (slave keyboard), I2C0 is used as slave
	 * (initialized by keyboard framework, not here).
	 * Both drivers cannot coexist on the same I2C0 peripheral. */
	#if DEVCNF_USE_IIC && (!defined(KB_IS_MASTER) || KB_IS_MASTER)
		err = dev_init_i2c(0);
		if(err < E_OK) return err;
	#endif

	/* UART0 "sera" */
	#if DEVCNF_USE_SER
		err = dev_init_ser(0);
		if(err < E_OK) return err;
	#endif

	/* USB HID Keyboard "usbk" — master (USB-connected) side only */
	#if DEVCNF_USE_USBHID && (!defined(KB_IS_MASTER) || KB_IS_MASTER)
		err = dev_init_usb_hid(0);
		if(err < E_OK) return err;
	#endif

	/* W5100S Ethernet "neta" */
	#if DEVCNF_USE_W5100S
		err = dev_init_w5100s(0);
		if(err < E_OK) return err;
	#endif

#endif

	return E_OK;
}

#if USE_SHUTDOWN
/* ------------------------------------------------------------------------ */
/*
 * System finalization
 *	Called just before system shutdown.
 *	Execute finalization that must be done before system shutdown.
 */
EXPORT ER knl_finish_device( void )
{
	return E_OK;
}

#endif /* USE_SHUTDOWN */

#endif /* PICO_RP2040 */
