#ifndef USB_H
#define USB_H

#include <stdint.h>

struct usb_request;

typedef void (*usb_out_callback)(void *context, const uint8_t *data, int size);
typedef void (*usb_ctl_callback)(void *context, const struct usb_request *r,
                                 const uint8_t *data, int data_size);

/* Initializes the USB device. */
void usb_init(void);

/* Registers a callback function 'fn' to be executed each time data becomes
 * available on the OUT endpoint 'ep'. Returns 0 on success or -1 if the
 * endpoint specified is invalid. */
int usb_set_out_callback(int ep, usb_out_callback fn, void *context);

/* Registers a callback function 'fn' to be executed each time a control
 * transfer is received on endpoint 0. */
void usb_set_ctl_callback(usb_ctl_callback fn, void *context);

/* Attach the USB device to the bus. */
void usb_attach(void);

/* Detaches the USB device from the bus. */
void usb_detach(void);

/* Transmits the buffer 'data' of length 'size' on the OUT or control endpoint
 * 'ep', blocking until transmission completes. */
void usb_send(int ep, const unsigned char *data, int size);

/* Transmits the buffer 'data' of length 'size' on the OUT or control endpoint
 * 'ep'. */
void usb_send_nonblocking(int ep, const unsigned char *data, int size);

/* Send a zero-length packet on endpoint 'ep'. */
void usb_send_zlp(int ep); 

/* Sends a stall response on the EP given. */
void usb_stall_ep(int ep);

/* Process USB operations */
void usb_process(void);


#endif
