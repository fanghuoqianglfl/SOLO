/*
 * Part of oneloopcalc
 * 
 * Copyright 2012 David Zaslavsky
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _GLUONDIST_H_
#define _GLUONDIST_H_

#include <string>
#include "interp2d.h"

/**
 * Converter between x, Y, and Qs2.
 */
class SaturationScale {
public:
    SaturationScale(const double Q02, const double x0, const double lambda);
    /**
     * Converts a value of Y to a value of x.
     */
    virtual double xY(const double Y) const;
    /**
     * Converts a value of x to a value of Y.
     */
    virtual double Yx(const double x) const;
    /**
     * Return the saturation scale corresponding to the given value of x.
     */
    virtual double Qs2x(const double x) const;
    /**
     * Return the saturation scale corresponding to the given value of Y.
     */
    virtual double Qs2Y(const double Y) const;
private:
    const double Q02x0lambda;
    const double lambda;
};

/**
 * A gluon distribution.
 */
class GluonDistribution {
public:
    GluonDistribution(const SaturationScale& satscale) : satscale(satscale) {};
    /**
     * Return the value of the dipole gluon distribution at the given
     * values of r^2 and Y. r2 is the squared magnitude of the
     * dipole displacement vector x - y (equivalently, r2 is the
     * squared dipole size).
     */
    virtual double S2(double r2, double Y) = 0;
    /**
     * Return the value of the quadrupole gluon distribution at the given
     * values of r^2, s^2, t^2, and Y. r2 is the squared magnitude of
     * x - y, s2 is the squared magnitude of x - b, and t2 is the squared
     * magnitude of y - b, so that as vectors, r = s - t. r2, s2, and t2
     * thus are the squared lengths of three sides of a triangle.
     */
    virtual double S4(double r2, double s2, double t2, double Y) = 0;
    /**
     * Return the value of the momentum-space dipole gluon distribution
     * at the given values of q^2 and Y.
     */
    virtual double F(double q2, double Y) = 0;
    /**
     * Returns a human-readable name for the gluon distribution.
     */
    virtual const char* name() = 0;
    /**
     * The object that computes the saturation scale.
     */
    const SaturationScale& satscale;
};

/**
 * The GBW gluon distribution.
 */
class GBWGluonDistribution: public GluonDistribution {
public:
    GBWGluonDistribution(const SaturationScale& satscale);
    /**
     * Returns the value of the GBW dipole gluon distribution,
     * exp(-r2 Qs2 / 4)
     */
    double S2(double r2, double Y);
    /**
     * Returns the value of the GBW quadrupole gluon distribution,
     * exp(-s2 Qs2 / 4) exp(-t2 Qs2 / 4)
     * This is just a product of two dipole distributions (valid
     * in the large-Nc limit)
     */
    double S4(double r2, double s2, double t2, double Y);
    /**
     * Returns the value of the GBW momentum space dipole gluon
     * distribution, exp(-q2 / Qs2) / (pi * Qs2)
     */
    double F(double q2, double Y);
    const char* name();
};

/**
 * An abstract gluon distribution which has only a position space
 * definition, and the values in momentum space are computed
 * automatically.
 * 
 * This is a base class for gluon distributions which have no
 * analytic expression or other direct definition in momentum
 * space. Only the position space data is available - that is,
 * for when you have S(r, Qs) but not F(k, Qs). This means the
 * value of the distribution in momentum space has to be determined
 * numerically.
 * 
 * This implementation establishes a 2D grid in ln(q2) and ln(Qs2)
 * and computes the value of the distribution at the grid points
 * via a numerical integral. When F(q2, Qs2) is called, if the
 * values of q2 and Qs2 are within the boundaries of the grid,
 * the value of the distribution F is computed by 2D interpolation.
 * If q2 is smaller than the lower boundary of the grid, then
 * the value of the distribution F is computed from a series
 * expansion around q2 = 0. The series coefficients are
 * interpolated in Qs2 only.
 */
class AbstractPositionGluonDistribution : public GluonDistribution {
public:
    /**
     * Constructs a new position gluon distribution object.
     * 
     * `q2min`, `q2max`, `Ymin`, and `Ymax` specify the boundaries
     * of the region in which to interpolate the momentum space
     * distribution. The grid will be set up using these boundaries,
     * with a spacing automatically chosen to be reasonably accurate.
     * The series expansion used for `q2` < `q2min` is accurate to three
     * digits up to around `q2` = 1e-3 or so, but it's probably safer
     * to pass `q2min` around 1e-6. The other limits should be chosen
     * to include the range of values that will be needed.
     * 
     * `subinterval_limit` specifies the maximum number of subdivisions
     * used in computing the numeric integrals. Pass a larger value for
     * extreme parameters if the program crashes with a subdivision
     * error.
     */
    AbstractPositionGluonDistribution(const SaturationScale& satscale, double q2min, double q2max, double Ymin, double Ymax, size_t subinterval_limit = 10000);
    ~AbstractPositionGluonDistribution();

    /**
     * Returns the value of the position space quadrupole gluon distribution,
     * by default computed as the product of two dipole gluon distributions,
     * S2(s2, Qs2) * S2 (t2, Qs2). This is usually valid in the large-Nc
     * limit.
     */
    virtual double S4(double r2, double s2, double t2, double Y);
    /**
     * Returns the value of the MV momentum space dipole gluon
     * distribution.
     */
    double F(double q2, double Y);
    /**
     * Returns the name of the distribution, which should incorporate
     * the values of the parameters.
     */
    virtual const char* name() = 0;

protected:
    /**
     * Handles the actual calculation of the points to use for interpolation.
     * This should be called from the constructor of each subclass that
     * implements S2.
     */
    void setup();
    
private:
    double q2min, q2max;
    double Ymin, Ymax;
    /** Values of ln(q2) for the interpolation. */
    double* log_q2_values;
    /** Values of Y for the interpolation. */
    double* Y_values;
    /** Values of the leading coefficient in the series for small q2. */
    double* F_dist_leading_q2;
    /** Values of the subleading coefficient in the series for small q2. */
    double* F_dist_subleading_q2;
    /** Values of the gluon distribution for interpolation when q2 > q2min. */
    double* F_dist;
    
    // Structures used to do the interpolation
    gsl_interp* interp_dist_leading_q2;
    gsl_interp* interp_dist_subleading_q2;
    // one of interp_dist_momentum_1D or interp_dist_momentum_2D will be NULL and the other one
    // will be used, depending on whether there is a range of Y values or
    // just a single one
    gsl_interp* interp_dist_momentum_1D;
    interp2d* interp_dist_momentum_2D;
    
    gsl_interp_accel* q2_accel;
    gsl_interp_accel* Y_accel;
    
    size_t q2_dimension;
    size_t Y_dimension;
    
    size_t subinterval_limit;
#ifdef GLUON_DIST_DRIVER
public:
    /**
     * This writes out the entire grid of the 2D interpolation to standard output.
     * 
     * Only used for testing.
     */
    void write_pspace_grid(std::ostream&);
#endif
};

/**
 * The MV gluon distribution.
 * 
 * The position space dipole distribution takes the form
 * exp(-(r2 Qs02MV)^gammaMV ln(e + 1 / (LambdaMV r)) / 4)
 * and the position space quadrupole distribution is computed
 * as a product of two factors of that form.
 */
class MVGluonDistribution: public AbstractPositionGluonDistribution {
public:
    /**
     * Constructs a new MV gluon distribution object.
     */
    MVGluonDistribution(const SaturationScale& satscale, double LambdaMV, double gammaMV, double q2min, double q2max, double Ymin, double Ymax, size_t subinterval_limit = 10000);
    ~MVGluonDistribution() {};
    
    /**
     * Returns the value of the MV dipole gluon distribution,
     * exp(-(r2 Qs02MV)^gammaMV ln(e + 1 / (LambdaMV r)) / 4)
     */
    double S2(double r2, double Y);
    /**
     * Returns the name of the distribution, which incorporates
     * the values of the parameters.
     */
    const char* name();
protected:
    double LambdaMV;
    double gammaMV;
    
    std::string _name;
};

/**
 * A modified version of the MV gluon distribution which ignores the actual
 * saturation scale and uses a fixed value instead.
 */
class FixedSaturationMVGluonDistribution : public MVGluonDistribution {
public:
    /**
     * Constructs a new modified gluon distribution object.
     */
    FixedSaturationMVGluonDistribution(const SaturationScale& satscale, double LambdaMV, double gammaMV, double q2min, double q2max, double YMV, size_t subinterval_limit = 10000);
    ~FixedSaturationMVGluonDistribution() {};
    
    /**
     * Returns the value of the dipole gluon distribution,
     * exp(-(r2 Qs02)^gammaMV ln(e + 1 / (LambdaMV r)) / 4)
     * 
     * The parameter Y is not used.
     */
    double S2(double r2, double Y);
protected:
    double YMV;
};

class FileDataGluonDistribution : public GluonDistribution {
public:
    /**
     * Constructs a new gluon distribution reading from the specified file.
     */
    FileDataGluonDistribution(const SaturationScale& satscale, std::string pos_filename, std::string mom_filename, double xinit);
    ~FileDataGluonDistribution();
    
    /**
     * Returns the interpolated value of the gluon distribution at the specified values.
     */
    double S2(double r2, double Y);
    
    double S4(double r2, double s2, double t2, double Y);
    
    double F(double q2, double Y);
    
    /**
     * Returns the name of the gluon distribution.
     */
    const char* name();
    
protected:
    /**
     * Calculates the interpolation in position space.
     */
    void calculate_position_interpolation();
private:
    /** Values of ln(r2) for the interpolation. */
    double* r2_values;
    /** Values of Y for the position interpolation. */
    double* Y_values_rspace;
    /** Values of ln(q2) for the interpolation. */
    double* q2_values;
    /** Values of Y for the position interpolation. */
    double* Y_values_pspace;
    /** Values of the position space gluon distribution for interpolation. */
    double* S_dist;
    /** Values of the momentum space gluon distribution for interpolation. */
    double* F_dist;
    
    // one of interp_dist_momentum_1D or interp_dist_momentum_2D will be NULL and the other one
    // will be used, depending on whether there is a range of Qs2 values or just a single one
    gsl_interp* interp_dist_momentum_1D;
    interp2d* interp_dist_momentum_2D;
    // same for position
    gsl_interp* interp_dist_position_1D;
    interp2d* interp_dist_position_2D;
    
    gsl_interp_accel* r2_accel;
    gsl_interp_accel* q2_accel;
    gsl_interp_accel* Y_accel_r;
    gsl_interp_accel* Y_accel_p;
    
    size_t r2_dimension;
    size_t q2_dimension;
    size_t Y_dimension_r;
    size_t Y_dimension_p;
    
    size_t subinterval_limit;
    
    std::string _name;
#ifdef GLUON_DIST_DRIVER
public:
    /**
     * This writes out the entire grid of the 2D momentum space interpolation to standard output.
     * 
     * Only used for testing.
     */
    void write_pspace_grid(std::ostream&);
    /**
     * This writes out the entire grid of the 2D position space interpolation to standard output.
     * 
     * Only used for testing.
     */
    void write_rspace_grid(std::ostream&);
#endif
};

/** Prints the name of the gluon distribution to the given output stream. */
std::ostream& operator<<(std::ostream& out, GluonDistribution& gdist);

/**
 * A wrapper object that prints out every call made to the gluon distribution.
 */
class GluonDistributionTraceWrapper : public GluonDistribution {
public:
    GluonDistributionTraceWrapper(GluonDistribution* gdist, const char* trace_filename = "trace_gdist.output") : GluonDistribution(gdist->satscale), gdist(gdist), trace_stream(trace_filename) {}
    ~GluonDistributionTraceWrapper() { delete gdist; trace_stream.close(); }

    double S2(double r2, double Y);
    double S4(double r2, double s2, double t2, double Y);
    double F(double q2, double Y);
    const char* name();
    
private:
    GluonDistribution* gdist;
    std::ofstream trace_stream;
};


#endif // _GLUONDIST_H_