#include "sibyageonstep.h"
#include "stock.h"
#include "areatime.h"
#include "loglinearregression.h"
#include "stockaggregator.h"
#include "gadget.h"

SIByAgeOnStep::SIByAgeOnStep(CommentStream& infile, const IntVector& areas,
  const IntMatrix& ages, const char* arealabel, const CharPtrVector& ageindex,
  const TimeClass* const TimeInfo, const char* datafilename)
  : SIOnStep(infile, datafilename, arealabel, TimeInfo, ageindex),
    aggregator(0), Ages(ages), Areas(areas) {
}

SIByAgeOnStep::~SIByAgeOnStep() {
  delete aggregator;
}

void SIByAgeOnStep::SetStocks(const StockPtrVector& Stocks) {

  /* This function initializes aggregator. It should:
   *  merge all the areas in Areas
   *  merge all length groups of the stocks
   *  merge all the stocks in Stocks
   *  merge all the age groups in Ages[i]
   * This means that aggregator.ReturnSum will return an
   * AgeBandMatrixPtrVector with only one element. That element will have
   * Ages.Nrow() lines (i.e. ages) and only 1 column (i.e. 1 length group)*/

  IntMatrix areamatrix(1, Areas.Size());
  int i;
  for (i = 0; i < Areas.Size(); i++)
    areamatrix[0][i] = Areas[i];

  //create a LengthGroupDivision for aggregator to use.
  double minlength = Stocks[0]->ReturnLengthGroupDiv()->Minlength(0);
  double maxlength = minlength;
  double stockminlength, stockmaxlength;
  for (i = 0; i < Stocks.Size(); i++) {
    stockminlength = Stocks[i]->ReturnLengthGroupDiv()->Minlength(0);
    stockmaxlength = Stocks[i]->ReturnLengthGroupDiv()->Maxlength(Stocks[i]->ReturnLengthGroupDiv()->NoLengthGroups() - 1);
    if (stockminlength < minlength)
      minlength = stockminlength;
    if (maxlength < stockmaxlength)
      maxlength = stockmaxlength;
  }

  LengthGroupDivision* LgrpDiv = new LengthGroupDivision(minlength, maxlength, maxlength - minlength);
  aggregator = new StockAggregator(Stocks, LgrpDiv, areamatrix, Ages);
}

void SIByAgeOnStep::Sum(const TimeClass* const TimeInfo) {
  if (!(this->IsToSum(TimeInfo)))
    return;
  aggregator->Sum();
  //Use that the AgeBandMatrixPtrVector aggregator->ReturnSum returns has only one element.
  //Copy the information from it -- we only want to keep the abundance numbers.
  const AgeBandMatrix* Alptr = &(aggregator->ReturnSum()[0]);
  int maxage = Alptr->Maxage();
  DoubleVector numbers(maxage + 1);
  int age;
  for (age = 0; age <= maxage; age++)
    numbers[age] = (*Alptr)[age][0].N;
  this->KeepNumbers(numbers);
}
