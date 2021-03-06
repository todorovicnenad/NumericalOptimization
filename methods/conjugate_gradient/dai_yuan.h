#ifndef DAI_YUAN_H_INCLUDED
#define DAI_YUAN_H_INCLUDED

#include <cmath>
#include "../base_method.h"

namespace opt {
namespace method {
namespace conjugate_gradient {

template<class real>
class dai_yuan : public base_method<real> {
public:
    dai_yuan() : base_method<real>(), nu(0.1) {}
    dai_yuan(real nu) : base_method<real>(), nu(nu) {}
    dai_yuan(real nu, real epsilon) : base_method<real>(epsilon), nu(nu) {}
    dai_yuan(real nu, real epsilon, size_t max_iter) : base_method<real>(epsilon, max_iter), nu(nu) {}
    dai_yuan(real nu, real epsilon, size_t max_iter, real working_precision) : base_method<real>(epsilon, max_iter, working_precision), nu(nu) {}

    void operator()(function::function<real>& f, line_search::base_line_search<real>& ls, arma::Col<real>& x) {
        this->iter_count = 0;
        ls.clear_f_vals();

        this->tic();

        real f_curr = f(x);
        real f_prev = f_curr + 1;

        arma::Col<real> gr = f.gradient(x);
        arma::Col<real> gr_old;

        arma::Col<real> pk = -gr;

        while (arma::norm(gr) > this->epsilon && this->iter_count < this->max_iter && fabs(f_prev-f_curr)/(1+fabs(f_curr)) > this->working_precision) {
            ++this->iter_count;
            ls.push_f_val(f_curr);
            ls.set_current_f_val(f_curr);
            ls.set_current_g_val(gr);

            f_prev = f_curr;
            gr_old = gr;

            x += pk * ls(f, x, pk);

            f_curr = ls.get_current_f_val();
            gr = ls.get_current_g_val();

            real beta_dy = arma::dot(gr, gr) / arma::dot(pk, gr - gr_old);

            // if restart coefficient greater than nu, apply reset
            real rc = fabs(arma::dot(gr, gr_old)) / arma::dot(gr, gr);
            if (rc > nu) {
                beta_dy = 0;
            }

            pk *= beta_dy;
            pk -= gr;
        }

        this->toc();
        this->f_min = f_curr;
        this->gr_norm = arma::norm(gr);
        this->f_call_count = f.get_call_count();
        this->g_call_count = f.get_grad_count();
        this->h_call_count = f.get_hess_count();
    }
protected:
    real nu; // used for conditionally restarting beta
};

}
}
}

#endif //DAI_YUAN_H_INCLUDED
