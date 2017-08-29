#include <codex/vision/bitmap.hpp>
#include <fstream>

namespace  codex { namespace vision{

image load_from( const std::string& file ) {
    if ( file.size() < 4 )
        return image(0,0);

    static std::string bmp_ext(".bmp");

    auto it = file.rbegin();
    auto it2 = bmp_ext.rbegin();
    for ( int i = 0 ; i < 4 ; ++i ) {
        if ( *it != *it2 ) {
            return image(0,0);
        }
        ++it; ++it2;
    }

    bitmap_file_header bfh;
    bitmap_info_header bih;

    std::ifstream is (file, std::ifstream::binary);
    if (is) {
        is.read(reinterpret_cast<char*>(&bfh), sizeof(bfh));
        if ( bfh.type != 0x4d42 )
            return image(0,0);

        is.read(reinterpret_cast<char*>(&bih) , sizeof(bih));

        int channel = bih.bitcount / 8;
        int width = bih.width;
        int height = bih.height;

        is.seekg(bfh.offbits , is.beg);
        image img(width,height,channel);
        for ( int r = height - 1 ; r >= 0 ; --r ) {
            is.read(reinterpret_cast<char*>(img.ptr(r)) , img.stride() );
        }
        return img;
    }
    return image(0,0);
}


void save_to(const image& img, const std::string& file) {
  bitmap_file_header bfh;
  bitmap_info_header bih;
  memset(&bfh, 0x00, sizeof(bfh));
  memset(&bih, 0x00, sizeof(bih));

  bfh.type = 0x4d42;
  bfh.size = static_cast<int>( sizeof(bfh) + sizeof(bih) + 1024 + img.stride() * img.height() );
  bfh.offbits = sizeof(bfh) + sizeof(bih) + 1024;

  bih.size = sizeof(bih);
  bih.width = static_cast<int>(img.width());
  bih.height = static_cast<int>(img.height());
  bih.planes = 1;
  bih.bitcount = 8;
  bih.compression = 0;//BI_RGB;
  bih.size_image = bih.width * bih.height;
 
  std::ofstream os(file, std::ofstream::binary);
  os.write(reinterpret_cast<char*>(&bfh), sizeof(bfh));
  os.write(reinterpret_cast<char*>(&bih), sizeof(bih));
  for (int i = 0; i < 256; ++i) {
    rgb_quad gray_scale = {   static_cast<uint8_t>(i)
                            , static_cast<uint8_t>(i)
                            , static_cast<uint8_t>(i)
                            , 0 };
    os.write(reinterpret_cast<char*>(&gray_scale), sizeof(gray_scale));
  }
  for (int r = static_cast<int>(img.height()) - 1; r >= 0; --r) {
    os.write(reinterpret_cast<const char*>(img.ptr(r)), img.stride());
  }
  os.close();
}

}}
