#include "include/audiodecoder.h"

#define AVCODEC_MAX_AUDIO_FRAME_SIZE 192000

AudioDecoder::AudioDecoder()
	: m_file("")
	, m_acodec("")
	, m_bits(0)
	, m_duration(0)
{
	// init
	av_register_all();
	avcodec_register_all();
	pFormatCtx = avformat_alloc_context();
}

AudioDecoder::~AudioDecoder()
{
	// clear
	avformat_free_context(pFormatCtx);
}

std::string AudioDecoder::toString()
{
	std::stringstream duration;
	duration << std::setfill('0') << std::setw(2) << getHour();
	duration << "h" << std::setfill('0') << std::setw(2) << getMinute();
	duration << "m" << std::setfill('0') << std::setw(2) << getSecond() << "s";
	return std::to_string(m_bits) + " bits, codec: " + m_acodec + ", duration: " + duration.str();
}

std::vector<double> AudioDecoder::getSamples()
{
	return m_samples;
}

int64_t AudioDecoder::getMS()
{
    return m_duration;
}

int AudioDecoder::getUSecond()
{
    return m_duration % 60;
}

int AudioDecoder::getSecond()
{
    return (m_duration / AV_TIME_BASE) % 60;
}

int AudioDecoder::getMinute()
{
    return ((m_duration / AV_TIME_BASE) / 60) % 60;
}

int AudioDecoder::getHour()
{
    return ((m_duration / AV_TIME_BASE) / 60) / 60;
}

#include <iostream>
void AudioDecoder::decode(std::string file)
{
	m_file = file;
	m_samples.clear();
	
	avformat_open_input(&pFormatCtx, file.c_str(), NULL, NULL);
	if (pFormatCtx->duration == INT64_MIN || pFormatCtx->duration == INT64_MAX){
        avformat_find_stream_info(pFormatCtx, NULL);
    }
    m_duration = pFormatCtx->duration;
    
    // Find the index of the first audio stream
    int i = 0, audio_index = -1;
    while (i<pFormatCtx->nb_streams && audio_index == -1){
    	if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO){
            audio_index = i;
        }
        i++;
    }
    
    if (audio_index != -1){
        AVStream* stream = pFormatCtx->streams[audio_index];
        AVCodecContext *actx = stream->codec;
        AVCodec *a_codec = avcodec_find_decoder(actx->codec_id);
		m_acodec = a_codec->name;
		
		if (avcodec_open2(actx, a_codec, NULL)<0){
			// std::cout << "Codec cannot be found" << std::endl;
		}
		
		AVSampleFormat sfmt = actx->sample_fmt;
		switch (sfmt){
			case AV_SAMPLE_FMT_U8:
			case AV_SAMPLE_FMT_U8P:
				m_bits = 8;
				break;
			case AV_SAMPLE_FMT_S16:
			case AV_SAMPLE_FMT_S16P:
				m_bits = 16;
				break;
			case AV_SAMPLE_FMT_S32:
			case AV_SAMPLE_FMT_S32P:
				m_bits = 32;
				break;
			default:
				break;
		}
		
		AVPacket packet;
		av_init_packet(&packet);
		
		AVFrame *frame = av_frame_alloc();
		int buffer_size = AVCODEC_MAX_AUDIO_FRAME_SIZE + FF_INPUT_BUFFER_PADDING_SIZE;
		uint8_t *buffer = new uint8_t[buffer_size];
		packet.data = buffer;
		packet.size = buffer_size;
		int frameFinished = 0;
		int plane_size;
		
		while (av_read_frame(pFormatCtx, &packet) >= 0){
			if (packet.stream_index == audio_index){
				int len = avcodec_decode_audio4(actx, frame, &frameFinished, &packet);
				int data_size = av_samples_get_buffer_size(&plane_size, actx->channels, frame->nb_samples, actx->sample_fmt, 1);
				int bytesPerFrame = 0;
				int ndata = 0;
				if (frameFinished){
					int write_p = 0;
					switch (sfmt){
						case AV_SAMPLE_FMT_U8:
						case AV_SAMPLE_FMT_U8P:
							bytesPerFrame = plane_size / (plane_size/sizeof(int8_t));
							ndata = (plane_size/sizeof(uint8_t))/bytesPerFrame;
							for (size_t nb=0;nb<ndata;nb++){
				                for (int ch = 0; ch < actx->channels; ch++) {
				                    m_samples.push_back( double(((uint8_t*)frame->extended_data[ch])[nb]) / UINT8_MAX);
				                }
				            }
							break;
						case AV_SAMPLE_FMT_S16:
						case AV_SAMPLE_FMT_S16P:
							bytesPerFrame = plane_size / (plane_size/sizeof(int16_t));
							ndata = (plane_size/sizeof(uint16_t))/bytesPerFrame;
							for (size_t nb=0;nb<ndata;nb++){
				                for (int ch = 0; ch < actx->channels; ch++) {
				                    m_samples.push_back( double(((int16_t*)frame->extended_data[ch])[nb]) / INT16_MAX);
				                }
				            }
							break;
						case AV_SAMPLE_FMT_S32:
						case AV_SAMPLE_FMT_S32P:
							bytesPerFrame = plane_size / (plane_size/sizeof(int32_t));
							ndata = (plane_size/sizeof(uint32_t))/bytesPerFrame;
							for (size_t nb=0;nb<ndata;nb++){
				                for (int ch = 0; ch < actx->channels; ch++) {
				                    m_samples.push_back( double(((int32_t*)frame->extended_data[ch])[nb]) / INT32_MAX);
				                }
				            }
							break;
						default:
							break;
					}
				} else {
				
				}
			}
			av_free_packet(&packet);
		}
		avcodec_close(actx);
		delete buffer;
    }
	
    avformat_close_input(&pFormatCtx);
}
