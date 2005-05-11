#include "stock.h"
#include "keeper.h"
#include "areatime.h"
#include "naturalm.h"
#include "grower.h"
#include "stockprey.h"
#include "stockpredator.h"
#include "initialcond.h"
#include "migration.h"
#include "readfunc.h"
#include "mathfunc.h"
#include "maturity.h"
#include "renewal.h"
#include "transition.h"
#include "spawner.h"
#include "stray.h"
#include "errorhandler.h"
#include "gadget.h"

extern ErrorHandler handle;

/* In this file most of the member functions for the class stock are
 * defined. Most of them are rather simple, call member functions of
 * AgeLengthKeys etc. Migration function in stock.
 * Migration->Getmatrix returns a pointer to migrationlist. */

void Stock::Migrate(const TimeClass* const TimeInfo) {
  //age dependent migrations should also be here.
  if (doesmigrate) {
    Alkeys.Migrate(migration->MigrationMatrix(TimeInfo));
    if (tagAlkeys.numTagExperiments() > 0)
      tagAlkeys.Migrate(migration->MigrationMatrix(TimeInfo), Alkeys);
  }
}

//-------------------------------------------------------------------
//Sum up into Growth+Predator and Prey Lengthgroups.  Storage is a
//vector of PopInfo that stores the sum over each lengthgroup.
void Stock::calcNumbers(int area, const AreaClass* const Area, const TimeClass* const TimeInfo) {
  int i, inarea = this->areaNum(area);
  Alkeys[inarea].sumColumns(NumberInArea[inarea]);
  if (doesgrow)
    grower->Sum(NumberInArea[inarea], area);
  if (iseaten) {
    prey->Sum(Alkeys[inarea], area, TimeInfo->getSubStep());
    for (i = 0; i < allTags.Size(); i++)
      allTags[i]->storeNumberPriorToEating(area, this->getName());
  }
  if (doeseat)
    ((StockPredator*)predator)->Sum(Alkeys[inarea], area);
}

//-------------------------------------------------------------------
void Stock::calcEat(int area, const AreaClass* const Area, const TimeClass* const TimeInfo) {
  if (doeseat)
    predator->Eat(area, Area, TimeInfo);
}

void Stock::checkEat(int area, const TimeClass* const TimeInfo) {
  if (iseaten)
    prey->checkConsumption(area, TimeInfo->numSubSteps());
}

void Stock::adjustEat(int area, const TimeClass* const TimeInfo) {
  if (doeseat)
    predator->adjustConsumption(area, TimeInfo);
}

//-------------------------------------------------------------------
void Stock::reducePop(int area, const TimeClass* const TimeInfo) {

  //Predation
  if (iseaten)
    prey->Subtract(Alkeys[this->areaNum(area)], area);

  //Natural mortality
  if (TimeInfo->numSubSteps() == 1) {
    Alkeys[this->areaNum(area)].Multiply(naturalm->getProportion());

  } else {
    //changed to include the possibility of substeps
    DoubleVector* PropSurviving;
    PropSurviving = new DoubleVector(naturalm->getProportion());
    double timeratio = 1.0 / TimeInfo->numSubSteps();

    int i;
    for (i = 0; i < PropSurviving->Size(); i++)
      (*PropSurviving)[i] = pow((*PropSurviving)[i], timeratio);

    Alkeys[this->areaNum(area)].Multiply(*PropSurviving);
    delete PropSurviving;
  }

  if (tagAlkeys.numTagExperiments() > 0)
    tagAlkeys[this->areaNum(area)].updateAndTagLoss(Alkeys[this->areaNum(area)], tagAlkeys.getTagLoss());
}

//-----------------------------------------------------------------------
//Function that updates the length distributions and makes part of the stock Mature.
void Stock::Grow(int area, const AreaClass* const Area, const TimeClass* const TimeInfo) {

  if (!doesgrow)
    return;

  if (doeseat)
    grower->GrowthCalc(area, Area, TimeInfo, ((StockPredator*)predator)->FPhi(area),
      ((StockPredator*)predator)->maxConByLength(area));
  else
    grower->GrowthCalc(area, Area, TimeInfo);

  int inarea = this->areaNum(area);
  if (grower->getFixedWeights()) {
    //Weights at length are fixed to the value in the input file
    grower->GrowthImplement(area, LgrpDiv);
    if (doesmature) {
      if (maturity->isMaturationStep(area, TimeInfo)) {
        Alkeys[inarea].Grow(grower->increaseLength(area), grower->getWeight(area), maturity, TimeInfo, Area, area);
        if (tagAlkeys.numTagExperiments() > 0)
          tagAlkeys[inarea].Grow(grower->increaseLength(area), Alkeys[inarea], maturity, TimeInfo, Area, area);
      } else {
        Alkeys[inarea].Grow(grower->increaseLength(area), grower->getWeight(area));
        if (tagAlkeys.numTagExperiments() > 0)
          tagAlkeys[inarea].Grow(grower->increaseLength(area), Alkeys[inarea]);
      }
    } else {
      Alkeys[inarea].Grow(grower->increaseLength(area), grower->getWeight(area));
      if (tagAlkeys.numTagExperiments() > 0)
        tagAlkeys[inarea].Grow(grower->increaseLength(area), Alkeys[inarea]);
    }

  } else {
    //New weights at length are calculated
    grower->GrowthImplement(area, NumberInArea[inarea], LgrpDiv);
    if (doesmature) {
      if (maturity->isMaturationStep(area, TimeInfo)) {
        Alkeys[inarea].Grow(grower->increaseLength(area), grower->increaseWeight(area), maturity, TimeInfo, Area, area);
        if (tagAlkeys.numTagExperiments() > 0)
          tagAlkeys[inarea].Grow(grower->increaseLength(area), Alkeys[inarea], maturity, TimeInfo, Area, area);
      } else {
        Alkeys[inarea].Grow(grower->increaseLength(area), grower->increaseWeight(area));
        if (tagAlkeys.numTagExperiments() > 0)
          tagAlkeys[inarea].Grow(grower->increaseLength(area), Alkeys[inarea]);
      }
    } else {
      Alkeys[inarea].Grow(grower->increaseLength(area), grower->increaseWeight(area));
      if (tagAlkeys.numTagExperiments() > 0)
        tagAlkeys[inarea].Grow(grower->increaseLength(area), Alkeys[inarea]);
    }
  }
}

//-----------------------------------------------------------------------
//A number of Special functions, Spawning, Renewal, Maturation and
//Transition to other Stocks, Maturity due to age and increased age.
void Stock::updateAgePart1(int area, const TimeClass* const TimeInfo) {
  if (doesmove)
    if (transition->isTransitionStep(area, TimeInfo))
      transition->keepAgeGroup(area, Alkeys[this->areaNum(area)], tagAlkeys[this->areaNum(area)], TimeInfo);
}

void Stock::updateAgePart2(int area, const TimeClass* const TimeInfo) {
  if (this->Birthday(TimeInfo)) {
    Alkeys[this->areaNum(area)].IncrementAge();
    if (tagAlkeys.numTagExperiments() > 0)
      tagAlkeys[this->areaNum(area)].IncrementAge(Alkeys[this->areaNum(area)]);
  }
}

void Stock::updateAgePart3(int area, const TimeClass* const TimeInfo) {
  if (doesmove) {
    if (transition->isTransitionStep(area, TimeInfo)) {
      updateTransitionStockWithTags(TimeInfo);
      transition->Move(area, TimeInfo);
    }
  }
}

void Stock::updatePopulationPart1(int area, const TimeClass* const TimeInfo) {
  if (doesspawn) {
    if (spawner->isSpawnStepArea(area, TimeInfo)) {
      spawner->Spawn(Alkeys[this->areaNum(area)], area, TimeInfo);
      if (tagAlkeys.numTagExperiments() > 0)
        tagAlkeys[this->areaNum(area)].updateNumbers(Alkeys[this->areaNum(area)]);
    }
  }
}

void Stock::updatePopulationPart2(int area, const TimeClass* const TimeInfo) {
  if (doesmature) {
    if (maturity->isMaturationStep(area, TimeInfo)) {
      updateMatureStockWithTags(TimeInfo);
      maturity->Move(area, TimeInfo);
    }
  }
}

void Stock::updatePopulationPart3(int area, const TimeClass* const TimeInfo) {
  if (doesrenew) {
    if (renewal->isRenewalStepArea(area, TimeInfo)) {
      renewal->addRenewal(Alkeys[this->areaNum(area)], area, TimeInfo);
      if (tagAlkeys.numTagExperiments() > 0)
        tagAlkeys[this->areaNum(area)].updateRatio(Alkeys[this->areaNum(area)]);
    }
  }

  if (doesspawn)
    if (spawner->isSpawnStepArea(area, TimeInfo))
      spawner->addSpawnStock(area, TimeInfo);
}

void Stock::updatePopulationPart4(int area, const TimeClass* const TimeInfo) {
  if (doesstray)
    if (stray->isStrayStepArea(area, TimeInfo))
      stray->storeStrayingStock(area, Alkeys[this->areaNum(area)], tagAlkeys[this->areaNum(area)], TimeInfo);
}

void Stock::updatePopulationPart5(int area, const TimeClass* const TimeInfo) {
  if (doesstray) {
    if (stray->isStrayStepArea(area, TimeInfo)) {
      updateStrayStockWithTags(TimeInfo);
      stray->addStrayStock(area, TimeInfo);
    }
  }
}

void Stock::updateMatureStockWithTags(const TimeClass* const TimeInfo) {
  int i;
  for (i = 0; i < matureTags.Size(); i++)
    matureTags[i]->updateMatureStock(TimeInfo);
  matureTags.deleteAll();
}

void Stock::updateTransitionStockWithTags(const TimeClass* const TimeInfo) {
  int i;
  for (i = 0; i < transitionTags.Size(); i++)
    transitionTags[i]->updateTransitionStock(TimeInfo);
  transitionTags.deleteAll();
}

void Stock::updateStrayStockWithTags(const TimeClass* const TimeInfo) {
  int i;
  for (i = 0; i < strayTags.Size(); i++)
    strayTags[i]->updateStrayStock(TimeInfo);
  strayTags.deleteAll();
}

void Stock::Add(const AgeBandMatrix& Addition, const ConversionIndex* const CI,
  int area, double ratio, int MinAge, int MaxAge) {

  Alkeys[this->areaNum(area)].Add(Addition, *CI, ratio, MinAge, MaxAge);
}

void Stock::Add(const AgeBandMatrixRatioPtrVector& Addition, int AddArea, const ConversionIndex* const CI,
  int area, double ratio, int MinAge, int MaxAge) {

  if ((Addition.numTagExperiments() > 0) && (Addition.numTagExperiments() <= tagAlkeys.numTagExperiments())) {
    AgebandmratioAdd(tagAlkeys, this->areaNum(area), Addition, AddArea, *CI, ratio, MinAge, MaxAge);
    tagAlkeys[this->areaNum(area)].updateRatio(Alkeys[this->areaNum(area)]);
  }
}

void Stock::calcMigration(const TimeClass* const TimeInfo) {
  if (doesmigrate)
    migration->MigrationRecalc(TimeInfo->getYear());
}

void Stock::updateTags(AgeBandMatrixPtrVector* tagbyagelength, Tags* newtag, double tagloss) {

  tagAlkeys.addTag(tagbyagelength, Alkeys, newtag->getName(), tagloss);
  allTags.resize(1, newtag);
  if (doesmature) {
    maturity->addMaturityTag(newtag->getName());
    matureTags.resize(1, newtag);
  }
  if (doesmove) {
    transition->addTransitionTag(newtag->getName());
    transitionTags.resize(1, newtag);
  }
  if (doesstray) {
    stray->addStrayTag(newtag->getName());
    strayTags.resize(1, newtag);
  }
}

void Stock::deleteTags(const char* tagname) {
  allTags.Delete(tagAlkeys.getTagID(tagname));
  tagAlkeys.deleteTag(tagname);
  if (doesmature)
    maturity->deleteMaturityTag(tagname);
  if (doesmove)
    transition->deleteTransitionTag(tagname);
  if (doesstray)
    stray->deleteStrayTag(tagname);
}
