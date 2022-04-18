#include "Quantization.h"

Quantization::DImage Quantization::Quantize(DImage& DCT)
{
	DImage Q(NWindow, std::vector<DPixel>(NWindow, DPixel(0))); // Initialize NWindow x NWindow image with 0 everywhere

	double dctVal;
	double qmVal;

	for (size_t i = 0; i < NWindow; i++) {
		for (size_t j = 0; j < NWindow; j++) {
			dctVal = DCT[i][j];
			qmVal = DQMatrix[i][j];

			Q[i][j] = DPixel(std::lround(dctVal / qmVal));
		}
	}

	return Q;
}

Quantization::DImage Quantization::Dequantize(DImage& Input)
{
	DImage RES(NWindow, std::vector<DPixel>(NWindow, DPixel(0))); // Initialize NWindow x NWindow image with 0 everywhere

	double inputVal;
	double qmVal;

	for (size_t i = 0; i < NWindow; i++) {
		for (size_t j = 0; j < NWindow; j++) {
			inputVal = Input[i][j];
			qmVal = DQMatrix[i][j];

			RES[i][j] = DPixel(std::lround(inputVal * qmVal));
		}
	}

	return RES;
}

void Quantization::SetQualityScalingFactor(double qualityScalingFactor)
{
	for (size_t i = 0; i < NWindow; ++i) {
		for (size_t j = 0; j < NWindow; ++j) {
			DQMatrix[i][j] = DPixel(DQMatrixDefault[i][j] * qualityScalingFactor);
		}
	}
}