#include "maturity.h"
#include "stock.h"
#include "mathfunc.h"
#include "readfunc.h"
#include "conversion.h"
#include "print.h"
#include "errorhandler.h"
#include "gadget.h"

Maturity::Maturity(const intvector& tmpareas, int minage, const intvector& minlength,
  const intvector& size, const LengthGroupDivision* const lgrpdiv)
  : LivesOnAreas(tmpareas), LgrpDiv(new LengthGroupDivision(*lgrpdiv)),
    Storage(tmpareas.Size(), minage, minlength, size) {
    TagStorage.resize(tmpareas.Size(), minage, minlength, size);

}

Maturity::~Maturity() {
  int i;
  for (i = 0; i < NameOfMatureStocks.Size(); i++)
    delete[] NameOfMatureStocks[i];
  for (i = 0; i < CI.Size(); i++)
    delete CI[i];
  delete LgrpDiv;
}

void Maturity::SetStock(Stockptrvector& stockvec) {
  int index = 0;
  int i, j;
  doublevector tmpratio;

  for (i = 0; i < stockvec.Size(); i++)
    for (j = 0; j < NameOfMatureStocks.Size(); j++)
      if (strcasecmp(stockvec[i]->Name(), NameOfMatureStocks[j]) == 0) {
        MatureStocks.resize(1);
        tmpratio.resize(1);
        MatureStocks[index] = stockvec[i];
        tmpratio[index] = Ratio[j];
        index++;
      }

  if (index != NameOfMatureStocks.Size()) {
    cerr << "Error: Did not find the stock(s) matching:\n";
    for (i = 0; i < NameOfMatureStocks.Size(); i++)
      cerr << (const char*)NameOfMatureStocks[i] << sep;
    cerr << "\nwhen searching for mature stock(s) - found only:\n";
    for (i = 0; i < stockvec.Size(); i++)
      cerr << stockvec[i]->Name() << sep;
    cerr << endl;
    exit(EXIT_FAILURE);
  }

  //Now we clean Ratio and then copy from tmpratio to Ratio.
  for (i = Ratio.Size(); i > 0; i--)
    Ratio.Delete(0);
  Ratio.resize(tmpratio.Size());
  for (i = 0; i < tmpratio.Size(); i++)
    Ratio[i] = tmpratio[i];
  //Set the conversion index.
  CI.resize(MatureStocks.Size(), 0);
  for (i = 0; i < MatureStocks.Size(); i++)
    CI[i] = new ConversionIndex(LgrpDiv, MatureStocks[i]->ReturnLengthGroupDiv());
}

void Maturity::Print(ofstream& outfile) const {
  int i;
  outfile << "\nMaturity\n\tRead names of mature stocks:";
  for (i = 0; i < NameOfMatureStocks.Size(); i++)
    outfile << sep << (const char*)(NameOfMatureStocks[i]);
  outfile << "\n\tNames of mature stocks (through pointers):";
  for (i = 0; i < MatureStocks.Size(); i++)
    outfile << sep << (const char*)(MatureStocks[i]->Name());
  outfile << "\n\tStored numbers:\n";
  for (i = 0; i < areas.Size(); i++) {
    outfile << "Area " << areas[i] << endl;
    Printagebandm(outfile, Storage[i]);
  }
  outfile << endl;
}

//Move Immature stock to mature stocks.  Ratio is read in.  Ratio is
//not set to 1/MatureStocks.size for sometimes the mature stocks
//only have different age composition. The class has special
//conversionindex for each mature stock.
void Maturity::Move(int area, const TimeClass* const TimeInfo) {
  assert(this->IsMaturationStep(area, TimeInfo));
  const int inarea = AreaNr[area];
  int i;
  for (i = 0; i < MatureStocks.Size(); i++) {
    MatureStocks[i]->Add(Storage[inarea], CI[i], area, Ratio[i],
      Storage[inarea].Minage(), Storage[inarea].Maxage());

    if (TagStorage.NrOfTagExp() > 0)
      MatureStocks[i]->Add(TagStorage, inarea, CI[i], area, Ratio[i],
        TagStorage[inarea].Minage(), TagStorage[inarea].Maxage());
  }
  Storage[inarea].SettoZero();
}

//Put fish that becomes mature in temporary storage.  Later in the
//simulation the fish is moved from that storage to the Mature stock.
void Maturity::PutInStorage(int area, int age, int length, double number,
  double weight, const TimeClass* const TimeInfo) {

  assert(this->IsMaturationStep(area, TimeInfo));
  Storage[AreaNr[area]][age][length].N = (number > 0 ? number : 0.0);
  Storage[AreaNr[area]][age][length].W = (weight > 0 ? weight : 0.0);
}

//Put fish which becomes mature from tagging experiement with identity = id
//into temporary storage. Number of tagging experiments set must be greater
//then zero and 0 <= id < number of tagging experiments.
void Maturity::PutInStorage(int area, int age, int length, double number,
  const TimeClass* const TimeInfo, int id) {

  assert(this->IsMaturationStep(area, TimeInfo));
  if (TagStorage.NrOfTagExp() <= 0) {
    cerr << "Error in tagging maturity - no tagging experiment\n";
    exit(EXIT_FAILURE);
  } else if ((id >= TagStorage.NrOfTagExp()) || (id < 0)) {
    cerr << "Error in tagging maturity - illegal tagging experiment id\n";
    exit(EXIT_FAILURE);
  } else
    *(TagStorage[AreaNr[area]][age][length][id].N) = (number > 0.0 ? number: 0.0);
}

void Maturity::Precalc(const TimeClass* const TimeInfo) {
  int area, age, l, tag;
  if (TimeInfo->CurrentTime() == 1) {
    for (area = 0; area < areas.Size(); area++) {
      for (age = Storage[area].Minage(); age <= Storage[area].Maxage(); age++) {
        for (l = Storage[area].Minlength(age); l < Storage[area].Maxlength(age); l++) {
          Storage[area][age][l].N = 0.0;
          Storage[area][age][l].W = 0.0;
          for (tag = 0; tag < TagStorage.NrOfTagExp(); tag++)
            *(TagStorage[area][age][l][tag].N) = 0.0;
        }
      }
    }
  }
}

const Stockptrvector& Maturity::GetMatureStocks() {
  return MatureStocks;
}

// Add a new tagging experiment with name = tagname to tagStorage.
// This tagging experiment has id = number of tagging experiment before adding the new tag.
// For all areas, ages and lengths in TagStorage have added a new tag with number of fish = -1.0 and ratio = -1.0, or
// TagStorage[area][age][length][id].N = -1.0,
// TagStorage[area][age][length][id].R = -1.0
// New memory has been allocated for the double TagStorage[area][age][length].N.
void Maturity::AddTag(const char* tagname) {
  TagStorage.addTag(tagname);
}

void Maturity::DeleteTag(const char* tagname) {
  int minage, maxage, minlen, maxlen, age, length, i;
  int id = TagStorage.getId(tagname);

  if (id >= 0) {
    minage = TagStorage[0].Minage();
    maxage = TagStorage[0].Maxage();

    // Remove allocated memory
    for (i = 0; i < TagStorage.Size(); i++) {
      for (age = minage; age <= maxage; age++) {
        minlen = TagStorage[i].Minlength(age);
        maxlen = TagStorage[i].Maxlength(age);
        for (length = minlen; length < maxlen; length++) {
          delete [] (TagStorage[i][age][length][id].N);
          (TagStorage[i][age][length][id].N) = NULL;
        }
      }
    }
    TagStorage.deleteTag(tagname);

  } else {
    cerr << "Error in tagging maturity - trying to delete tag with name: "
      << tagname << " in maturity but there is not any tag with that name\n";
  }
}