#ifndef transition_h
#define transition_h

#include "areatime.h"
#include "conversionindex.h"
#include "commentstream.h"
#include "agebandmatrixptrvector.h"
#include "agebandmatrixratioptrvector.h"
#include "stock.h"

class Keeper;

/**
 * \class Transition
 * \brief This is the class used to calculate the movement from a young stock to older stocks
 */
class Transition : protected LivesOnAreas {
public:
  /**
   * \brief This is the Transition constructor
   * \param infile is the CommentStream to read the transition data from
   * \param areas is the list of areas that the movements will be calculated on
   * \param age is the maximum age for the (young) stock
   * \param lgrpdiv is the LengthGroupDivision for the stock
   * \param keeper is the Keeper for the current model
   */
  Transition(CommentStream& infile, const IntVector& areas, int age,
    const LengthGroupDivision* const lgrpdiv, Keeper* const keeper);
  /**
   * \brief This is the default Transition destructor
   */
  ~Transition();
  /**
   * \brief This will select the stocks required for the movement between stocks
   * \param stockvec is the StockPtrVector listing all the available stocks
   */
  void SetStock(StockPtrVector& stockvec);
  /**
   * \brief This will store the part of the younger stock that will move into the older stocks
   * \param area is the area that the maturation is being calculated on
   * \param Alkeys is the AgeBandMatrix of the stock that fish will move from
   * \param TagAlkeys is the AgeBandMatrixRatio of the tagged stock that fish will move from
   * \param TimeInfo is the TimeClass for the current model
   */
  void KeepAgegroup(int area, AgeBandMatrix& Alkeys, AgeBandMatrixRatio& TagAlkeys, const TimeClass* const TimeInfo);
  /**
   * \brief This will move the younger stock into the older stock age-length cells
   * \param area is the area that the movement is being calculated on
   * \param TimeInfo is the TimeClass for the current model
   */
  void Move(int area, const TimeClass* const TimeInfo);
  /**
   * \brief This function will print the transition information
   * \param outfile is the ofstream that all the model information gets sent to
   */
  void Print(ofstream& outfile) const;
  /**
   * \brief This will calculate the number of stocks the young stock can move into
   * \return number of transition stocks
   */
  int NoOfTransitionStocks() { return TransitionStocks.Size(); };
  /**
   * \brief This will calculate the stocks the young stock can move into
   * \return transition stocks
   */
  const StockPtrVector& getTransitionStocks();
  /**
   * \brief This will add a tagging experiment to the transition calculations
   * \param tagname is the name of the tagging experiment
   */
  void addTransitionTag(const char* tagname);
  /**
   * \brief This will remove a tagging experiment from the transition calculations
   * \param tagname is the name of the tagging experiment
   */
  void deleteTransitionTag(const char* tagname);
  /**
   * \brief This function will reset the transition data
   */
  void Reset();
protected:
  /**
   * \brief This is the StockPtrVector of the stocks that the young stock will move to
   */
  StockPtrVector TransitionStocks;
  /**
   * \brief This is the CharPtrVector of the names of the transition stocks
   */
  CharPtrVector TransitionStockNames;
  /**
   * \brief This is the vector of the ratio of the young stock to move into each older stock
   */
  DoubleVector Ratio;
  /**
   * \brief This is the ConversionIndexPtrVector used to convert from the young stock lengths to the old stock lengths
   */
  ConversionIndexPtrVector CI;
  /**
   * \brief This is the LengthGroupDivision of the stock
   */
  LengthGroupDivision* LgrpDiv;
  /**
   * \brief This is the timestep that the movement between stocks will occur on
   */
  int TransitionStep;
  /**
   * \brief This is the AgeBandMatrixPtrVector used to store the calculated old stocks
   */
  AgeBandMatrixPtrVector AgeGroup;
  /**
   * \brief This is the AgeBandMatrixRatioPtrVector used to store the calculated tagged stocks
   */
  AgeBandMatrixRatioPtrVector TagAgeGroup;
  /**
   * \brief This is the age that the younger stock will move into the older stocks
   */
  int age;
  /**
   * \brief This is the minimum length group of the younger stock that exists in the older stock
   */
  int minLength;
  /**
   * \brief This is the mortality that the transition process can induce in the stock
   */
  Formula mortality;
};

#endif
