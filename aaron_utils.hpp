#ifndef _AARON_UTILS_H
#define _AARON_UTILS_H

//#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <assert.h>
#include <stdlib.h>
#include <sys/time.h>
#include <map>
#include <vector>

using namespace std;

typedef long long int64;
typedef unsigned long long uint64;
typedef unsigned int uint;

enum {FALSE=0, TRUE=1};
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define P(...) printf(__VA_ARGS__)
#define Pf(f, ...) fprintf(f, __VA_ARGS__)
#define Perror(...) fprintf(stderr, __VA_ARGS__)
#define Pn(...) do { P(__VA_ARGS__); fputc('\n', stdout); } while (0)
#define Perrorn(...) do { Perror(__VA_ARGS__); fputc('\n', stderr); } while (0)
//#define PP(x) Pn("%s:%s", #x, show(x).c_str())
#define PP(x) cout << #x << ":" << x << endl
#define PPt(x) cout << #x << ":" << x << '\t'
#define PPnn(x) cout << #x << ":" << x
#define PP2(x,y) cout << #x << ',' << #y << ":\t" << x << " , " << y << endl
#define PP3(x,y,z)             cout << #x << ',' << #y << ',' << #z                              << ":\t" << x << " , " << y << " , " << z << endl
#define PP4(x,y,z,w)           cout << #x << ',' << #y << ',' << #z << ',' << #w                 << ":\t" << x << " , " << y << " , " << z << " , " << w << endl
#define PPLg(x) Pn("%s:%20.11Lg", #x, x)
std::string thousandsSeparated(uint64 x);
#define PPdec(x) cout << #x << ":" << thousandsSeparated(x) << endl
#define PPhex(x) cout << #x << ":" << std::hex << std::setw(20) << x << std::dec << endl
#define Print(x)  P("%s", show(x).c_str())
#define Printn(x) P("%s\n", show(x).c_str())
#define v1(...) do { if(global_verbose_flag)  PP(__VA_ARGS__); } while(0)
#define v2(...) do { if(global_verbose_flag) PP2(__VA_ARGS__); } while(0)
#define v3(...) do { if(global_verbose_flag) PP3(__VA_ARGS__); } while(0)
enum {
	EXIT_UNSPECIFIED=1  // other (unspecified) error. Try to avoid using this
	, EXIT_FILEOPEN     // child process failed because it couldn't open the requested file
	, EXIT_CHILD     // other error in the child process, e.g. execl failed
	};
#define Die(...) ( ({ Perrorn(__VA_ARGS__); exit(EXIT_UNSPECIFIED); 0; }) )
#define orDie(x ,errorCond ,msg) ({ typeof(x) y = (x); y!=errorCond || Die(msg); y; })
#define pipe_orDie(filedes)            orDie(pipe(filedes) ,-1 ,"Couldn't pipe(2)")
#define fork_orDie()                   orDie(fork()        ,-1 ,"Couldn't fork(2)")
#define orDie1(x) orDie(x, -1, "died @ " __FILE__ ":" TOSTRING(__LINE__) "\t(" #x ")")
#define orExitChild(x) ({ typeof(x) y = (x); if (testForError(y)) exit(EXIT_CHILD); y; })

int testForError(FILE * x);
int testForError(int x);

std::string show(int64 x);
std::string show(const char * x);
std::string show(const std::string &x);

#define unless(x) if(!(x))


struct DummyOutputStream {
	DummyOutputStream& operator << (int);
	DummyOutputStream& operator << (const char*);
};
extern DummyOutputStream dummyOutputStream;

#define assertEQint(x,y) do { unless((x)==(y)) { PP(x); PP(y); } assert((x)==(y)); } while(0)
#define DYINGWORDS(x) for (int klsdjfslkfj = (x) ? 0 : 1; klsdjfslkfj!=0; klsdjfslkfj--, ({ assert (x); }) )
#define UNUSED __attribute__ ((__unused__))

namespace amd {

istream *zcatThis(const char *gzippedFile);
bool fileExists(const char *);


struct NotImplemented {
};

struct FormatFlagStack {
	std :: vector< ios_base :: fmtflags> the_stack;
	std :: vector< std      :: streamsize > the_stack_of_precision; // setprecision(...)
	struct PushT {
		FormatFlagStack * const parent_stack;
		PushT (FormatFlagStack *parent_stack_);
	} push;
	struct PopT {
		FormatFlagStack * const parent_stack;
		PopT  (FormatFlagStack *parent_stack_);
	} pop;
	FormatFlagStack();
	void do_push(ostream &str);
	void do_pop(ostream &str);
};
ostream & operator<< (ostream &str, const FormatFlagStack :: PushT & pusher);
ostream & operator<< (ostream &str, const FormatFlagStack :: PopT  & pusher);

} // namespace amd

#define VERYCLOSE(a,b) (1e-10 > fabs((a)-(b)))

#define printfstring(...) ({ char str[1000]; sprintf(str, __VA_ARGS__) ; (std::string (str)); })

#endif
