#pragma once
#include "ps.h"
#include "solver/async_sgd.h"
#include "config.pb.h"
namespace dmlc {
namespace fm {

using FeaID = ps::Key;
using Real = float;
static const int kPushFeaCnt = 1;

class Progress : public VectorProgress {
 public:
  Progress() : VectorProgress(4, 3) {}
  virtual ~Progress() { }


  /// head string for printing
  virtual std::string HeadStr() {
    return " ttl #ex  inc #ex |   |w|_0       |V|_0  | logloss   AUC";
  }

  /// string for printing
  virtual std::string PrintStr(const IProgress* const prev) {
    Progress* const p = (Progress* const) prev;
    if (num_ex() == 0) return "";
    double cnt = (double)count();
    double num = (double)num_ex();
    char buf[256];
    snprintf(buf, 256, "%7.2g  %7.2g | %9.4g  %9.4g | %6.4lf  %6.4lf ",
             (double)(p->num_ex() + num), num,
             (double)(p->nnz_w() + nnz_w()), (double)(p->nnz_V() + nnz_V()),
             objv() / num, auc() / cnt);
    return std::string(buf);
  }

  // mutator
  double& objv() { return fvec_[0]; }
  double objv() const { return fvec_[0]; }
  double& auc() { return fvec_[1]; }

  double& copc() { return fvec_[2]; }

  int64_t& count() { return ivec_[0]; }
  int64_t& num_ex() { return ivec_[1]; }
  int64_t num_ex() const { return ivec_[1]; }
  int64_t& nnz_w() { return ivec_[2]; }
  int64_t nnz_w() const { return ivec_[2]; }
  int64_t& nnz_V() { return ivec_[3]; }
  int64_t nnz_V() const { return ivec_[3]; }
};

class FMScheduler : public solver::AsyncSGDScheduler<Progress> {
 public:
  FMScheduler(const Config& conf) {
    worker_local_data_ = conf.use_worker_local_data();
    train_data_        = conf.train_data();
    val_data_          = conf.val_data();
    data_format_       = conf.data_format();
    num_part_per_file_ = conf.num_parts_per_file();
    max_data_pass_     = conf.max_data_pass();
    disp_itv_          = conf.disp_itv();
    save_model_        = conf.save_model();
    load_model_        = conf.load_model();
    if (save_model_) CHECK(conf.model_out().size()) << "empty model_out";
    if (load_model_) CHECK(conf.model_in().size()) << "empty model_in";
  }
  virtual ~FMScheduler() { }
};

}  // namespace fm
}  // namespace dmlc
