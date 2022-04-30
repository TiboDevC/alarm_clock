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
#define EPD_3IN7_WIDTH       (280)
#define EPD_3IN7_HEIGHT      (480)

void EPD_3IN7_4Gray_Clear(void);
void EPD_3IN7_4Gray_Init(void);
void EPD_3IN7_4Gray_Display(const UBYTE *Image);

void EPD_3IN7_1Gray_Clear(void);
void EPD_3IN7_1Gray_Init(void);
void EPD_3IN7_1Gray_Display(const UBYTE *Image);
void EPD_3IN7_1Gray_Display_Part(const uint8_t *Image,
                                 const uint16_t image_x_size,
                                 const uint16_t image_y_size,
                                 const uint32_t buf_size_byte, uint16_t Xstart,
                                 uint16_t Ystart);

void EPD_3IN7_Sleep(void);

#define EPD_CMD_DRV_OUTPUT_CTRL 0x01
#define EPD_CMD_GATE_DRV_CTRL 0x03
#define EPD_CMD_SRC_DRV_VOLT_CTRL 0x04
#define EPD_CMD_INIT_SET_OTP 0x08
#define EPD_CMD_WRITE_REG_INIT_CODE_SET 0x09
#define EPD_CMD_READ_REG_INIT_CODE_SET 0x0A
#define EPD_CMD_BOOSTER_SOFT_START_CTRL 0x0C
#define EPD_CMD_DEEP_SLEEP 0x10
#define EPD_CMD_DATA_ENTRY 0x11
#define EPD_CMD_SW_RESET 0x12
#define EPD_CMD_HV_READY_DETECT 0x14
#define EPD_CMD_VCI_DETECT 0x15
#define EPD_CMD_TEMP_SENS 0x18
#define EPD_CMD_WRITE_TEMP_SENS 0x1A
#define EPD_CMD_READ_TEMP_SENS 0x1B
#define EPD_CMD_WRITE_EXT_TEMP_SENS 0x1C
#define EPD_CMD_UPDATE_SCREEN 0x20
#define EPD_CMD_DISP_UPT_CTRL_1 0x21
#define EPD_CMD_DISP_UPT_CTRL_2 0x22
#define EPD_CMD_WRITE_RAM_BW 0x24
#define EPD_CMD_WRITE_RAM_GREY 0x26
#define EPD_CMD_VCOM_SENSE 0x28
#define EPD_CMD_VCOM_SENSE_DUR 0x29
#define EPD_CMD_VCOM_CONF_1 0x2A
#define EPD_CMD_VCOM_CONF_2 0x2B
#define EPD_CMD_VCOM_CONF_3 0x2C
#define EPD_CMD_WRITE_LUT 0x32
#define EPD_CMD_WRITE_DISP_OPT 0x37
#define EPD_CMD_BORDER_WAVEFORM 0x3C
#define EPD_CMD_RAM_X_POS 0x44
#define EPD_CMD_RAM_Y_POS 0x45
#define EPD_CMD_SET_RAM_X_POS 0x4E
#define EPD_CMD_SET_RAM_Y_POS 0x4F
#define EPD_CMD_NOP 0x7F

#endif
