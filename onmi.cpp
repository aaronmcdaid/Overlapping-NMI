/*
 *     Copyright: Aaron F. McDaid 2011 aaronmcdaid@gmail.com
 *     See our paper on arXiv: "Normalized Mutual Information to evaluate overlapping
 *     community finding algorithms" by Aaron F. McDaid, Derek Greene, Neil Hurley.
 *
 *     This program is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 *
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License
 *     along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <getopt.h>
#include <math.h>
#include <limits.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <exception>

#include <algorithm>
#include <vector>
#include <set>
#include <tr1/unordered_map>
#include <tr1/unordered_set>

namespace extra = std::tr1;
// namespace extra = boost;


#include "aaron_utils.hpp"

#include "cmdline.h"


#define DEBUGBYDEFAULT


using namespace std;

/*
 * Examples of args
 * -z
 *  -f 1,2,3
 *  -a (optional arg)
 */

//exceptions
struct MissingFile {};
struct EmptyFile {};

void oNMI(const char * file1, const char * file2, const bool do_omega_also);

static int global_verbose_flag = 0;

int main(int argc, char ** argv) {
	// std::locale system_locale("");
	// std::cout.imbue(system_locale); // to get comma-separated integers.
	gengetopt_args_info args_info;
	if (cmdline_parser (argc, argv, &args_info) != 0)
		exit(1) ;
	if(args_info.inputs_num != 2) {
		cmdline_parser_print_help();
		exit(1);
	}
	if(args_info.verbose_flag) {
		global_verbose_flag = 1;
	}
	const char *file1 = args_info.inputs[0];
	const char *file2 = args_info.inputs[1];
	oNMI(file1, file2, args_info.omega_flag);
}

typedef std::string Node;
typedef std::vector< std::set< Node > > Grouping;
struct NodeToGroup : public
		     extra::unordered_map< Node, set<int> >
{
	int sharedGroups(const Node n_, const Node m_) const {
		// PP2(n_,m_);
		static const set<int> emptySet;

		const set<int> * nGrps;
		if(this->count(n_)==1)
			nGrps = &(this->find(n_)->second);
		else
			nGrps = &emptySet;
		// PP(nGrps->size());

		const set<int> * mGrps;
		if(this->count(m_)==1)
			mGrps = &(this->find(m_)->second);
		else
			mGrps = &emptySet;
		// PP(mGrps->size());

		vector<int> intersection;
		set_intersection(
				nGrps->begin(), nGrps->end(),
				mGrps->begin(), mGrps->end(),
				back_inserter(intersection)
				);
		const int inter = intersection.size();
		// PP(inter);

		assert(inter <= (int)nGrps->size());
		assert(inter <= (int)mGrps->size());

		return inter;
	}
};
struct OverlapMatrix {
	std::map< pair<int,int> , int> om; // the pair is an ordered pair.
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
		if(s.size()==0) {
			cerr << "Warning: ignoring empty sets in file: " << file << endl;
		} else {
			ss.push_back(s);
		}
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
	extra::unordered_set< Node > nodes;
	forEach(const NodeToGroup::value_type &n, amd::mk_range(ng2)) {
		nodes.insert(n.first);
	}
	forEach(const NodeToGroup::value_type &n, amd::mk_range(ng1)) {
		nodes.insert(n.first);
	}
	forEach(const Node &n, amd::mk_range(nodes)) {
		if(ng1.count(n)) forEach(const int g1, amd::mk_range(ng1.find(n)->second)) {
			if(ng2.count(n)) forEach(const int g2, amd::mk_range(ng2.find(n)->second)) {
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
double h (const double p) {
	if(p==0)
		return 0;
	assert(p>0);
	return -p*log2(p);
}
double H_X_given_Y (const int y, const int x, const int o, const int N) {
	// the NON-NORMALIZED mutual information
	// given sets of size 'l' and 'r', where there are 'o' nodes in common, what's their similarity score?
		assert(o>0 && y>=o && x>=o && y   <= N && x   <= N);
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
		// PP(h(Px0y0));
		// PP(h(Px1y1));
		// PP(h(Px0y1));
		// PP(h(Px1y0));
		// PP( h(Px0y0)+h(Px1y1) -h(Px0y1)-h(Px0y1) );
		if( h(Px0y0)+h(Px1y1) <= h(Px0y1)+h(Px1y0) )
			return H_X;
		const double H_XY =
			  H(N-x-y+o, N)
			+ H(x-o, N)
			+ H(y-o, N)
			+ H(o, N)
			;
		if(H_X < H_XY - H_Y) { // this shouldn't really happen, except by a roundoff error
			DYINGWORDS( VERYCLOSE(H_X , H_XY - H_Y)) {
				PP(H_X);
				PP(H_XY - H_Y);
				PP(H_X - (H_XY - H_Y));
			}
			return H_X;
		} else
			return H_XY - H_Y;
}

double HX_given_BestY (const OverlapMatrix &om, const Grouping &g1, const Grouping &g2, const int realxId) {
	// realxId is a community in g2.
	// X is g2, Y is g1
	// we're looking for the bits to encode X_realxId given all of Y
	const int sizeOfXComm = g2.at(realxId).size();
	double bestSoFar = H(sizeOfXComm,om.N) + H(om.N-sizeOfXComm,om.N);
	std::map< pair<int,int> ,int >::const_iterator            i = om.om.lower_bound(make_pair(realxId, INT_MIN));
	std::map< pair<int,int> ,int >::const_iterator   endOfRange = om.om.lower_bound(make_pair(realxId+1, INT_MIN));
	for(; i != endOfRange; ++           i)
	{
		int xId =  i->first.first;
		int yId =  i->first.second;
		assert(realxId == xId);
		const int overlap = i->second;
		const double H_XgivenY = H_X_given_Y(g1.at(yId).size(),g2.at(xId).size(), overlap, om.N);
		if(bestSoFar > H_XgivenY)
			bestSoFar = H_XgivenY;
		// cout << '\t'; PPt(g1.at(fromId).size());
		// PPt(g2.at(xId).size());
		// PPt(overlap);
		// PP(H_XgivenY);
	}
	return bestSoFar;
}

template<bool normalizeTooSoon>
double VI_oneSide (const OverlapMatrix &om, const Grouping &g1, const Grouping &g2) {
	// this doesn't return the (N)MI. It's the non-mutual information, optionally normalized too soon
	const int N = om.N;
	double total = 0.0;
	for(int toId = 0; toId < (int)g2.size(); toId++) {
		const double unnorm = HX_given_BestY(om, g1, g2, toId);
		if(normalizeTooSoon) {
			const int x = g2.at(toId).size();
			const double H_X = H(x,N) + H(N-x,N);
			const double norm = unnorm / H_X; // might be NaN
			if(H_X == 0.0) { // the communities take up the whole set of nodes, and hence won't need any bits to be encoded. No need to add anything to 'total'
				assert(unnorm == 0.0);

				// in this case norm is 0/0, but we'll just define this as 1 // This is the bugfix/ambiguityfix to make it the same as the LFK software
				total += 1.0;
			} else {
				unless(norm <= 1.01) {
					PP(x);
					PP(N);
					PP(H_X);
					PP(unnorm);
					PP(unnorm / H_X);
					PP(unnorm - H_X);
				}
				assert(norm <= 1.01);
				assert(norm >= -0.01);
				total += norm;
			}
		} else {
			total += unnorm;
		}
	}
	if(normalizeTooSoon) {
		return total / g2.size(); // why is this total zero? When it should be one in some cases.
	} else
		return total;
}
double LFKNMI(const OverlapMatrix &om, const OverlapMatrix &omFlipped, const Grouping &g1, const Grouping &g2) {
	return 1.0 - 0.5 *
		( VI_oneSide<true>(omFlipped, g1, g2)
		+ VI_oneSide<true>(om       , g2, g1) );
}
struct Max {
	double operator() (const double H_Xs, const double H_Ys) const {
		return H_Xs > H_Ys ? H_Xs : H_Ys;
	}
};
struct Sum {
	double operator() (const double H_Xs, const double H_Ys) const {
		return 0.5 * (H_Xs + H_Ys);
	}
};
struct Min {
	double operator() (const double H_Xs, const double H_Ys) const {
		return H_Xs > H_Ys ? H_Ys : H_Xs;
	}
};
template<class Combiner>
double aaronNMI(const OverlapMatrix &om, const OverlapMatrix &omFlipped, const Grouping &g1, const Grouping &g2) {
	double H_Xs = 0.0;
	for(int toId = 0; toId < (int)g2.size(); toId++) {
		const int x = g2.at(toId).size();
		H_Xs += H(x, om.N)+H(om.N-x, om.N);
	}
	double H_Ys = 0.0;
	for(int fromId = 0; fromId < (int)g1.size(); fromId++) {
		const int x = g1.at(fromId).size();
		H_Ys += H(x, om.N)+H(om.N-x, om.N);
	}
	return
		0.5*( H_Xs+H_Ys - VI_oneSide<false>(omFlipped, g1, g2) - VI_oneSide<false>(om, g2, g1) ) 
		/ Combiner()(H_Xs, H_Ys)
		;
}

typedef long long int lli;

pair<double,double> omega(const NodeToGroup &ng1, const NodeToGroup &ng2) {
	set<Node> nodes;
	for(NodeToGroup::const_iterator i = ng1.begin(); i!=ng1.end(); i++) { nodes.insert(i->first); }
	for(NodeToGroup::const_iterator i = ng2.begin(); i!=ng2.end(); i++) { nodes.insert(i->first); }

	vector<Node> nodesv;
	for(set<Node>::const_iterator i=nodes.begin(); i!=nodes.end(); i++) { nodesv.push_back(*i); }
	const int N=nodesv.size();

	map<int,lli> A;
	map< pair<int,int> ,int> B;
	map<int,lli> N_bottom;
	map<int,lli> N_side;

	int minJK = 0;
	lli sumOfSquares = 0;
	for(int n=0; n<N; n++) {
		for(int m=0; m<n; m++) {
			// PP2(n,m);
			const Node n_ = nodesv.at(n);
			const Node m_ = nodesv.at(m);
			// PP2(n_,m_);
			const int a = ng1.sharedGroups(n_,m_);
			// const int b = ng1.sharedGroups(m_,n_);
			const int c = ng2.sharedGroups(n_,m_);
			// const int d = ng2.sharedGroups(m_,n_);
			// assert(a==b);
			// assert(c==d);
			// PP4(a,b,c,d);
			// assert(a!=1 || d!=0);
			if(a==c)
				A[a]++;
			B[make_pair(a,c)]++;
			N_bottom[a]++;
			N_side  [c]++;
			if(minJK < a)
				minJK=a;
			if(minJK < c)
				minJK=c;


			{ // Latouches's L2 norm
				// v2(a-c, (a-c)*(a-c));
				sumOfSquares += (a-c)*(a-c);
			}
		}
	}
	v1(minJK);

	lli bigN = 0;
	forEach(const typeof(pair<int,int>) &Nj, amd::mk_range(N_bottom)) {
		bigN += Nj.second;
	}
	{ // verification
		/*
		forEach(const typeof(pair<int,int>) &Aj, amd::mk_range(A)) {
			PP2(Aj.first, Aj.second);
		}
		forEach(const typeof(pair< pair<int,int>,int>) &Bij, amd::mk_range(B)) {
			PP3(Bij.first.first, Bij.first.second, Bij.second);
		}
		forEach(const typeof(pair<int,int>) &Nj, amd::mk_range(N_bottom)) {
			// PP2(Nj.first, Nj.second);
		}
		*/
		int verifyNumPairs3 = 0;
		forEach(const typeof(pair<int,int>) &Nj, amd::mk_range(N_side  )) {
			// PP2(Nj.first, Nj.second);
			verifyNumPairs3 += Nj.second;
		}
		assert(bigN == N*(N-1)/2);
		assert(verifyNumPairs3 == N*(N-1)/2);
	}

	double L2norm = sqrt(sumOfSquares);

	lli numerator = 0;
	for(int j=0; j<=minJK; j++) {
		numerator += bigN * A[j];
		numerator -= N_bottom[j] * N_side[j];
		assert(numerator >= 0);
	}
	lli denominator = 0;
	denominator += bigN * bigN;
	for(int j=0; j<=minJK; j++) {
		denominator -= N_bottom[j] * N_side[j];
		assert(denominator >= 0);
	}
	v1(numerator);
	v1(denominator);
	double O = double(numerator) / double(denominator);
	return make_pair(O,L2norm);
}

void oNMI(const char * file1, const char * file2, const bool do_omega_also) {
	Grouping g1 = fileToSet(file1);
	Grouping g2 = fileToSet(file2);
	v1(g1.size());
	v1(g2.size());
	unless(g1.size() > 0) throw EmptyFile();
	unless(g2.size() > 0) throw EmptyFile();
	NodeToGroup n2g1 = nodeToGroup(g1);
	NodeToGroup n2g2 = nodeToGroup(g2);
	v1(n2g1.size());
	v1(n2g2.size());
	const OverlapMatrix om = overlapMatrix(n2g1, n2g2);

	OverlapMatrix omFlipped;
	omFlipped.N = om.N;
	forEach(typeof(pair< pair<int,int> ,int>) p, amd::mk_range(om.om)) {
		swap(p.first.first, p.first.second);
		bool wasInserted = omFlipped.om.insert(p).second;
		assert(wasInserted);
	}
	assert(omFlipped.om.size() == om.om.size());

	if(global_verbose_flag) {
		cout << "  \'" << file2 << "\' given \'" << file1 << "\"" << endl;
		for(int toId = 0; toId < (int)g2.size(); toId++) {
			PP(HX_given_BestY(omFlipped, g1, g2, toId));
		}
		cout << "  \'" << file1 << "\' given \'" << file2 << "\"" << endl;
		for(int fromId = 0; fromId < (int)g1.size(); fromId++) {
			PP(HX_given_BestY(om       , g2, g1, fromId));
		}
		cout << "Here:" << endl;
	}
	const double LFKnmi_ = LFKNMI(om, omFlipped, g1, g2);
	if(do_omega_also) {
		pair<double,double> OmegaAndL2Norm = omega(n2g1, n2g2);
		const double Omega = OmegaAndL2Norm.first;
		const double L2norm = OmegaAndL2Norm.second;
		cout << "Datum:\t"; PP(Omega);
		cout << "Datum:\t"; PP(L2norm);
	}
	cout << "NMI<Max>:\t"; cout << aaronNMI<Max>(om, omFlipped, g1, g2) << endl;
	cout << "Other measures:" << endl;
	cout << "  lfkNMI:\t"; cout << LFKnmi_ << endl;
	cout << "  NMI<Sum>:\t"; cout << aaronNMI<Sum>(om, omFlipped, g1, g2) << endl;
	// PP(aaronNMI<Min>(om, g1, g2)); This is awful :-)
}
