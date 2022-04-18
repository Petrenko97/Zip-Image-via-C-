#ifndef H_UTIL_JPEG_ENC
#define H_UTIL_JPEG_ENC

#include <iostream>
#include <vector>
#include <cmath>
#include <math.h>

namespace Util
{
	static const int NWindow = 8;
	typedef std::bitset<NWindow> Pixel;
	typedef std::vector< std::vector<Pixel> > Image;
	typedef double DPixel;
	typedef std::vector< std::vector<DPixel> > DImage;

	/** Creates the 8x8 window */
	DImage CalculateWindow(DImage& image, size_t i, size_t j, size_t rows, size_t cols)
	{
		DImage window(NWindow, std::vector<DPixel>(NWindow, DPixel(0))); // Initializes the 2D 8x8 vector with 0 (size 1 byte) everywhere

		size_t end_i = (i + NWindow > rows) ? rows - 1 : i + NWindow - 1;
		size_t end_j = (j + NWindow > rows) ? cols - 1 : j + NWindow - 1;

		size_t i_lastInBounds, j_lastInBounds;

		for (size_t ii = i; ii < i+NWindow; ++ii)
		{
			for (size_t jj = j; jj < j+NWindow; ++jj)
			{
				i_lastInBounds = (ii < end_i) ? ii : end_i;
				j_lastInBounds = (jj < end_j) ? jj : end_j;

				window[ii-i][jj-j] = image[i_lastInBounds][j_lastInBounds];
			}
		}

		return window;
	}

	void AddWindowToImage(DImage& image,     DImage window, 
                          const size_t i,    const size_t j, 
                          const size_t rows, const size_t cols)
	{
		size_t end_i = (i + NWindow > rows) ? rows - 1 : i + NWindow - 1;
		size_t end_j = (j + NWindow > cols) ? cols - 1 : j + NWindow - 1;

		/*std::cout << "Image  size: rows(height)=" << rows << ", cols(width)=" << cols << "\n";
		std::cout << "Window size: rows(height)=" << window.size() << ", cols(width)=" << window[0].size() << "\n";
		std::cout << "Adding i in {" << i << "," << end_i << "}, j in {" << j << "," << end_j << "}\n";*/

		size_t i_image, i_window, j_image, j_window;

		for (i_image = i, i_window = 0; i_image <= end_i; ++i_image, ++i_window)
		{
			for (j_image = j, j_window = 0; j_image <= end_j; ++j_image, ++j_window)
			{
				image[i_image][j_image] = window[i_window][j_window];
			}
		}
	}

	void ShiftFor(DImage& window, int shiftForVal)
	{
		size_t rows = window.size(), cols = window[0].size();

		for (size_t i = 0; i < rows; ++i) {
			for (size_t j = 0; j < cols; ++j) {
				double val = window[i][j];
				val += shiftForVal;

				window[i][j] = DPixel(val);
			}
		}
	}

	void PrintImageArray(DImage& image)
	{
		std::cout << "\nPrinting image:\n\n";

		for (std::vector< DPixel >& row : image)
		{
			for (DPixel pixel : row)
			{
				std::cout << std::lround(pixel) << " ";
			}

			std::cout << "\n";
		}

		std::cout << "\n";
	}

	void PrintImageArray(Image& image)
	{
		std::cout << "\nPrinting image:\n\n";

		for (std::vector< Pixel >& row : image)
		{
			for (Pixel pixel : row)
			{
				std::cout << static_cast<int>(pixel.to_ulong()) << " ";
			}

			std::cout << "\n";
		}

		std::cout << "\n";
	}

	double PSNR(DImage& image, DImage& groundTruth)
	{
		int image_max = std::pow(2, NWindow) - 1; // 255 for 8bit

		size_t rows = image.size(), cols = image[0].size();
		
		double sum = 0;

		// Calculate MSE (Mean Squared Error)
		for (size_t i = 0; i < rows; ++i)
		{
			for (size_t j = 0; j < cols; ++j)
			{
				sum += std::pow(groundTruth[i][j] - image[i][j], 2);
			}
		}

		double mse = sum / (rows*cols);

		return 10 * log10(std::pow(image_max, 2) / mse);
	}

	template<typename PixelType>
	DImage ConvertImageToDoubleImage(Image& image)
	{
		DImage dimage(image.size(), std::vector<DPixel>(image[0].size(), DPixel(0)));

		for (size_t i = 0; i < image.size(); ++i)
		{
			for (size_t j = 0; j < image[0].size(); ++j)
			{
				PixelType val = static_cast<PixelType>(image[i][j].to_ulong());

				dimage[i][j] = static_cast<double>(val);
			}
		}

		return dimage;
	}

	Image ConvertDoubleImageToImage(DImage& dimage)
	{
		Image image(dimage.size(), std::vector<Pixel>(dimage[0].size(), Pixel(0)));

		for (size_t i = 0; i < dimage.size(); ++i)
		{
			for (size_t j = 0; j < dimage[0].size(); ++j)
			{
				image[i][j] = Pixel( std::lround(dimage[i][j]) );
			}
		}

		return image;
	}

	void BoundDoubleImage(DImage& dimage, double low, double high)
	{
		for (size_t i = 0; i < dimage.size(); ++i)
		{
			for (size_t j = 0; j < dimage[0].size(); ++j)
			{
				double val = dimage[i][j];

				if (val < low)  { dimage[i][j] = low;  }
				if (val > high) { dimage[i][j] = high; }
			}
		}
	}
}

#endif // !H_UTIL_JPEG_ENC