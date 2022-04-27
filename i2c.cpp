#include "i2c.h"

UCD_str_t ucd_cmd[] = {
	{.type = UCD_TEMP_COMMON_TYPE, .str = (char *)"UCD Temp"},
	{.type = UCD_VOUT_TYPE, .str = (char *)"12V", .rail = 0, .vout_exp = -10, .min = 10.2, .max = 13.8},
	{.type = UCD_IOUT_TYPE, .str = (char *)"12V Curr", .rail = 0, .min = 0, .max = 12.5},
	{.type = UCD_VOUT_TYPE, .str = (char *)"3V3_AUX", .rail = 1, .vout_exp = -12, .min = 2.97, .max = 3.63},
	{.type = UCD_VOUT_TYPE, .str = (char *)"5V_BIAS", .rail = 2, .vout_exp = -12, .min = 4.5, .max = 5.5},
	{.type = UCD_VOUT_TYPE, .str = (char *)"3V3_CLK", .rail = 3, .vout_exp = -12, .min = 0, .max = 12.5},
	{.type = UCD_VOUT_TYPE, .str = (char *)"VCORE", .rail = 4, .vout_exp = -14, .min = 0.8835, .max = 1.0165},
	{.type = UCD_TEMP_TYPE, .str = (char *)"FPGA Temp", .rail = 4, .min = -40, .max = 115},
	{.type = UCD_VOUT_TYPE, .str = (char *)"MGTAVCC", .rail = 5, .vout_exp = -14, .min = 0.95, .max = 1.05},
	{.type = UCD_IOUT_TYPE, .str = (char *)"MGTAVCC Curr", .rail = 5, .min = 0, .max = 12},
	{.type = UCD_TEMP_TYPE, .str = (char *)"DC Temp", .rail = 5, .min = -40, .max = 125},
	{.type = UCD_VOUT_TYPE, .str = (char *)"MGTAVTT", .rail = 6, .vout_exp = -14, .min = 1.14, .max = 1.26},
	{.type = UCD_IOUT_TYPE, .str = (char *)"MGTAVTT Curr", .rail = 6, .min = 0, .max = 12},
	{.type = UCD_VOUT_TYPE, .str = (char *)"MGTVAUX_1V8", .rail = 7, .vout_exp = -13, .min = 1.71, .max = 1.89},
	{.type = UCD_VOUT_TYPE, .str = (char *)"VAUX_1V8", .rail = 8, .vout_exp = -13, .min = 1.71, .max = 1.89},
	{.type = UCD_VOUT_TYPE, .str = (char *)"VDDR_1V2", .rail = 9, .vout_exp = -14, .min = 1.08, .max = 1.32},
	{.type = UCD_VOUT_TYPE, .str = (char *)"VPP_2V5", .rail = 10, .vout_exp = -13, .min = 2.25, .max = 2.75},
	{.type = UCD_VOUT_TYPE, .str = (char *)"VTT_C0", .rail = 11, .vout_exp = -15, .min = 0.54, .max = 0.66},
	{.type = UCD_VOUT_TYPE, .str = (char *)"VTT_C1", .rail = 12, .vout_exp = -15, .min = 0.54, .max = 0.66},
	{.type = UCD_VOUT_TYPE, .str = (char *)"FMC_VADJ", .rail = 13, .vout_exp = -13, .min = 1.62, .max = 1.98},
	{.type = UCD_IOUT_TYPE, .str = (char *)"FMC_VADJ Curr", .rail = 13, .min = 0, .max = 4},
	{.type = UCD_VOUT_TYPE, .str = (char *)"FMC_3V3", .rail = 14, .vout_exp = -12, .min = 2.97, .max = 3.63},
	{.type = UCD_IOUT_TYPE, .str = (char *)"FMC_3V3 Curr", .rail = 14, .min = 0, .max = 3},
	{.type = UCD_VOUT_TYPE, .str = (char *)"FMC_12V0", .rail = 15, .vout_exp = -10, .min = 10.8, .max = 13.2},
	{.type = UCD_IOUT_TYPE, .str = (char *)"FMC_12V0 Curr", .rail = 15, .min = 0, .max = 1.5},
};

UCD_str_t tps_cmd[] = {
	//{.type = UCD_VID_TYPE, .str = (char*)"Vout"},
	{.type = UCD_IOUT_COMMON_TYPE, .str = (char *)"Iout"},
	{.type = UCD_TEMP_COMMON_TYPE, .str = (char *)"Temp"},
};

i2c::i2c(Wr_base *base, u32 fpga, u32 ba, u32 i2caddr)
{
	core_addr = ba;
	m_fpga = fpga;
	wr_base = base;
	i2c_addr = i2caddr;
}

void i2c::Init()
{
	Vpx_write_i2c(core_addr + 0x4, i2c_addr);
	Vpx_write_i2c(core_addr + 0xC, 125); //(100 * 10^6) / (200 * 10^3 * 4) = 125   prescale = Fclk / (FI2Cclk * 4)
}

void i2c::Vpx_write_i2c(u32 a, u32 d)
{
	if (m_fpga == FPGA_TX)
		wr_base->tx_write(a, d);
	else if (m_fpga == FPGA_RX)
		wr_base->rx_write(a, d);
}

u32 i2c::Vpx_read_i2c(u32 a)
{
	if (m_fpga == FPGA_TX)
		return wr_base->tx_read(a);
	else if (m_fpga == FPGA_RX)
		return wr_base->rx_read(a);
	return -1;
}
void i2c::Command(u32 cmd)
{
	Vpx_write_i2c(core_addr + 0x4, i2c_addr | cmd);
}
void i2c::Write(u32 data)
{
	Vpx_write_i2c(core_addr + 0x8, data);
}
u32 i2c::Read()
{
	return Vpx_read_i2c(core_addr + 0x8);
}
void i2c::Status()
{
	u32 state = Vpx_read_i2c(core_addr);
	if (state & RD_FULL)
		std::cout << "RD_FULL\n";
	if (state & RD_EMPTY)
		std::cout << "RD_EMPTY\n";
	if (state & WR_OVF)
		std::cout << "WR_OVF\n";
	if (state & WR_FULL)
		std::cout << "WR_FULL\n";
	if (state & WR_EMPTY)
		std::cout << "WR_EMPTY\n";
	if (state & CMD_OVF)
		std::cout << "CMD_OVF\n";
	if (state & CMD_FULL)
		std::cout << "CMD_FULL\n";
	if (state & CMD_EMPTY)
		std::cout << "CMD_EMPTY\n";
	if (state & MISS_ACK)
		std::cout << "MISS_ACK\n";
	if (state & BUS_ACT)
		std::cout << "BUS_ACT\n";
	if (state & BUS_CONT)
		std::cout << "BUS_CONT\n";
	if (state & BUSY)
		std::cout << "BUSY\n";
}

//----------------------------------------------------------------------------
EEPROM_fmc::EEPROM_fmc(Wr_base *base, u32 fpga, u32 ba, u32 i2caddr) : i2c(base, fpga, ba, i2caddr) {}

void EEPROM_fmc::EWrite(u32 addr, u32 data)
{
	Write(addr | (1 << 8));
	Write(data | (1 << 9) | (1 << 8));
	Command(I2C_CMD_START);
	Command(I2C_CMD_WRITE);
	Command(I2C_CMD_WRITE);
	Command(I2C_CMD_STOP);
}
u32 EEPROM_fmc::ERead(u32 addr)
{
	Write(addr);
	Command(I2C_CMD_WRITE);

	Command(I2C_CMD_START);
	Command(I2C_CMD_READ);
	Command(I2C_CMD_STOP);
	usleep(1000);
	uint32_t data = Read();
	if (data & (1 << 8))
	{
		printf("Read byte = 0x%02X  \n", (uint8_t)data);
		return data;
	}
	else
		return -1;
}
//----------------------------------------------------------------------------
UCD90xxx::UCD90xxx(Wr_base *base, u32 fpga, u32 ba, u32 i2caddr) : i2c(base, fpga, ba, i2caddr) {}
double UCD90xxx::ReadVolt(u32 rail)
{
	//[St] [AddrW] [A] [W:x8B] [A] [Sr] [AddrR] [A] [R:Data_1] [A] [R:Data_2] [N] [Sp]
	SelectRail(rail);
	usleep(100);

	int8_t Range = ReadRR();
	usleep(100);

	Write(0x8B);
	Command(I2C_CMD_WRITE);

	Command(I2C_CMD_START);
	Command(I2C_CMD_READ);
	Command(I2C_CMD_READ);
	Command(I2C_CMD_STOP);
	usleep(1000);
	uint8_t byte0 = 0;
	uint8_t byte1 = 0;
	uint32_t data = Read();
	if (data & (1 << 8))
	{
		// printf("Read byte0 = 0x%02X  \n", (uint8_t)data);
		byte0 = (uint8_t)data;
	}
	else
		return -1;

	data = Read();
	if (data & (1 << 8))
	{
		// printf("Read byte1 = 0x%02X  \n", (uint8_t)data);
		byte1 = (uint8_t)data;
		double Voltage = (double)(byte1 * 256 + byte0) * pow(2, (double)Range);
		// printf("Voltage = %f V \n", Voltage);
		return Voltage;
	}
	else
		return -1;
}
double UCD90xxx::ReadCurr(u32 rail)
{
	//[St] [AddrW] [A] [W:x8C] [A] [Sr] [AddrR] [A] [R:Data_1] [A] [R:Data_2] [N] [Sp]
	SelectRail(rail);
	usleep(100);

	Write(0x8C);
	Command(I2C_CMD_WRITE);
	Command(I2C_CMD_START);

	Command(I2C_CMD_READ);
	Command(I2C_CMD_READ);
	Command(I2C_CMD_STOP);
	usleep(1000);
	uint8_t byte0 = 0;
	uint8_t byte1 = 0;

	uint32_t data = Read();
	if (data & (1 << 8))
	{
		// printf("Read byte0 = 0x%02X  \n", (uint8_t)data);
		byte0 = (uint8_t)data;
	}
	else
		return -1;

	data = Read();
	if (data & (1 << 8))
	{
		// printf("Read byte1 = 0x%02X  \n", (uint8_t)data);
		byte1 = (uint8_t)data;
		uint16_t M = (byte1 & 0x0007) * 256 + byte0;
		int8_t Range = 0xE0 | (byte1 >> 3);
		// printf("ReadRR Range = %d  \n", Range);

		double Current = (double)M * pow(2, (double)Range);
		// printf("Current = %f A \n", Current);
		return Current;
	}
	else
		return -1;
}
double UCD90xxx::ReadTemp(u32 rail)
{
	//[St] [AddrW] [A] [W:x8C] [A] [Sr] [AddrR] [A] [R:Data_1] [A] [R:Data_2] [N] [Sp]
	SelectRail(rail);
	usleep(100);

	Write(0x8E);
	Command(I2C_CMD_WRITE);
	Command(I2C_CMD_START);

	Command(I2C_CMD_READ);
	Command(I2C_CMD_READ);
	Command(I2C_CMD_STOP);
	usleep(1000);
	uint8_t byte0 = 0;
	uint8_t byte1 = 0;

	uint32_t data = Read();
	if (data & (1 << 8))
	{
		// printf("Read byte0 = 0x%02X  \n", (uint8_t)data);
		byte0 = (uint8_t)data;
	}
	else
		return -1;

	data = Read();
	if (data & (1 << 8))
	{
		// printf("Read byte1 = 0x%02X  \n", (uint8_t)data);
		byte1 = (uint8_t)data;
		uint16_t M = (byte1 & 0x0007) * 256 + byte0;
		int8_t Range = 0xE0 | (byte1 >> 3);
		// printf("ReadRR Range = %d  \n", Range);

		double Temperature = (double)M * pow(2, (double)Range);
		// printf("Temperature = %f C \n", Temperature);
		return Temperature;
	}
	else
		return -1;
}
u32 UCD90xxx::UcdReport()
{
	std::vector<std::string> v_err;
	std::string s_err;
	printf("UCD90xxx Report:\n");
	for (int i = 0; i < (int)(sizeof(ucd_cmd) / sizeof(UCD_str_t)); i++)
	{
		switch (ucd_cmd[i].type)
		{
		case UCD_VOUT_TYPE:
		{
			double Voltage = ReadVolt(ucd_cmd[i].rail);
			printf("%-15s%f V\n", ucd_cmd[i].str, Voltage);
			if ((Voltage > ucd_cmd[i].max) || (Voltage < ucd_cmd[i].min))
			{
				s_err.append("Error "); 
				s_err.append(ucd_cmd[i].str); 
				s_err.append(" = "); 
				s_err.append(std::to_string(Voltage));
				s_err.append(" V\n");
				v_err.push_back(s_err);
			}
				
		}
		break;
		case UCD_IOUT_TYPE:
		{
			double Current = ReadCurr(ucd_cmd[i].rail);
			printf("%-15s%f A\n", ucd_cmd[i].str, Current);
			if ((Current > ucd_cmd[i].max) || (Current < ucd_cmd[i].min))
			{
				s_err.append("Error "); 
				s_err.append(ucd_cmd[i].str); 
				s_err.append(" = "); 
				s_err.append(std::to_string(Current));
				s_err.append(" A\n");
				v_err.push_back(s_err);
			}
		}
		break;
		case UCD_TEMP_TYPE:
		{
			double Temperature = ReadTemp(ucd_cmd[i].rail);
			printf("%-15s%f °C\n", ucd_cmd[i].str, Temperature);
			if ((Temperature > ucd_cmd[i].max) || (Temperature < ucd_cmd[i].min))
			{
				s_err.append("Error "); 
				s_err.append(ucd_cmd[i].str); 
				s_err.append(" = "); 
				s_err.append(std::to_string(Temperature));
				s_err.append(" A\n");
				v_err.push_back(s_err);
			}
		}
		break;
		default:
			break;
		}
		s_err.clear();
	}
	if(v_err.size() == 0) printf("Monitoring OK!\n");
	else 
	{
		printf("+++------------------------------------------------------------------------++++\n");
		for (auto &p : v_err)
    	{
        	std::cout << p;
    	}
		return -1;
	}

	return 0;
}
void UCD90xxx::SelectRail(u32 rail)
{
	//[St] [AddrW] [A] [W:x00] [A] [W:x00] [A] [Sp]
	Write(0x00 | (1 << 8));
	Write(rail | (1 << 8) | (1 << 9));
	Command(I2C_CMD_START);
	Command(I2C_CMD_WRITE);
	Command(I2C_CMD_WRITE);
	Command(I2C_CMD_STOP);
}
int8_t UCD90xxx::ReadRR()
{
	//[St] [AddrW] [A] [W:x20] [A] [Sr] [AddrR] [A] [R:Data_1] [N] [Sp]
	Write(0x20);
	Command(I2C_CMD_WRITE);
	Command(I2C_CMD_START);
	Command(I2C_CMD_READ);
	Command(I2C_CMD_STOP);
	usleep(1000);
	uint32_t data = Read();
	if (data & (1 << 8))
	{
		// printf("ReadRR Read byte0 = 0x%08X  \n", data);
		int8_t Range = 0xE0 | (uint8_t)data;
		// printf("ReadRR Range = %d  \n", Range);
		return Range;
	}
	else
		return -1;
}
TPS53647::TPS53647(Wr_base *base, u32 fpga, u32 ba, u32 i2caddr) : i2c(base, fpga, ba, i2caddr) {}
double TPS53647::ReadCurr()
{
	//[St] [AddrW] [A] [W:x8C] [A] [Sr] [AddrR] [A] [R:Data_1] [A] [R:Data_2] [N] [Sp]
	Write(0x8C);
	Command(I2C_CMD_WRITE);
	Command(I2C_CMD_START);

	Command(I2C_CMD_READ);
	Command(I2C_CMD_READ);
	Command(I2C_CMD_STOP);
	usleep(1000);
	uint8_t byte0 = 0;
	uint8_t byte1 = 0;

	uint32_t data = Read();
	if (data & (1 << 8))
	{
		//printf("Read byte0 = 0x%02X  \n", (uint8_t)data);
		byte0 = (uint8_t)data;
	}
	else
		return -1;

	data = Read();
	if (data & (1 << 8))
	{
		//printf("Read byte1 = 0x%02X  \n", (uint8_t)data);
		byte1 = (uint8_t)data;
		uint16_t M = (byte1 & 0x0007) * 256 + byte0;
		int8_t Range = 0xE0 | (byte1 >> 3);
		// printf("ReadRR Range = %d  \n", Range);

		double Current = (double)M * pow(2, (double)Range);
		//printf("Current = %f A \n", Current);
		return Current;
	}
	else
		return -1;
}
double TPS53647::ReadTemp()
{
	//[St] [AddrW] [A] [W:x8C] [A] [Sr] [AddrR] [A] [R:Data_1] [A] [R:Data_2] [N] [Sp]

	Write(0x8D);
	Command(I2C_CMD_WRITE);
	Command(I2C_CMD_START);

	Command(I2C_CMD_READ);
	Command(I2C_CMD_READ);
	Command(I2C_CMD_STOP);
	usleep(1000);
	uint8_t byte0 = 0;
	uint8_t byte1 = 0;

	uint32_t data = Read();
	if (data & (1 << 8))
	{
		// printf("Read byte0 = 0x%02X  \n", (uint8_t)data);
		byte0 = (uint8_t)data;
	}
	else
		return -1;

	data = Read();
	if (data & (1 << 8))
	{
		// printf("Read byte1 = 0x%02X  \n", (uint8_t)data);
		byte1 = (uint8_t)data;
		uint16_t M = (byte1 & 0x0007) * 256 + byte0;
		int8_t Range = 0xE0 | (byte1 >> 3);
		// printf("ReadRR Range = %d  \n", Range);

		double Temperature = (double)M * pow(2, (double)Range);
		//printf("Temperature = %f C \n", Temperature);
		return Temperature;
	}
	else
		return -1;
}

u32 TPS53647::TpsReport()
{

	printf("TPS53647 Report:\n");
	for (int i = 0; i < (int)(sizeof(tps_cmd) / sizeof(UCD_str_t)); i++)
	{
		switch (tps_cmd[i].type)
		{
		break;
		case UCD_IOUT_COMMON_TYPE:
		{
			double Current = ReadCurr();
			printf("%-15s%f A\n", tps_cmd[i].str, Current);
		}
		break;
		case UCD_TEMP_COMMON_TYPE:
		{
			double Temperature = ReadTemp();
			printf("%-15s%f °C\n", tps_cmd[i].str, Temperature);

		}
		break;
		default:
			break;
		}

	}

	return 0;
}