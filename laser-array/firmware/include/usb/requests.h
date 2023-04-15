#ifndef USB_REQUESTS_H
#define USB_REQUESTS_H

#define USB_STDRQST_GET_DESCRIPTOR    0x0680
#define USB_STDRQST_SET_ADDRESS       0x0500
#define USB_STDRQST_SET_CONFIGURATION 0x0900
#define USB_STDRQST_DEVICE_GET_STATUS 0x0080

#define USB_REQUEST_DEV2HOST    (1 << 7)
#define USB_REQUEST_TYPE_CLASS  (1 << 5)
#define USB_REQUEST_TYPE_VENDOR (2 << 5)
#define USB_REQUEST_RCPT_IFACE   1
#define USB_REQUEST_RCPT_ENDPT   2
#define USB_REQUEST_RCPT_OTHER   3

struct usb_request {
	uint8_t  bmRequestType;
	uint8_t  bRequest;
	uint16_t wValue;
	uint16_t wIndex;
	uint16_t wLength;
} __attribute__((packed));

#endif
