#include "doublematrix.h"
#include "mathfunc.h"
#include "gadget.h"

#ifndef GADGET_INLINE
#include "doublematrix.icc"
#endif

DoubleMatrix::DoubleMatrix(int nr, int nc) {
  assert((nr > 0) && (nc > 0));
  nrow = nr;
  v = new DoubleVector*[nr];
  int i;
  for (i = 0; i < nr; i++)
    v[i] = new DoubleVector(nc);
}

DoubleMatrix::DoubleMatrix(double* initial, int nr, int nc) {
  assert(initial != NULL);
  assert((nr > 0) && (nc > 0));
  nrow = nr;
  v = new DoubleVector*[nr];
  int i;
  for (i = 0; i < nr; i++)
    v[i] = new DoubleVector(&initial[i * nc], nc);
}

DoubleMatrix::DoubleMatrix(int nr, int nc, double value) {
  nrow = nr;
  v = new DoubleVector*[nr];
  int i, j;
  for (i = 0; i < nr; i++)
    v[i] = new DoubleVector(nc);
  for (i = 0; i < nr; i++) {
    for (j = 0; j < nc; j++)
      (*v[i])[j] = value;
  }
}

DoubleMatrix::DoubleMatrix(int nr, const IntVector& nc) {
  nrow = nr;
  v = new DoubleVector*[nr];
  int i;
  for (i = 0; i < nr; i++)
    v[i] = new DoubleVector(nc[i]);
}

DoubleMatrix::DoubleMatrix(int nr, const IntVector& nc, double value) {
  nrow = nr;
  v = new DoubleVector*[nr];
  int i, j;
  for (i = 0; i < nr; i++)
    v[i] = new DoubleVector(nc[i]);
  for (i = 0; i < nr; i++)
    for (j = 0; j < nc[i]; j++)
      (*v[i])[j] = value;
}

DoubleMatrix::DoubleMatrix(const DoubleMatrix& initial) : nrow(initial.nrow) {
  int i;
  if (nrow >= 0) {
    v = new DoubleVector*[nrow];
    for (i = 0; i < nrow; i++)
      v[i] = new DoubleVector(initial[i]);
  } else {
    v = 0;
    nrow = 0;
  }
}

DoubleMatrix::~DoubleMatrix() {
  int i;
  if (v != 0) {
    for (i = 0; i < nrow; i++)
      delete v[i];
    delete[] v;
    v = 0;
  }
}

void DoubleMatrix::AddRows(int add, int length) {
  if (v == 0)
    nrow = 0;
  DoubleVector** vnew = new DoubleVector*[nrow + add];
  int i;
  for (i = 0; i < nrow; i++)
    vnew[i] = v[i];
  delete[] v;
  v = vnew;
  for (i = nrow; i < nrow + add; i++)
    v[i] = new DoubleVector(length);
  nrow += add;
}

void DoubleMatrix::AddRows(int add, int length, double initial) {
  if (v == 0)
    nrow = 0;
  int oldnrow = nrow;
  this->AddRows(add, length);
  int i, j;
  for (i = oldnrow; i < nrow; i++)
    for (j = 0; j < length; j++)
      (*v[i])[j] = initial;
}

void DoubleMatrix::DeleteRow(int row) {
  assert(nrow > 0);
  assert(0 <= row && row < nrow);
  delete v[row];
  DoubleVector** vnew = new DoubleVector*[nrow - 1];
  int i;
  for (i = 0; i < row; i++)
    vnew[i] = v[i];
  for (i = row; i < nrow - 1; i++)
    vnew[i] = v[i + 1];
  delete[] v;
  v = vnew;
  nrow--;
}

int DoubleMatrix::maxRowSize() const {
  int maxrowsize = 0;
  int i;
  for (i = 0; i<nrow; i++)
    if (Ncol(i) > maxrowsize)
      maxrowsize = Ncol(i);
  return maxrowsize;
}

int DoubleMatrix::minRowSize() const {
  if (nrow == 0)
    return 0;
  int minrowsize = Ncol(0);
  int i;
  for (i = 1; i<nrow; i++)
    if (Ncol(i) < minrowsize)
      minrowsize = Ncol(i);
  return minrowsize;
}

int DoubleMatrix::isRectangular() const {
  return minRowSize() == maxRowSize();
}

/*  addMatrix
 *
 *  Purpose:  Return a new matrix that is the sum of this and
 *            the argument matrix
 *
 *  In:   DoubleMatrix& d       :matrix to add to this
 *        int nrow              :number of rows in result
 *        int ncol      :number of columns in result
 */
DoubleMatrix&  DoubleMatrix::addMatrix(const DoubleMatrix& d, int nr, int ncol) const {
  DoubleMatrix* result = new DoubleMatrix(nr, ncol, 0);
  int i, j, maxcol, maxrow;
  maxrow = min(nr, d.Nrow());
  for (i = 0; i < maxrow; i++) {
    maxcol = min(ncol, d.Ncol(i));
    for (j = 0; j < maxcol; j++)
      (*result)[i][j] = d[i][j];
  }
  maxrow = min(nr, Nrow());
  for (i = 0; i < maxrow; i++) {
    maxcol = min(ncol, Ncol());
    for (j = 0; j < maxcol; j++)
      (*result)[i][j] += (*this)[i][j];
  }
  return *result;
}

/*  subMatrix
 *
 *  Purpose:  Return a new matrix that is the difference of this and
 *            the argument matrix
 *
 *  In:  DoubleMatrix& d       :matrix to subtract from this
 *       int nrow              :number of rows in result
 *       int ncol              :number of columns in result
 */
DoubleMatrix& DoubleMatrix::subMatrix(const DoubleMatrix& d, int nr, int ncol) const {
  DoubleMatrix* result = new DoubleMatrix(nr, ncol, 0);
  int i, j, maxcol, maxrow;
  maxrow = min(nr, Nrow());
  for (i = 0; i < maxrow; i++) {
    maxcol = min(ncol, Ncol(i));
    for (j = 0; j < maxcol; j++)
      (*result)[i][j] = (*this)[i][j];
  }
  maxrow = min(nr, d.Nrow());
  for (i = 0; i < maxrow; i++) {
    maxcol = min(ncol, d.Ncol());
    for (j = 0; j < maxcol; j++)
      (*result)[i][j] -= d[i][j];
  }
  return *result;
}

DoubleMatrix& DoubleMatrix::operator += (const DoubleMatrix& d) {
  int i, j, maxcol, maxrow;
  maxrow = min(Nrow(), d.Nrow());
  for (i = 0; i < maxrow; i++) {
    maxcol = min(Ncol(i), d.Ncol(i));
    for (j = 0; j < maxcol; j++)
      (*this)[i][j] += d[i][j];
  }
  return *this;
}

DoubleMatrix& DoubleMatrix::operator -= (const DoubleMatrix& d) {
  int i, j, maxcol, maxrow;
  maxrow = min(Nrow(), d.Nrow());
  for (i = 0; i < maxrow; i++) {
    maxcol = min(Ncol(i), d.Ncol(i));
    for (j = 0; j < maxcol; j++)
      (*this)[i][j] -= d[i][j];
  }
  return *this;
}

DoubleMatrix& DoubleMatrix::operator = (const DoubleMatrix& d) {
  if (this == &d)
    return *this;
  int i;
  if (v != 0) {
    for (i = 0; i < nrow; i++)
      delete v[i];
    delete[] v;
  }
  nrow = d.nrow;
  if (nrow >= 0) {
    v = new DoubleVector*[nrow];
    for (i = 0; i < nrow; i++)
      v[i] = new DoubleVector(d[i]);
  } else {
    v = 0;
    nrow = 0;
  }
  return *this;
}

int DoubleMatrix::operator == (const DoubleMatrix& d) const {
  if (Nrow() != d.Nrow())
    return 0;
  int i, j;
  for (i = 0; i < Nrow(); i++) {
    if (Ncol(i) != d.Ncol(i))
      return 0;
    for (j = 0; j < Ncol(i); j++)
      if ((*this)[i][j] != d[i][j])
        return 0;
  }
  return 1;
}

void DoubleMatrix::setElementsTo(double d) {
  int i;
  for (i = 0; i < nrow; i++)
    v[i]->setElementsTo(d);
}

DoubleMatrix& DoubleMatrix::operator *= (double d) {
  int i;
  for (i = 0; i < nrow; i++)
    (*v[i])*=d;
  return *this;
}

DoubleMatrix& DoubleMatrix::operator * (double d) const {
  DoubleMatrix* result = new DoubleMatrix(*this);
  (*result) *= d;
  return *result;
}

DoubleMatrix& DoubleMatrix::operator -= (double d) {
  return ((*this) += (-d));
}

DoubleMatrix& DoubleMatrix::operator - (double d) const {
  DoubleMatrix* result = new DoubleMatrix(*this);
  (*result) -= d;
  return *result;
}

DoubleMatrix& DoubleMatrix::operator += (double d) {
  int i;
  for (i = 0; i < nrow; i++)
    (*v[i]) += d;
  return *this;
}

DoubleMatrix& DoubleMatrix::operator + (double d) const {
  DoubleMatrix* result = new DoubleMatrix(*this);
  (*result) += d;
  return *result;
}

DoubleMatrix& DoubleMatrix::operator * (const DoubleMatrix& d) const {
  int i, j, k;
  if (!isRectangular() || !d.isRectangular()) {
    cerr << "Error: matrix must be rectangular for multiplication!\n";
    exit(EXIT_FAILURE);
  }
  if (Ncol() != d.Nrow()) {
    cerr << "Error: wrong dimensions for matrix multiplication!\n";
    exit(EXIT_FAILURE);
  }
  DoubleMatrix* result = new DoubleMatrix(Nrow(), d.Ncol(), 0);
  for (i = 0; i < result->Nrow(); i++)
    for (j = 0; j < result->Ncol(); j++)
      for (k = 0; k < d.Nrow(); k++)
        (*result)[i][j] += (*this)[i][k] * d[k][j];
  return *result;
}

ostream& operator << (ostream& out, const DoubleMatrix& d) {
  int i;
  for (i = 0; i < d.Nrow(); i++)
    out << d[i] << endl;
  return out;
}
