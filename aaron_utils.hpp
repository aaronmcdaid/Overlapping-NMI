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

#define unless(x) if(!(x))

#define DYINGWORDS(x) for (int klsdjfslkfj = (x) ? 0 : 1; klsdjfslkfj!=0; klsdjfslkfj--, ({ assert (x); }) )

#define VERYCLOSE(a,b) (1e-10 > fabs((a)-(b)))

#endif
