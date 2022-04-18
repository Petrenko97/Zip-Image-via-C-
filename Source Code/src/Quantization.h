#ifndef H_QUANTIZATION_JPEG_ENC
#define H_QUANTIZATION_JPEG_ENC

#include <iostream>
#include <vector>
#include <cmath>
#include <math.h>

/** class Quantization
    Performs operations related to quantization/dequantization of DCT transformations
*/
class Quantization
{
public:

	static const int NWindow = 8;
	typedef double DPixel;
	typedef std::vector< std::vector<DPixel> > DImage;

	DImage DQMatrixDefault = {
		{ 16,  11,  10,  16,  24,   40,   51,   61  },
		{ 12,  12,  14,  19,  26,   58,   60,   55  },
		{ 14,  13,  16,  24,  40,   57,   69,   56  },
		{ 14,  17,  22,  29,  51,   87,   80,   62  },
		{ 18,  22,  37,  56,  68,   109,  103,  77  },
		{ 24,  35,  55,  64,  81,   104,  113,  92  },
		{ 49,  64,  78,  87,  103,  121,  120,  101 },
		{ 72,  92,  95,  98,  112,  100,  103,  99  }
	};

	DImage DQMatrix = DQMatrixDefault;
	
	/** Performs quantization
	@param DCT the 8x8 dct transformation window
	@return the window quantized
	*/
	DImage Quantize(DImage& DCT);

	/** Performs quantization
	@param Input the quantized window
	@return the approximated 8x8 dct window
	*/
	DImage Dequantize(DImage& Input);

	/** Each value of the quantization matrix is multiplied by the parameter
	@param qualityScalingFactor the value, 0.5->5 where 0.5 means best quality
	*/
	void SetQualityScalingFactor(double qualityScalingFactor);

};

#endif // !H_QUANTIZATION_JPEG_ENC