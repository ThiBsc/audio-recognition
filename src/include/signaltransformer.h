#ifndef SIGNAL_TRANSFORMER_H
#define SIGNAL_TRANSFORMER_H

#include <vector>
#include <complex>
#include <valarray>

/**
 * https://rosettacode.org/wiki/Fast_Fourier_transform#C.2B.2B
 * https://www.geeksforgeeks.org/program-find-correlation-coefficient/
 */

const double PI = 3.141592653589793238460;

typedef std::complex<double> Complex;
typedef std::valarray<Complex> CArray;

class SignalTransformer
{
public:
	enum Value { MINIMUM=0, MAXIMUM, AVERAGE, STD_DEV, VARIANCE };
	SignalTransformer();
	virtual ~SignalTransformer();
	void transformSignal(const std::vector<double>& signal);
	double getValue(SignalTransformer::Value val);
	std::vector<double> getValues();
	CArray getSpectrum();
	static double corrCoef(std::vector<double> X, std::vector<double> Y);
	static double XcorrCoef(CArray X, CArray Y);
	static void fft(CArray& x);
	static void ifft(CArray& x);

private:
	double m_min;
	double m_max;
	double m_average;
	double m_stdDev;
	double m_variance;
	CArray m_spectrum;
	
};

#endif // SIGNAL_TRANSFORMER_H