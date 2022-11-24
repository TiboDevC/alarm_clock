/*****************************************************************************
* | File      	:   EPD_3IN7.h
* | Author      :   Waveshare team
* | Function    :   3.7inch e-paper
* | Info        :
*----------------
* |	This version:   V1.0
* | Date        :   2020-08-10
* | Info        :
* -----------------------------------------------------------------------------
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
******************************************************************************/
#ifndef __EPD_3IN7_H_
#define __EPD_3IN7_H_

#include "DEV_Config.h"

// Display resolution
#define EPD_3IN7_WIDTH  (280)
#define EPD_3IN7_HEIGHT (480)

void EPD_3IN7_4Gray_Clear(void);
void EPD_3IN7_4Gray_Init(void);
void EPD_3IN7_4Gray_Display(const UBYTE *Image);

void EPD_3IN7_1Gray_Clear(void);
void EPD_3IN7_1Gray_Init(void);
void EPD_3IN7_1Gray_Display(const UBYTE *Image);

void EPD_3IN7_Sleep(void);

enum gate_driving_voltage_t {
	vgh_20   = 0x00,
	vgh_12   = 0x07,
	vgh_12_5 = 0x08,
	vgh_13   = 0x09,
	vgh_13_5 = 0x0a,
	vgh_14   = 0x0b,
	vgh_14_5 = 0x0c,
	vgh_15   = 0x0d,
	vgh_15_5 = 0x0e,
	vgh_16   = 0x0f,
	vgh_16_5 = 0x10,
	vgh_17   = 0x11,
	vgh_17_5 = 0x12,
	vgh_18   = 0x13,
	vgh_18_5 = 0x14,
	vgh_19   = 0x15,
	vgh_19_5 = 0x16,
};

enum source_driving_positive_voltage_t {
	vshx_2_4  = 0x8e,
	vshx_2_5  = 0x8F,
	vshx_2_6  = 0x90,
	vshx_2_7  = 0x91,
	vshx_2_8  = 0x92,
	vshx_2_9  = 0x93,
	vshx_3    = 0x94,
	vshx_3_1  = 0x95,
	vshx_3_2  = 0x96,
	vshx_3_3  = 0x97,
	vshx_3_4  = 0x98,
	vshx_3_5  = 0x99,
	vshx_3_6  = 0x9A,
	vshx_3_7  = 0x9B,
	vshx_3_8  = 0x9C,
	vshx_3_9  = 0x9D,
	vshx_4    = 0x9E,
	vshx_4_1  = 0x9F,
	vshx_4_2  = 0xA0,
	vshx_4_3  = 0xA1,
	vshx_4_4  = 0xA2,
	vshx_4_5  = 0xA3,
	vshx_4_6  = 0xA4,
	vshx_4_7  = 0xA5,
	vshx_4_8  = 0xA6,
	vshx_4_9  = 0xA7,
	vshx_5    = 0xA8,
	vshx_5_1  = 0xA9,
	vshx_5_2  = 0xAA,
	vshx_5_3  = 0xAB,
	vshx_5_4  = 0xAC,
	vshx_5_5  = 0xAD,
	vshx_5_6  = 0xAE,
	vshx_5_7  = 0xAF,
	vshx_5_8  = 0xB0,
	vshx_5_9  = 0xB1,
	vshx_6    = 0xB2,
	vshx_6_1  = 0xB3,
	vshx_6_2  = 0xB4,
	vshx_6_3  = 0xB5,
	vshx_6_4  = 0xB6,
	vshx_6_5  = 0xB7,
	vshx_6_6  = 0xB8,
	vshx_6_7  = 0xB9,
	vshx_6_8  = 0xBA,
	vshx_6_9  = 0xBB,
	vshx_7    = 0xBC,
	vshx_7_1  = 0xBD,
	vshx_7_2  = 0xBE,
	vshx_7_3  = 0xBF,
	vshx_7_4  = 0xC0,
	vshx_7_5  = 0xC1,
	vshx_7_6  = 0xC2,
	vshx_7_7  = 0xC3,
	vshx_7_8  = 0xC4,
	vshx_7_9  = 0xC5,
	vshx_8    = 0xC6,
	vshx_8_1  = 0xC7,
	vshx_8_2  = 0xC8,
	vshx_8_3  = 0xC9,
	vshx_8_4  = 0xCA,
	vshx_8_5  = 0xCB,
	vshx_8_6  = 0xCC,
	vshx_8_7  = 0xCD,
	vshx_8_8  = 0xCE,
	vshx_9    = 0x23,
	vshx_9_2  = 0x24,
	vshx_9_4  = 0x25,
	vshx_9_6  = 0x26,
	vshx_9_8  = 0x27,
	vshx_10   = 0x28,
	vshx_10_2 = 0x29,
	vshx_10_4 = 0x2A,
	vshx_10_6 = 0x2B,
	vshx_10_8 = 0x2C,
	vshx_11   = 0x2D,
	vshx_11_2 = 0x2E,
	vshx_11_4 = 0x2F,
	vshx_11_6 = 0x30,
	vshx_11_8 = 0x31,
	vshx_12   = 0x32,
	vshx_12_2 = 0x33,
	vshx_12_4 = 0x34,
	vshx_12_6 = 0x35,
	vshx_12_8 = 0x36,
	vshx_13   = 0x37,
	vshx_13_2 = 0x38,
	vshx_13_4 = 0x39,
	vshx_13_6 = 0x3A,
	vshx_13_8 = 0x3B,
	vshx_14   = 0x3C,
	vshx_14_2 = 0x3D,
	vshx_14_4 = 0x3E,
	vshx_14_6 = 0x3F,
	vshx_14_8 = 0x40,
	vshx_15   = 0x41,
	vshx_15_2 = 0x42,
	vshx_15_4 = 0x43,
	vshx_15_6 = 0x44,
	vshx_15_8 = 0x45,
	vshx_16   = 0x46,
	vshx_16_2 = 0x47,
	vshx_16_4 = 0x48,
	vshx_16_6 = 0x49,
	vshx_16_8 = 0x4A,
	vshx_17   = 0x4B,
};

enum source_driving_negative_voltage_t {
	vsl_minus_9    = 0x1a,
	vsl_minus_9_5  = 0x1C,
	vsl_minus_10   = 0x1E,
	vsl_minus_10_5 = 0x20,
	vsl_minus_11   = 0x22,
	vsl_minus_11_5 = 0x24,
	vsl_minus_12   = 0x26,
	vsl_minus_12_5 = 0x28,
	vsl_minus_13   = 0x2A,
	vsl_minus_13_5 = 0x2C,
	vsl_minus_14   = 0x2E,
	vsl_minus_14_5 = 0x30,
	vsl_minus_15   = 0x32,
	vsl_minus_15_5 = 0x34,
	vsl_minus_16   = 0x36,
	vsl_minus_16_5 = 0x38,
	vsl_minus_17   = 0x3A,
};

enum vcom_voltage_t {
	vcom_minus_0_2 = 0x8,
	vcom_minus_0_3 = 0xc,
	vcom_minus_0_4 = 0x10,
	vcom_minus_0_5 = 0x14,
	vcom_minus_0_6 = 0x18,
	vcom_minus_0_7 = 0x1C,
	vcom_minus_0_8 = 0x20,
	vcom_minus_0_9 = 0x24,
	vcom_minus_1   = 0x28,
	vcom_minus_1_1 = 0x2C,
	vcom_minus_1_2 = 0x30,
	vcom_minus_1_3 = 0x34,
	vcom_minus_1_4 = 0x38,
	vcom_minus_1_5 = 0x3C,
	vcom_minus_1_6 = 0x40,
	vcom_minus_1_7 = 0x44,
	vcom_minus_1_8 = 0x48,
	vcom_minus_1_9 = 0x4C,
	vcom_minus_2   = 0x50,
	vcom_minus_2_1 = 0x54,
	vcom_minus_2_2 = 0x58,
	vcom_minus_2_3 = 0x5C,
	vcom_minus_2_4 = 0x60,
	vcom_minus_2_5 = 0x64,
	vcom_minus_2_6 = 0x68,
	vcom_minus_2_7 = 0x6C,
	vcom_minus_2_8 = 0x70,
	vcom_minus_2_9 = 0x74,
	vcom_minus_3   = 0x78,
	vcom_minus_3_1 = 0x7C,
	vcom_minus_3_2 = 0x80,
	vcom_minus_3_3 = 0x84,
	vcom_minus_3_4 = 0x88,
	vcom_minus_3_5 = 0x8C,
	vcom_minus_3_6 = 0x90,
	vcom_minus_3_7 = 0x94,
	vcom_minus_3_8 = 0x98,
	vcom_minus_3_9 = 0x9C,
	vcom_minus_4   = 0xA0,
};

enum data_entry_t {
	y_decrement_x_decrement = 0b00,
	y_decrement_x_increment = 0b01,
	y_increment_x_decrement = 0b10,
	y_increment_x_increment = 0b11,
};

#define EPD_CMD_DRV_OUTPUT_CTRL         0x01
#define EPD_CMD_GATE_DRV_CTRL           0x03
#define EPD_CMD_SRC_DRV_VOLT_CTRL       0x04
#define EPD_CMD_INIT_SET_OTP            0x08
#define EPD_CMD_WRITE_REG_INIT_CODE_SET 0x09
#define EPD_CMD_READ_REG_INIT_CODE_SET  0x0A
#define EPD_CMD_BOOSTER_SOFT_START_CTRL 0x0C
#define EPD_CMD_DEEP_SLEEP              0x10
#define EPD_CMD_DATA_ENTRY              0x11
#define EPD_CMD_SW_RESET                0x12
#define EPD_CMD_HV_READY_DETECT         0x14
#define EPD_CMD_VCI_DETECT              0x15
#define EPD_CMD_TEMP_SENS               0x18
#define EPD_CMD_WRITE_TEMP_SENS         0x1A
#define EPD_CMD_READ_TEMP_SENS          0x1B
#define EPD_CMD_WRITE_EXT_TEMP_SENS     0x1C
#define EPD_CMD_UPDATE_SCREEN           0x20
#define EPD_CMD_DISP_UPT_CTRL_1         0x21
#define EPD_CMD_DISP_UPT_CTRL_2         0x22
#define EPD_CMD_WRITE_RAM_BW            0x24
#define EPD_CMD_WRITE_RAM_GREY          0x26
#define EPD_CMD_VCOM_SENSE              0x28
#define EPD_CMD_VCOM_SENSE_DUR          0x29
#define EPD_CMD_VCOM_CONF_1             0x2A
#define EPD_CMD_VCOM_CONF_2             0x2B
#define EPD_CMD_VCOM_CONF_3             0x2C
#define EPD_CMD_WRITE_LUT               0x32
#define EPD_CMD_WRITE_DISP_OPT          0x37
#define EPD_CMD_BORDER_WAVEFORM         0x3C
#define EPD_CMD_RAM_X_POS               0x44
#define EPD_CMD_RAM_Y_POS               0x45
#define EPD_CMD_SET_RAM_X_POS           0x4E
#define EPD_CMD_SET_RAM_Y_POS           0x4F
#define EPD_CMD_NOP                     0x7F

#endif
