#include <getopt.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <exception>

#include <vector>
#include <set>
#include <boost/unordered_map.hpp>


#include "aaron_utils.hpp"
#include "easy_options.hpp"
#include "gitstatus.hpp"


#define DEBUGBYDEFAULT


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

void oNMI(const char * file1, const char * file2);

int main(int argc, char ** argv) {
	std::locale system_locale("");
	std::cout.imbue(system_locale); // to get comma-separated integers.
#ifdef DEBUGBYDEFAULT
	if(1)
#else
	if(getenv("DEBUG"))
#endif
	{
		PP(gitstatus);
		for (int i=0; i<argc; i++) {
			PP(argv[i]);
		}
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
	if(argc - optind != 2) {
		throw UsageMessage();
	}
	const char *file1 = argv[optind];
	const char *file2 = argv[optind+1];
	oNMI(file1, file2);
}

typedef std::string Node;
typedef std::vector< std::set< Node > > Grouping;
typedef boost::unordered_map< Node, set<int> > NodeToGroup;

Grouping fileToSet(const char * file) {
	Grouping ss;
	std::ifstream f(file);
	unless(f.is_open())
		throw  MissingFile();
	forEach(const std::string &line, amd::rangeOverStream(f)) {
		Grouping::value_type s;
		istringstream fields(line);
		forEach(const std::string &field, amd::rangeOverStream(fields, "\t ")) {
			if(field.length() == 0) {
				cerr << "Warning: two consecutive tabs, or tab at the start of a line. Ignoring empty fields like this" << endl;
			} else {
				s.insert(field);
			}
		}
		ss.push_back(s);
	}
	return ss;
}
NodeToGroup nodeToGroup(const Grouping &g) {
	NodeToGroup n2g;
	for(int grpId = 0; grpId < (int)g.size(); grpId++) {
		const Grouping::value_type &grp = g.at(grpId);
		forEach(const Node &n, amd::mk_range(grp)) {
			n2g[n].insert(grpId);
		}
	}
	return n2g;
}

void oNMI(const char * file1, const char * file2) {
	Grouping g1 = fileToSet(file1);
	Grouping g2 = fileToSet(file2);
	PP(g1.size());
	PP(g2.size());
	NodeToGroup n2g1 = nodeToGroup(g1);
	NodeToGroup n2g2 = nodeToGroup(g2);
	PP(n2g1.size());
	PP(n2g2.size());
}
