#include "surveyindices.h"
#include "readfunc.h"
#include "readword.h"
#include "readaggregation.h"
#include "areatime.h"
#include "errorhandler.h"
#include "stock.h"
#include "sibylengthonstep.h"
#include "sibylengthandageonstep.h"
#include "sibyageonstep.h"
#include "gadget.h"

SurveyIndices::SurveyIndices(CommentStream& infile, const AreaClass* const Area,
  const TimeClass* const TimeInfo, Keeper* const keeper, double likweight, const char* name)
  : Likelihood(SURVEYINDICESLIKELIHOOD, likweight) {

  ErrorHandler handle;
  char text[MaxStrLength];
  strncpy(text, "", MaxStrLength);
  int i;

  intmatrix Ages, tmpareas;
  doublevector Lengths;
  charptrvector areaindex;
  charptrvector ageindex;
  charptrvector lenindex;

  char datafilename[MaxStrLength];
  char aggfilename[MaxStrLength];
  char sitype[MaxStrLength];
  strncpy(datafilename, "", MaxStrLength);
  strncpy(aggfilename, "", MaxStrLength);
  strncpy(sitype, "", MaxStrLength);

  ifstream datafile;
  CommentStream subdata(datafile);

  surveyname = new char[strlen(name) + 1];
  strcpy(surveyname, name);
  ReadWordAndValue(infile, "datafile", datafilename);
  ReadWordAndValue(infile, "sitype", sitype);

  //Read in area aggregation from file
  ReadWordAndValue(infile, "areaaggfile", aggfilename);
  datafile.open(aggfilename);
  CheckIfFailure(datafile, aggfilename);
  handle.Open(aggfilename);
  i = ReadAggregation(subdata, tmpareas, areaindex);
  handle.Close();
  datafile.close();
  datafile.clear();

  //Check if we read correct input
  if (tmpareas.Nrow() != 1)
    handle.Message("Error - there should be only one area for the survey indices");

  for (i = 0; i < tmpareas.Ncol(0); i++)
    areas.resize(1, tmpareas[0][i]);

  //Check area data
  for (i = 0; i < areas.Size(); i++)
    if ((areas[i] = Area->InnerArea(areas[i])) == -1)
      handle.UndefinedArea(areas[i]);

  if (strcasecmp(sitype, "lengths") == 0) {
    ReadWordAndValue(infile, "lenaggfile", aggfilename);
    datafile.open(aggfilename);
    CheckIfFailure(datafile, aggfilename);
    handle.Open(aggfilename);
    i = ReadLengthAggregation(subdata, Lengths, lenindex);
    handle.Close();
    datafile.close();
    datafile.clear();

  } else if (strcasecmp(sitype, "ages") == 0) {
    ReadWordAndValue(infile, "ageaggfile", aggfilename);
    datafile.open(aggfilename);
    CheckIfFailure(datafile, aggfilename);
    handle.Open(aggfilename);
    i = ReadAggregation(subdata, Ages, ageindex);
    handle.Close();
    datafile.close();
    datafile.clear();

  } else if (strcasecmp(sitype, "ageandlengths") == 0) {
    ReadWordAndValue(infile, "lenaggfile", aggfilename);
    datafile.open(aggfilename);
    CheckIfFailure(datafile, aggfilename);
    handle.Open(aggfilename);
    i = ReadLengthAggregation(subdata, Lengths, lenindex);
    handle.Close();
    datafile.close();
    datafile.clear();

    ReadWordAndValue(infile, "ageaggfile", aggfilename);
    datafile.open(aggfilename);
    CheckIfFailure(datafile, aggfilename);
    handle.Open(aggfilename);
    i = ReadAggregation(subdata, Ages, ageindex);
    handle.Close();
    datafile.close();
    datafile.clear();

  } else
    handle.Unexpected("lengths, ages or ageandlengths", sitype);

  //Read in the stocknames
  i = 0;
  infile >> text >> ws;
  if (!(strcasecmp(text, "stocknames") == 0))
    handle.Unexpected("stocknames", text);
  infile >> text;
  while (!infile.eof() && !(strcasecmp(text, "fittype") == 0)) {
    stocknames.resize(1);
    stocknames[i] = new char[strlen(text) + 1];
    strcpy(stocknames[i++], text);
    infile >> text >> ws;
  }

  //We have now read in all the data from the main likelihood file
  if (strcasecmp(sitype, "lengths") == 0) {
    SI = new SIByLengthOnStep(infile, areas, Lengths, areaindex[0],
      lenindex, TimeInfo, datafilename);

  } else if (strcasecmp(sitype, "ages") == 0) {
    SI = new SIByAgeOnStep(infile, areas, Ages, areaindex[0],
      ageindex, TimeInfo, datafilename);

  } else if (strcasecmp(sitype, "ageandlengths") == 0) {
    SI = new SIByLengthAndAgeOnStep(infile, areas, Lengths, Ages, TimeInfo,
      keeper, lenindex, ageindex, areaindex[0], datafilename);

  } else
    handle.Message("Error in surveyindices - unrecognised type", sitype);

  //prepare for next likelihood component
  infile >> ws;
  if (!infile.eof()) {
    infile >> text >> ws;
    if (!(strcasecmp(text, "[component]") == 0))
      handle.Unexpected("[component]", text);
  }

  //index ptrvectors are not required - free up memory
  for (i = 0; i < areaindex.Size(); i++)
    delete[] areaindex[i];
  for (i = 0; i < ageindex.Size(); i++)
    delete[] ageindex[i];
  for (i = 0; i < lenindex.Size(); i++)
    delete[] lenindex[i];
}

SurveyIndices::~SurveyIndices() {
  int i;
  for (i = 0; i < stocknames.Size(); i++)
    delete[] stocknames[i];
  delete SI;
  delete[] surveyname;
}

void SurveyIndices::LikelihoodPrint(ofstream& outfile) const {
  int i;
  outfile << "\nSurvey Indices\nlikelihood " << likelihood
    << "\nStocknames: ";
  for (i = 0; i < stocknames.Size(); i++)
    outfile << stocknames[i] << sep;
  outfile << endl;
  Likelihood::LikelihoodPrint(outfile);
  outfile << "Areas\n";
  for (i = 0; i < areas.Size(); i++)
    outfile << areas[i] << TAB;
  outfile << endl;
  SI->LikelihoodPrint(outfile);
}

void SurveyIndices::AddToLikelihood(const TimeClass* const TimeInfo) {
  likelihood = 0;
  SI->Sum(TimeInfo);
  if (TimeInfo->CurrentTime() == TimeInfo->TotalNoSteps())
    likelihood += SI->Regression();
}

void SurveyIndices::SetStocks(Stockptrvector& Stocks) {
  int i, j;
  int found = 0;
  Stockptrvector s;
  for (i = 0; i < stocknames.Size(); i++) {
    found = 0;
    for (j = 0; j < Stocks.Size(); j++)
      if (strcasecmp(stocknames[i], Stocks[j]->Name()) == 0) {
        found = 1;
        s.resize(1, Stocks[j]);
      }
    if (found == 0) {
      cerr << "Error when searching for names of stocks for survey indices.\n"
        << "Did not find any name matching " << stocknames[i] << endl;
      exit(EXIT_FAILURE);
    }
  }
  SI->SetStocks(s);
}

void SurveyIndices::Reset(const Keeper* const keeper) {
  SI->Reset(keeper);
  Likelihood::Reset(keeper);
}

void SurveyIndices::Print(ofstream& outfile) const {
  int i;
  outfile << "\nSurvey Indices " << surveyname << "\nlikelihood " << likelihood << "\nStocknames: ";
  for (i = 0; i < stocknames.Size(); i++)
    outfile << stocknames[i] << sep;
  outfile << endl;
  SI->Print(outfile);
}