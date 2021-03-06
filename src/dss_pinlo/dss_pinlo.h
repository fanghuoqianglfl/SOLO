/*
 * A C++ interface to the DSS pion fragmentation functions.
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

#ifndef DSS_PINLO_H_INCLUDED
#define DSS_PINLO_H_INCLUDED

#include <exception>
#include <sstream>
#include "interp2d.h"

class FragmentationFunctionRangeException : public std::exception {
private:
    double _z, _mu2;
    std::string _message;
public:
    FragmentationFunctionRangeException(double z, double mu2) throw() :
        _z(z), _mu2(mu2) {
        std::ostringstream s;
        s << "Fragmentation function evaluated at " << z << "," << mu2 << " (out of range)";
        _message = s.str();
    }
    FragmentationFunctionRangeException(const FragmentationFunctionRangeException& e) throw() :
        _z(e._z), _mu2(e._mu2), _message(e._message) {
    }
    void operator=(const FragmentationFunctionRangeException& e) throw() {
        _z = e._z;
        _mu2 = e._mu2;
        _message = e._message;
    }
    virtual ~FragmentationFunctionRangeException() throw() {
    }
    double z() const {
        return _z;
    }
    double mu2() const {
        return _mu2;
    }
    const char* what() const throw() {
        return _message.c_str();
    }
};


/**
 * A fairly simple C++ interface to the DSS fragmentation functions.
 *
 * To use the class, construct an instance passing the name of the
 * file to read data from. It should have 11 columns of numeric data
 * (two plus the number of parton flavors). Then call update() to
 * set the values of z and Q_s^2, and then fragmentation() to read out
 * the value of the desired fragmentation function for the desired pion
 * at the current values of z and Q_s^2.
 */
class DSSpiNLO {
private:
    /** The number of parton flavors there are fragmentation functions for. */
    const static size_t number_of_flavors = 9;

    // for the interpolation
    size_t number_of_lnz_values;
    size_t number_of_lnqs2_values;

    double* lnz_array;
    double* lnqs2_array;
    double** ff_arrays;

    /** The name of the file data was read from */
    const char* m_filename;

    // Also for the interpolation. There's one interpolator for each FF.
    interp2d* interpolators[number_of_flavors];
    gsl_interp_accel* lnz_accel[number_of_flavors];
    gsl_interp_accel* lnqs2_accel[number_of_flavors];

    /** The current value of ln(z). */
    double lnz;
    /** The current value of ln(Q_s^2). */
    double lnqs2;
    /** The value of the pi+ fragmentation functions at the current z and Q_s^2. */
    double pi_plus_ff[number_of_flavors];
    /** The value of the pi- fragmentation functions at the current z and Q_s^2. */
    double pi_minus_ff[number_of_flavors];
    /** The value of the pi0 fragmentation functions at the current z and Q_s^2. */
    double pi_zero_ff[number_of_flavors];

public:
    /** Constants representing the parton flavors. */
    enum flavor {gluon, up, up_bar, down, down_bar, strange, strange_bar, charm, charm_bar};
    /** Constants representing the hadron types. */
    enum hadron {pi_plus, pi_zero, pi_minus};

    /** Construct an object, reading data from the given file. */
    DSSpiNLO(const char* filename);
    ~DSSpiNLO();
    /** Give the filename the object was constructed with. */
    const char* filename();
    /** Set the current values of z and Q_s^2. */
    void update(double z, double qs2);
    /** Get the value of a fragmentation function at the current z and Q_s^2. */
    double fragmentation(flavor f, hadron h);
};

#endif