#include "../include/qr_generator.h"
#include <qrencode.h>
#include <png.h>
#include <ace/Log_Msg.h>
#include <cstring>
#include <sstream>

namespace RemoteAccessSystem {
namespace Common {

std::vector<uint8_t> QRGenerator::GeneratePNG(const std::string& data, int size) {
    std::vector<uint8_t> png_data;
    
    QRcode* qr = QRcode_encodeString(data.c_str(), 0, QR_ECLEVEL_M, QR_MODE_8, 1);
    if (!qr) {
        ACE_ERROR((LM_ERROR, "[QRGenerator] Failed to generate QR code\n"));
        return png_data;
    }
    
    int qr_size = qr->width;
    int scale = size / qr_size;
    if (scale < 1) scale = 1;
    int img_size = qr_size * scale;
    int margin = scale * 2;
    int total_size = img_size + (margin * 2);
    
    std::vector<uint8_t> image(total_size * total_size * 3, 255);
    
    for (int y = 0; y < qr_size; y++) {
        for (int x = 0; x < qr_size; x++) {
            if (qr->data[y * qr_size + x] & 1) {
                for (int dy = 0; dy < scale; dy++) {
                    for (int dx = 0; dx < scale; dx++) {
                        int px = (x * scale + dx + margin);
                        int py = (y * scale + dy + margin);
                        int idx = (py * total_size + px) * 3;
                        if (idx + 2 < (int)image.size()) {
                            image[idx] = 0;
                            image[idx + 1] = 0;
                            image[idx + 2] = 0;
                        }
                    }
                }
            }
        }
    }
    
    QRcode_free(qr);
    
    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png) {
        ACE_ERROR((LM_ERROR, "[QRGenerator] Failed to create PNG write struct\n"));
        return png_data;
    }
    
    png_infop info = png_create_info_struct(png);
    if (!info) {
        png_destroy_write_struct(&png, nullptr);
        return png_data;
    }
    
    if (setjmp(png_jmpbuf(png))) {
        png_destroy_write_struct(&png, &info);
        return png_data;
    }
    
    struct PNGWriter {
        static void write_data(png_structp png_ptr, png_bytep data, png_size_t length) {
            std::vector<uint8_t>* p = (std::vector<uint8_t>*)png_get_io_ptr(png_ptr);
            p->insert(p->end(), data, data + length);
        }
        static void flush_data(png_structp png_ptr) {}
    };
    
    png_set_write_fn(png, &png_data, PNGWriter::write_data, PNGWriter::flush_data);
    png_set_IHDR(png, info, total_size, total_size, 8,
                 PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    png_write_info(png, info);
    
    std::vector<png_bytep> rows(total_size);
    for (int y = 0; y < total_size; y++) {
        rows[y] = &image[y * total_size * 3];
    }
    png_write_image(png, rows.data());
    png_write_end(png, nullptr);
    png_destroy_write_struct(&png, &info);
    
    ACE_DEBUG((LM_INFO, "[QRGenerator] Generated PNG: %zu bytes\n", png_data.size()));
    return png_data;
}

std::string QRGenerator::GenerateSVG(const std::string& data, int size) {
    QRcode* qr = QRcode_encodeString(data.c_str(), 0, QR_ECLEVEL_M, QR_MODE_8, 1);
    if (!qr) return "";
    
    int qr_size = qr->width;
    int scale = size / qr_size;
    if (scale < 1) scale = 1;
    int margin = scale * 2;
    int total_size = (qr_size * scale) + (margin * 2);
    
    std::ostringstream svg;
    svg << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" "
        << "width=\"" << total_size << "\" height=\"" << total_size << "\">\n"
        << "<rect width=\"" << total_size << "\" height=\"" << total_size << "\" fill=\"white\"/>\n"
        << "<g fill=\"black\">\n";
    
    for (int y = 0; y < qr_size; y++) {
        for (int x = 0; x < qr_size; x++) {
            if (qr->data[y * qr_size + x] & 1) {
                svg << "<rect x=\"" << (x * scale + margin) << "\" y=\"" << (y * scale + margin) 
                    << "\" width=\"" << scale << "\" height=\"" << scale << "\"/>\n";
            }
        }
    }
    svg << "</g>\n</svg>";
    QRcode_free(qr);
    return svg.str();
}

} // namespace Common
} // namespace RemoteAccessSystem
