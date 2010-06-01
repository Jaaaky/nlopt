/* Copyright (c) 2007-2010 Massachusetts Institute of Technology
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 
 */

// C++ style wrapper around NLopt API
// nlopt.hpp is AUTOMATICALLY GENERATED from nlopt-in.hpp - edit the latter!

#include <nlopt.h>

#include <vector>
#include <stdexcept>
#include <new>

// convenience overloading for below (not in nlopt:: since has nlopt_ prefix)
inline nlopt_result nlopt_get_initial_step(const nlopt_opt opt, double *dx) {
      return nlopt_get_initial_step(opt, (const double *) NULL, dx);
}

namespace nlopt {

  //////////////////////////////////////////////////////////////////////
  // nlopt::* namespace versions of the C enumerated types
  //          AUTOMATICALLY GENERATED, DO NOT EDIT
  // GEN_ENUMS_HERE
  enum algorithm {
     GN_DIRECT = 0,
     GN_DIRECT_L,
     GN_DIRECT_L_RAND,
     GN_DIRECT_NOSCAL,
     GN_DIRECT_L_NOSCAL,
     GN_DIRECT_L_RAND_NOSCAL,
     GN_ORIG_DIRECT,
     GN_ORIG_DIRECT_L,
     GD_STOGO,
     GD_STOGO_RAND,
     LD_LBFGS_NOCEDAL,
     LD_LBFGS,
     LN_PRAXIS,
     LD_VAR1,
     LD_VAR2,
     LD_TNEWTON,
     LD_TNEWTON_RESTART,
     LD_TNEWTON_PRECOND,
     LD_TNEWTON_PRECOND_RESTART,
     GN_CRS2_LM,
     GN_MLSL,
     GD_MLSL,
     GN_MLSL_LDS,
     GD_MLSL_LDS,
     LD_MMA,
     LN_COBYLA,
     LN_NEWUOA,
     LN_NEWUOA_BOUND,
     LN_NELDERMEAD,
     LN_SBPLX,
     LN_AUGLAG,
     LD_AUGLAG,
     LN_AUGLAG_EQ,
     LD_AUGLAG_EQ,
     LN_BOBYQA,
     GN_ISRES,
     NUM_ALGORITHMS /* not an algorithm, just the number of them */
  };
  enum result {
     FAILURE = -1, /* generic failure code */
     INVALID_ARGS = -2,
     OUT_OF_MEMORY = -3,
     ROUNDOFF_LIMITED = -4,
     FORCED_STOP = -5,
     SUCCESS = 1, /* generic success code */
     STOPVAL_REACHED = 2,
     FTOL_REACHED = 3,
     XTOL_REACHED = 4,
     MAXEVAL_REACHED = 5,
     MAXTIME_REACHED = 6
  };
  // GEN_ENUMS_HERE
  //////////////////////////////////////////////////////////////////////

  // virtual base class for objective function and constraints:
  class func {
  public:
    // should return function value, and set grad to gradient
    // (x and grad are length n)
    virtual double operator()(unsigned n, const double *x, double *grad) = 0;

    // should return function value (x is length n)
    virtual double operator()(unsigned n, const double *x) = 0;
  };

  // alternative that uses std::vector<double> for the arguments; note
  // that this is less efficient as it requires a copy to be made
  class vfunc {
  public:
    // should return function value, and set grad to gradient
    // (x and grad are same length n, and length of grad must not be changed)
    virtual double operator()(const std::vector<double> &x, 
			      std::vector<double> &grad) = 0;

    // should return function value (x is length n)
    virtual double operator()(const std::vector<double> &x) = 0;
  };

  //////////////////////////////////////////////////////////////////////
  
  // NLopt-specific exceptions (corresponding to error codes):
  class roundoff_limited : public std::runtime_error {
  public:
    roundoff_limited() : std::runtime_error("nlopt roundoff-limited") {}
  };

  class forced_stop : public std::runtime_error {
  public:
    forced_stop() : std::runtime_error("nlopt forced stop") {}
  };

  //////////////////////////////////////////////////////////////////////

  class opt {
  private:
    nlopt_opt o;
    
    void mythrow(nlopt_result ret) const {
      switch (ret) {
      case NLOPT_FAILURE: throw std::runtime_error("nlopt failure");
      case NLOPT_OUT_OF_MEMORY: throw std::bad_alloc();
      case NLOPT_INVALID_ARGS: throw std::invalid_argument("nlopt invalid argument");
      case NLOPT_ROUNDOFF_LIMITED: throw roundoff_limited();
      case NLOPT_FORCED_STOP: throw forced_stop();
      default: break;
      }
    }

    // nlopt_func wrapper around C++ "functional"
    static double myfunc(unsigned n, const double *x, double *grad, void *f_) {
      func *f = reinterpret_cast<func*>(f_);
      return grad ? (*f)(n, x, grad) : (*f)(n, x);
    }

    // nlopt_func wrapper around C++ "functional", using std::vector<double>
    static double myvfunc(unsigned n, const double *x, double *grad, void *f_){
      func *f = reinterpret_cast<vfunc*>(f_);
      std::vector<double> xv(n);
      for (unsigned i = 0; i < n; ++i) xv[i] = x[i];
      if (grad) {
	std::vector<double> gradv(n);
	double val = (*f)(xv, gradv);
	for (unsigned i = 0; i < n; ++i) grad[i] = gradv[i];
	return val;
      }
      else
	return (*f)(xv);
    }

  public:
    // Constructors etc.
    opt() : o(NULL) {}
    opt(nlopt_algorithm a, unsigned n) : o(nlopt_create(a, n)) {
      if (!o) throw std::bad_alloc();
    }
    opt(algorithm a, unsigned n) : o(nlopt_create(nlopt_algorithm(a), n)) {
      if (!o) throw std::bad_alloc();
    }
    opt(const nlopt_opt o0) : o(nlopt_copy(o0)) {
      if (o0 && !o) throw std::bad_alloc();
    }
    ~opt() { nlopt_destroy(o); }
    opt(const opt& from) : o(nlopt_copy(from.o)) {
      if (from.o && !o) throw std::bad_alloc();
    }
    opt& operator=(opt const& f) {
      if (this == &f) return *this; // self-assignment
      nlopt_destroy(o);
      o = nlopt_copy(f.o);
      if (f.o && !o) throw std::bad_alloc();
      return *this;
    }

    // Do the optimization:
    result optimize(double *x, double &opt_f) {
      nlopt_result ret = nlopt_optimize(o, x, &opt_f);
      mythrow(ret);
      return result(ret);
    }
    result optimize(std::vector<double> &x, double &opt_f) {
      if (o && nlopt_get_dimension(o) != x.size())
        throw std::invalid_argument("dimension mismatch");
      return optimize(x.empty() ? NULL : &x[0], opt_f);
    }

    // accessors:
    algorithm get_algorithm() const {
      if (!o) throw std::runtime_error("uninitialized nlopt::opt");
      return algorithm(nlopt_get_algorithm(o));
    }
    const char *get_algorithm_name() const {
      if (!o) throw std::runtime_error("uninitialized nlopt::opt");
      return nlopt_algorithm_name(nlopt_get_algorithm(o));
    }
    unsigned get_dimension() const {
      if (!o) throw std::runtime_error("uninitialized nlopt::opt");
      return nlopt_get_dimension(o);
    }

    // Set the objective function
    void set_min_objective(nlopt_func f, void *f_data) {
      nlopt_result ret = nlopt_set_min_objective(o, f, f_data);
      mythrow(ret);
    }
    void set_min_objective(func *f) {
      set_min_objective(myfunc, f);
    }
    void set_min_objective(vfunc *f) {
      set_min_objective(myvfunc, f);
    }
    void set_max_objective(nlopt_func f, void *f_data) {
      nlopt_result ret = nlopt_set_max_objective(o, f, f_data);
      mythrow(ret);
    }
    void set_max_objective(func *f) {
      set_max_objective(myfunc, f);
    }
    void set_max_objective(vfunc *f) {
      set_max_objective(myvfunc, f);
    }

    // Nonlinear constraints:

    void remove_inequality_constraints(void) {
      nlopt_result ret = nlopt_remove_inequality_constraints(o);
      mythrow(ret);
    }
    void add_inequality_constraint(nlopt_func f, void *f_data, double tol=0) {
      nlopt_result ret = nlopt_add_inequality_constraint(o, f, f_data, tol);
      mythrow(ret);
    }
    void add_inequality_constraint(func *f, double tol=0) {
      add_inequality_constraint(myfunc, f, tol);
    }

    void remove_equality_constraints(void) {
      nlopt_result ret = nlopt_remove_equality_constraints(o);
      mythrow(ret);
    }
    void add_equality_constraint(nlopt_func f, void *f_data, double tol=0) {
      nlopt_result ret = nlopt_add_equality_constraint(o, f, f_data, tol);
      mythrow(ret);
    }
    void add_equality_constraint(func *f, double tol=0) {
      add_equality_constraint(myfunc, f, tol);
    }

#define NLOPT_GETSET_VEC(name)						\
    void get_##name(double *v) const {					\
      nlopt_result ret = nlopt_get_##name(o, v);			\
      mythrow(ret);							\
    }									\
    void set_##name(const double *v) {					\
      nlopt_result ret = nlopt_set_##name(o, v);			\
      mythrow(ret);							\
    }									\
    void set_##name(double val) {					\
      nlopt_result ret = nlopt_set_##name##1(o, val);			\
      mythrow(ret);							\
    }									\
    void get_##name(std::vector<double> &v) const {			\
      if (o && nlopt_get_dimension(o) != v.size())			\
        throw std::invalid_argument("dimension mismatch");		\
      get_##name(v.empty() ? NULL : &v[0]);				\
    }									\
    std::vector<double> get_##name(void) const {			\
      if (!o) throw std::runtime_error("uninitialized nlopt::opt");	\
      std::vector<double> v(nlopt_get_dimension(o));			\
      get_##name(v);							\
      return v;								\
    }			 						\
    void set_##name(const std::vector<double> &v) {			\
      if (o && nlopt_get_dimension(o) != v.size())			\
        throw std::invalid_argument("dimension mismatch");		\
      set_##name(v.empty() ? NULL : &v[0]);				\
    }

    NLOPT_GETSET_VEC(lower_bounds)
    NLOPT_GETSET_VEC(upper_bounds)

    // stopping criteria:

#define NLOPT_GETSET(T, name)						\
    T get_##name() const {						\
      if (!o) throw std::runtime_error("uninitialized nlopt::opt");	\
      return nlopt_get_##name(o);					\
    }									\
    void set_##name(T name) {						\
      nlopt_result ret = nlopt_set_##name(o, name);			\
      mythrow(ret);							\
    }
    NLOPT_GETSET(double, stopval)
    NLOPT_GETSET(double, ftol_rel)
    NLOPT_GETSET(double, ftol_abs)
    NLOPT_GETSET(double, xtol_rel)
    NLOPT_GETSET_VEC(xtol_abs)
    NLOPT_GETSET(int, maxeval)
    NLOPT_GETSET(double, maxtime)

    NLOPT_GETSET(int, force_stop)
    void force_stop() { set_force_stop(1); }

    // algorithm-specific parameters:

    void set_local_optimizer(const nlopt_opt lo) {
      nlopt_result ret = nlopt_set_local_optimizer(o, lo);
      mythrow(ret);
    }
    void set_local_optimizer(const opt &lo) {
      set_local_optimizer(lo.o);
    }

    NLOPT_GETSET(unsigned, population)
    NLOPT_GETSET_VEC(initial_step)

    void set_default_initial_step(const double *x) {
      nlopt_result ret = nlopt_set_default_initial_step(o, x);
      mythrow(ret);
    }
    void set_default_initial_step(const std::vector<double> &x) {
      set_default_initial_step(x.empty() ? NULL : &x[0]);
    }
    void get_initial_step(const double *x, double *dx) const {
      nlopt_result ret = nlopt_get_initial_step(o, x, dx);
      mythrow(ret);
    }
    void get_initial_step(const std::vector<double> &x, std::vector<double> &dx) const {
      if (o && (nlopt_get_dimension(o) != x.size()
		|| nlopt_get_dimension(o) != dx.size()))
        throw std::invalid_argument("dimension mismatch");
      get_initial_step(x.empty() ? NULL : &x[0],
		       dx.empty() ? NULL : &dx[0]);
    }
    std::vector<double> get_initial_step(const std::vector<double> &x) const {
      if (!o) throw std::runtime_error("uninitialized nlopt::opt");
      std::vector<double> v(nlopt_get_dimension(o));
      get_initial_step(x, v);
      return v;
    }
  };

#undef NLOPT_GETSET
#undef NLOPT_GETSET_VEC

  //////////////////////////////////////////////////////////////////////

  inline void srand(unsigned long seed) { nlopt_srand(seed); }
  inline void srand_time(void) { nlopt_srand_time(); }
  inline void version(int &major, int &minor, int &bugfix) {
    nlopt_version(&major, &minor, &bugfix);
  }
  inline const char *algorithm_name(nlopt_algorithm a) {
    return nlopt_algorithm_name(a);
  }
  inline const char *algorithm_name(algorithm a) {
    return algorithm_name(nlopt_algorithm(a));
  }

  //////////////////////////////////////////////////////////////////////

} // namespace nlopt
