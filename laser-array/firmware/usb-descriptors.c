#include <string.h>
#include <usb/descriptors.h>
#include <usb/endpoint.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(*a))

static const struct usb_device_descriptor device_descriptor = {
        .bLength = sizeof(struct usb_device_descriptor),
        .bDescriptorType = USB_DESCTYPE_DEVICE,
        .bcdUSB = 0x0200,
        .bDeviceClass = 0xFF,   // vendor defined
        .bDeviceSubClass = 0xFF,
        .bDeviceProtocol = 0xFF,
        .bMaxPacketSize = 64,
        .idVendor = 0x03EB,    // Atmel
        .idProduct = 0x2423,   // USB_PID_ATMEL_ASF_VENDOR_CLASS
        .bcdDevice = 0,
        .iManufacturer = 1,
        .iProduct = 2,
        .iSerialNumber = 0,
        .bNumConfigurations = 1
};

static const struct configuration_hierarchy {
        struct usb_configuration_descriptor configuration;
        struct usb_interface_descriptor interface;
        struct usb_endpoint_descriptor endpoints[NUM_ENDPOINTS - 1];
} __attribute__((packed)) configuration_hierarchy = {
        .configuration = {
                .bLength = sizeof(struct usb_configuration_descriptor),
                .bDescriptorType = USB_DESCTYPE_CONFIGURATION,
                .wTotalLength = sizeof(struct configuration_hierarchy),
                .bNumInterfaces = 1,
                .bConfigurationValue = 1,
                .iConfiguration = 0,
                .bmAttributes = USB_ATTR_BUSPOWERED,
                .bMaxPower = 400 / 2,
        },
        .interface = {
                .bLength = sizeof(struct usb_interface_descriptor),
                .bDescriptorType = USB_DESCTYPE_INTERFACE,
                .bInterfaceNumber = 0,
                .bAlternateSetting = 0,
                .bNumEndpoints = NUM_ENDPOINTS - 1,
                .bInterfaceClass = 0xFF,
                .bInterfaceSubClass = 0xFF,
                .bInterfaceProtocol = 0xFF,
                .iInterface = 0,
        },
        .endpoints = {
                [0] = {
                        .bLength = 7,
                        .bDescriptorType = USB_DESCTYPE_ENDPOINT,
                        .bEndpointAddress = 1 | USB_EPADDR_DIR_IN,
                        .bmAttributes = USB_ATTR_XFER_INTERRUPT,
                        .wMaxPacketSize = 64,
                        .bInterval = 11,
                },
        },
};

static const struct usb_string_descriptor string_descriptors[] = {
        {.bLength = 2 + 2,
         .bDescriptorType = 0x03,
         .string = {0x09, 0x04},
        }, {
         .bLength = 2 + strlen("University of Pennsylvania") * 2,
         .bDescriptorType = 0x03,
         .string =
               "U\0""n\0i\0v\0e\0r\0s\0i\0t\0y\0 \0o\0f\0 \0"
	       "P\0e\0n\0n\0s\0y\0l\0v\0a\0n\0i\0a\0 \0"
        }, {
         .bLength = 2 + strlen("F1TENTH VL53L4CX Array") * 2,
         .bDescriptorType = 0x03,
         .string = "F\0""1\0T\0E\0N\0T\0H\0 \0"
	           "V\0L\0""5\0""3\0L\0""4\0C\0X\0 \0A\0r\0r\0a\0y\0"
        }
};


const struct usb_device_descriptor *usb_device_descriptor = &device_descriptor;
const int usb_device_descriptor_size = sizeof(device_descriptor);

const struct configuration_hierarchy *usb_configuration_hierarchy =
                                      &configuration_hierarchy;
const int usb_configuration_hierarchy_size = sizeof(configuration_hierarchy);

const struct usb_string_descriptor *usb_string_descriptors = string_descriptors;
const int usb_string_descriptors_count = ARRAY_SIZE(string_descriptors);
const int usb_string_descriptors_size = sizeof(string_descriptors);
