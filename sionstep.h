#ifndef sionstep_h
#define sionstep_h

#include "doublematrix.h"
#include "intmatrix.h"
#include "charptrvector.h"
#include "commentstream.h"
#include "actionattimes.h"
#include "keeper.h"
#include "agebandmatrix.h"
#include "gadget.h"

class SIOnStep;
class TimeClass;

class SIOnStep {
public:
  SIOnStep(CommentStream& infile, const char* datafilename, const char* arealabel,
    const TimeClass* const TimeInfo, int numcols,
    const CharPtrVector& index1, const CharPtrVector& index2);
  SIOnStep(CommentStream& infile, const char* datafilename, const char* arealabel,
    const TimeClass* const TimeInfo, const CharPtrVector& colindex);
  virtual ~SIOnStep();
  virtual void Sum(const TimeClass* const TimeInfo) = 0;
  virtual void SetStocks(const StockPtrVector& Stocks) = 0;
  int Error() const { return error; };
  void Clear() { error = 0; };
  virtual double Regression();
  virtual void Reset(const Keeper* const keeper);
  virtual void Print(ofstream& outfile) const;
  virtual void LikelihoodPrint(ofstream& outfile) const;
  virtual void printMoreLikInfo(ofstream& outfile) const {};
  virtual void printHeader(ofstream&, const PrintInfo& print, const char* name) {};
  virtual void print(ofstream&, const TimeClass& time, const PrintInfo&) {};
  virtual void PrintLikelihood(ofstream&, const TimeClass& time, const char*) {};
  virtual void PrintLikelihoodHeader(ofstream&, const char*) {};
protected:
  void SetError() { error = 1; };
  int IsToSum(const TimeClass* const TimeInfo) const;
  void KeepNumbers(const DoubleVector& numbers);
  IntVector Years;
  IntVector Steps;
  IntVector YearsInFile;
  ActionAtTimes AAT;
  //JMB - made Indices and abundance protected
  DoubleMatrix Indices;
  DoubleMatrix abundance;
  enum FitType { LogLinearFit = 0, FixedSlopeLogLinearFit, FixedLogLinearFit, LinearFit, PowerFit,
    FixedSlopeLinearFit, FixedLinearFit, FixedInterceptLinearFit, FixedInterceptLogLinearFit };
  FitType getFitType() { return fittype; };
private:
  void ReadSIData(CommentStream&, const char*, const CharPtrVector&, const CharPtrVector&, const TimeClass*);
  void ReadSIData(CommentStream&, const char*, const CharPtrVector&, const TimeClass*);
  double Fit(const DoubleVector& stocksize, const DoubleVector& indices, int col);
  int NumberOfSums;
  FitType fittype;
  double slope;
  double intercept;
  //Additions to facilitate printing
  DoubleVector slopes;
  DoubleVector intercepts;
  DoubleVector sse;
  int error;
};

#endif
