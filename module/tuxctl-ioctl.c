/* tuxctl-ioctl.c
 *
 * Driver (skeleton) for the mp2 tuxcontrollers for ECE391 at UIUC.
 *
 * Mark Murphy 2006
 * Andrew Ofisher 2007
 * Steve Lumetta 12-13 Sep 2009
 * Puskar Naha 2013
 */

#include <asm/current.h>
#include <asm/uaccess.h>

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/file.h>
#include <linux/miscdevice.h>
#include <linux/kdev_t.h>
#include <linux/tty.h>
#include <linux/spinlock.h>

#include "tuxctl-ld.h"
#include "tuxctl-ioctl.h"
#include "mtcp.h"

#define debug(str, ...) \
	printk(KERN_DEBUG "%s: " str, __FUNCTION__, ## __VA_ARGS__)

/************************ Protocol Implementation *************************/

/* tuxctl_handle_packet()
 * IMPORTANT : Read the header for tuxctl_ldisc_data_callback() in 
 * tuxctl-ld.c. It calls this function, so all warnings there apply 
 * here as well.
 */
void tuxctl_handle_packet (struct tty_struct* tty, unsigned char* packet)
{
    unsigned a, b, c;

    a = packet[0]; /* Avoid printk() sign extending the 8-bit */
    b = packet[1]; /* values when printing them. */
    c = packet[2];

	switch ( a )
	{
		/* save state, call init */
		case MTCP_RESET:
			( void )tux_init( tty );
			tuxctl_ldisc_put( tty, led_state, LED_STATE_SIZE );
			break;

		/* acknowledge */
		case MTCP_ACK:
			ACK = CONTROLLER_FREE;
			break;

		// /* respond to button input */
		case MTCP_BIOC_EVENT:
		{
			/* r l d u c b a start */
			char new_buttons = 0;
			char left = 0;
			char down = 0;
			char right = 0;
			char up = 0;

			/* store lower 4 bits in new_buttons */
			new_buttons |= ( b & 0x0F );

			/* store value of dir bits in var */
			left = ( c & 0x02 ) >> 1;
			down = ( c & 0x04 ) >> 2;
			right = ( c & 0x08 ) >> 3;
			up = ( c & 0x01 );

			/* manually set bits */
			new_buttons |= ( down << 5 );
			new_buttons |= ( left << 6 );
			new_buttons |= ( right << 7 );
			new_buttons |= ( up << 4 );

			/* set active high */
			button_state = ~new_buttons;
			break;
		}
	}

    //printk("packet : %x %x %x\n", a, b, c); 
	//printk( "Hex val: %02X\n", (unsigned char)button_state );
}

/******** IMPORTANT NOTE: READ THIS BEFORE IMPLEMENTING THE IOCTLS ************
 *                                                                            *
 * The ioctls should not spend any time waiting for responses to the commands *
 * they send to the controller. The data is sent over the serial line at      *
 * 9600 BAUD. At this rate, a byte takes approximately 1 millisecond to       *
 * transmit; this means that there will be about 9 milliseconds between       *
 * the time you request that the low-level serial driver send the             *
 * 6-byte SET_LEDS packet and the time the 3-byte ACK packet finishes         *
 * arriving. This is far too long a time for a system call to take. The       *
 * ioctls should return immediately with success if their parameters are      *
 * valid.                                                                     *
 *                                                                            *
 ******************************************************************************/
int tuxctl_ioctl (struct tty_struct* tty, struct file* file, 
	      unsigned cmd, unsigned long arg)
{
    switch (cmd) {

	case TUX_INIT:
		return tux_init( tty );

	case TUX_BUTTONS:
		if ( arg == 0 )
			return -EINVAL;
		return tux_buttons( tty, arg );

	/* update saved state */
	case TUX_SET_LED:
		if ( ACK == CONTROLLER_BUSY ) return 0;

		ACK = CONTROLLER_BUSY;
		return tux_set_led( tty, arg );

	case TUX_LED_ACK:
		return -EINVAL;
	case TUX_LED_REQUEST:
		return -EINVAL;
	case TUX_READ_LED:
		return -EINVAL;
	default:
	    return -EINVAL;
    }
}

/* led data representing which lights should be active for each hex digit */
unsigned char led_data[16] = { 0xE7, 0x06, 0xCB, 0x8F, 0x2E, 0xAD, 0xED, 0x86, 
							   0xEF, 0xAF, 0xEE, 0x6D, 0xE1, 0x4F, 0xE9, 0xE8 };

/*
 * tux_init
 *   DESCRIPTION: helper function to initialize the tux controller
 *   INPUTS: tty 
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: initialize tux
 */
int tux_init( struct tty_struct* tty )
{
	unsigned char buf[6];

	/* enable BIOC events and set led to user mode */
	buf[0] = MTCP_BIOC_ON;
	buf[1] = MTCP_LED_USR;
	ACK = CONTROLLER_BUSY;
	tuxctl_ldisc_put( tty, buf, 6 );

	return 0;
}

/*
 * tux_buttons
 *   DESCRIPTION: helper function to transmit the value of the tux buttons to the users
 *   INPUTS: tty, unsigned long arg -- pointer to be filled with button value
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: send tux buttons to user
 */
int tux_buttons( struct tty_struct* tty, unsigned long arg )
{
	copy_to_user( ( void * )arg, ( void * )&button_state, 1 );
	return 0;
}

/*
 * tux_set_led
 *   DESCRIPTION: helper function to set the leds on the tux 
 *   INPUTS: tty, arg -- contains information on desired led settings
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: set tux leds
 */
int tux_set_led( struct tty_struct* tty, unsigned long arg )
{
	int i;
	unsigned char digits[NUM_DIGITS];

	char active_displays;
	char dps;

	for ( i = 0; i < NUM_DIGITS; i++ )
	{
		/* save 4 bits per digit */
		digits[i] = led_data[arg & 0xF];
		arg = arg >> 4;
	}
	
	/* save bits 20-17 to determine which leds to turn on */
	active_displays = arg & 0x0F;

	/* save bits 27-24 to set decimal points */
	dps = ( arg >> 8 ) & 0x0F;

	led_state[0] = MTCP_LED_SET;
	/* set to update all displays */
	led_state[1] = 0x0F;

	for ( i = 0; i < NUM_DIGITS; i++ )
	{
		/* if the display is active, add packet */
		if ( ( ( active_displays >> i ) & 0x01 ) == 0x01 )
		{
			/* set led display */
			led_state[i + 2] = digits[i];
		}
		else
			led_state[i + 2] = 0x00;

		/* set decimal point if high */
		if ( ( ( dps >> i ) & 0x01 ) == 0x01 )
			led_state[i + 2] |= 0x10;
	}

	/* send packets to tux */
	tuxctl_ldisc_put( tty, led_state, LED_STATE_SIZE );
	return 0;
}

