#include <getopt.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <exception>

#include <vector>
#include <set>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>


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
struct OverlapMatrix {
	boost::unordered_map< pair<int,int> , int> om; // the pair is an ordered pair. It's the overlap from the "ground truth" community to the "found" community.
	int N;
};

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
		if(s.size()==0)
			cerr << "Warning: ignoring empty sets in file: " << file << endl;
		else
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


const OverlapMatrix overlapMatrix(const NodeToGroup &ng1, const NodeToGroup &ng2) {
	OverlapMatrix om;
	boost::unordered_set< Node > nodes;
	forEach(const NodeToGroup::value_type &n, amd::mk_range(ng2)) {
		nodes.insert(n.first);
	}
	forEach(const NodeToGroup::value_type &n, amd::mk_range(ng1)) {
		nodes.insert(n.first);
	}
	forEach(const Node &n, amd::mk_range(nodes)) {
		if(ng1.count(n)) forEach(const int g1, amd::mk_range(ng1.at(n))) {
			if(ng2.count(n)) forEach(const int g2, amd::mk_range(ng2.at(n))) {
				om.om[make_pair(g1,g2)] ++;
			}
		}
	}
	om.N=nodes.size();
	return om;
}

double H(const int x, const int N) {
	if(x==0)
		return 0.0;
	const double Px = double(x) / double(N);
	assert(x>0 && Px > 0.0);
	return -x * log2(Px);
}
double H_X_given_Y (const int y, const int x, const int o, const int N) {
	// the NON-NORMALIZED mutual information
	// given sets of size 'l' and 'r', where there are 'o' nodes in common, what's their similarity score?
		assert(o>0 && y>=o && x>=o && y*2 <= N && x*2 <= N); // shouldn't have a group over half the size. TODO
		const double H_Y = H(y,N) + H(N-y,N);
		const double H_X = H(x,N) + H(N-x,N); // just used in the assertion

		const double Px0y0 = (N-x-y+o)  /double(N); // TODO delete these, and following, lines
		const double Px1y0 = (x-o)      /double(N);
		const double Px0y1 = (y-o)      /double(N);
		const double Px1y1 =  o         /double(N);
		DYINGWORDS(VERYCLOSE(Px0y0 + Px0y1 + Px1y0 + Px1y1, 1.0)) {
			PP(Px0y0);
			PP(Px1y0);
			PP(Px0y1);
			PP(Px1y1);
			PP(Px0y0 + Px0y1 + Px1y0 + Px1y1 - 1.0);
		}
		const double H_XY =
			  H(N-x-y+o, N)
			+ H(x-o, N)
			+ H(y-o, N)
			+ H(o, N)
			;
		DYINGWORDS(H_X >= H_XY - H_Y) {
			PP(H_X);
			PP(H_XY - H_Y);
			PP(H_X - (H_XY - H_Y));
		}
		return H_XY - H_Y;
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
	const OverlapMatrix om = overlapMatrix(n2g1, n2g2);
	forEach(const typeof(pair< const pair<int,int> , int >) &o, amd::mk_range(om.om)) {
		const int fromId = o.first.first;
		const int   toId = o.first.second;
		const int overlap = o.second;
		// PP2(fromId, toId);
		// PP(overlap);
		const double H_XgivenY = H_X_given_Y(g1.at(fromId).size(),g2.at(toId).size(), overlap, om.N);
		assert(H_XgivenY >= 0.0);
		PP(H_XgivenY);
	}
}
