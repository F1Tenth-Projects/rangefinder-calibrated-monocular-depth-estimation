#include <stdint.h>
#include <string.h>

#include <vl53lx_platform.h>

#define VL53LX_get_register_name(VL53LX_p_007,VL53LX_p_032) VL53LX_COPYSTRING(VL53LX_p_032, "");


VL53LX_Error VL53LX_CommsInitialise(VL53LX_Dev_t *pdev, uint8_t comms_type,
                                    uint16_t comms_speed_khz)
{
}


VL53LX_Error VL53LX_CommsClose(VL53LX_Dev_t *pdev)
{
}



VL53LX_Error VL53LX_WriteMulti(VL53LX_Dev_t *pdev, uint16_t index,
                               uint8_t *pdata, uint32_t count)
{
}


VL53LX_Error VL53LX_ReadMulti(VL53LX_Dev_t *pdev, uint16_t index,
                              uint8_t *pdata, uint32_t count)
{
}


VL53LX_Error VL53LX_WrByte(VL53LX_Dev_t *pdev, uint16_t index,
                           uint8_t VL53LX_p_003)
{
}


VL53LX_Error VL53LX_WrWord(VL53LX_Dev_t *pdev, uint16_t index,
                           uint16_t VL53LX_p_003)
{
}


VL53LX_Error VL53LX_WrDWord(VL53LX_Dev_t *pdev, uint16_t index,
                            uint32_t VL53LX_p_003)
{
}


VL53LX_Error VL53LX_RdByte(VL53LX_Dev_t *pdev, uint16_t index, uint8_t *pdata)
{
}


VL53LX_Error VL53LX_RdWord(VL53LX_Dev_t *pdev, uint16_t index, uint16_t *pdata)
{
}


VL53LX_Error VL53LX_RdDWord(VL53LX_Dev_t *pdev, uint16_t index, uint32_t *pdata)
{
}



VL53LX_Error VL53LX_WaitUs(VL53LX_Dev_t *pdev, int32_t wait_us)
{
}


VL53LX_Error VL53LX_WaitMs(VL53LX_Dev_t *pdev, int32_t wait_ms)
{
}



VL53LX_Error VL53LX_GetTimerFrequency(int32_t *ptimer_freq_hz)
{
}


VL53LX_Error VL53LX_GetTimerValue(int32_t *ptimer_count)
{
}




VL53LX_Error VL53LX_GpioSetMode(uint8_t pin, uint8_t mode)
{
}


VL53LX_Error VL53LX_GpioSetValue(uint8_t pin, uint8_t value)
{
}


VL53LX_Error VL53LX_GpioGetValue(uint8_t pin, uint8_t *pvalue)
{
}



VL53LX_Error VL53LX_GpioXshutdown(uint8_t value)
{
}


VL53LX_Error VL53LX_GpioCommsSelect(uint8_t value)
{
}


VL53LX_Error VL53LX_GpioPowerEnable(uint8_t value)
{
}


VL53LX_Error VL53LX_GpioInterruptEnable(void (*function)(void),
                                        uint8_t edge_type)
{
}


VL53LX_Error  VL53LX_GpioInterruptDisable(void)
{
}


VL53LX_Error VL53LX_GetTickCount(VL53LX_Dev_t *pdev, uint32_t *ptick_count_ms)
{
}


VL53LX_Error VL53LX_WaitValueMaskEx(VL53LX_Dev_t *pdev, uint32_t timeout_ms,
                                    uint16_t index, uint8_t value,
                                    uint8_t mask, uint32_t poll_delay_ms)
{
}
