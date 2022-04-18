#ifndef H_PARSER_JPEG_ENC
#define H_PARSER_JPEG_ENC

#include <iostream>
#include <bitset>
#include <string>
#include <vector>

#include "tinytiffreader.h"
#include "tinytiffwriter.h"

/** namespace Parser
    Performs I/O operations of 8bit images using the TinyTIFF library
*/
namespace Parser
{
	typedef std::bitset<8> Pixel;
	typedef std::vector< std::vector<Pixel> > Image;
	static const int NWindow = 8; // Actually only works for 8 for now

	/** Reads images from file
	@param filename path to file (must include extension .tif or .tiff). Could be absolute or relative path
	@return the image created from the file contents
	*/
	Image ReadImage(std::string filename);

	/** Writes image to file
	@param result the image that will be written to file
	@param filename the desired filename (must include extension .tif or .tiff)
	@param width the width of the image
	@param height the height of the image
	*/
	void WriteImage(Image& result, std::string filename, size_t width, size_t height);

}

#endif // !H_PARSER_JPEG_ENC