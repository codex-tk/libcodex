#ifndef BITMAP_HPP
#define BITMAP_HPP

#include <codex/codex.hpp>
#include <codex/vision/image.hpp>

namespace  codex { namespace vision {

#pragma pack(1)
struct bitmap_file_header{      // Bitmap file header
    uint16_t type;              // Used to identify the BMP and DIB file is 0x42 0x4D in hexadecimal, same as BM in ASCII
    uint32_t size;              // size of the BMP file in bytes
    uint16_t reserved1;         // Reserved; actual value depends on the application that creates the image
    uint16_t reserved2;         // "                                                                       "
    uint32_t offbits;           // offset, i.e. starting address, of the byte where the bitmap image data (pixel array) can be found
};

struct bitmap_info_header {
    uint32_t size;
    uint32_t width;
    uint32_t height;
    uint16_t planes;
    uint16_t bitcount;
    uint32_t compression;
    uint32_t size_image;
    uint32_t x_pers_per_meter;
    uint32_t y_pers_per_meter;
    uint32_t clr_used;
    uint32_t clr_important;
};

struct rgb_quad {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
    uint8_t reserved;
};
#pragma pack()

image load_from( const std::string& file );
void save_to(const image& img , const std::string& file);

}}

#endif // BITMAP_HPP
