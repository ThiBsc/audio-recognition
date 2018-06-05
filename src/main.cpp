#include <iostream>
#include <getopt.h>
#include <unistd.h>
#include <fstream>

#include "include/audiodecoder.h"
#include "include/signaltransformer.h"

const std::string progname = "vrecog";

void print_help();

int main(int argc, char *argv[]) {

	int help = 0;
	std::string file, file_cmp;
	std::string out_signal, out_spectrum;

	int c, idxopt;

	struct option t_opt[] =
	{
		{"help",			no_argument,			&help,		1},
		{"file",			required_argument,		NULL,		'f'},
		{"compare",			required_argument,		NULL,		'c'},
		{"out_signal",		required_argument,		NULL,		'o'},
		{"out_spectrum",	required_argument,		NULL,		's'},
        {0,         		0,              		NULL,		0}
	};
	
	while ((c = getopt_long(argc, argv, "hf:c:o:s:", t_opt, &idxopt)) != -1){
		switch (c){
			case 'h':
				help = 1;
				break;
			case 'f':
				file = optarg;
				break;
			case 'c':
				file_cmp = optarg;
				break;
			case 'o':
				out_signal = optarg;
				break;
			case 's':
				out_spectrum = optarg;
				break;
			default:
				break;
		}
  	}
  	
  	if (!help && !file.empty()){
		AudioDecoder decoder;
  		decoder.decode(file);
  		std::cout << file << std::endl;
  		std::cout << "-> " << decoder.toString() << std::endl;
  		
  		SignalTransformer st;
  		st.transformSignal(decoder.getSamples());
  		
  		if (!file_cmp.empty()){
			AudioDecoder decoder_cmp;
			decoder_cmp.decode(file_cmp);
	  		std::cout << file_cmp << std::endl;
	  		std::cout << "-> " << decoder_cmp.toString() << std::endl;
	  		
	  		SignalTransformer st_cmp;
	  		st_cmp.transformSignal(decoder_cmp.getSamples());
	  		
	  		std::cout << std::endl;
	  		std::cout << "Ref values similarity: "
	  		<< SignalTransformer::corrCoef(st.getValues(), st_cmp.getValues())
	  		<< std::endl;
	  		
	  		std::cout << "Signal similarity: "
	  		<< SignalTransformer::corrCoef(decoder.getSamples(), decoder_cmp.getSamples())
	  		<< std::endl;
	  		
	  		CArray z = st.getSpectrum()*st_cmp.getSpectrum();
			std::cout << "Spectrums sum: " << std::abs(z.sum())/z.size() << std::endl;
  		}
  		
  		if (!out_signal.empty()){
  			std::ofstream sample_file;
  			sample_file.open(out_signal, std::ios::out);
  			if (sample_file.is_open()){
  				std::vector<double> samples = decoder.getSamples();
  				int xAxis = 0;
  				for (double d : samples){
  					sample_file << xAxis++ << " " << d << "\n";
  				}
  				sample_file.close();
  			} else {
  				std::cerr << "Failed to open " << out_signal << std::endl;
  			}
  		}
  		
  		if (!out_spectrum.empty()){
  			std::ofstream spectrum_file;
  			spectrum_file.open(out_spectrum, std::ios::out);
  			if (spectrum_file.is_open()){
  				CArray spectrum = st.getSpectrum();
  				int xAxis = 0;
  				for (Complex c : spectrum){
  					spectrum_file << xAxis++ << " " << std::abs(c) << "\n";
  				}
  				spectrum_file.close();
  			} else {
  				std::cerr << "Failed to open " << out_spectrum << std::endl;
  			}
  		}
  	} else {
  		print_help();
  	}
	
	return 0;
}

void print_help(){
    std::cout << "Usage: " << progname << " -f word.wav [-OPTION...]\n"
        "\t--help, -h\t\t Show help\n"
        "\t--file, -f\t\t [audio file] Set the audio word reference\n"
        "\t--compare, -c\t\t [audio file] Set the audio word to compare\n"
        "\t--out_signal, -o\t signal.dat\n"
        "\t--out_spectrum, -s\t spectrum.dat\n\n"
        "Example of use:\n"
        << progname << " -f chocolat.wav -c banana.wav\n"
        << progname << " -f chocolat.wav -o chocolat.dat\n" << std::endl;
}
