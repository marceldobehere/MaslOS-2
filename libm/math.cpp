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
    while(x > 6.2831) x -= 6.2831;
    while(x < 0) x += 6.2831;
    return (double)(x - (pow(x, 3)/6) + (pow(x, 5)/120) - (pow(x, 7)/5040) + (pow(x, 9)/362880) - (pow(x,11)/39916800) + (pow(x,13)/6227020800) - (pow(x,15)/1307674368000) + (pow(x,17)/355687428096000));
}

double cos(double x)
{
    return sin(1.570796 - x);
}

double tan(double x)
{
    return sin(x)/cos(x);
}
