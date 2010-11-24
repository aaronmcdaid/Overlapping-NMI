#include <getopt.h>
#include <iostream>
#include <sstream>
#include <fstream>

#include <vector>


#include "aaron_utils.hpp"
#include "easy_options.hpp"
#include "gitstatus.hpp"




using namespace std;

/*
 * Examples of args
 * -z
 *  -f 1,2,3
 *  -a (optional arg)
 */


struct arg_o {};
struct arg_d {};
namespace easy_options {
template <       > struct Flag<arg_o> : public RequiredArg, StringArgument, FlagPresent { static const char option_character = 'o'; };
template <       > struct Flag<arg_d> : public RequiredArg, StringArgument, FlagPresent { static const char option_character = 'd'; };
}

//exceptions
struct UsageMessage { };
struct BadlyFormedArg_o {};
struct MissingFile {};

int main(int argc, char ** argv) {
	std::locale system_locale("");
	std::cout.imbue(system_locale); // to get comma-separated integers.
	PP(gitstatus);
	for (int i=0; i<argc; i++) {
		PP(argv[i]);
	}
	typedef 	pair< arg_o,
			pair< arg_d,
			easy_options::nil
			>
			>
		Flags;
	easy_options::MyGetOpt<Flags> f2;
	f2.processOptions(argc, argv);


	// cout << "-d arg is '" << f2.get_<arg_d>("\t") << "'" << endl;
	// cout << "-f arg is '" << f2.get_<arg_o>("1-") << "'" << endl;
	if(argc - optind == 0) {
		throw UsageMessage();
	} else {
		throw UsageMessage();
	}

}
