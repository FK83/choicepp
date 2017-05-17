#include <Rcpp.h>
#include "helpers.h"
using namespace Rcpp;


//////////////////////////////////////////////////////////////////////////////
//
//    EDIT EXPERIENCE
//
//////////////////////////////////////////////////////////////////////////////

// [[Rcpp::export]]
std::vector<double> edit1(std::vector<double> ss, int size = 0){
  std::map<double, double> tab;
  double unit = 1.0 / ss.size();
  for(int i = 0; i < ss.size(); i++){
    double o = ss[i];
    std::map<double, double>::const_iterator got = tab.find(o);
    if( got == tab.end() ){
      tab[o] = unit;
    } else {
      tab[o] += unit;
    }
  }
  int add = 0;
  if(size > tab.size()) add = size - tab.size();
  std::vector<double> ed_opt;
  std::map<double, double>::const_iterator it;
  for(it = tab.begin(); it != tab.end(); ++it) ed_opt.push_back(it->first);
  for(int i = 0; i < add; i++) ed_opt.push_back(0);
  for(it = tab.begin(); it != tab.end(); ++it) ed_opt.push_back(it->second);
  for(int i = 0; i < add; i++) ed_opt.push_back(0);
  return arrange(ed_opt);
}


// [[Rcpp::export]]
std::vector<double> edit2(std::vector<double> ss, NumericVector opt, bool add_n = false, bool do_arrange = true){
  std::map<double, double> tab;
  double unit = 1.0 / ss.size();
  int no = std::floor(opt.size()/2);
  for(int i = 0; i < no; i++) tab[opt[i]] = 0;
  for(int i = 0; i < ss.size(); i++){
    double o = ss[i];
    tab[o] += unit;
    }
  int add = 0;
  if(no > tab.size()) add = no - tab.size();
  std::vector<double> ed_opt;
  std::map<double, double>::const_iterator it;
  for(it = tab.begin(); it != tab.end(); ++it) ed_opt.push_back(it->first);
  for(int i = 0; i < add; i++) ed_opt.push_back(0);
  for(it = tab.begin(); it != tab.end(); ++it) ed_opt.push_back(it->second);
  for(int i = 0; i < add; i++) ed_opt.push_back(0);
  if(do_arrange == true){
    std::vector<double> arr_opt = arrange(ed_opt);
    if(add_n == true) arr_opt.push_back(ss.size());
    return arr_opt;
    } else {
    if(add_n == true) ed_opt.push_back(ss.size());
    return ed_opt;
    }
  }


//////////////////////////////////////////////////////////////////////////////
//' Edit experiences
//'
//' \code{edit_exp} converts the collected samples into a problem table of
//'   the same format as the original problem table. See link{p_arrange}.
//'
//' @param ss a list of samples as generated by, e.g., \link{sampl_n}.
//' @param prob a problem table as produced by \link{p_arrange}.
//' @param frequency
//'
//' @return a problem table matching \code{prob}.
//'
//' @export
// [[Rcpp::export]]
GenericVector edit_exp(GenericVector ss, GenericVector prob, bool add_n = false, bool do_arrange = true){
  int add_col = 0;
  if(add_n == true) add_col = 1;
  NumericMatrix As = prob[0];
  NumericMatrix Bs = prob[1];
  NumericMatrix As_n(As.nrow(),As.ncol() + add_col);
  NumericMatrix Bs_n(Bs.nrow(),Bs.ncol() + add_col);
  int np = ss.size();
  for(int p = 0; p < np; p++){
    GenericVector oo = ss[p];
    std::vector<double> edA = edit2(oo[0],As(p,_),add_n,do_arrange);
    std::vector<double> edB = edit2(oo[1],Bs(p,_),add_n,do_arrange);
    int nA = edA.size(), nB = edB.size();
    for(int i = 0; i < nA; i++) As_n(p,i) = edA[i];
    for(int i = 0; i < nB; i++) Bs_n(p,i) = edB[i];
  }
  GenericVector prob_n(2);
  prob_n[0] = As_n;
  prob_n[1] = Bs_n;
  return prob_n;
}


//////////////////////////////////////////////////////////////////////////////
//
//    SAMPLING HELPERS
//
//////////////////////////////////////////////////////////////////////////////

// [[Rcpp::export]]
double smpl(std::vector<double> opt){
  int no = std::floor(opt.size()/2);
  std::vector<double> os,ps;
  for(int i = 0; i < no; i++){
    os.push_back(opt[i]);
    ps.push_back(opt[i + no]);
    }
  int k, i = 0, n = ps.size();
  double v, sum = 0, r = double(std::rand()) / RAND_MAX;
  for(k = 0; k < no; k++){
    sum += ps[k];
    }
  v = ps[0] / double(sum);
  while(i < n && v <= r){
    i++;
    v += ps[i] / double(sum);
    }
  return os[i];
  }

// [[Rcpp::export]]
double smpl_f(NumericVector opt){
  int no = std::floor(opt.size()/2);
  double r = double(std::rand()) / RAND_MAX;
  double v = opt[no];
  int j = no, lim = (no*2-1);
  while(j < lim && v <= r){
    j++;
    v += opt[j];
    }
  return opt[j - no];
  }

// [[Rcpp::export]]
std::vector<double> smpl_n(NumericVector opt, int n){
  std::vector<double> ss;
  for(int i = 0; i < n; i++){
    double res = smpl_f(opt);
    ss.push_back(res);
    }
  return ss;
  }

//////////////////////////////////////////////////////////////////////////////
//
//    SAMPLING N
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//' Sample a fixed number of outcomes per option
//'
//' \code{sampl_n} draws a fixed number of samples for each option in the
//'   set of problems.
//'
//' @param prob a problem table as produced by \link{p_arrange}.
//' @param ns numceric vector whose length is either one or the number of
//'   problems in the set.
//'
//' @return a list of samples.
//'
//' @export
// [[Rcpp::export]]
GenericVector sampl_n(GenericVector prob, std::vector<int> ns){
  NumericMatrix As = prob[0];
  NumericMatrix Bs = prob[1];
  int np = As.nrow();
  int nn = ns.size();
  if(nn != np) for(int p = 0; p < np; p++) ns.push_back(ns[0]);
  GenericVector ss(np);
  for(int p = 0; p < np; p++){
    NumericVector A = As(p,_);
    NumericVector B = Bs(p,_);
    GenericVector oo(2);
    oo[0] = smpl_n(A,ns[p]);
    oo[1] = smpl_n(B,ns[p]);
    ss[p] = oo;
    }
  return ss;
  }


//////////////////////////////////////////////////////////////////////////////
//
//    SAMPLING: STOP WHEN EASY
//
//////////////////////////////////////////////////////////////////////////////


double p_stop(double uA, double uB, double n, double phi){
  double pa = 1 / (1+exp( n * phi * std::abs((uB-uA))));
  return 2 * (std::max(pa,1.-pa)-.5);
  }

NumericVector getopt(std::map<double, double> tab){
  std::vector<double> ed_opt;
  std::map<double, double>::const_iterator it;
  for(it = tab.begin(); it != tab.end(); ++it) ed_opt.push_back(it->first);
  for(it = tab.begin(); it != tab.end(); ++it) ed_opt.push_back(it->second);
  std::vector<double> arr_opt = arrange(ed_opt);
  return Rcpp::wrap(arr_opt);
  }


// [[Rcpp::export]]
GenericVector smpl_easy(NumericVector a, NumericVector b, double phi, std::vector<double> par, int type){
  std::map<double, double> taba,tabb;
  std::vector<double> ssa, ssb;
  NumericVector expa, expb;
  double sa, sb, ua, ub, ps, r;
  bool cont = true;
  int i = 0;

  // sample
  while(cont == true){
    i++;
    sa = smpl_f(a);
    sb = smpl_f(b);
    ssa.push_back(sa);
    ssb.push_back(sb);
    taba[sa]++;
    tabb[sb]++;
    expa = getopt(taba);
    expb = getopt(tabb);
    ua = utility(expa,par,type);
    ub = utility(expb,par,type);
    ps = p_stop(ua,ub,i,phi);
    //std::cout << ex << '_' << ua << '_' << ub << '_' << i/phi << '\n';
    r = double(std::rand()) / RAND_MAX;
    if(r < ps) cont = false;
    }

  GenericVector oo(2);
  oo[0] = ssa;
  oo[1] = ssb;
  return oo;
  }

//////////////////////////////////////////////////////////////////////////////
//' Sampling algorithm using stop when easy stopping rule
//'
//' \code{smpl_easy} implements a sampling process that terminates sampling
//'   increasingly likely as the distance between the options becomes large
//'
//' @param prob a problem table as produced by \link{p_arrange}.
//' @param ns numceric vector whose length is either one or the number of
//'   problems in the set.
//' @param phi a numeric specifying the sensitivity to the difference in
//'   option utilities. Large numbers result in early search termination.
//' @param par a numeric vector to be passed on to \link{utility}.
//' @param type an integer to be passed on to \link{utility}.
//'
//'
//' @return a list of witch each element containing the samples for option
//'   A, the samples for option B, and the choice.
//'
//' @export
// [[Rcpp::export]]
GenericVector sampl_easy(GenericVector prob, double phi, std::vector<double> par, int type){
  NumericMatrix As = prob[0];
  NumericMatrix Bs = prob[1];
  int np = As.nrow();
  GenericVector ss(np);
  for(int p = 0; p < np; p++){
    NumericVector A = As(p,_);
    NumericVector B = Bs(p,_);
    ss[p] = smpl_easy(A,B,phi,par,type);
    }
  return ss;
  }


//////////////////////////////////////////////////////////////////////////////
//
//    SAMPLING: STOP WHEN EVEN
//
//////////////////////////////////////////////////////////////////////////////


// [[Rcpp::export]]
GenericVector smpl_even(NumericVector a, NumericVector b, double gamma_eq, double gamma_uneq){
  std::map<double, double> taba,tabb;
  std::vector<double> ssa, ssb;
  NumericVector expa, expb;
  double sa, sb, ps, r;
  bool cont = true;
  int i = 0;

  // sample
  while(cont == true){
    i++;
    sa = smpl_f(a);
    sb = smpl_f(b);
    ssa.push_back(sa);
    ssb.push_back(sb);
    taba[sa]++;
    tabb[sb]++;
    if(taba.size() == tabb.size()){
      ps = 1. - std::pow(1./i,gamma_eq);
      } else {
      ps = 1. - std::pow(1./i,gamma_uneq);
      }
    //std::cout << pstop << '\n';
    r = double(std::rand()) / RAND_MAX;
    if(r < ps) cont = false;
    }

  GenericVector oo(2);
  oo[0] = ssa;
  oo[1] = ssb;
  return oo;
  }

//////////////////////////////////////////////////////////////////////////////
//' Sampling algorithm using a stop when even stopping rule
//'
//' \code{sample_even} implements a sampling process that terminates more
//'   likly when equally many outcomes in both options have been experienced.
//'
//' @param prob a problem table as produced by \link{p_arrange}.
//' @param gamma_eq a numeric controlling the likelihood with which search
//'   stops in the case of equally many experienced outcomes. Large number
//'   result in early search termination
//' @param gamma_uneq a numeric controlling the likelihood with which search
//'   stops in the case of equally many experienced outcomes. Large numbers
//'   result in early search termination.
//'
//' @return a list of witch each element containing the samples for option
//'   A, the samples for option B, and the choice.
//'
//' @export
// [[Rcpp::export]]
GenericVector sampl_even(GenericVector prob, double gamma_eq, double gamma_uneq){
  NumericMatrix As = prob[0];
  NumericMatrix Bs = prob[1];
  int np = As.nrow();
  GenericVector ss(np);
  for(int p = 0; p < np; p++){
    NumericVector A = As(p,_);
    NumericVector B = Bs(p,_);
    ss[p] = smpl_even(A,B,gamma_eq,gamma_uneq);
    }
  return ss;
  }

//////////////////////////////////////////////////////////////////////////////
//
//    SAMPLING: STOP WHEN COMPLETE
//
//////////////////////////////////////////////////////////////////////////////


// [[Rcpp::export]]
NumericVector get_nout(GenericVector prob){
  NumericMatrix As = prob[0];
  NumericMatrix Bs = prob[1];
  int nA = std::floor(As.ncol() / 2.0);
  int nB = std::floor(Bs.ncol() / 2.0);
  int np = As.nrow();
  NumericVector nouts(np);
  for(int p = 0; p < np; ++p){
    NumericVector A = As(p,_);
    NumericVector B = Bs(p,_);
    int nout = 0;
    for(int i = 0; i < nA; ++i) if(A[i + nA] > 0) nout++;
    for(int i = 0; i < nB; ++i) if(B[i + nB] > 0) nout++;
    nouts[p] = nout;
    }
  return nouts;
  }


// [[Rcpp::export]]
int get_minn(GenericVector prob){
  NumericMatrix As = prob[0];
  NumericMatrix Bs = prob[1];
  int nA = std::floor(As.ncol() / 2.0);
  int nB = std::floor(Bs.ncol() / 2.0);
  int np = As.nrow();
  int min_nout = 0;
  for(int p = 0; p < np; ++p){
    NumericVector A = As(p,_);
    NumericVector B = Bs(p,_);
    int nout = 0;
    for(int i = 0; i < nA; ++i) if(A[i + nA] > 0) nout++;
    for(int i = 0; i < nB; ++i) if(B[i + nB] > 0) nout++;
    if(nout < min_nout) min_nout = nout;
    }
  return min_nout;
  }


// [[Rcpp::export]]
GenericVector smpl_complete(NumericVector a,
                            NumericVector b,
                            int n,
                            double gamma_compl,
                            double gamma_incompl){
  std::map<double, double> taba,tabb;
  std::vector<double> ssa, ssb;
  NumericVector expa, expb;
  double sa, sb, ps, r;
  bool cont = true;
  int i = 0;

  // sample
  while(cont == true){
    i++;
    sa = smpl_f(a);
    sb = smpl_f(b);
    ssa.push_back(sa);
    ssb.push_back(sb);
    taba[sa]++;
    tabb[sb]++;
    if(taba.size() + tabb.size() >= n){
      ps = 1. - std::pow(1./i,gamma_compl);
    } else {
      ps = 1. - std::pow(1./i,gamma_incompl);
    }
    //std::cout << pstop << '\n';
    r = double(std::rand()) / RAND_MAX;
    if(r < ps) cont = false;
  }

  GenericVector oo(2);
  oo[0] = ssa;
  oo[1] = ssb;
  return oo;
}

// [[Rcpp::export]]
GenericVector smpl_complete2(NumericVector a,
                            NumericVector b,
                            int n,
                            double gamma_compl,
                            double gamma_incompl){
  std::map<double, double> taba,tabb;
  std::vector<double> ssa, ssb;
  NumericVector expa, expb;
  double sa, sb, ps, r;
  bool cont = true;
  int i = 0;
  bool ta = false, tb = false;

  // sample
  while(cont == true){
    i++;
    sa = smpl_f(a);
    sb = smpl_f(b);
    if(sa != 0) ta = true;
    if(sb != 0) tb = true;
    ssa.push_back(sa);
    ssb.push_back(sb);
    taba[sa]++;
    tabb[sb]++;
    if(taba.size() + tabb.size() >= n && ta && tb){
      ps = 1. - std::pow(1./i,gamma_compl);
    } else {
      ps = 1. - std::pow(1./i,gamma_incompl);
    }
    //std::cout << pstop << '\n';
    r = double(std::rand()) / RAND_MAX;
    if(r < ps) cont = false;
  }

  GenericVector oo(2);
  oo[0] = ssa;
  oo[1] = ssb;
  return oo;
}

//////////////////////////////////////////////////////////////////////////////
//' Sampling algorithm using a stop when complete stopping rule
//'
//' \code{sample_complete} implements a sampling process that terminates more
//'   likly when (at least) an expected number of outcomes has been observed
//'   across both options. The expected number of outcomes is drawn from the
//'   distribution of number of outcomes across the entire problem set.
//'
//' @param prob a problem table as produced by \link{p_arrange}.
//' @param ns numceric vector whose length is either one or the number of
//'   problems in the set.
//' @param gamma_compl a numeric controlling the likelihood with which search
//'   stops in the case of equally many experienced outcomes. Large numbers
//'   result in early search termination.
//' @param gamma_incompl a numeric controlling the likelihood with which
//'   search stops in the case of equally many experienced outcomes. Large
//'   numbers result in early search termination.
//'
//' @return a list of witch each element containing the samples for option
//'   A, the samples for option B, and the choice.
//'
//' @export
// [[Rcpp::export]]
GenericVector sampl_complete(GenericVector prob, double gamma_compl, double gamma_incompl){
  NumericVector ns = get_nout(prob);
  NumericMatrix As = prob[0];
  NumericMatrix Bs = prob[1];
  int np = As.nrow();
  GenericVector ss(np);
  for(int p = 0; p < np; p++){
    int n = ns[std::round(rnf(0,np-1))];
    NumericVector A = As(p,_);
    NumericVector B = Bs(p,_);
    ss[p] = smpl_complete(A,B,n,gamma_compl,gamma_incompl);
  }
  return ss;
}



//////////////////////////////////////////////////////////////////////////////
//' Sampling algorithm using a stop when complete stopping rule
//'
//' \code{sample_complete2} implements a sampling process that terminates more
//'   likly when a minimum number of outcomes has been observed across both
//'   options and non-zero outcomes have been observed for both options.
//'   The minimum number of outcomes equals the minimum the distribution of
//'   number of outcomes across the entire problem set.
//'
//' @param prob a problem table as produced by \link{p_arrange}.
//' @param ns numceric vector whose length is either one or the number of
//'   problems in the set.
//' @param gamma_compl a numeric controlling the likelihood with which search
//'   stops in the case of equally many experienced outcomes. Large numbers
//'   result in early search termination.
//' @param gamma_incompl a numeric controlling the likelihood with which
//'   search stops in the case of equally many experienced outcomes. Large
//'   numbers result in early search termination.
//'
//' @return a list of witch each element containing the samples for option
//'   A, the samples for option B, and the choice.
//'
//' @export
// [[Rcpp::export]]
GenericVector sampl_complete2(GenericVector prob, double gamma_compl, double gamma_incompl){
  NumericVector ns = get_nout(prob);
  NumericMatrix As = prob[0];
  NumericMatrix Bs = prob[1];
  int np = As.nrow();
  GenericVector ss(np);
  for(int p = 0; p < np; p++){
    int n = ns[std::round(rnf(0,np-1))];
    NumericVector A = As(p,_);
    NumericVector B = Bs(p,_);
    ss[p] = smpl_complete2(A,B,n,gamma_compl,gamma_incompl);
    }
  return ss;
  }
