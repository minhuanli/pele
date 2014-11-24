#ifndef _PELE_DISTANCE_H
#define _PELE_DISTANCE_H

#include <cmath>
#include <stdexcept>
#include <type_traits>

#include "array.h"

// roundis missing in visual studio
#ifdef _MSC_VER
    inline double round(double r) {
        return (r > 0.0) ? floor(r + 0.5) : ceil(r - 0.5);
    }
#endif

/**
 * These classes and structs are used by the potentials to compute distances.
 * They must have a member function get_rij() with signature
 *
 *    void get_rij(double * r_ij, double const * const r1, 
 *                                double const * const r2) 
 *
 * Where r1 and r2 are the position of the two atoms and r_ij is an array of
 * size 3 which will be used to return the distance vector from r1 to r2.
 * 
 * References:
 * Used here: general reference on template meta-programming and recursive template functions:
 * http://www.itp.phys.ethz.ch/education/hs12/programming_techniques
 */

namespace pele
{

/**
* compute the cartesian distance
*/
template<size_t IDX>
struct meta_dist {
    static void f(double * const r_ij, double const * const r1,
            double const * const r2)
    {
        const static size_t k = IDX - 1;
        r_ij[k] = r1[k] - r2[k];
        return meta_dist<k>::f(r_ij, r1, r2);
    }
};

template<>
struct meta_dist<1> {
    static void f(double * const r_ij, double const * const r1,
            double const * const r2)
    {
        r_ij[0] = r1[0] - r2[0];
    }
};  

template<size_t ndim>
struct cartesian_distance {
    static const size_t _ndim = ndim;
    void get_rij(double * const r_ij, double const * const r1,
            double const * const r2) const
    {
        static_assert(ndim > 0, "illegal box dimension");
        return meta_dist<ndim>::f(r_ij, r1, r2);
    }
};

/**
* periodic boundary conditions in rectangular box
*/

template<size_t IDX>
struct meta_periodic_distance {
    static void f(double * const r_ij, double const * const r1,
                 double const * const r2, const double* _box, const double* _ibox)
    {
        const static size_t k = IDX - 1;
        r_ij[k] = r1[k] - r2[k];
        r_ij[k] -= round(r_ij[k] * _ibox[k]) * _box[k];
        return meta_periodic_distance<k>::f(r_ij, r1, r2, _box, _ibox);
    } 
};

template<>
struct meta_periodic_distance<1> {
    static void f(double * const r_ij, double const * const r1,
                 double const * const r2, const double* _box, const double* _ibox)
    {
        r_ij[0] = r1[0] - r2[0];
        r_ij[0] -= round(r_ij[0] * _ibox[0]) * _box[0];
    }
};

template<size_t IDX>
struct meta_image {
    static void f(double*const& x, const double* _ibox, const double* _box)
    {
        const static size_t k = IDX - 1;
        x[k] -= round(x[k] * _ibox[k]) * _box[k];
    }
};

template<>
struct meta_image<1> {
    static void f(double*const& x, const double* _ibox, const double* _box)
    {
        x[0] -= round(x[0] * _ibox[0]) * _box[0];
    }
};

template<size_t ndim>
class periodic_distance {
public:
    static const size_t _ndim = ndim;
    double _box[ndim];
    double _ibox[ndim];

    periodic_distance(Array<double> const box)
    {
        if (box.size() != _ndim) {
            throw std::invalid_argument("box.size() must be equal to ndim");
        }
        for (size_t i=0;i<ndim;++i) {
            _box[i] = box[i];
            _ibox[i] = 1/box[i];
        }
    }

    periodic_distance()
    { 
        throw std::runtime_error("the empty constructor is not available for periodic boundaries"); 
    }

    inline void get_rij(double * const r_ij, double const * const r1,
                 double const * const r2) const
    {
        static_assert(ndim > 0, "illegal box dimension");
        return meta_periodic_distance<ndim>::f(r_ij, r1, r2, _box, _ibox);
    }

    inline void put_in_box(Array<double>& coords) const
    {
        size_t natoms = coords.size()/_ndim;
        for (size_t i=0;i<natoms;++i){
            size_t i1 = i*_ndim;
            static_assert(ndim > 0, "illegal box dimension");
            return meta_image<ndim>::f(&coords[i1], _ibox, _box);
        }
    }
};

/*
 * Interface classes to pass a generic non template pointer to DistanceInterface
 * (this should reduce the need for templating where best performance is not essential)
 */

class DistanceInterface{
protected:
public:
    virtual void get_rij(double * const r_ij, double const * const r1, double const * const r2) const =0;
    virtual ~DistanceInterface(){ }
};

template<size_t ndim>
class CartesianDistanceWrapper : public DistanceInterface{
protected:
    cartesian_distance<ndim> _dist;
public:
    static const size_t _ndim = ndim;
    inline void get_rij(double * const r_ij, double const * const r1,
            double const * const r2) const
    {
        _dist.get_rij(r_ij, r1, r2);
    }
};

template<size_t ndim>
class PeriodicDistanceWrapper : public DistanceInterface{
protected:
    periodic_distance<ndim> _dist;
public:
    static const size_t _ndim = ndim;
    PeriodicDistanceWrapper(Array<double> const box)
        : _dist(box)
    {};

    inline void get_rij(double * const r_ij, double const * const r1,
            double const * const r2) const
    {
        _dist.get_rij(r_ij, r1, r2);
    }
};

}
#endif
