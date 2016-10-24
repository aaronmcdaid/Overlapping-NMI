#ifndef _AARON_UTILS_H
#define _AARON_UTILS_H

#include <cassert>

#define PP(x)                  do{ std::cout << #x                                                        << ":"   << x                                           << std::endl ;}while(0)
#define PP2(x,y)               do{ std::cout << #x << ',' << #y                                           << ":\t" << x << " , " << y                             << std::endl ;}while(0)
#define PP3(x,y,z)             do{ std::cout << #x << ',' << #y << ',' << #z                              << ":\t" << x << " , " << y << " , " << z               << std::endl ;}while(0)
#define PP4(x,y,z,w)           do{ std::cout << #x << ',' << #y << ',' << #z << ',' << #w                 << ":\t" << x << " , " << y << " , " << z << " , " << w << std::endl ;}while(0)

#define PP1_v(...) do { if(global_verbose_flag)  PP(__VA_ARGS__); } while(0)
#define PP2_v(...) do { if(global_verbose_flag) PP2(__VA_ARGS__); } while(0)

#define unless(x) if(!(x))

#define DYINGWORDS(x) for (int klsdjfslkfj = (x) ? 0 : 1; klsdjfslkfj!=0; klsdjfslkfj--, ({ assert (x); }) )

#define VERYCLOSE(a,b) (1e-10 > fabs((a)-(b)))

#endif
