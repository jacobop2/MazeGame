// All necessary declarations for the Tux Controller driver must be in this file

#ifndef TUXCTL_H
#define TUXCTL_H

#define TUX_SET_LED _IOR('E', 0x10, unsigned long)
#define TUX_READ_LED _IOW('E', 0x11, unsigned long*)
#define TUX_BUTTONS _IOW('E', 0x12, unsigned long*)
#define TUX_INIT _IO('E', 0x13)
#define TUX_LED_REQUEST _IO('E', 0x14)
#define TUX_LED_ACK _IO('E', 0x15)


#define LED_STATE_SIZE 6
#define NUM_DIGITS 4

#define CONTROLLER_BUSY 0
#define CONTROLLER_FREE 1

unsigned char led_state[LED_STATE_SIZE];
char button_state;

/* 0 means busy, 1 means tux free */
int ACK;

int tux_init( struct tty_struct* tty );
int tux_buttons( struct tty_struct* tty, unsigned long arg );
int tux_set_led( struct tty_struct* tty, unsigned long arg );

#endif

