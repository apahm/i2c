#pragma once
#include "wr_base.h"
#include <cmath>
#include <vector>
#define I2C_CMD_STOP    (1 << 12)
#define I2C_CMD_WR_M    (1 << 11)
#define I2C_CMD_WRITE   (1 << 10)
#define I2C_CMD_READ    (1 << 9)
#define I2C_CMD_START   (1 << 8)
#define RD_FULL         (1 << 15)
#define RD_EMPTY        (1 << 14)
#define WR_OVF          (1 << 13)
#define WR_FULL         (1 << 12)
#define WR_EMPTY        (1 << 11)
#define CMD_OVF         (1 << 10)
#define CMD_FULL        (1 << 9)
#define CMD_EMPTY       (1 << 8)
#define MISS_ACK        (1 << 3)
#define BUS_ACT         (1 << 2)
#define BUS_CONT        (1 << 1)
#define BUSY            1

#define C_OVER_FAULT	"\x1b[38;5;15;48;5;160m"
#define C_OVER_WARN   	"\x1b[38;5;232;48;5;214m"
#define C_UNDER_FAULT	"\x1b[38;5;15;48;5;21m"
#define C_UNDER_WARN	"\x1b[38;5;232;48;5;45m"
#define C_UNDEF			"\x1b[0m"

#define TAB 28

enum{
	UCD_VOUT_TYPE = 0,
	UCD_VOUT_COMMON_TYPE = 1,
	UCD_IOUT_TYPE = 2,
	UCD_IOUT_COMMON_TYPE = 3,
	UCD_TEMP_COMMON_TYPE = 4,
	UCD_TEMP_TYPE = 5,
	UCD_FAN_SPEED_TYPE = 6,
	UCD_VID_TYPE = 7,
};
enum{
	UNDEF_STS,
	OVER_FAULT_STS,
	OVER_WARN_STS,
	UNDER_FAULT_STS,
	UNDER_WARN_STS,
};

typedef struct {
    uint8_t type;
    char *str;
	uint8_t rail;
	int vout_exp;
    double min;
    double max;
} UCD_str_t;

class i2c
{
public:
    i2c(Wr_base *base, u32 fpga, u32 ba, u32 i2caddr);
    void Init();
    u32 Read();
    void Write(u32 data);
    void Command(u32 cmd);
    void Status();
private:
    u32 m_fpga = 0;
    u32 core_addr;
    u32 i2c_addr;
    Wr_base *wr_base = nullptr;
    void Vpx_write_i2c(u32 a, u32 d);
    u32 Vpx_read_i2c(u32 a);
};
class EEPROM_fmc: public i2c {
public:
    EEPROM_fmc(Wr_base *base, u32 fpga, u32 ba, u32 i2caddr);
    void EWrite(u32 addr, u32 data);
    u32 ERead(u32 addr);
};

class UCD90xxx: public i2c {
public:
    UCD90xxx(Wr_base *base, u32 fpga, u32 ba, u32 i2caddr);
    double ReadVolt(u32 rail);
    double ReadCurr(u32 rail);
    double ReadTemp(u32 rail);
    u32 UcdReport();
    void SelectRail (u32 rail);
    int8_t ReadRR();
};

class TPS53647: public i2c {
public:
    TPS53647(Wr_base *base, u32 fpga, u32 ba, u32 i2caddr);
    double ReadCurr();
    double ReadTemp();
    u32 TpsReport();

};