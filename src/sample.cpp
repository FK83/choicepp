#include <Rcpp.h>
#include<unordered_map>
#include "helpers.h"
using namespace Rcpp;

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
  int j = no;
  while(j < no*2 && v <= r){
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
//    EDIT EXPERIENCE
//
//////////////////////////////////////////////////////////////////////////////

// [[Rcpp::export]]
std::vector<double> edit1(std::vector<double> ss, int size = 0){
  std::unordered_map<double, double> tab;
  double unit = 1.0 / ss.size();
  for(int i = 0; i < ss.size(); i++){
    double o = ss[i];
    std::unordered_map<double, double>::const_iterator got = tab.find(o);
    if( got == tab.end() ){
      tab[o] = unit;
      } else {
      tab[o] += unit;
      }
    }
  int add = 0;
  if(size > tab.size()) add = size - tab.size();
  std::vector<double> ed_opt;
  std::unordered_map<double, double>::const_iterator it;
  for(it = tab.begin(); it != tab.end(); ++it) ed_opt.push_back(it->first);
  for(int i = 0; i < add; i++) ed_opt.push_back(0);
  for(it = tab.begin(); it != tab.end(); ++it) ed_opt.push_back(it->second);
  for(int i = 0; i < add; i++) ed_opt.push_back(0);
  return arrange(ed_opt);
  }

// [[Rcpp::export]]
std::vector<double> edit2(std::vector<double> ss, NumericVector opt){
  std::unordered_map<double, double> tab;
  double unit = 1.0 / ss.size();
  int no = std::floor(opt.size()/2);
  for(int i = 0; i < no; i++) tab[opt[i]] = 0;
  for(int i = 0; i < ss.size(); i++){
    double o = ss[i];
    tab[o] += unit;
    }
  std::vector<double> ed_opt;
  std::unordered_map<double, double>::const_iterator it;
  for(it = tab.begin(); it != tab.end(); ++it) ed_opt.push_back(it->first);
  for(it = tab.begin(); it != tab.end(); ++it) ed_opt.push_back(it->second);
  return arrange(ed_opt);
  }

// [[Rcpp::export]]
GenericVector edit_exp(GenericVector ss, GenericVector prob){
  NumericMatrix As = prob[0];
  NumericMatrix Bs = prob[1];
  NumericMatrix As_n(As.nrow(),As.ncol());
  NumericMatrix Bs_n(Bs.nrow(),Bs.ncol());
  int np = ss.size();
  for(int p = 0; p < np; p++){
    GenericVector oo = ss[p];
    std::vector<double> edA = edit2(oo[0],As(p,_));
    std::vector<double> edB = edit2(oo[1],Bs(p,_));
    for(int i = 0; i < edA.size(); i++) As_n(p,i) = edA[i];
    for(int i = 0; i < edB.size(); i++) Bs_n(p,i) = edB[i];
    }
  GenericVector prob_n(2);
  prob_n[0] = As_n;
  prob_n[1] = Bs_n;
  return prob_n;
  }


