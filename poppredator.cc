#include "poppredator.h"
#include "gadget.h"

PopPredator::PopPredator(const char* givenname, const intvector& Areas,
  const LengthGroupDivision* const OtherLgrpDiv, const LengthGroupDivision* const GivenLgrpDiv)
  : Predator(givenname, Areas) {

  int i;
  if (GivenLgrpDiv->dl() != 0)
    LgrpDiv = new LengthGroupDivision(*GivenLgrpDiv);

  else {
    doublevector dv(GivenLgrpDiv->NoLengthGroups() + 1);
    for (i = 0; i < dv.Size() - 1; i++)
      dv[i] = GivenLgrpDiv->Minlength(i);
    dv[i] = GivenLgrpDiv->Maxlength(i - 1);
    LgrpDiv = new LengthGroupDivision(dv);
  }
  CI = new ConversionIndex(OtherLgrpDiv, LgrpDiv);
}

PopPredator::~PopPredator() {
  delete LgrpDiv;
  delete CI;
}

void PopPredator::Print(ofstream& outfile) const {
  Predator::Print(outfile);
  int i, area;

  outfile << "\tMeanlengths of length groups ";
  for (i = 0; i < LgrpDiv->NoLengthGroups(); i++) {
    outfile.precision(smallprecision);
    outfile.width(printwidth);
    outfile << sep << LgrpDiv->Meanlength(i);
  }
  outfile << endl;
  for (area = 0; area < areas.Size(); area++) {
    outfile << "\tNumber of predator on area " << areas[area];
    for (i = 0; i < LgrpDiv->NoLengthGroups(); i++) {
      outfile.precision(smallprecision);
      outfile.width(printwidth);
      outfile << sep << Prednumber[area][i].N;
    }
    outfile << "\n\tWeight of predator on area " << areas[area];
    for (i = 0; i < LgrpDiv->NoLengthGroups(); i++) {
      outfile.precision(smallprecision);
      outfile.width(printwidth);
      outfile << sep << Prednumber[area][i].W;
    }
    outfile << "\n\tTotal amount eaten on area " << areas[area];
    for (i = 0; i < LgrpDiv->NoLengthGroups(); i++) {
      outfile.precision(smallprecision);
      outfile.width(printwidth);
      outfile << sep << totalconsumption[area][i];
    }
    outfile << "\n\tOverconsumption on area    " << areas[area];
    for (i = 0; i < LgrpDiv->NoLengthGroups(); i++) {
      outfile.precision(smallprecision);
      outfile.width(printwidth);
      outfile << sep << overconsumption[area][i];
    }
    outfile << endl;
  }
}

const doublevector& PopPredator::Consumption(int area) const {
  return totalconsumption[AreaNr[area]];
}

const bandmatrix& PopPredator::Consumption(int area, const char* preyname) const {
  int prey;
  for (prey = 0; prey < NoPreys(); prey++)
    if (strcasecmp(Preyname(prey), preyname) == 0)
      return consumption[AreaNr[area]][prey];

  cerr << "Predator " << this->Name() << " was asked for consumption\n"
    << "of prey " << preyname << " which he does not eat\n";
  exit(EXIT_FAILURE);
}

const double PopPredator::consumedBiomass(int prey_nr, int area_nr) const{

  int age, len;
  double tons = 0.0;
  const bandmatrix& bio = consumption[area_nr][prey_nr];
  for (age = bio.Minage(); age <= bio.Maxage(); age++)
    for (len = bio.Minlength(age); len < bio.Maxlength(age); len++)
      tons += bio[age][len];

  return tons;
}

const doublevector& PopPredator::OverConsumption(int area) const {
  return overconsumption[AreaNr[area]];
}

const LengthGroupDivision* PopPredator::ReturnLengthGroupDiv() const {
  return LgrpDiv;
}

void PopPredator::DeleteParametersForPrey(int prey, Keeper* const keeper) {
  Predator::DeleteParametersForPrey(prey, keeper);
}

int PopPredator::NoLengthGroups() const {
  return LgrpDiv->NoLengthGroups();
}

double PopPredator::Length(int i) const {
  return LgrpDiv->Meanlength(i);
}

void PopPredator::Reset(const TimeClass* const TimeInfo) {
  this->Predator::Reset(TimeInfo);
  //Now the matrices Suitability(prey) are of the correct size.
  //We must adjust the size of consumption accordingly.
  int i, j, area, prey;
  for (area = 0; area < areas.Size(); area++) {
    for (prey = 0; prey < NoPreys(); prey++) {
      if (this->DidChange(prey, TimeInfo)) {
        //Adjust the size of consumption[area][prey].
        cons.ChangeElement(area, prey, Suitability(prey));
        consumption.ChangeElement(area, prey, Suitability(prey));
        for (i = 0; i < Suitability(prey).Nrow(); i++)
          for (j = consumption[area][prey].Mincol(i);
              j < consumption[area][prey].Maxcol(i); j++)
            consumption[area][prey][i][j] = 0.0;
      }
    }
  }
  if (TimeInfo->CurrentTime() == 1) {
    for (area = 0; area < areas.Size(); area++) {
      for (i = 0; i < LgrpDiv->NoLengthGroups(); i++) {
        Prednumber[area][i].N = 0.0;
        Prednumber[area][i].W = 0.0;
        overconsumption[area][i] = 0;
        totalconsumption[area][i] = 0.0;
      }
    }
  }
}

void PopPredator::ResizeObjects() {
  Predator::ResizeObjects();
  while (consumption.Nrow())
    consumption.DeleteRow(0);
  while (cons.Nrow())
    cons.DeleteRow(0);
  while (totalconsumption.Nrow())
    totalcons.DeleteRow(0);
  while (totalcons.Nrow())
    totalconsumption.DeleteRow(0);
  while (Prednumber.Nrow())
    Prednumber.DeleteRow(0);

  popinfo nullpop;
  //Add rows to matrices and initialize.
  int numareas = areas.Size();
  int numlengths = LgrpDiv->NoLengthGroups();
  cons.AddRows(numareas, NoPreys());
  totalcons.AddRows(numareas, numlengths, 0.0);
  overcons.AddRows(numareas, numlengths, 0);
  consumption.AddRows(numareas, NoPreys());
  totalconsumption.AddRows(numareas, numlengths, 0.0);
  overconsumption.AddRows(numareas, numlengths, 0);
  Prednumber.AddRows(numareas, numlengths, nullpop);
}

void PopPredator::Multiply(Agebandmatrix& stock_alkeys, const doublevector& ratio) {
  //written by kgf 31/7 98
  //Note! ratio is supposed to have equal dimensions to PopPredator.
  stock_alkeys.Multiply(ratio, *CI);
}