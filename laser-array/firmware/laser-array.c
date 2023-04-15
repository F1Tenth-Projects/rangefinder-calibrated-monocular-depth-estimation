#include <usb.h>
#include <usb/requests.h>

int main()
{
	usb_init();
	usb_attach();

	while (1) {
		usb_process();
	}

	return 0;
}
