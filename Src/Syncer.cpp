#include <Syncer.h>

ClassImp(Syncer);

//////////

Syncer::Syncer(const TString& a_file_list) : file_list(a_file_list)
{
  fout = new TFile("GEM_Aging_Synced_Tree.root", "RECREATE");

  data_tree = new TTree("T", "Synced data for GEM aging");

  data_tree->Branch("Year", &year);
  data_tree->Branch("Month", &month);
  data_tree->Branch("Day", &day);

  data_tree->Branch("Hour", &hour);
  data_tree->Branch("Minute", &minute);
  data_tree->Branch("Second", &second);

  data_tree->Branch("Mean of RO current", &ro_mean);
  data_tree->Branch("Standard deviation of RO current", &ro_std);

  data_tree->Branch("Temperature", &temperature);
  data_tree->Branch("Pressure", &pressure);
  data_tree->Branch("Pressure", &relative_humidity);

  Read_Files();
}//Syncer::Syncer(const TString& a_list)

//////////

Syncer::~Syncer()
{
  fout->cd();
  data_tree->Write();
  fout->Close();
}//Syncer::~Syncer()

//////////

void Syncer::Read_Files()
{
  ifstream fin_list;
  fin_list.open(file_list);

  if(!fin_list.is_open())
    {
      cerr << "Can not find file list!" << endl; 

      return;
    }
  
  string buf;
  while(!fin_list.eof())
    {
      getline(fin_list, buf);
      if(buf.compare("")==0) break;

      cout << buf << endl;

      //read data file for Env. data
      stringstream ss;
      ss.str(buf);

      string buf1;
      getline(ss, buf1, '-');
      year = stoi(buf, nullptr);
      
      getline(ss, buf1, '-');
      month  = stoi(buf, nullptr);

      getline(ss, buf1, '_');
      day  = stoi(buf, nullptr);

      TString path_env_data = "../Data/Data_Env/";
      path_env_data += to_string(year) + "_" + to_string(month) +;
      
      //read data file for readout current data
      TString path_ro_current_data = "../Data/Data_Readout_Current/";
      path_ro_current_data += buf;

      ifstream fin_ro_current_data;
      fin_ro_current_data.open(path_ro_current_data);

      if(!fin_ro_current_data.is_open())
	{
	  cerr << "Can not find " << buf << endl;

	  return;
	}

      while(!fin_ro_current_data.eof())
	{
	  getline(fin_ro_current_data, buf);
	  if(buf.compare("")==0) break;

	  stringstream ss;
	  ss.str(buf);

	  getline(ss, buf, ' ');
	  
	  getline(ss, buf, ':');
	  hour  = stoi(buf, nullptr);

	  getline(ss, buf, ':');
	  minute = stoi(buf, nullptr);

	  getline(ss, buf, ',');
	  second = round(stof(buf, nullptr));
	  
	  getline(ss, buf, ',');
	  ro_mean = stof(buf, nullptr);

	  getline(ss, buf, ',');

	  getline(ss, buf, ',');
	  ro_std = stof(buf, nullptr);

	  
	}//wile over ro current data
      
    }//while over file list
  
  return;
}//Syncer::Read_Files()

//////////
