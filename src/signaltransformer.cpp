#include "include/signaltransformer.h"

SignalTransformer::SignalTransformer()
	: m_min(1.0)
	, m_max(0.0)
	, m_average(0.0)
	, m_stdDev(0.0)
	, m_variance(0.0)
{
}

SignalTransformer::~SignalTransformer()
{
}

void SignalTransformer::transformSignal(const std::vector<double>& signal)
{
	int N = signal.size();
	m_spectrum.resize(N);
	int i = 0;
	for (double d : signal){
		m_spectrum[i++] = d;
	}
	fft(m_spectrum);
	double S1 = 0.0, S2 = 0.0;
	for (Complex c : m_spectrum){
		double abs = std::abs(c);
		if (m_min > abs){
			m_min = abs;
		}
		if (m_max < abs){
			m_max = abs;
		}
		S1 += abs;
		S2 += (abs*abs);
	}
	m_average = S1/N;
	m_variance = S2/N-m_average*m_average;
	m_stdDev = sqrt(m_variance);
}

double SignalTransformer::getValue(SignalTransformer::Value val)
{
	return getValues()[val];
}

std::vector<double> SignalTransformer::getValues()
{
	return {m_min, m_max, m_average, m_stdDev, m_variance};
}

CArray SignalTransformer::getSpectrum()
{
	return m_spectrum;
}

double SignalTransformer::corrCoef(std::vector<double> X, std::vector<double> Y)
{
	int n = std::max(X.size(), Y.size());
	if (X.size() < Y.size()){
		X.resize(Y.size());
	} else {
		Y.resize(X.size());
	}
	double sum_X = 0.0, sum_Y = 0.0, sum_XY = 0.0;
    double squareSum_X = 0.0, squareSum_Y = 0.0;
 
    for (int i = 0; i < n; i++){
        // sum of elements of array X.
        sum_X = sum_X + X[i];
 
        // sum of elements of array Y.
        sum_Y = sum_Y + Y[i];
 
        // sum of X[i] * Y[i].
        sum_XY = sum_XY + X[i] * Y[i];
 
        // sum of square of array elements.
        squareSum_X = squareSum_X + X[i] * X[i];
        squareSum_Y = squareSum_Y + Y[i] * Y[i];
    }
 
    // use formula for calculating correlation coefficient.
    float corr = (float)(n * sum_XY - sum_X * sum_Y) 
                  / sqrt((n * squareSum_X - sum_X * sum_X) 
                      * (n * squareSum_Y - sum_Y * sum_Y));
 
    return corr;
}

void SignalTransformer::fft(CArray& x)
{
	const size_t N = x.size();
    if (N <= 1) return;
 
    // divide
    CArray even = x[std::slice(0, N/2, 2)];
    CArray  odd = x[std::slice(1, N/2, 2)];
 
    // conquer
    fft(even);
    fft(odd);
 
    // combine
    for (size_t k = 0; k < N/2; ++k){
        Complex t = std::polar(1.0, -2 * PI * k / N) * odd[k];
        x[k    ] = even[k] + t;
        x[k+N/2] = even[k] - t;
    }
}