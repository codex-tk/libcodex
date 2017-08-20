#include <codex/vision/bitmap.hpp>
#include <fstream>

namespace  codex { namespace vision{

byte_image load_from( const std::string& file ) {
    class scoped_ifstream {
    public:
        scoped_ifstream( std::ifstream& is )
            : _is(is)
        {}
        ~scoped_ifstream(){
            _is.close();
        }
    private:
        std::ifstream& _is;
    };

    if ( file.size() < 4 )
        return byte_image(0,0);
    if ( file.substr( file.size() - 3 , 3 ) != "bmp")
        return byte_image(0,0);
    bitmap_file_header bfh;
    bitmap_info_header bih;

    std::ifstream is (file, std::ifstream::binary);
    if (is) {
        scoped_ifstream sis(is);
        is.read(reinterpret_cast<char*>(&bfh), sizeof(bfh));
        if ( bfh.type != 0x4d42 )
            return byte_image(0,0);

        is.read(reinterpret_cast<char*>(&bih) , sizeof(bih));

        int channel = bih.bitcount / 8;
        int width = bih.width;
        int height = bih.height;

        is.seekg(bfh.offbits , is.beg);
        byte_image img(width,height,channel);
        for ( int r = height - 1 ; r >= 0 ; --r ) {
            is.read(reinterpret_cast<char*>(img.ptr(r)) , img.stride() );
        }
        return img;
    }
    return byte_image(0,0);
}


void save_to(const byte_image& img, const std::string& file) {
  bitmap_file_header bfh;
  bitmap_info_header bih;
  memset(&bfh, 0x00, sizeof(bfh));
  memset(&bih, 0x00, sizeof(bih));

  bfh.type = 0x4d42;
  bfh.size = sizeof(bfh) + sizeof(bih) + 1024 + img.stride() * img.height();
  bfh.offbits = sizeof(bfh) + sizeof(bih) + 1024;

  bih.size = sizeof(bih);
  bih.width = img.width();
  bih.height = img.height();
  bih.planes = 1;
  bih.bitcount = 8;
  bih.compression = BI_RGB;
  bih.size_image = bih.width * bih.height;
 
  std::ofstream os(file, std::ofstream::binary);
  os.write(reinterpret_cast<char*>(&bfh), sizeof(bfh));
  os.write(reinterpret_cast<char*>(&bih), sizeof(bih));
  for (int i = 0; i < 256; ++i) {
    rgb_quad gray_scale = { i , i , i , 0 };
    os.write(reinterpret_cast<char*>(&gray_scale), sizeof(gray_scale));
  }
  for (int r = img.height() - 1; r >= 0; --r) {
    os.write(reinterpret_cast<const char*>(img.ptr(r)), img.stride());
  }
  os.close();
}

}}
