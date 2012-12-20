#ifndef _GLUONDIST_H_
#define _GLUONDIST_H_

#include "interp2d.h"

class GluonDistribution {
public:
    virtual double S2(double r2, double Qs2) = 0;
    virtual double S4(double r2, double s2, double t2, double Qs2) = 0;
    virtual double F(double q2, double Qs2) = 0;
};

class GBWGluonDistribution: public GluonDistribution {
public:
    double S2(double r2, double Qs2);
    double S4(double r2, double s2, double t2, double Qs2);
    double F(double q2, double Qs2);
};

class MVGluonDistribution: public GluonDistribution {
public:
    MVGluonDistribution(double LambdaMV, double q2min, double q2max, double Qs2min, double Qs2max);
    ~MVGluonDistribution();
    double S2(double r2, double Qs2);
    double S4(double r2, double s2, double t2, double Qs2);
    double F(double q2, double Qs2);
private:
    double LambdaMV;
    double q2min, q2max;
    double Qs2min, Qs2max;
    double* log_q2_values;
    double* log_Qs2_values;
    double* F_dist_leading_q2;
    double* F_dist_subleading_q2;
    double* F_dist;
    gsl_interp* interp_dist_leading_q2;
    gsl_interp* interp_dist_subleading_q2;
    interp2d* interp_dist;
    gsl_interp_accel* q2_accel;
    gsl_interp_accel* Qs2_accel;
#ifdef GLUON_DIST_DRIVER
public:
    void write_grid();
#endif
};

#endif // _GLUONDIST_H_