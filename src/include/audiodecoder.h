#ifndef AUDIO_DECODER_H
#define AUDIO_DECODER_H

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <cstdint>

class AudioDecoder
{
public:
	AudioDecoder();
	virtual ~AudioDecoder();
	std::string toString();
	std::vector<double> getSamples();
	int64_t getMS();
	int getUSecond();
	int getSecond();
	int getMinute();
	int getHour();
	void decode(std::string file);

private:
	AVFormatContext *pFormatCtx;
	std::string m_file;
	std::string m_acodec;
	std::vector<double> m_samples;
	int m_bits;
	int64_t m_duration;
	
};

#endif // AUDIO_DECODER_H