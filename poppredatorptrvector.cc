#include "poppredatorptrvector.h"
#include "gadget.h"

#ifndef GADGET_INLINE
#include "poppredatorptrvector.icc"
#endif

PopPredatorPtrVector::PopPredatorPtrVector(int sz) {
  size = (sz > 0 ? sz : 0);
  if (size > 0)
    v = new PopPredator*[size];
  else
    v = 0;
}

PopPredatorPtrVector::PopPredatorPtrVector(int sz, PopPredator* value) {
  size = (sz > 0 ? sz : 0);
  int i;
  if (size > 0) {
    v = new PopPredator*[size];
    for (i = 0; i < size; i++)
      v[i] = value;
  } else
    v = 0;
}

PopPredatorPtrVector::PopPredatorPtrVector(const PopPredatorPtrVector& initial) {
  size = initial.size;
  int i;
  if (size > 0) {
    v = new PopPredator*[size];
    for (i = 0; i < size; i++)
      v[i] = initial.v[i];
  } else
    v = 0;
}

PopPredatorPtrVector::~PopPredatorPtrVector() {
  if (v != 0) {
    delete[] v;
    v = 0;
  }
}

void PopPredatorPtrVector::resize(int addsize, PopPredator* value) {
  int oldsize = size;
  this->resize(addsize);
  int i;
  if (addsize > 0)
    for (i = oldsize; i < size; i++)
      v[i] = value;
}

void PopPredatorPtrVector::resize(int addsize) {
  int i;
  if (v == 0) {
    size = addsize;
    v = new PopPredator*[size];
  } else if (addsize > 0) {
    PopPredator** vnew = new PopPredator*[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
    size += addsize;
  }
}

void PopPredatorPtrVector::Delete(int pos) {
  int i;
  if (size > 1) {
    PopPredator** vnew = new PopPredator*[size - 1];
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
