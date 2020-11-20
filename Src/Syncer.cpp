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
  data_tree->Branch("Relative Humidity", &relative_humidity);

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

string Syncer::Construct_Path_Env_Data(const Int_t& mode)
{
  string path = "../Data/Data_Env/";

  tm tm_buf = tm_date;
  
  if(mode==0) --tm_buf.tm_mday;
  else if(mode==2) ++tm_buf.tm_mday;

  year_env_data = tm_buf.tm_year + 1900;
  month_env_data = tm_buf.tm_mon + 1;
  day_env_data = tm_buf.tm_mday;
  
  path += to_string(year_env_data) + "_";
  path += string(2 - to_string(month_env_data).length(), '0') + to_string(month_env_data) + "_";
  path += string(2 - to_string(day_env_data).length(), '0') + to_string(day_env_data) + ".dat";
  
  return path;
}//string Syncer::Construct_Path_Env_Data(const Int_t& mode)

//////////

void Syncer::Construct_TM()
{
  tm_date.tm_year = year - 1900;
  tm_date.tm_mon = month-1;
  tm_date.tm_mday = day;
  tm_date.tm_hour = 0;
  tm_date.tm_min = 0;
  tm_date.tm_sec = 0;
  tm_date.tm_isdst = 0;
  
  mktime(&tm_date);
  
  return;
}//void Syncer::Construct_TM()

//////////

Int_t Syncer::Get_Synced_Env_Data(const Int_t& index)
{
  tm tm_ro_current;
  tm_ro_current.tm_year = year - 1900;
  tm_ro_current.tm_mon = month - 1;
  tm_ro_current.tm_mday = day;
  tm_ro_current.tm_hour = hour;
  tm_ro_current.tm_min = minute;
  tm_ro_current.tm_sec = second;
  tm_ro_current.tm_isdst = 0;

  time_t time_ro_current = mktime(&tm_ro_current);

  time_t time_env_data_prev;
  time_t time_env_data_current;

  Int_t i = index - 10;
  Int_t i_prev;
  if(i<0) i = 0;  

  for(i; i<vec_env_data.size(); i++)
    {
      tm tm_env_data;
      tm_env_data.tm_year = vec_env_data[i].year - 1900;
      tm_env_data.tm_mon = vec_env_data[i].month - 1;
      tm_env_data.tm_mday = vec_env_data[i].day;
      tm_env_data.tm_hour = vec_env_data[i].hour;
      tm_env_data.tm_min = vec_env_data[i].min;
      tm_env_data.tm_sec = vec_env_data[i].sec;
      tm_env_data.tm_isdst = 0;

      time_env_data_current = mktime(&tm_env_data);
      
      if(time_ro_current < time_env_data_current)
	{
	  //linear approximation
	  Int_t time_diff_numer = time_ro_current - time_env_data_prev;
	  Int_t time_diff_deno = time_env_data_current - time_env_data_prev;

	  Env_Data env_data_curr = vec_env_data[i];
	  Env_Data env_data_prev = vec_env_data[i_prev];

	  Float_t temperature_diff = env_data_curr.temperature - env_data_prev.temperature;
	  Float_t pressure_diff = env_data_curr.pressure - env_data_prev.pressure;
	  Float_t rh_diff = env_data_curr.relative_humidity - env_data_prev.relative_humidity;

	  temperature = env_data_prev.temperature + temperature_diff*time_diff_numer/time_diff_deno;
	  pressure = env_data_prev.pressure + pressure_diff*time_diff_numer/time_diff_deno;
	  relative_humidity = env_data_prev.relative_humidity + rh_diff*time_diff_numer/time_diff_deno;

	  // cout << time_env_data_prev << " " << time_ro_current << " " << time_env_data_current << endl;

	  // cout << time_diff_numer << " " << time_diff_deno << endl;
	  
	  // cout << env_data_prev.temperature << " " << env_data_curr.temperature << endl;
	  // cout << env_data_prev.pressure << " " << env_data_curr.pressure << endl;
	  // cout << env_data_prev.relative_humidity << " " << env_data_curr.relative_humidity << endl;
	  // cout << temperature << " " << pressure << " " << relative_humidity << endl; 
	  // cout << endl;
	  
	  break;
	}
      else
	{
	  time_env_data_prev = time_env_data_current;
	  i_prev = i;
	}
    }
  
  return i;
}//Int_t Syncer::Get_Synced_Env_Data(const Int_t& index)

//////////

void Syncer::Parsing_Date(stringstream& ss)
{
  string buf;

  getline(ss, buf, '-');
  year = stoi(buf, nullptr);

  getline(ss, buf, '-');
  month = stoi(buf, nullptr);

  getline(ss, buf, '_');
  day  = stoi(buf, nullptr);
  
  return;
}//void Syncer::Parsing_Date(const stringstream& ss)

//////////

void Syncer::Parsing_Env_Data(stringstream& ss)
{
  string buf;

  env_data.year = year_env_data;
  env_data.month = month_env_data;
  env_data.day = day_env_data;
  
  getline(ss, buf, ':');
  env_data.hour = stoi(buf, nullptr);

  getline(ss, buf, ':');
  env_data.min = stoi(buf, nullptr);

  getline(ss, buf, ',');
  env_data.sec = stoi(buf, nullptr);

  getline(ss, buf, ',');
  env_data.temperature = stof(buf, nullptr);

  getline(ss, buf, ',');
  env_data.pressure = stof(buf, nullptr);

  getline(ss, buf, ' ');
  env_data.relative_humidity = stof(buf, nullptr);

  //cout << hour << " " << minute << " " << second << " " << temperature << " " << pressure <<" " << relative_humidity << endl;
  
  return;
}//void Syncer::Parsing_Env_Data(stringstream& ss)

//////////

void Syncer::Parsing_RO_Current(stringstream& ss)
{
  string buf;
  
  getline(ss, buf, ' ');

  getline(ss, buf, ':');
  hour = stoi(buf, nullptr);

  getline(ss, buf, ':');
  minute = stoi(buf, nullptr);

  getline(ss, buf, ',');
  second = round(stof(buf, nullptr));

  getline(ss, buf, ',');
  ro_mean = stof(buf, nullptr);

  getline(ss, buf, ',');

  getline(ss, buf, ',');
  ro_std = stof(buf, nullptr);
  
  return;
}//void Syncer::Parsing_RO_Current(const stringstream& ss)

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

      //read data files for Env. data
      stringstream ss;
      ss.str(buf);

      //get current date
      Parsing_Date(ss);

      Construct_TM();

      //read date file for env data; yesterday, today and tomorrow
      //and fill vec_env_data
      vec_env_data.clear();
      
      for(Int_t i=0; i<3; i++)
	{
	  TString path_env_data = Construct_Path_Env_Data(i);

	  cout << path_env_data << endl;

	  ifstream fin_env_data;
	  fin_env_data.open(path_env_data);

	  if(!fin_env_data.is_open()) cerr << "Can not find " << path_env_data << endl;
	  else
	    {
	      while(!fin_env_data.eof())
		{
		  string buf;

		  getline(fin_env_data, buf);
		  if(buf.compare("")==0) break;

		  stringstream ss;
		  ss.str(buf);

		  Parsing_Env_Data(ss);

		  vec_env_data.push_back(env_data);
		}
	    }
	}
      
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

      Int_t index = 0;
      while(!fin_ro_current_data.eof())
	{
	  getline(fin_ro_current_data, buf);
	  if(buf.compare("")==0) break;

	  stringstream ss;
	  ss.str(buf);

	  Parsing_RO_Current(ss);

	  index = Get_Synced_Env_Data(index);

	  data_tree->Fill();
	}//wile over ro current data
    }//while over file list
  
  return;
}//Syncer::Read_Files()

//////////
