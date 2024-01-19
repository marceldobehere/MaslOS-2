#include "math.h"

// inline int min(int a, int b)
// {
//     if (a <= b)
//         return a;
//     return b;
// }

// inline int max(int a, int b)
// {
//     if (a >= b)
//         return a;
//     return b;
// }

const double MATH_PI = 3.14159265;
const double MATH_EULER = 2.71828;

MPoint::MPoint(int x, int y)
{
    this->x = x;
    this->y = y;
}

MPoint::MPoint()
{
    this->x = 0;
    this->y = 0;
}

bool MPoint::operator==(const MPoint& other)
{
    return (this->x == other.x && this->y == other.y);
}

bool MPoint::operator!=(const MPoint& other)
{
    return (this->x != other.x || this->y != other.y);
}

double pow(double x, int n)
{
    double val = 1;
    for(int i = 0; i < n; i++) val*=x;
    return val;
}

double sin(double x)
{
    double res;
    __asm__("fsin" : "=t"(res) : "0"(x));
    return res;
}

double cos(double x)
{
    double res;
    __asm__("fcos" : "=t"(res) : "0"(x));
    return res;
}

double tan(double x)
{
    return sin(x)/cos(x);
}

double csc(double x)
{
    return 1/sin(x);
}

double sec(double x)
{
    return 1/cos(x);
}

double ctan(double x)
{
    return 1/tan(x);
}

double Asin(double x)
{
    return (double)(x + (pow(x, 3))/6 + (3 * pow(x, 5))/40 + (5 * pow(x, 7))/112 + (35 * pow(x, 9))/1152);
}

double Acos(double x)
{
    return (1.570796 - Asin(x));
}

double Atan(double x)
{
    return (double)(x - (pow(x, 3)/3) + (pow(x, 5)/5) - (pow(x, 7)/7) + (pow(x, 9)/9) - (pow(x,11)/11) + (pow(x,13)/13) - (pow(x,15)/15) + (pow(x,17)/17));
}

double approx_derivative(double (*f)(double), double point, double precision)
{
    double diff = (*f)(point) - (*f)(point+(0.01/precision));
    return (diff * precision)/0.01;
}

double approx_integral(double (*f)(double), double a, double b, double precision)
{
    double length = (a-b)/(100*precision);
    double RiemannSum = 0;
    for(double i = a; i < b; i+=length) RiemannSum += length * (*f)(i);
    return RiemannSum;
}