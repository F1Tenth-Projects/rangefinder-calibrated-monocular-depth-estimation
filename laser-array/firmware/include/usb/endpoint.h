#ifndef USB_ENDPOINT_H
#define USB_ENDPOINT_H

#include <samd51.h>
#include <stdalign.h>

#define NUM_ENDPOINTS        2
#define USB_ENDPOINT_BUFSIZE 128

union usb_ep_memory {
        UsbDeviceDescBank bank[2];
        struct {
                UsbDeviceDescBank out;
                UsbDeviceDescBank in;
        };
};

struct endpoint {
        union {
                usb_out_callback out_callback;
                usb_ctl_callback ctl_callback;
        };
        void *callback_context;
        uint8_t out_ready;
        typeof(USB->DEVICE.DeviceEndpoint[0]) *reg;
        alignas(4) uint8_t out_buffer[USB_ENDPOINT_BUFSIZE];
        alignas(4) uint8_t in_buffer[USB_ENDPOINT_BUFSIZE];
};

#endif
