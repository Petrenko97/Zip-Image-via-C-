#ifndef H_DCT_JPEG_ENC
#define H_DCT_JPEG_ENC

#include <iostream>
#include <vector>
#include <cmath>
#include <math.h> 

/** class DCT
	Performs  two-dimensional type-II DCT operations on 8x8 windows
*/
class DCT
{
public:
	static const int NWindow = 8;
	typedef double DPixel;
	typedef std::vector< std::vector<DPixel> > DImage;

	/** Performs the DCT transformation
	@param window The 8x8 window
	@return the result of the transformation
	*/
	DImage Transform(DImage window);

	/** Performs the Inverse DCT transformation
	@param window The 8x8 window
	@return the result of the inverse transformation
	*/
	DImage InverseTransform(DImage window);

	/** Sets the 2D cosines array for DCT transformation
	    (Is not called in the constructor to make it clear that it happens)
	*/
	void CalculateCosines();

	/** Sets the 2D coefficients array for DCT transformation
		(Is not called in the constructor to make it clear that it happens)
	*/
	void CalculateCoefficients();

private:
	const double pi = 3.14159265359;

	double Cosines[NWindow][NWindow];
	double Coefficient[NWindow][NWindow];
};

#endif // !H_DCT_JPEG_ENC