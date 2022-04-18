#include <string>
#include <iostream>
#include <bitset>
#include <vector>

#include "Util.h"
#include "DCT.h"
#include "Quantization.h"
#include "Parser.h"

/** Main execution function */
int main(int argc, char** argv)
{
	static const int NWindow = 8;
	typedef std::bitset<NWindow> Pixel;
	typedef std::vector< std::vector<Pixel> > Image;
	typedef double DPixel;
	typedef std::vector< std::vector<DPixel> > DImage;

	std::string option, inputImagePath, groundTruthImagePath;
	double qualityScalingFactor;
	
	//------------- PARSING INPUT -------------
	
	if (argc - 1 == 3) {
		// Arguments are provided through the command line
		option = argv[1];

		if (option == "e" || option == "d" || option == "a") {
			// For encode / decode
			qualityScalingFactor = std::stod(argv[2]);
			inputImagePath = argv[3];
		}
		else if (option == "p") {
			// For PSNR mode
			inputImagePath       = argv[2];
			groundTruthImagePath = argv[3];
		}
		else {
			std::cerr << "Invalid mode\n";
			exit(EXIT_FAILURE);
		}
	}
	else {
		// Interactive mode selection
		do {
			std::cout << "Give 'e' for encode, 'd' for decode, 'p' for PSNR calculation, 'a' for all: ";
			std::cin >> option;
		} while (!(option == "e") && !(option == "d") && 
                 !(option == "p") && !(option == "a")); // This is more complicated because of cin
		
		if (option == "p") {
			std::cout << "Give image file name (must be on the same directory as this executable): ";
			std::cin >> inputImagePath;
			std::cout << "Give ground truth image file name (on the same directory as this executable): ";
			std::cin >> groundTruthImagePath;
		}
		else {
			//Quality product scalar
			do {
				std::cout << "Give quality scaling factor (default=1,best=0.5) in the range [0.5,5]: ";
				std::cin >> qualityScalingFactor;
			} while (qualityScalingFactor < 0.5 || qualityScalingFactor > 5.0);

			if (option == "e" || option == "a") {
				std::cout << "Give image file name (must be on the same directory as this executable): ";
			}
			else {
				std::cout << "Give encoded image file name (on the same directory as this executable): ";
			}
			std::cin >> inputImagePath;
		}

		std::cout << "\n";
	}

	//------------- FOR OPTION P (PSNR CALCULATION) -------------

	if (option == "p") {
		// PSNR
		Image  inputImage = Parser::ReadImage(inputImagePath);
		DImage dimage     = Util::ConvertImageToDoubleImage<uint8_t>(inputImage);

		Image  groundTruth  = Parser::ReadImage(groundTruthImagePath);
		DImage dgroundTruth = Util::ConvertImageToDoubleImage<uint8_t>(groundTruth);

		std::cout << "\nPSNR = " << Util::PSNR(dimage, dgroundTruth) << "\n";

		return 0;
	}

	//------------- READING IMAGE (FOR ENCODING/DECODING) -------------

	Image  image;  // The raw input image
	DImage dimage; // The input image with double pixel type

	if (inputImagePath == "DEFAULT") {
		std::cout << "Using default sample image\n";

		image = {
			{52,55,61,66,70,61,64,73},
			{63,59,55,90,109,85,69,72},
			{62,59,68,113,144,104,66,73},
			{63,58,71,122,154,106,70,69},
			{67,61,68,104,126,88,68,70},
			{79,65,60,70,77,68,58,75},
			{85,71,64,59,55,61,65,83},
			{87,79,69,68,65,76,78,94}
		};
	}
	else {
		image  = Parser::ReadImage(inputImagePath);
	}
	
	if (option == "e" || option == "a") {
		// Encoding, the original images are 8bit 0->255, so uint8_t
		dimage = Util::ConvertImageToDoubleImage<uint8_t>(image);
	}
	else {
		// Decoding, the input images are 8bit -128->127, so int8_t
		dimage = Util::ConvertImageToDoubleImage<int8_t>(image);
	}
	
	size_t rows = image.size(), cols = image[0].size();
	
	//------------- INITIALIZATION OPERATIONS -------------

	// The calculations happen for double pixel type, but are saved as 8-bit
	Image   Result  (rows, std::vector<Pixel>(cols, Pixel(0)));   // Initializes the output image with 0 (size 1 byte) everywhere
	DImage  DResult (rows, std::vector<DPixel>(cols, DPixel(0))); // Initializes the output image with 0 (size 1 byte) everywhere

	DCT dct;
	Quantization quantization;
	
	dct.CalculateCosines();
	dct.CalculateCoefficients();
	quantization.SetQualityScalingFactor(qualityScalingFactor);

	//------------- ALGORITHM -------------

	int progress = 0;
	
	if (option == "e" || option == "a") {
		// Encode
		std::cout << "\nEncoding...0%";

		for (size_t i = 0; i < rows; i += NWindow) {
			for (size_t j = 0; j < cols; j += NWindow) {
				DImage window = Util::CalculateWindow(dimage, i, j, rows, cols); // Find 8x8 window
				Util::ShiftFor(window, -128);
				DImage dctImage = dct.Transform(window);
				DImage quantizedImage = quantization.Quantize(dctImage);
				
				Util::AddWindowToImage(DResult, quantizedImage, i, j, rows, cols); // Add quantized segment to result
			}

			// Progress update
			if (100 * (i + 1) / rows > progress + 10) {
				progress += 10;
				std::cout << "\rEncoding..." << progress << "%";
			}
		}

		// Finishing
		std::cout << "\rEncoding...100%\n";
		Util::BoundDoubleImage(DResult, -128, 127);

		if (option == "e") {		
			Image imageFromDImage = Util::ConvertDoubleImageToImage(DResult);
			Parser::WriteImage(imageFromDImage, "res_enc.tif", cols, rows);
		}
		else {
			// Option "a"
			dimage = DResult;
		}
	}

	if (option == "d" || option == "a") {
		// Decode
		std::cout << "\nDecoding...0%";

		for (size_t i = 0; i < rows; i += NWindow) {
			for (size_t j = 0; j < cols; j += NWindow) {
				DImage window = Util::CalculateWindow(dimage, i, j, rows, cols); // Find 8x8 window
				DImage deq    = quantization.Dequantize(window);
				DImage idctImage = dct.InverseTransform(deq);
				Util::ShiftFor(idctImage, 128);
				
				Util::AddWindowToImage(DResult, idctImage,	 i, j, rows, cols); // Add decoded segment to result
			}

			// Progress update
			if (100 * (i + 1) / rows > progress + 10) {
				progress += 10;
				std::cout << "\rDecoding..." << progress << "%";
			}
		}

		// Finishing
		std::cout << "\rDecoding...100%\n";
		Util::BoundDoubleImage(DResult, 0, 255);
		Image imageFromDImage = Util::ConvertDoubleImageToImage(DResult);

		if (option == "d") {
			Parser::WriteImage(imageFromDImage, "res_dec.tif", cols, rows);
		}
		else {
			// option "a"
			Parser::WriteImage(imageFromDImage, inputImagePath.substr(0, inputImagePath.find(".")) 
				+ "_" + std::to_string(qualityScalingFactor) +".tif", 
				cols, rows);	
		}
	}

	if (option == "a") {
		dimage = Util::ConvertImageToDoubleImage<uint8_t>(image);

		std::cout << "\nPSNR = " << Util::PSNR(DResult, dimage) << "\n";
	}

	return 0;
}
