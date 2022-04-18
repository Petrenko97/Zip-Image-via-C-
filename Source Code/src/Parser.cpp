#include "Parser.h"

Parser::Image Parser::ReadImage(std::string filename)
{
	Image result;

	TinyTIFFReaderFile* tiffr = NULL;
	tiffr = TinyTIFFReader_open(filename.c_str());
	if (!tiffr) {
		std::cout << "    ERROR reading (not existent, not accessible or no TIFF file)\n";
	}
	else {
		if (TinyTIFFReader_wasError(tiffr)) std::cout << "   ERROR:" << TinyTIFFReader_getLastError(tiffr) << "\n";
		std::cout << "    ImageDescription:\n" << TinyTIFFReader_getImageDescription(tiffr) << "\n";
		uint32_t frames = TinyTIFFReader_countFrames(tiffr);
		std::cout << "    frames: " << frames << "\n";
		if (TinyTIFFReader_wasError(tiffr)) std::cout << "   ERROR:" << TinyTIFFReader_getLastError(tiffr) << "\n";
		uint32_t frame = 0;
		do {
			uint32_t width = TinyTIFFReader_getWidth(tiffr);
			uint32_t height = TinyTIFFReader_getHeight(tiffr);
			bool ok = true;
			if (width > 0 && height > 0) std::cout << "    size of frame " << frame << ": " << width << "x" << height << "\n";
			else { std::cout << "    ERROR IN FRAME " << frame << ": size too small " << width << "x" << height << "\n"; ok = false; }
			if (ok) {
				frame++;
				//uint16_t* image = (uint16_t*)calloc(width*height, sizeof(uint16_t)); // The library's default
				uint8_t* image = (uint8_t*)calloc(width*height, sizeof(uint8_t)); // Our addition
				TinyTIFFReader_getSampleData(tiffr, image, 0);
				if (TinyTIFFReader_wasError(tiffr)) { ok = false; std::cout << "   ERROR:" << TinyTIFFReader_getLastError(tiffr) << "\n"; }

				///////////////////////////////////////////////////////////////////
				// HERE WE CAN DO SOMETHING WITH THE IMAGE IN image (ROW-MAJOR!)
				///////////////////////////////////////////////////////////////////
				result = Image(height, std::vector<Pixel>(width, Pixel(0))); // Initialize the return image

				for (size_t i = 0; i < height; ++i)
				{
					for (size_t j = 0; j < width; ++j)
					{
						// result[i][j] = Pixel(image[j*height + i]);
						result[i][j] = Pixel(image[i*width + j]);
					}
				}

				free(image);
			}
		} while (TinyTIFFReader_readNext(tiffr)); // iterate over all frames
		std::cout << "    read " << frame << " frames\n";
	}
	TinyTIFFReader_close(tiffr);

	return result;
}

void Parser::WriteImage(Image& result, std::string filename, size_t width, size_t height)
{
	// Transform to array
	uint8_t* data = (uint8_t*)calloc(width*height, sizeof(uint8_t));

	size_t del = 0;
	for (size_t i = 0; i < height; ++i)
	{
		for (size_t j = 0; j < width; ++j)
		{
			// data[j*height+i] = static_cast<int>(result[i][j].to_ulong());
			data[i*width + j] = static_cast<int>(result[i][j].to_ulong());
		}
	}

	// Write

	TinyTIFFFile* tif = TinyTIFFWriter_open(filename.c_str(), NWindow, (uint32_t)width, (uint32_t)height);
	if (tif) {
		TinyTIFFWriter_writeImage(tif, data);
		TinyTIFFWriter_close(tif);
	}
}