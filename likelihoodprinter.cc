#include "likelihoodprinter.h"
#include "likelihoodptrvector.h"
#include "conversionindex.h"
#include "stockaggregator.h"
#include "areatime.h"
#include "readfunc.h"
#include "readword.h"
#include "errorhandler.h"
#include "mortprey.h"
#include "stock.h"
#include "lenstock.h"
#include "catchdistribution.h"
#include "surveydistribution.h"
#include "stomachcontent.h"
#include "formatedprinting.h"
#include "runid.h"
#include "gadget.h"

extern RunID RUNID;
extern ErrorHandler handle;

/*  LikelihoodPrinter
 *
 *  Purpose:  Constructor
 *
 *  In:  CommentStream& infile      :Input file
 *       AreaClass* Area            :Area defenition
 *       TimeClass* TimeInfo        :Time defenition
 *
 *  Usage:  LikelihoodPrinter(infile, Area, Time)
 *
 *  Pre:  infile, Area, & Time are valid according to StockPrinter documentation.
 */
LikelihoodPrinter::LikelihoodPrinter(CommentStream& infile,
  const AreaClass* const AreaInfo, const TimeClass* const TimeInfo)
  : Printer(LIKELIHOODPRINTER) {

  char filename[MaxStrLength];
  char text[MaxStrLength];
  strncpy(filename, "", MaxStrLength);
  strncpy(text, "", MaxStrLength);

  printCatch = 0;
  printSurvey = 0;
  printStomach = 0;
  printtimeid = 1; //not used
  infile >> text >> ws;
  while (!(strcasecmp(text, "[component]")==0) && !infile.eof()) {
    if ((strcasecmp(text, "catchfile") == 0)) {
      infile >> filename >> ws;
      catchFile.open(filename, ios::out);
      handle.checkIfFailure(catchFile, filename);
      RUNID.print(catchFile);
      catchFile.flush();
      printCatch = 1;
    } else if ((strcasecmp(text, "surveyfile") == 0)) {
      infile >> filename >> ws;
      surveyFile.open(filename, ios::out);
      handle.checkIfFailure(surveyFile, filename);
      RUNID.print(surveyFile);
      surveyFile.flush();
      printSurvey = 1;
    } else if ((strcasecmp(text, "stomachfile") == 0)) {
      infile >> filename >> ws;
      stomachFile.open(filename, ios::out);
      handle.checkIfFailure(stomachFile, filename);
      RUNID.print(stomachFile);
      stomachFile.flush();
      printStomach = 1;
    } else {
      handle.Unexpected("catchfile, surveyfile, or stomachfile", text);
    }
    infile >> text >> ws;
  }
}

void LikelihoodPrinter::setLikely(LikelihoodPtrVector& likevec) {
  int i;
  for (i = 0; i < likevec.Size(); i++) {
    if (likevec[i]->Type() == CATCHDISTRIBUTIONLIKELIHOOD)
      catchvec.resize(1, likevec[i]);
    else if (likevec[i]->Type() == SURVEYDISTRIBUTIONLIKELIHOOD)
      surveyvec.resize(1, likevec[i]);
    else if (likevec[i]->Type() == STOMACHCONTENTLIKELIHOOD)
      stomachvec.resize(1, likevec[i]);
  }

  if (printCatch) {
    catchFile << "Likelihood printing: Catch\nLikelihood components in this file: " << catchvec.Size() << "\n\n";
    for (i = 0; i < catchvec.Size(); i++) {
      ((CatchDistribution*)catchvec[i])->PrintLikelihoodHeader(catchFile);
      catchFile << endl;
      catchFile.flush();
    }
  }
  if (printSurvey) {
    surveyFile << "Likelihood printing: Survey\nLikelihood components in this file: " << surveyvec.Size() << "\n\n";
    for (i = 0; i < surveyvec.Size(); i++) {
      ((SurveyDistribution*)surveyvec[i])->PrintLikelihoodHeader(surveyFile);
      surveyFile << endl;
      surveyFile.flush();
    }
  }
  if (printStomach) {
    stomachFile << "Likelihood printing: Stomach\nLikelihood components in this file: " << stomachvec.Size() << "\n\n";
    for (i = 0; i < stomachvec.Size(); i++) {
      ((StomachContent*)stomachvec[i])->PrintLikelihoodHeader(stomachFile);
      stomachFile << endl;
      stomachFile.flush();
    }
  }
}

/*  Print
 *
 *  Purpose:  Print stocks according to configuration in constructor.
 *
 *  In:  TimeClass& TimeInfo         :Current time
 *
 *  Usage:  Print(TimeInfo)
 *
 *  Pre:  setStock has been called
 */
void LikelihoodPrinter::Print(const TimeClass* const TimeInfo, int printtime) {

  if (printtime == 0)
    return;

  int i;
  if (printCatch) {
    for (i = 0; i < catchvec.Size(); i++)
      ((CatchDistribution*)catchvec[i])->PrintLikelihood(catchFile, *TimeInfo);
    catchFile.flush();
  }

  if (printSurvey) {
    for (i = 0; i < surveyvec.Size(); i++)
      ((SurveyDistribution*)surveyvec[i])->PrintLikelihood(surveyFile, *TimeInfo);
    surveyFile.flush();
  }

  if (printStomach) {
    for (i = 0; i < stomachvec.Size(); i++)
      ((StomachContent*)stomachvec[i])->PrintLikelihood(stomachFile, *TimeInfo);
    stomachFile.flush();
  }
}

/*  ~LikelihoodPrinter
 *
 *  Purpose:  Destructor
 */
LikelihoodPrinter::~LikelihoodPrinter() {
  if (printStomach) {
    stomachFile.close();
    stomachFile.clear();
  }
  if (printSurvey) {
    surveyFile.close();
    surveyFile.clear();
  }
  if (printCatch) {
    catchFile.close();
    catchFile.clear();
  }
}
