#include "DCT.h"

DCT::DImage DCT::Transform(DImage window)
{
	DImage DCT(NWindow, std::vector<DPixel>(NWindow, DPixel(0))); // Initialize NWindow x NWindow image with 0 everywhere

	for (int u = 0; u < NWindow; u++) {
		for (int v = 0; v < NWindow; v++) {
			double val = Coefficient[u][v] / 4;
			double sum = 0;

			for (int x = 0; x < NWindow; x++) {
				for (int y = 0; y < NWindow; y++) {
					sum += Cosines[x][u] *
						Cosines[y][v] *
						window[x][y];
				}
			}

			DCT[u][v] = DPixel(val*sum);
		}
	}

	return DCT;
}

DCT::DImage DCT::InverseTransform(DImage window)
{
	DImage IDCT(NWindow, std::vector<DPixel>(NWindow, DPixel(0))); // Initialize NWindow x NWindow image with 0 everywhere

	for (int x = 0; x < NWindow; x++) {
		for (int y = 0; y < NWindow; y++) {
			double val = 1.0 / 4;
			double sum = 0;

			for (int u = 0; u < NWindow; u++) {
				for (int v = 0; v < NWindow; v++) {
					sum += Coefficient[u][v] *
						Cosines[x][u] *
						Cosines[y][v] *
						window[u][v];
				}
			}

			IDCT[x][y] = DPixel(val*sum);
		}
	}

	return IDCT;
}

void DCT::CalculateCosines()
{
	for (int i = 0; i < NWindow; ++i)
	{
		for (int j = 0; j < NWindow; ++j)
		{
			double val = (2 * i + 1) * j * pi;
			Cosines[i][j] = std::cos(val / (2.0 * NWindow));
		}
	}
}

void DCT::CalculateCoefficients()
{
	for (int i = 0; i < NWindow; ++i)
	{
		for (int j = 0; j < NWindow; ++j)
		{
			double a_u = (i == 0) ? (1.0 / sqrt(2.0)) : 1.0;
			double a_v = (j == 0) ? (1.0 / sqrt(2.0)) : 1.0;

			Coefficient[i][j] = a_u * a_v;
		}
	}
}