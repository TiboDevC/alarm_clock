#pragma once

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
