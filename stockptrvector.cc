#include "stockptrvector.h"
#include "gadget.h"

#ifndef GADGET_INLINE
#include "stockptrvector.icc"
#endif

Stockptrvector::Stockptrvector(int sz) {
  size = (sz > 0 ? sz : 0);
  if (size > 0)
    v = new Stock*[size];
  else
    v = 0;
}

Stockptrvector::Stockptrvector(int sz, Stock* value) {
  size = (sz > 0 ? sz : 0);
  int i;
  if (size > 0) {
    v = new Stock*[size];
    for (i = 0; i < size; i++)v[i] = value;
  } else
    v = 0;
}

Stockptrvector::Stockptrvector(const Stockptrvector& initial) {
  size = initial.size;
  int i;
  if (size > 0) {
    v = new Stock*[size];
    for (i = 0; i < size; i++)
      v[i] = initial.v[i];
  } else
    v = 0;
}

Stockptrvector::~Stockptrvector() {
  if (v != 0) {
    delete[] v;
    v = 0;
  }
}

//The function resize add addsize elements to a Stockptrvector and fills it vith value.
void Stockptrvector::resize(int addsize, Stock* value) {
  int oldsize = size;
  this->resize(addsize);
  int i;
  if (addsize > 0)
    for (i = oldsize; i < size; i++)
      v[i] = value;
}

void Stockptrvector::resize(int addsize) {
  int i;
  if (v == 0) {
    size = addsize;
    v = new Stock*[size];
  } else if (addsize > 0) {
    Stock** vnew = new Stock*[addsize + size];
    for (i = 0; i < size; i++)
      vnew[i] = v[i];
    delete[] v;
    v = vnew;
    size += addsize;
  }
}

void Stockptrvector::Delete(int pos) {
  assert(size > 0);
  assert(0 <= pos && pos < size);
  Stock** vnew = new Stock*[size - 1];
  int i;
  for (i = 0; i < pos; i++)
    vnew[i] = v[i];
  for (i = pos; i < size - 1; i++)
    vnew[i] = v[i + 1];
  delete[] v;
  v = vnew;
  size--;
}

Stockptrvector& Stockptrvector::operator = (const Stockptrvector& d) {
  if (this == &d)
    return(*this);
  int i;
  if (size == d.size) {
    for (i = 0; i < size; i++)
      v[i] = d[i];
    return(*this);
  }
  if (v != 0) {
    delete [] v;
    v = 0;
  }
  size = d.size;
  if (size > 0) {
    v = new Stock*[size];
    for (i = 0; i < size; i++)
      v[i] = d.v[i];
  } else
    v = 0;
  return *this;
}