#include "totalpredator.h"
#include "keeper.h"
#include "conversion.h"
#include "prey.h"
#include "gadget.h"

TotalPredator::TotalPredator(CommentStream& infile, const char* givenname,
  const intvector& Areas, const LengthGroupDivision* const OtherLgrpDiv,
  const LengthGroupDivision* const GivenLgrpDiv, const TimeClass* const TimeInfo,
  Keeper* const keeper, double multi)
  : LengthPredator(givenname, Areas, OtherLgrpDiv, GivenLgrpDiv, multi) {

  keeper->AddString("predator");
  keeper->AddString(givenname);

  ReadSuitabilityMatrix(infile, "amount", TimeInfo, keeper);

  keeper->ClearLast();
  keeper->ClearLast();
  //Predator::SetPrey will call ResizeObjects.
}

TotalPredator::~TotalPredator() {
}

void TotalPredator::Eat(int area, double LengthOfStep, double Temperature,
  double Areasize, int CurrentSubstep, int NrOfSubsteps) {

  //The parameters LengthOfStep, Temperature and Areasize will not be used.
  const int inarea = AreaNr[area];
  double wanttoeat, tmpsteps;
  //indices in for loops:
  int prey, predl, preyl;

  if (CurrentSubstep == 1)
    scaler[inarea] = 0;

  for (predl = 0; predl < LgrpDiv->NoLengthGroups(); predl++) {
    totalcons[inarea][predl] = rathersmall;
    if (CurrentSubstep == 1) {
      totalconsumption[inarea][predl] = 0.0;
      overconsumption[inarea][predl] = 0.0;
      for (prey = 0; prey < NoPreys(); prey++) {
        if (Preys(prey)->IsInArea(area)) {
          for (preyl = Suitability(prey)[predl].Mincol();
              preyl < Suitability(prey)[predl].Maxcol(); preyl++) {
            consumption[inarea][prey][predl][preyl] = 0.0;
          }
        }
      }
    }
  }

  //Calculate consumption up to a multiplicative constant.
  for (prey = 0; prey < NoPreys(); prey++) {
    if (Preys(prey)->IsInArea(area)) {
      if (Preys(prey)->Biomass(area) > 0) {
        for (predl = 0; predl < LgrpDiv->NoLengthGroups(); predl++) {
          for (preyl = Suitability(prey)[predl].Mincol();
              preyl < Suitability(prey)[predl].Maxcol(); preyl++) {
            cons[inarea][prey][predl][preyl]
              = Suitability(prey)[predl][preyl] * Preys(prey)->Biomass(area, preyl);
            totalcons[inarea][predl] += cons[inarea][prey][predl][preyl];
          }
        }
      } else {
        for (predl = 0; predl < LgrpDiv->NoLengthGroups(); predl++) {
          for (preyl = Suitability(prey)[predl].Mincol();
              preyl < Suitability(prey)[predl].Maxcol(); preyl++) {
            cons[inarea][prey][predl][preyl] = 0.0;
          }
        }
      }
    }
  }

  //Adjust the consumption by the multiplicative factor.
  tmpsteps = 1 / NrOfSubsteps;
  for (prey = 0; prey < NoPreys(); prey++) {
    if (Preys(prey)->IsInArea(area)) {
      if (Preys(prey)->Biomass(area) > 0) {
        for (predl = 0; predl < LgrpDiv->NoLengthGroups(); predl++) {
          wanttoeat = Prednumber[inarea][predl].N * Prednumber[inarea][predl].W * tmpsteps;
          for (preyl = Suitability(prey)[predl].Mincol();
              preyl < Suitability(prey)[predl].Maxcol(); preyl++) {
            cons[inarea][prey][predl][preyl] *= wanttoeat / totalcons[inarea][predl];
          }
        }
      }
    }
  }

  //set the multiplicative constant
  for (predl = 0; predl < LgrpDiv->NoLengthGroups(); predl++)
    scaler[inarea] += totalcons[inarea][predl];
  if (CurrentSubstep == NrOfSubsteps)
    if (scaler[inarea] > 0)
      scaler[inarea] = 1.0 / scaler[inarea];

  //Inform the preys of the consumption.
  for (prey = 0; prey < NoPreys(); prey++)
    if (Preys(prey)->IsInArea(area))
      if (Preys(prey)->Biomass(area) > 0)
        for (predl = 0; predl < LgrpDiv->NoLengthGroups(); predl++)
          Preys(prey)->AddConsumption(area, cons[inarea][prey][predl]);

  //Set totalconsumption to the actual total consumption
  for (prey = 0; prey < NoPreys(); prey++)
    if (Preys(prey)->IsInArea(area))
      if (Preys(prey)->Biomass(area) > 0)
        for (predl = 0; predl < LgrpDiv->NoLengthGroups(); predl++)
          totalcons[inarea][predl] = Prednumber[inarea][predl].N *
            Prednumber[inarea][predl].W * tmpsteps;
}

void TotalPredator::AdjustConsumption(int area, int NrOfSubsteps, int CurrentSubstep) {
  double MaxRatioConsumed = pow(MAX_RATIO_CONSUMED, NrOfSubsteps);
  int prey, predl, preyl;
  int AnyPreyEatenUp = 0;
  int AnyPreyOnArea = 0;
  const int inarea = AreaNr[area];
  for (predl = 0; predl < LgrpDiv->NoLengthGroups(); predl++)
    overcons[inarea][predl] = 0.0;

  double ratio, tmp;
  for (prey = 0; prey < NoPreys(); prey++) {
    if (Preys(prey)->IsInArea(area)) {
      if (Preys(prey)->Biomass(area) > 0) {
        AnyPreyOnArea = 1;
        if (Preys(prey)->TooMuchConsumption(area) == 1) {
          AnyPreyEatenUp = 1;
          for (predl = 0; predl < LgrpDiv->NoLengthGroups(); predl++) {
            for (preyl = Suitability(prey)[predl].Mincol();
                preyl < Suitability(prey)[predl].Maxcol(); preyl++) {
              ratio = Preys(prey)->Ratio(area, preyl);
              if (ratio > MaxRatioConsumed) {
                tmp = MaxRatioConsumed / ratio;
                overcons[inarea][predl] += (1 - tmp) * cons[inarea][prey][predl][preyl];
                cons[inarea][prey][predl][preyl] *= tmp;
              }
            }
          }
        }
      }
    }
  }

  tmp = 1 / NrOfSubsteps;
  if (AnyPreyEatenUp == 1)
    for (predl = 0; predl < LgrpDiv->NoLengthGroups(); predl++)
      totalcons[inarea][predl] -= overcons[inarea][predl];

  if (AnyPreyOnArea == 0)
    for (predl = 0; predl < LgrpDiv->NoLengthGroups(); predl++)
      overcons[inarea][predl] = Prednumber[inarea][predl].N *
        Prednumber[inarea][predl].W * tmp;

  //Add to consumption by predator change made after it was possible
  //to divide each timestep in number of parts.
  for (predl = 0; predl < LgrpDiv->NoLengthGroups(); predl++) {
    totalconsumption[inarea][predl] += totalcons[inarea][predl];
    overconsumption[inarea][predl] += overcons[inarea][predl];
  }

  for (prey = 0; prey < NoPreys(); prey++)
    if (Preys(prey)->IsInArea(area))
      if (Preys(prey)->Biomass(area) > 0)
        for (predl = 0; predl < LgrpDiv->NoLengthGroups(); predl++)
          for (preyl = Suitability(prey)[predl].Mincol();
              preyl < Suitability(prey)[predl].Maxcol(); preyl++)
            consumption[inarea][prey][predl][preyl] += cons[inarea][prey][predl][preyl];
}

void TotalPredator::Print(ofstream& outfile) const {
  outfile << "TotalPredator\n";
  PopPredator::Print(outfile);
}

const popinfovector& TotalPredator::NumberPriortoEating(int area, const char* preyname) const {
  int prey;
  for (prey = 0; prey < NoPreys(); prey++)
    if (strcasecmp(Preyname(prey), preyname) == 0)
      return Preys(prey)->NumberPriortoEating(area);

  cerr << "Predator " << this->Name() << " was asked for consumption\n"
    << "of prey " << preyname << " which he does not eat\n";
  exit(EXIT_FAILURE);
}