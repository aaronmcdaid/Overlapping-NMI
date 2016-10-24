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

#define PP(x) cout << #x << ":" << x << endl
#define PP2(x,y) cout << #x << ',' << #y << ":\t" << x << " , " << y << endl
#define PP3(x,y,z)             cout << #x << ',' << #y << ',' << #z                              << ":\t" << x << " , " << y << " , " << z << endl
#define PP4(x,y,z,w)           cout << #x << ',' << #y << ',' << #z << ',' << #w                 << ":\t" << x << " , " << y << " , " << z << " , " << w << endl

#define PP1_v(...) do { if(global_verbose_flag)  PP(__VA_ARGS__); } while(0)
#define PP2_v(...) do { if(global_verbose_flag) PP2(__VA_ARGS__); } while(0)

#define unless(x) if(!(x))

#define DYINGWORDS(x) for (int klsdjfslkfj = (x) ? 0 : 1; klsdjfslkfj!=0; klsdjfslkfj--, ({ assert (x); }) )

#define VERYCLOSE(a,b) (1e-10 > fabs((a)-(b)))

#endif
