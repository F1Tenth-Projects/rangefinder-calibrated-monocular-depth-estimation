#include <samd51.h>
#include <string.h>
#include <usb.h>
#include <usb/descriptors.h>
#include <usb/requests.h>
#include <usb/endpoint.h>

#define USB_PCKSIZE_64  3
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

static void usb_handle_reset(void);
static void usb_handle_control_request(void);
static void usb_handle_vendor_control_transfer(const struct usb_request *r);

static alignas(4) union usb_ep_memory ep_mem[NUM_ENDPOINTS];
static struct endpoint endpoint[NUM_ENDPOINTS] = {0};

void usb_init(void)
{
        int i;

        memset(ep_mem, 0, sizeof(ep_mem));

        memset(endpoint, 0, sizeof(endpoint));
        for (i = 0; i < NUM_ENDPOINTS; i++) {
                endpoint[i].out_ready = 1;
                endpoint[i].reg = &USB->DEVICE.DeviceEndpoint[i];
        }

        /* Setup pinmux for USB pins: DM -> PA24  DP -> PA25 */
        PORT->Group[0].PINCFG[24].reg |= PORT_PINCFG_PMUXEN;
        PORT->Group[0].PINCFG[25].reg |= PORT_PINCFG_PMUXEN;
        PORT->Group[0].PMUX[24 >> 1].reg |= PORT_PMUX_PMUXE(0x07);
        PORT->Group[0].PMUX[25 >> 1].reg |= PORT_PMUX_PMUXO(0x07);

	/* Setup clock generators and enable USB peripheral. */
	MCLK->APBBMASK.reg |= MCLK_APBBMASK_USB;
        GCLK->PCHCTRL[USB_GCLK_ID].reg |= GCLK_PCHCTRL_GEN_GCLK0
                                        | GCLK_PCHCTRL_CHEN;
        MCLK->AHBMASK.bit.USB_ = 1;
        while (GCLK->SYNCBUSY.bit.GENCTRL0);

	/* Reset, configure, and enable. */
        USB->DEVICE.CTRLA.bit.SWRST = 1;
        while (USB->DEVICE.SYNCBUSY.bit.SWRST);

        USB->DEVICE.DESCADD.reg = (uint32_t) ep_mem;
        USB->DEVICE.CTRLA.bit.MODE = USB_CTRLA_MODE_DEVICE_Val;
        USB->DEVICE.CTRLA.bit.RUNSTDBY = 1;
        USB->DEVICE.CTRLB.bit.SPDCONF = USB_DEVICE_CTRLB_SPDCONF_FS_Val;
        USB->DEVICE.INTENSET.reg = USB_DEVICE_INTENSET_EORST;
        endpoint[0].reg->EPINTENSET.bit.RXSTP = 1;

        USB->DEVICE.CTRLA.reg |= USB_CTRLA_ENABLE;
        while (USB->DEVICE.SYNCBUSY.bit.ENABLE);
}

void usb_process(void)
{
	if (USB->DEVICE.INTFLAG.bit.EORST) {
		usb_handle_reset();
	}

	if (endpoint[0].reg->EPINTFLAG.bit.RXSTP) {
		usb_handle_control_request();
		endpoint[0].reg->EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_RXSTP;
		endpoint[0].reg->EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_TRCPT0;
		endpoint[0].reg->EPSTATUSCLR.reg =
			USB_DEVICE_EPSTATUSCLR_BK0RDY;
	}

	if (endpoint[1].reg->EPINTFLAG.bit.TRCPT0) {
		int bytes_received;

		endpoint[1].reg->EPINTFLAG.reg =
			USB_DEVICE_EPINTFLAG_TRCPT0;

		bytes_received = ep_mem[1].out.PCKSIZE.bit.BYTE_COUNT;
		if (endpoint[1].out_callback != NULL) {
			endpoint[1].out_callback(
				endpoint[1].callback_context,
				endpoint[1].out_buffer,
				bytes_received);
		}

		ep_mem[1].out.PCKSIZE.bit.BYTE_COUNT = 0;
		if (endpoint[1].out_ready) {
			endpoint[1].reg->EPSTATUSCLR.reg =
				USB_DEVICE_EPSTATUSCLR_BK0RDY;
		}
	}
}

static void usb_handle_reset(void)
{
	/* Clear end-of-reset flag and set device address to zero */
	USB->DEVICE.INTFLAG.reg = USB_DEVICE_INTFLAG_EORST;
	USB->DEVICE.DADD.reg = USB_DEVICE_DADD_ADDEN;


	/* Endpoint 0 Configuration: Control */
	endpoint[0].reg->EPCFG.reg = USB_DEVICE_EPCFG_EPTYPE0(1) |
	                             USB_DEVICE_EPCFG_EPTYPE1(1);

	endpoint[0].reg->EPSTATUSSET.reg = USB_DEVICE_EPSTATUSSET_BK0RDY;
	endpoint[0].reg->EPSTATUSCLR.reg = USB_DEVICE_EPSTATUSCLR_BK1RDY;

	ep_mem[0].in.PCKSIZE.bit.SIZE = USB_PCKSIZE_64;
	ep_mem[0].in.ADDR.reg = (uint32_t) endpoint[0].in_buffer;

	ep_mem[0].out.ADDR.reg = (uint32_t) endpoint[0].out_buffer;
	ep_mem[0].out.PCKSIZE.bit.SIZE = USB_PCKSIZE_64;
	ep_mem[0].out.PCKSIZE.bit.MULTI_PACKET_SIZE = 64;
	ep_mem[0].out.PCKSIZE.bit.BYTE_COUNT = 0;

	endpoint[0].reg->EPINTENSET.bit.RXSTP = 1;
	endpoint[0].reg->EPINTENSET.bit.TRCPT0 = 1;


	/* Endpoint 1 Configuration: Interrupt IN */
	endpoint[1].reg->EPCFG.reg = USB_DEVICE_EPCFG_EPTYPE1(0x04);

	ep_mem[1].in.ADDR.reg = (uint32_t) endpoint[1].in_buffer;
	ep_mem[1].in.PCKSIZE.bit.SIZE = USB_PCKSIZE_64;
	endpoint[1].reg->EPINTENSET.bit.TRCPT1 = 1;
	endpoint[1].reg->EPSTATUSCLR.reg = USB_DEVICE_EPSTATUSCLR_BK1RDY;
}

static void usb_handle_control_request(void)
{
	const struct usb_request *r;
	uint32_t request_id;

	r = (struct usb_request *) endpoint[0].out_buffer;
	if (sizeof(*r) != ep_mem[0].out.PCKSIZE.bit.BYTE_COUNT) {
		usb_stall_ep(0);
		return;
	}

	request_id = (r->bRequest << 8) | r->bmRequestType;

	switch (request_id) {
	case USB_STDRQST_GET_DESCRIPTOR:
		int desc_type;
		int index;

		desc_type = r->wValue >> 8;
		index = r->wValue & 0xFF;

		switch (desc_type) {
		case USB_DESCTYPE_DEVICE:
			usb_send(0, (void *) usb_device_descriptor,
				       usb_device_descriptor_size);
			break;
		case USB_DESCTYPE_CONFIGURATION: {
			size_t len;

			len = MIN(r->wLength, usb_configuration_hierarchy_size);
			usb_send(0, (void *) usb_configuration_hierarchy, len);
			break;
		}
		case USB_DESCTYPE_STRING:
			if (index > usb_string_descriptors_count) {
				usb_stall_ep(0);
			}
			usb_send(0, (void *) &usb_string_descriptors[index],
			         usb_string_descriptors[index].bLength);
			break;
		default:
			usb_stall_ep(0);
			break;
		}
		break;
	case USB_STDRQST_SET_ADDRESS:
		usb_send_zlp(0);
		USB->DEVICE.DADD.reg = USB_DEVICE_DADD_ADDEN | r->wValue;
		break;
	case USB_STDRQST_SET_CONFIGURATION:
		if (r->wValue != 1) {
			usb_stall_ep(0);
		}
		usb_send_zlp(0);
		break;
	case USB_STDRQST_DEVICE_GET_STATUS:
		usb_send(0, (uint8_t *) "\0\0", 2);
		break;
	default:
		if (r->bmRequestType & USB_REQUEST_TYPE_VENDOR) {
			usb_handle_vendor_control_transfer(r);
		} else {
			usb_stall_ep(0);
		}
		break;
	}
}

static void usb_handle_vendor_control_transfer(const struct usb_request *r)
{
	uint8_t *data;
	int data_size;

	if (endpoint[0].ctl_callback == NULL) {
		usb_stall_ep(0);
		return;
	}

	if (r->wLength > 0) {
		/* Wait for data phase */
                endpoint[0].reg->EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_TRCPT0;
                endpoint[0].reg->EPSTATUSCLR.reg =
                        USB_DEVICE_EPSTATUSCLR_BK0RDY;
                while (!endpoint[0].reg->EPINTFLAG.bit.TRCPT0);

                /* setup data to be passed to callback */
                data = endpoint[0].out_buffer + sizeof(*r);
                data_size = r->wLength;
	} else {
		data = NULL;
		data_size = 0;
	}

	endpoint[0].ctl_callback(endpoint[0].callback_context, r,
	                         data, data_size);
}

void usb_send_zlp(int ep)
{
	ep_mem[ep].in.PCKSIZE.bit.BYTE_COUNT = 0;

	endpoint[0].reg->EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_TRCPT1;
	endpoint[0].reg->EPSTATUSSET.bit.BK1RDY = 1;
	while (!endpoint[0].reg->EPINTFLAG.bit.TRCPT1);
}

void usb_stall_ep(int ep)
{
	endpoint[ep].reg->EPSTATUSSET.bit.STALLRQ1 = 1;
	endpoint[ep].reg->EPSTATUSSET.bit.STALLRQ0 = 1;
}

void usb_send_nonblocking(int ep, const unsigned char *data, int size)
{
	memcpy(endpoint[ep].in_buffer, data, MIN(size, USB_ENDPOINT_BUFSIZE));
	ep_mem[ep].in.PCKSIZE.bit.AUTO_ZLP = 1;
	ep_mem[ep].in.PCKSIZE.bit.BYTE_COUNT = size;
	ep_mem[ep].in.PCKSIZE.bit.MULTI_PACKET_SIZE = 0;

	endpoint[ep].reg->EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_TRCPT1;
	endpoint[ep].reg->EPSTATUSSET.bit.BK1RDY = 1;
}

void usb_send(int ep, const unsigned char *data, int size)
{
	usb_send_nonblocking(ep, data, size);
	while (!endpoint[ep].reg->EPINTFLAG.bit.TRCPT1);
}

void usb_set_ctl_callback(usb_ctl_callback fn, void *context)
{
	endpoint[0].ctl_callback = fn;
	endpoint[0].callback_context = context;
}

int usb_set_out_callback(int ep, usb_out_callback fn, void *context)
{
	if (ep > NUM_ENDPOINTS) {
		return -1;
	} else {
		endpoint[ep].out_callback = fn;
		endpoint[ep].callback_context = context;
		return 0;
	}
}

void usb_attach(void)
{
	USB->DEVICE.CTRLB.bit.DETACH = 0;
}

void usb_detach(void)
{
	USB->DEVICE.CTRLB.bit.DETACH = 1;
}
