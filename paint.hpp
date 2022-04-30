#pragma once

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

enum img_rotation_t {
    rotate_0 = 0,
    rotate_90,
    rotate_180,
    rotate_270,
};

enum grey_scale_t {
    grey_scale_2 = 8,
    grey_scale_4 = 4,
};

enum color_t {
    black = 0,
    grey_0,
    grey_1,
    white,
};

class EPD_Image {
public:
    EPD_Image() = delete;

    EPD_Image(const uint16_t x_size, const uint16_t y_size, uint8_t *buf,
              const img_rotation_t img_rotation, const grey_scale_t grey_scale)
        : x_size(x_size), y_size(y_size), buf(buf), img_rotation(img_rotation),
          grey_scale(grey_scale) {
        memset(buf, white, x_size * y_size / 4);
    };

    const uint16_t x_size{};
    const uint16_t y_size{};
    uint8_t *buf{nullptr};
    const img_rotation_t img_rotation{};
    const grey_scale_t grey_scale{};

private:
};
