#ifndef __Syncer_h__
#define __Syncer_h__

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <ctime>

#include <TObject.h>
#include <TString.h>
#include <TFile.h>
#include <TTree.h>

namespace namespace_Env_Data
{
      typedef struct _Env_Data
      {
	Int_t year;
	Int_t month;
	Int_t day;
	Int_t hour;
	Int_t min;
	Int_t sec;
	Float_t temperature;
	Float_t pressure;
	Float_t relative_humidity;
      } Env_Data;
}

using namespace std;
using namespace namespace_Env_Data;

class Syncer : public TObject
{
 public:
  Syncer(const TString& a_file_list);
  ~Syncer();
  
 protected:
  TString file_list;

  namespace_Env_Data::Env_Data env_data;
  vector<namespace_Env_Data::Env_Data> vec_env_data;
  
  TTree* data_tree;

  Int_t year;
  Int_t month;
  Int_t day;

  Int_t year_env_data;
  Int_t month_env_data;
  Int_t day_env_data;
  
  tm tm_date;

  Int_t hour;
  Int_t minute;
  Int_t second;

  Float_t ro_mean;
  Float_t ro_std;

  Float_t temperature;
  Float_t pressure;
  Float_t relative_humidity;
  
  TFile* fout;

  string Construct_Path_Env_Data(const Int_t& mode);
  void Construct_TM();
  Int_t Get_Synced_Env_Data(const Int_t& index);
  void Parsing_Date(stringstream& ss);
  void Parsing_Env_Data(stringstream& ss);
  void Parsing_RO_Current(stringstream& ss);
  void Read_Files();
  
  ClassDef(Syncer, 1);
};

#endif /*__Syncer_h__ */
