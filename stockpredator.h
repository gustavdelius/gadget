#ifndef stockpredator_h
#define stockpredator_h

#include "areatime.h"
#include "lengthgroup.h"
#include "commentstream.h"
#include "agebandmatrixptrvector.h"
#include "poppredator.h"

class StockPredator : public PopPredator {
public:
  StockPredator(CommentStream& infile, const char* givenname, const IntVector& areas,
    const LengthGroupDivision* const OtherLgrpDiv, const LengthGroupDivision* const GivenLgrpDiv,
    int minage, int maxage, const TimeClass* const TimeInfo, Keeper* const keeper);
  virtual ~StockPredator() {};
  virtual void Sum(const AgeBandMatrix& Alkeys, int area);
  virtual void Eat(int area, double LengthOfStep, double Temperature,
    double Areasize, int CurrentSubstep, int numsubsteps);
  virtual const PopInfoVector& getNumberPriorToEating(int area, const char* preyname) const;
  virtual void adjustConsumption(int area, int numsubsteps, int CurrentSubstep);
  virtual void Print(ofstream& outfile) const;
  const BandMatrix& Alproportion(int area) const { return Alprop[this->areaNum(area)]; };
  const DoubleVector& FPhi(int area) const { return fphi[this->areaNum(area)]; };
  const DoubleVector& maxConByLength(int area) const { return maxconbylength[this->areaNum(area)]; };
  virtual void Reset(const TimeClass* const TimeInfo);
protected:
  virtual void calcMaxConsumption(double Temperature, int inarea, int CurrentSubstep,
    int numsubsteps, double LengthOfStep);
  double maxConsumption(double Length, const FormulaVector &maxcon, double Temperature);
  FormulaVector maxconsumption;
  Formula halfFeedingValue;
  DoubleMatrix Phi;  //[area][predLengthgroup]
  DoubleMatrix fphi; //[area][predLengthgroup]
  DoubleMatrix fphI; //[area][predLengthgroup]  fphi per substep
  BandMatrixPtrVector Alprop;     //[area][age][length group]
  DoubleMatrix maxconbylength; //[area][length group]
  AgeBandMatrixPtrVector Alkeys;  //[area][age][length group]
};

#endif
