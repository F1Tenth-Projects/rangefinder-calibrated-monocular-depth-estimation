#include <samd51.h>
#include <stdint.h>
#include <stdalign.h>
#include <string.h>
#include <endian.h>
#include <i2c.h>
#include <vl53lx_platform.h>

#define CMDBUF_SIZE 8192


VL53LX_Error VL53LX_CommsInitialise(VL53LX_Dev_t *pdev, uint8_t comms_type,
                                    uint16_t comms_speed_khz)
{
	return 0;
}


VL53LX_Error VL53LX_CommsClose(VL53LX_Dev_t *pdev)
{
	return 0;
}



VL53LX_Error VL53LX_WriteMulti(VL53LX_Dev_t *pdev, uint16_t index,
                               uint8_t *pdata, uint32_t count)
{
	int rc;
	alignas(4) uint8_t cmdbuf[CMDBUF_SIZE];

	if (count + 2 > CMDBUF_SIZE) {
		return VL53LX_ERROR_BUFFER_TOO_SMALL;
	}

	cmdbuf[0] = index >> 8;
	cmdbuf[1] = index & 0xFF;
	memcpy(cmdbuf + 2, pdata, count);

	rc = i2c_write(SERCOM0, pdev->i2c_slave_address, cmdbuf, count + 2);

	return (rc == 0) ? 0 : VL53LX_ERROR_CONTROL_INTERFACE;
}


VL53LX_Error VL53LX_ReadMulti(VL53LX_Dev_t *pdev, uint16_t index,
                              uint8_t *pdata, uint32_t count)
{
	int rc;
	uint16_t be_index;

	be_index = htobe16(index);
	rc = i2c_write(SERCOM0, pdev->i2c_slave_address,
	               (uint8_t *) &be_index, 2);
	if (rc != 0) {
		return VL53LX_ERROR_CONTROL_INTERFACE;
	}

	rc = i2c_read(SERCOM0, pdev->i2c_slave_address, pdata, count);
	return (rc == 0) ? 0 : VL53LX_ERROR_CONTROL_INTERFACE;
}


VL53LX_Error VL53LX_WrByte(VL53LX_Dev_t *pdev, uint16_t index,
                           uint8_t VL53LX_p_003)
{
	return VL53LX_WriteMulti(pdev, index, &VL53LX_p_003, 1);
}


VL53LX_Error VL53LX_WrWord(VL53LX_Dev_t *pdev, uint16_t index,
                           uint16_t VL53LX_p_003)
{
	uint16_t be_data;

	be_data = htobe16(VL53LX_p_003);
	return VL53LX_WriteMulti(pdev, index, (uint8_t*) &be_data, 2);
}


VL53LX_Error VL53LX_WrDWord(VL53LX_Dev_t *pdev, uint16_t index,
                            uint32_t VL53LX_p_003)
{
	uint32_t be_data;

	be_data = htobe32(VL53LX_p_003);
	return VL53LX_WriteMulti(pdev, index, (uint8_t*) &be_data, 4);
}


VL53LX_Error VL53LX_RdByte(VL53LX_Dev_t *pdev, uint16_t index, uint8_t *pdata)
{
	return VL53LX_ReadMulti(pdev, index, pdata, 1);
}


VL53LX_Error VL53LX_RdWord(VL53LX_Dev_t *pdev, uint16_t index, uint16_t *pdata)
{
	int rc;
	uint16_t be_data;

	rc = VL53LX_ReadMulti(pdev, index, (uint8_t *) &be_data, 2);
	if (rc != 0) {
		return rc;
	}

	*pdata = htobe16(be_data);
	return 0;
}


VL53LX_Error VL53LX_RdDWord(VL53LX_Dev_t *pdev, uint16_t index, uint32_t *pdata)
{
	int rc;
	uint32_t be_data;

	rc = VL53LX_ReadMulti(pdev, index, (uint8_t *) &be_data, 4);
	if (rc != 0) {
		return rc;
	}

	*pdata = htobe32(be_data);
	return 0;
}



VL53LX_Error VL53LX_WaitUs(VL53LX_Dev_t *pdev, int32_t wait_us)
{
	volatile unsigned long count;

	count = F_CPU * wait_us / 1000000UL / 1;
	while (count--);

	return 0;
}


VL53LX_Error VL53LX_WaitMs(VL53LX_Dev_t *pdev, int32_t wait_ms)
{
	volatile unsigned long count;

	count = F_CPU * wait_ms / 1000UL / 1;
	while (count--);

	return 0;
}



VL53LX_Error VL53LX_GetTimerFrequency(int32_t *ptimer_freq_hz)
{
	return VL53LX_ERROR_NOT_IMPLEMENTED;
}


VL53LX_Error VL53LX_GetTimerValue(int32_t *ptimer_count)
{
	return VL53LX_ERROR_NOT_IMPLEMENTED;
}




VL53LX_Error VL53LX_GpioSetMode(uint8_t pin, uint8_t mode)
{
	return VL53LX_ERROR_NOT_IMPLEMENTED;
}


VL53LX_Error VL53LX_GpioSetValue(uint8_t pin, uint8_t value)
{
	return VL53LX_ERROR_NOT_IMPLEMENTED;
}


VL53LX_Error VL53LX_GpioGetValue(uint8_t pin, uint8_t *pvalue)
{
	return VL53LX_ERROR_NOT_IMPLEMENTED;
}



VL53LX_Error VL53LX_GpioXshutdown(uint8_t value)
{
	return VL53LX_ERROR_NOT_IMPLEMENTED;
}


VL53LX_Error VL53LX_GpioCommsSelect(uint8_t value)
{
	return VL53LX_ERROR_NOT_IMPLEMENTED;
}


VL53LX_Error VL53LX_GpioPowerEnable(uint8_t value)
{
	return VL53LX_ERROR_NOT_IMPLEMENTED;
}


VL53LX_Error VL53LX_GpioInterruptEnable(void (*function)(void),
                                        uint8_t edge_type)
{
	return VL53LX_ERROR_NOT_IMPLEMENTED;
}


VL53LX_Error  VL53LX_GpioInterruptDisable(void)
{
	return VL53LX_ERROR_NOT_IMPLEMENTED;
}


VL53LX_Error VL53LX_GetTickCount(VL53LX_Dev_t *pdev, uint32_t *ptick_count_ms)
{
	return VL53LX_ERROR_NOT_IMPLEMENTED;
}


VL53LX_Error VL53LX_WaitValueMaskEx(VL53LX_Dev_t *pdev, uint32_t timeout_ms,
                                    uint16_t index, uint8_t value,
                                    uint8_t mask, uint32_t poll_delay_ms)
{
	return VL53LX_ERROR_NOT_IMPLEMENTED;
}
