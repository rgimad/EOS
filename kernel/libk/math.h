#ifndef _LIBK_MATH_H_
#define _LIBK_MATH_H_

#define fabs(x) ((x<0) ? -x : x)
#define abs(x)  ((x<0) ? -x : x)

int pow(int, int);
double sqrt(double);

//double percentage_relation(double, double);

int floor(double);
int ceil(double);

//long double ldexp(long double, int);
/*double sin(double);
double cos(double);
double asin(double);
double acos(double);
double tan(double);*/

int sign(double a);

#define isnan(in) ((in)!=(in))
#define isinf(in) ((in)==(in-1))

#endif
