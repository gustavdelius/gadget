#include "agebandmatrixptrvector.h"

#ifndef GADGET_INLINE
#include "agebandmatrixptrvector.icc"
#endif

AgeBandMatrixPtrVector::AgeBandMatrixPtrVector(int sz) {
  size = (sz > 0 ? sz : 0);
  if (size > 0)
    v = new AgeBandMatrix*[size];
  else
    v = 0;
}
AgeBandMatrixPtrVector::AgeBandMatrixPtrVector(int sz, AgeBandMatrix* value) {
  assert(value != NULL);
  size = (sz > 0 ? sz : 0);
  int i;
  if (size > 0) {
    v = new AgeBandMatrix*[size];
    for (i = 0; i < size; i++)
      v[i] = value;
  } else
    v = 0;
}

AgeBandMatrixPtrVector::AgeBandMatrixPtrVector(int size1, int minage,
  const IntVector& minl, const IntVector& size2) {

  assert(size1 >= 0);
  size = size1;
  int i;
  if (size == 0) {
    v = 0;
  } else {
    v = new AgeBandMatrix*[size];
    for (i = 0; i < size; i++)
      v[i] = new AgeBandMatrix(minage, minl, size2);
  }
}

AgeBandMatrixPtrVector::~AgeBandMatrixPtrVector() {
  int i;
  if (v != 0) {
    for (i = 0; i < size; i++)
      delete v[i];
    delete[] v;
    v = 0;
  }
}

void AgeBandMatrixPtrVector::resize(int addsize, AgeBandMatrix* value) {
  int oldsize = size;
  this->resize(addsize);
  int i;
  if (addsize > 0)
    for (i = oldsize; i < size; i++)
      v[i] = value;
}

void AgeBandMatrixPtrVector::resize(int addsize) {
  int i;
  if (v == 0) {
    size = addsize;
    v = new AgeBandMatrix*[size];
  } else if (addsize > 0) {
    AgeBandMatrix** vnew = new AgeBandMatrix*[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
    size += addsize;
  }
}

void AgeBandMatrixPtrVector::resize(int addsize, int minage,
  const IntVector& minl, const IntVector& lsize) {

  assert(addsize > 0);
  AgeBandMatrix** vnew = new AgeBandMatrix*[size + addsize];
  int i;
  for (i = 0; i < size; i++)
    vnew[i] = v[i];
  for (i = size; i < size + addsize; i++)
    vnew[i] = new AgeBandMatrix(minage, minl, lsize);
  delete[] v;
  v = vnew;
  size += addsize;
}

void AgeBandMatrixPtrVector::resize(int addsize, int minage,
  int minl, const PopInfoMatrix& matr) {

  assert(addsize > 0);
  int i;
  if (v == 0) {
    size = addsize;
    v = new AgeBandMatrix*[size];
    for (i = 0; i < size; i++)
      v[i] = new AgeBandMatrix(minage, minl, matr);

  } else {
    AgeBandMatrix** vnew = new AgeBandMatrix*[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
    for (i = size; i < addsize + size; i++)
      v[i] = new AgeBandMatrix(minage, minl, matr);
    size = addsize + size;
  }
}

void AgeBandMatrixPtrVector::Delete(int pos) {
  int i;
  if (size > 1) {
    AgeBandMatrix** vnew = new AgeBandMatrix*[size - 1];
    for (i = 0; i < pos; i++)
      vnew[i] = v[i];
    for (i = pos; i < size - 1; i++)
      vnew[i] = v[i + 1];
    delete[] v;
    v = vnew;
    size--;
  } else {
    delete[] v;
    v = 0;
    size = 0;
  }
}
