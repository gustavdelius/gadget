#include "intvector.h"
#include "doublematrix.h"
#include "doubleindexvector.h"
#include "bandmatrix.h"
#include "mathfunc.h"
#include "gadget.h"

#ifndef GADGET_INLINE
#include "bandmatrix.icc"
#endif

BandMatrix::BandMatrix(const BandMatrix& initial)
  : nrow(initial.nrow), minage(initial.Minage()) {

  v = new DoubleIndexVector*[nrow];
  int i;
  for (i = 0; i < nrow; i++)
    v[i] = new DoubleIndexVector(*(initial.v[i]));
}

BandMatrix::BandMatrix(const IntVector& minl, const IntVector& size,
  int MinAge, double initial) : nrow(size.Size()), minage(MinAge) {

  assert(size.Size() == minl.Size());
  v = new DoubleIndexVector*[nrow];
  int i;
  for (i = 0; i < nrow; i++)
    v[i] = new DoubleIndexVector(size[i], minl[i], initial);
}

BandMatrix::BandMatrix(const DoubleMatrix& initial, int MinAge, int minl)
  : nrow(initial.Nrow()), minage(MinAge) {

  v = new DoubleIndexVector*[nrow];
  int i, j;
  int lower, upper;
  for (i = 0; i < nrow; i++) {
    lower = 0;
    upper = initial.Ncol(i) - 1;
    while (iszero(initial[i][lower]) && (lower < upper))
      lower++;
    while (iszero(initial[i][upper]) && (upper > lower))
      upper--;
    if (iszero(initial[i][lower]) && (lower == upper)) {
      lower = 0;
      upper = initial.Ncol(i) - 1;
    }
    v[i] = new DoubleIndexVector(upper - lower + 1, lower + minl);
    for (j = lower; j <= upper; j++)
      (*v[i])[j + minl] = initial[i][j];
  }
}

BandMatrix::BandMatrix(const DoubleIndexVector& initial, int age)
  : nrow(1), minage(age) {

  v = new DoubleIndexVector*[nrow];
  v[0] = new DoubleIndexVector(initial);
}

BandMatrix::BandMatrix(int minl, int lengthsize, int Minage, int Nrow,
  double initial) : nrow(Nrow), minage(Minage) {

  v = new DoubleIndexVector*[nrow];
  int i;
  for (i = 0; i < nrow; i++)
    v[i] = new DoubleIndexVector(lengthsize, minl, initial);
}

BandMatrix::~BandMatrix() {
  int i;
  if (v != 0) {
    for (i = 0; i < nrow; i++)
      delete v[i];
    delete[] v;
    v = 0;
  }
}

void BandMatrix::Colsum(DoubleVector& Result) const {
  int i, j;
  for (i = 0; i < nrow; i++)
    for (j = v[i]->Mincol(); j < v[i]->Maxcol(); j++)
      Result[j] += (*v[i])[j];
}

BandMatrixVector::BandMatrixVector(int sz) : size(sz) {
  assert(size >= 0);
  int i;
  if (size == 0) {
    v = 0;
    return;
  }
  v = new BandMatrix*[size];
  for (i = 0; i < size; i++)
    v[i] = 0;
}

BandMatrixVector::~BandMatrixVector() {
  int i;
  if (v != 0) {
    for (i = 0; i < size; i++)
      delete v[i];
    delete[] v;
    v = 0;
  }
}

void BandMatrixVector::ChangeElement(int nr, const BandMatrix& value) {
  if (v[nr] != 0)
    delete v[nr];
  v[nr] = new BandMatrix(value);
}

void BandMatrixVector::resize(int addsize) {
  assert(addsize > 0);
  int i;
  if (v == 0) {
    size = addsize;
    v = new BandMatrix*[size];
    for (i = 0; i < size; i++)
      v[i] = 0;
  } else {
    BandMatrix** vnew = new BandMatrix*[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    for (i = size; i < size + addsize; i++)
      vnew[i] = 0;
    delete[] v;
    v = vnew;
    size += addsize;
  }
}

void BandMatrixVector::resize(int addsize, const BandMatrix& initial) {
  assert(addsize > 0);
  int i;
  if (v == 0) {
    size = addsize;
    v = new BandMatrix*[size];
    for (i = 0; i < size; i++)
      v[i] = new BandMatrix(initial);
  } else if (addsize > 0) {
    BandMatrix** vnew = new BandMatrix*[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    for (i = size; i < size + addsize; i++)
      vnew[i] = new BandMatrix(initial);
    delete[] v;
    v = vnew;
    size += addsize;
  }
}

void BandMatrixVector::Delete(int pos) {
  assert(size > 0);
  assert(0 <= pos && pos < size);
  delete v[pos];
  BandMatrix** vnew = new BandMatrix*[size - 1];
  int i;
  for (i = 0; i < pos; i++)
    vnew[i] = v[i];
  for (i = pos; i < size - 1; i++)
    vnew[i] = v[i + 1];
  delete[] v;
  v = vnew;
  size--;
}

BandMatrixMatrix::BandMatrixMatrix(int NroW, int ncol) : nrow(NroW) {
  assert(nrow >= 0);
  assert(ncol >= 0);
  int i;
  if (nrow == 0) {
    v = 0;
    return;
  }
  v = new BandMatrixVector*[nrow];
  for (i = 0; i < nrow; i++)
    v[i] = new BandMatrixVector(ncol);
}

BandMatrixMatrix::~BandMatrixMatrix() {
  int i;
  if (v != 0) {
    for (i = 0; i < nrow; i++)
      delete v[i];
    delete[] v;
    v = 0;
  }
}

void BandMatrixMatrix::ChangeElement(int row, int col, const BandMatrix& value) {
  assert(0 <= row && row < nrow);
  v[row]->ChangeElement(col, value);
}

void BandMatrixMatrix::AddRows(int addrow, int ncol) {
  assert(addrow > 0);
  int i;
  if (v == 0) {
    nrow = addrow;
    v = new BandMatrixVector*[nrow];
    for (i = 0; i < nrow; i++)
      v[i] = new BandMatrixVector(ncol);
  } else {
    BandMatrixVector** vnew = new BandMatrixVector*[addrow + nrow];
    for (i = 0; i < nrow; i++)
      vnew[i] = v[i];
    for (i = nrow; i < nrow + addrow; i++)
      vnew[i] = new BandMatrixVector(ncol);
    delete[] v;
    v = vnew;
    nrow += addrow;
  }
}

void BandMatrixMatrix::DeleteRow(int row) {
  assert(nrow > 0);
  assert(0 <= row && row < nrow);
  delete v[row];
  BandMatrixVector** vnew = new BandMatrixVector*[nrow - 1];
  int i;
  for (i = 0; i < row; i++)
    vnew[i] = v[i];
  for (i = row; i < nrow - 1; i++)
    vnew[i] = v[i + 1];
  delete[] v;
  v = vnew;
  nrow--;
}

BandMatrix& BandMatrix::operator += (BandMatrix& b) {
  int i, j;
  for (i = max(Minrow(), b.Minrow()); i<min(Maxrow(), b.Maxrow()); i++)
    for (j = max(Mincol(i), b.Mincol(i)); j<min(Maxcol(i), b.Maxcol(i)); j++)
      (*this)[i][j] += b[i][j];
  return *this;
}
