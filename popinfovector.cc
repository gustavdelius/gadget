#include "popinfovector.h"
#include "gadget.h"

#ifndef GADGET_INLINE
#include "popinfovector.icc"
#endif

PopInfoVector::PopInfoVector(int sz) {
  size = (sz > 0 ? sz : 0);
  if (size > 0)
    v = new PopInfo[size];
  else
    v = 0;
}

PopInfoVector::PopInfoVector(int sz, PopInfo value) {
  size = (sz > 0 ? sz : 0);
  int i;
  if (size > 0) {
    v = new PopInfo[size];
    for (i = 0; i < size; i++)
      v[i] = value;
  } else
    v = 0;
}

PopInfoVector::PopInfoVector(const PopInfoVector& initial) {
  size = initial.size;
  int i;
  if (size > 0) {
    v = new PopInfo[size];
    for (i = 0; i < size; i++)
      v[i] = initial.v[i];
  } else
    v = 0;
}

PopInfoVector::~PopInfoVector() {
  if (v != 0) {
    delete[] v;
    v = 0;
  }
}

void PopInfoVector::resize(int addsize, PopInfo value) {
  int oldsize = size;
  this->resize(addsize);
  int i;
  if (addsize > 0)
    for (i = oldsize; i < size; i++)
      v[i] = value;
}

void PopInfoVector::resize(int addsize) {
  int i;
  if (v == 0) {
    size = addsize;
    v = new PopInfo[size];
  } else if (addsize > 0) {
    PopInfo* vnew = new PopInfo[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
    size += addsize;
  }
}

void PopInfoVector::Delete(int pos) {
  assert(size > 0);
  assert(0 <= pos && pos < size);
  PopInfo* vnew = new PopInfo[size - 1];
  int i;
  for (i = 0; i < pos; i++)
    vnew[i] = v[i];
  for (i = pos; i < size - 1; i++)
    vnew[i] = v[i + 1];
  delete[] v;
  v = vnew;
  size--;
}
