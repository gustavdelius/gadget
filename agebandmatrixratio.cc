#include "intvector.h"
#include "agebandmatrixratio.h"
#include "commentstream.h"
#include "gadget.h"

#ifndef GADGET_INLINE
#include "agebandmatrixratio.icc"
#endif

AgeBandMatrixRatio::AgeBandMatrixRatio(const AgeBandMatrixRatio& initial)
  : minage(initial.Minage()), nrow(initial.Nrow()) {

  int i;
  if (nrow > 0) {
    v = new PopRatioIndexVector*[nrow];
    for (i = 0; i < nrow; i++)
      v[i] = new PopRatioIndexVector(initial[i + minage]);
  } else
      v = 0;
}

AgeBandMatrixRatio::AgeBandMatrixRatio(int MinAge, const IntVector& minl,
  const IntVector& size) : minage(MinAge), nrow(size.Size()) {

  int i;
  if (nrow > 0) {
    v = new PopRatioIndexVector*[nrow];
    for (i = 0; i < nrow; i++)
      v[i] = new PopRatioIndexVector(size[i], minl[i]);
  } else
      v = 0;
}

AgeBandMatrixRatio::AgeBandMatrixRatio(int MinAge, const PopRatioIndexVector& initial)
  : minage(MinAge), nrow(1) {

  v = new PopRatioIndexVector*[1];
  v[0] = new PopRatioIndexVector(initial);
}

AgeBandMatrixRatio::~AgeBandMatrixRatio() {
  int i;
  if (v != 0) {
    for (i = 0; i < nrow; i++)
      delete v[i];
    delete[] v;
    v = 0;
  }
}

int AgeBandMatrixRatio::NrOfTagExp() const {
  int minlength;
  if (nrow > 0) {
    minlength = this->Minlength(minage);
    return (this->operator[](minage))[minlength].Size();
  } else
    return 0;
}
