#pragma once

struct Point
{
    int x;
    int y;
};


struct MPoint
{
    int x;
    int y;

    MPoint(int x, int y);
    MPoint();

    bool operator==(const MPoint& other);
    bool operator!=(const MPoint& other);
};

// inline int min(int a, int b);
// inline int max(int a, int b);

inline int min(int a, int b)
{
    if (a <= b)
        return a;
    return b;
}

inline int max(int a, int b)
{
    if (a >= b)
        return a;
    return b;
}

inline int abs(int val)
{
    if (val < 0)
        return -val;
    return val;
}

double pow(double x, int n);
double sin(double x);
double cos(double x);
double tan(double x);
double csc(double x);
double sec(double x);
double ctan(double x);
double Asin(double x);
double Acos(double x);
double Atan(double x);
double approx_derivative(double (*f)(double), double point, double precision);
double approx_integral(double (*f)(double), double a, double b, double precision);
