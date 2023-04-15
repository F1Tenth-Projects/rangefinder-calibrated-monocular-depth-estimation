#ifndef USB_DESCRIPTORS_H
#define USB_DESCRIPTORS_H

#include <usb.h>
#include <stdint.h>

#define USB_DESCTYPE_DEVICE        0x01
#define USB_DESCTYPE_CONFIGURATION 0x02
#define USB_DESCTYPE_STRING        0x03
#define USB_DESCTYPE_INTERFACE     0x04
#define USB_DESCTYPE_ENDPOINT      0x05

struct usb_device_descriptor {
	uint8_t  bLength;
	uint8_t  bDescriptorType;
	uint16_t bcdUSB;
	uint8_t  bDeviceClass;
	uint8_t  bDeviceSubClass;
	uint8_t  bDeviceProtocol;
	uint8_t  bMaxPacketSize;
	uint16_t idVendor;
	uint16_t idProduct;
	uint16_t bcdDevice;
	uint8_t  iManufacturer;
	uint8_t  iProduct;
	uint8_t  iSerialNumber;
	uint8_t  bNumConfigurations;
} __attribute__((packed));


#define USB_ATTR_REMOTEWAKE   (1 << 5)
#define USB_ATTR_SELFPOWERED  (1 << 6)
#define USB_ATTR_BUSPOWERED   (1 << 7)
struct usb_configuration_descriptor {
	uint8_t  bLength;
	uint8_t  bDescriptorType;
	uint16_t wTotalLength;
	uint8_t  bNumInterfaces;
	uint8_t  bConfigurationValue;
	uint8_t  iConfiguration;
	uint8_t  bmAttributes;
	uint8_t  bMaxPower;
} __attribute__((packed));


struct usb_interface_descriptor {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bInterfaceNumber;
	uint8_t bAlternateSetting;
	uint8_t bNumEndpoints;
	uint8_t bInterfaceClass;
	uint8_t bInterfaceSubClass;
	uint8_t bInterfaceProtocol;
	uint8_t iInterface;
} __attribute__((packed));


#define USB_EPADDR_DIR_IN     (1 << 7)
#define USB_ATTR_XFER_CONTROL      0x00
#define USB_ATTR_XFER_ISOCHRONOUS  0x01
#define USB_ATTR_XFER_BULK         0x02
#define USB_ATTR_XFER_INTERRUPT    0x03
struct usb_endpoint_descriptor {
	uint8_t  bLength;
	uint8_t  bDescriptorType;
	uint8_t  bEndpointAddress;
	uint8_t  bmAttributes;
	uint16_t wMaxPacketSize;
	uint8_t  bInterval;
} __attribute__((packed));


#define USB_STRING_DESCRIPTOR_MAXCHARS 63
struct usb_string_descriptor {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t string[USB_STRING_DESCRIPTOR_MAXCHARS * 2];
} __attribute__((packed));



extern const struct usb_device_descriptor *usb_device_descriptor;
extern const int usb_device_descriptor_size;

extern const struct configuration_hierarchy *usb_configuration_hierarchy;
extern const int usb_configuration_hierarchy_size;

extern const struct usb_string_descriptor *usb_string_descriptors;
extern const int usb_string_descriptors_count;
extern const int usb_string_descriptors_size;

#endif
