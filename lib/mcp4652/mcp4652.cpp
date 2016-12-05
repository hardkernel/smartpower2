#include <stdio.h>
#include <Wire.h>

#include "mcp4652.h"

// unsigned char wiper[] = {2, 4, 6, 8, 12, 15, 17, 20, 23, 26, 28, 30, 32, 33, 36,
//     38, 40, 42, 44, 46, 48, 49, 52, 54, 55, 58, 60, 62, 64, 65, 67, 69, 71, 74,
//     75, 77, 82, 84, 85, 86, 87, 90, 94, 96, 97, 98, 101, 103, 105, 108, 111, 112,
//     115, 117, 121, 122, 125, 127, 129, 131, 132, 135, 137, 138, 139, 140, 145,
//     149, 151, 155, 156, 158, 162, 164, 166, 168, 169, 172, 174, 176, 179, 182,
//     183, 186, 188, 190, 191, 192, 194, 197, 199, 202, 203, 205, 208, 210, 212,
//     214, 216, 218, 220, 223, 224, 227, 229, 230, 233, 235, 238, 240, 241, 243,
//     246, 248, 250, 253, 255};

void mcp4652_init(void)
{
	unsigned char data = 0;
	mcp4652_write(WRITE_TCON, 0x0B);
	mcp4652_write(WRITE_WIPER0, 0xff);
	mcp4652_write(WRITE_WIPER1, 0x00);
}

void mcp4652_write(unsigned char addr, unsigned char value)
{
	unsigned char cmd_byte = 0;
	cmd_byte |= (addr | CMD_WRITE);

	Wire.beginTransmission(MCP4652_I2C_ADDR);
	Wire.write(cmd_byte);
	Wire.write(value);
	Wire.endTransmission();
}

void ina231_write(unsigned char pointer_addr, unsigned short value)
{
	unsigned char wdata[2];

	wdata[0] = (unsigned char)((value >> 8) & 0x00ff);
	wdata[1] = (unsigned char)(value & 0x00ff);

	Wire.beginTransmission(INA231_I2C_ADDR);
	Wire.write(pointer_addr);
	Wire.write(wdata[0]);
	Wire.write(wdata[1]);
	Wire.endTransmission();
}

unsigned char ina231_read8(unsigned char pointer_addr)
{
	Wire.beginTransmission(INA231_I2C_ADDR);
	Wire.write((uint8_t)pointer_addr);
	Wire.endTransmission();

	Wire.requestFrom(INA231_I2C_ADDR, 1);
	return Wire.read();
}

unsigned short ina231_read16(unsigned char pointer_addr)
{
	uint16_t ret;
	Wire.beginTransmission(INA231_I2C_ADDR);
	Wire.write(pointer_addr);
	Wire.endTransmission();

	Wire.requestFrom(INA231_I2C_ADDR, 2);
	ret = (uint16_t)Wire.read() << 8;
	ret |= Wire.read();

	return ret;
}

void ina231_configure(void)
{
	unsigned short config;

	config = 0x45ff;
	ina231_write(INA231_REG_CONFIG, config);
	config = 0x08bd;
	ina231_write(INA231_REG_CALIBRATION, config);
	config = 0x0408;
	ina231_write(INA231_REG_MASK_ENABLE, config);
}

float ina231_read_voltage(void)
{
	unsigned short bus_vol;
	float LSB_V = 0.00125;

	bus_vol = ina231_read16(INA231_REG_BUS_VOL);

	return (float)(bus_vol * LSB_V);
}

float ina231_read_current(void)
{
	unsigned short curr;
	float LSB_I = 0.0001526;

	curr = ina231_read16(INA231_REG_CURRENT);

	if (curr < 59) {
		curr = 0;
	} else if (curr > 32768) {
		//if (g_onoff)
			//maesure_onoff(0);
		return 0;
	}

	return (float)(curr * LSB_I);
}

float ina231_read_power(void)
{
	unsigned short power;
	float LSB_P = 0.00381687;

	power = ina231_read16(INA231_REG_POWER);

	if (power < 10)
		power = 0;

	return (float)(power * LSB_P);
}
