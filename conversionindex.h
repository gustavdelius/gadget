#ifndef conversionindex_h
#define conversionindex_h

#include "doublevector.h"
#include "intvector.h"
#include "lengthgroup.h"

class ConversionIndex {
public:
  /**
   * \brief This is the ConversionIndex constructor
   * \param L1 is the first LengthGroupDivision
   * \param L2 is the second LengthGroupDivision
   * \param interp is a flag to denote whether the ConversionIndex will be used to interpolate between LengthGroupDivisions (default value 0)
   */
  ConversionIndex(const LengthGroupDivision* const L1,
    const LengthGroupDivision* const L2, int interp = 0);
  /**
   * \brief This is the default ConversionIndex destructor
   */
  ~ConversionIndex() {};
  int Pos(int i) const { return pos[i]; };
  int minLength() const { return minlength; };
  int maxLength() const { return maxlength; };
  int Minpos(int i) const { return minpos[i]; };
  int Maxpos(int i) const { return maxpos[i]; };
  int Nrof(int i) const { return nrof[i]; };
  int Offset() const { return offset; };
  int SameDl() const { return samedl; };
  int TargetIsFiner() const { return targetisfiner; };
  int Size() const { return pos.Size(); };
  void interpolateLengths(DoubleVector& Vf, const DoubleVector& Vc);
protected:
  int targetisfiner;
  /**
   * \brief This is a flag to denote whether the 2 LengthGroupDivisions have the same step length
   */
  int samedl;
  /**
   * \brief This is a flag to denote whether the ConversionIndex will be used to interpolate between LengthGroupDivisions
   */
  int interpolate;
  int offset;
  int minlength;
  int maxlength;
  IntVector pos;
  IntVector nrof;
  IntVector minpos;
  IntVector maxpos;
  DoubleVector interpratio;
  IntVector interppos;
};

#endif
