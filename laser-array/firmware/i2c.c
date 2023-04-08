#include <samd51.h>
#include <i2c.h>

static int i2c_start(sercom_register sercom, uint8_t addr);

void i2c_init(void)
{
	/* Setup PA04 and PA05 as I2C pins */
	PORT->Group[0].OUTCLR.reg = PORT_PA04;
	PORT->Group[0].DIRSET.reg = PORT_PA04;
	PORT->Group[0].PINCFG[4].reg |= PORT_PINCFG_INEN;
	PORT->Group[0].PINCFG[4].reg |= PORT_PINCFG_PULLEN;
	PORT->Group[0].PINCFG[4].reg |= PORT_PINCFG_PMUXEN;
	PORT->Group[0].PMUX[4>>1].reg |= PORT_PMUX_PMUXE(0x03);

	PORT->Group[0].OUTCLR.reg = PORT_PA05;
	PORT->Group[0].DIRSET.reg = PORT_PA05;
	PORT->Group[0].PINCFG[5].reg |= PORT_PINCFG_INEN;
	PORT->Group[0].PINCFG[5].reg |= PORT_PINCFG_PULLEN;
	PORT->Group[0].PINCFG[5].reg |= PORT_PINCFG_PMUXEN;
	PORT->Group[0].PMUX[5>>1].reg |= PORT_PMUX_PMUXO(0x03);

	/* Set up a 48MHz clock for the I2C Bus */
	GCLK->GENCTRL[7].reg = GCLK_GENCTRL_SRC(GCLK_GENCTRL_SRC_DFLL_Val) |
	                       GCLK_GENCTRL_IDC |
			       GCLK_GENCTRL_DIV(2) |
	                       GCLK_GENCTRL_GENEN;
	while (GCLK->SYNCBUSY.bit.GENCTRL7);

	MCLK->APBAMASK.reg |= MCLK_APBAMASK_SERCOM0;
	GCLK->PCHCTRL[SERCOM0_GCLK_ID_CORE].reg |= GCLK_PCHCTRL_GEN_GCLK7
	                                        |  GCLK_PCHCTRL_CHEN;
	while (GCLK->SYNCBUSY.bit.GENCTRL7);

	/* Configure I2C master */
	SERCOM0->I2CM.CTRLA.reg = SERCOM_I2CM_CTRLA_SPEED(0x1) |
	                          SERCOM_I2CM_CTRLA_SDAHOLD(0x2) |
	                          SERCOM_I2CM_CTRLA_MODE(0x5);
	SERCOM0->I2CM.CTRLB.reg = SERCOM_I2CM_CTRLB_SMEN;
	while (SERCOM0->I2CM.SYNCBUSY.bit.SYSOP);

	SERCOM0->I2CM.CTRLA.reg |= SERCOM_I2CM_CTRLA_INACTOUT(0x0);
	while (SERCOM0->I2CM.SYNCBUSY.bit.SYSOP);

	SERCOM0->I2CM.BAUD.reg = SERCOM_I2CM_BAUD_BAUD(8) |
	                        SERCOM_I2CM_BAUD_BAUDLOW(0);
	while (SERCOM0->I2CM.SYNCBUSY.bit.SYSOP);

	SERCOM0->I2CM.CTRLA.reg |= SERCOM_I2CM_CTRLA_ENABLE;
	while (SERCOM0->I2CM.SYNCBUSY.bit.ENABLE);

	/* Force I2C bus into IDLE state. */
	SERCOM0->I2CM.STATUS.reg |= SERCOM_I2CM_STATUS_BUSSTATE(1);
	while (SERCOM0->I2CM.SYNCBUSY.bit.SYSOP);
	
	SERCOM0->I2CM.INTENSET.reg = SERCOM_I2CM_INTENSET_MB |
	                             SERCOM_I2CM_INTENSET_SB;
}

int i2c_write(sercom_register sercom, uint8_t address,
              const uint8_t *data, long size)
{
	int i;

	sercom->I2CM.CTRLB.reg &= ~SERCOM_I2CM_CTRLB_ACKACT;
	while (sercom->I2CM.SYNCBUSY.reg);

	if (i2c_start(sercom, (address << 1) | 0x00) != 0) {
		return -1;
	}

	for (i = 0; i < size; i++) {
		sercom->I2CM.DATA.reg = data[i];
		while (sercom->I2CM.SYNCBUSY.bit.SYSOP);

		while (sercom->I2CM.INTFLAG.bit.MB == 0);
		if (sercom->I2CM.STATUS.reg & SERCOM_I2CM_STATUS_RXNACK) {
			sercom->I2CM.CTRLB.reg |= SERCOM_I2CM_CTRLB_CMD(3);
			return (i == size - 1) ? 0 : -1;
		}
	}
	sercom->I2CM.CTRLB.reg |= SERCOM_I2CM_CTRLB_CMD(3);

	return 0;
}

int i2c_read(sercom_register sercom, uint8_t address,
             uint8_t *data, long size)
{
	sercom->I2CM.CTRLB.reg &= ~SERCOM_I2CM_CTRLB_ACKACT;
	while (sercom->I2CM.SYNCBUSY.reg);

	if (i2c_start(sercom, (address << 1) | 0x01) != 0) {
		return -1;
	}

	while (size > 0) {
		while (sercom->I2CM.INTFLAG.bit.SB == 0);

		/* Send ACK on any byte but the last byte */
		if (size > 1) {
			sercom->I2CM.CTRLB.reg &= ~SERCOM_I2CM_CTRLB_ACKACT;
		} else {
			sercom->I2CM.CTRLB.reg |= SERCOM_I2CM_CTRLB_ACKACT |
			                          SERCOM_I2CM_CTRLB_CMD(0x3);
		}
		while (sercom->I2CM.SYNCBUSY.reg);

		*data = sercom->I2CM.DATA.reg;
		while (sercom->I2CM.SYNCBUSY.reg);

		size--;
		data++;
	}

	return 0;
}

static int i2c_start(sercom_register sercom, uint8_t addr)
{
	sercom->I2CM.INTFLAG.reg = SERCOM_I2CM_INTFLAG_ERROR;

	sercom->I2CM.ADDR.reg = addr;

	while ((sercom->I2CM.INTFLAG.reg & SERCOM_I2CM_INTFLAG_MB) == 0 &&
	       (sercom->I2CM.INTFLAG.reg & SERCOM_I2CM_INTFLAG_SB) == 0);

	if (sercom->I2CM.STATUS.bit.RXNACK || sercom->I2CM.INTFLAG.bit.ERROR) {
		sercom->I2CM.CTRLB.reg |= SERCOM_I2CM_CTRLB_CMD(3);
		return -1;
	}

	return 0;
}
