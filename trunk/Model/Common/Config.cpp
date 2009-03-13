#include "Config.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

  //*** Protected ***
Config::Config(void) {
}

Config* Config::pInstance = 0;// initialize pointer

Config* Config::getInstance () 
{
    if (pInstance == 0)  // is it the first call?
    {  
        pInstance = new Config;
    }
    return pInstance;
}


Config::~Config(void) {
  for (vector<Setting*>::iterator iter = settingData.begin(); iter!=settingData.end(); ++iter) {
    delete *iter;
  }
}

//*** Public ***

void Config::readFile(void) {
  m_strCurrentSection = "";
  string rivi;
  string filename = m_strFilepath.append(m_strFilename);
  ifstream kahva (filename.c_str());
  if (kahva.is_open())
  {
    while (! kahva.eof() )
    {
      getline(kahva, rivi);
      parseRow(rivi);
    }
    kahva.close();
  }
  else cout << "Configuration file \"" << m_strFilename << "\" could not be loaded."; //TODO: where to log errors?
  m_strCurrentSection = "";
}

int Config::getValueAsInt(string in_name) {
  for (vector<Setting*>::iterator iter = settingData.begin(); iter!=settingData.end(); ++iter) {
    if((*iter)->name == in_name) {
      return static_cast<SettingInt*>(*iter)->value;
    }
  }
  return 0;
}

int Config::getValueAsInt(string in_filename, string in_name) {
  for (vector<Setting*>::iterator iter = settingData.begin(); iter!=settingData.end(); ++iter) {
    if((*iter)->name == in_name && (*iter)->file == in_filename) {
      return static_cast<SettingInt*>(*iter)->value;
    }
  }
  return 0;
}

int Config::getValueAsInt(string in_filename, string in_section, string in_name){
  for (vector<Setting*>::iterator iter = settingData.begin(); iter!=settingData.end(); ++iter) {
    if((*iter)->name == in_name && (*iter)->file == in_filename && (*iter)->section == in_section) {
      return static_cast<SettingInt*>(*iter)->value;
    }
  }
  return 0;
}


bool Config::getValueAsBool(string in_name) {
  for (vector<Setting*>::iterator iter = settingData.begin(); iter!=settingData.end(); ++iter) {
    if((*iter)->name == in_name) {
      //return (bool)static_cast<SettingInt*>(*iter)->value;
      return (((SettingInt*)(*iter))->value) ? true : false;
    }
  }
  return false;
}

bool Config::getValueAsBool(string in_filename, string in_name) {
  for (vector<Setting*>::iterator iter = settingData.begin(); iter!=settingData.end(); ++iter) {
    if((*iter)->name == in_name && (*iter)->file == in_filename) {
      //return (bool)static_cast<SettingInt*>(*iter)->value;
      return (((SettingInt*)(*iter))->value) ? true : false;
    }
  }
  return false;
}

bool Config::getValueAsBool(string in_filename, string in_section, string in_name) {
  for (vector<Setting*>::iterator iter = settingData.begin(); iter!=settingData.end(); ++iter) {
    if((*iter)->name == in_name && (*iter)->file == in_filename && (*iter)->section == in_section) {
      //return (bool)static_cast<SettingInt*>(*iter)->value;
      return (((SettingInt*)(*iter))->value) ? true : false;
    }
  }
  return false;
}


string Config::getValueAsString(string in_name) {
  for (vector<Setting*>::iterator iter = settingData.begin(); iter!=settingData.end(); ++iter) {
    if((*iter)->name == in_name) {
      return static_cast<SettingString*>(*iter)->value;
    }
  }
  return "";
}

string Config::getValueAsString(string in_filename, string in_name) {
  for (vector<Setting*>::iterator iter = settingData.begin(); iter!=settingData.end(); ++iter) {
    if((*iter)->name == in_name && (*iter)->file == in_filename) {
      return static_cast<SettingString*>(*iter)->value;
    }
  }
  return "";
}

string Config::getValueAsString(string in_filename, string in_section, string in_name) {
  for (vector<Setting*>::iterator iter = settingData.begin(); iter!=settingData.end(); ++iter) {
    if((*iter)->name == in_name && (*iter)->file == in_filename && (*iter)->section == in_section) {
      return static_cast<SettingString*>(*iter)->value;
    }
  }
  return "";
}


wstring Config::getValueAsWString(string in_name)
{
  std::wstring wstrname;
  for (vector<Setting*>::iterator iter = settingData.begin(); iter!=settingData.end(); ++iter) {
    if((*iter)->name == in_name) {
      string tmpname = static_cast<SettingString*>(*iter)->value;
      wstrname.assign(tmpname.begin(), tmpname.end());
      return wstrname;
    }
  }
  return wstrname;
}
wstring Config::getValueAsWString(string in_filename, string in_name) {
  std::wstring wstrname;
  for (vector<Setting*>::iterator iter = settingData.begin(); iter!=settingData.end(); ++iter) {
    if((*iter)->name == in_name && (*iter)->file == in_filename) {
      string tmpname = static_cast<SettingString*>(*iter)->value;
      wstrname.assign(tmpname.begin(), tmpname.end());
      return wstrname;
    }
  }
  return wstrname;
}

wstring Config::getValueAsWString(string in_filename, string in_section, string in_name) {
  std::wstring wstrname;
  for (vector<Setting*>::iterator iter = settingData.begin(); iter!=settingData.end(); ++iter) {
    if((*iter)->name == in_name && (*iter)->file == in_filename && (*iter)->section == in_section) {
      string tmpname = static_cast<SettingString*>(*iter)->value;
      wstrname.assign(tmpname.begin(), tmpname.end());
      return wstrname;
    }
  }
  return wstrname;
}



bool Config::isNumeric(string str) {
  char merkki;
  bool out = true;
  for(unsigned int i = 0;i < str.length();i++) {
    merkki = str.at(i);
    switch(merkki) {
      case '-':
          if(i != 0) out = false;
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
      case '0':
        //OK!
        break;
      default:
        out = false;
        break;
    }
  }
  return out;
}

string Config::trim(string str) {
  string::size_type position = str.find_last_not_of(' ');
  if(position != string::npos) {
    str.erase(position + 1);
    position = str.find_first_not_of(' ');
    if(position != string::npos) str.erase(0, position);
  }
  else {
    str.erase(str.begin(), str.end());
  }
  return str;
}

int Config::stringToInteger (string str) {
  stringstream ss(str);
  int number;
  if(ss >> number) {
    return number;
  }
  else {
    return -1;
  }
}

void Config::printSettings() {
  for (vector<Setting*>::iterator iter = settingData.begin(); iter!=settingData.end(); ++iter) {
    printSetting(*iter);
  }
}

//*** Private ***

void Config::printSetting(Setting * set) {
    cout << "[" << set->section << "] Nimi: " << set->name << "\tTiedostosta: " << set->file << endl;
  switch(set->type) {
    case VARIABLE_INTEGER:
      cout << "Tyyppi:\t" << "kokonaisluku" << endl;
      cout << "Arvo:\t" << static_cast<SettingInt*>(set)->value << endl << endl;
      break;
    case VARIABLE_STRING:
      cout << "Tyyppi:\t" << "merkkijono" << endl;
      cout << "Arvo:\t" << static_cast<SettingString*>(set)->value << endl << endl;
      break;
  }
}

void Config::addSetting(string in_name, string in_value) {
  deleteSetting(in_name);
  VARIABLE_TYPE tyyppi = VARIABLE_STRING;
  Setting * set = new SettingString();
  static_cast<SettingString*>(set)->name = in_name;
  static_cast<SettingString*>(set)->type = tyyppi;
  static_cast<SettingString*>(set)->value = in_value;
  static_cast<SettingString*>(set)->file = m_strFilename;
  static_cast<SettingString*>(set)->section = m_strCurrentSection;
  settingData.push_back(set);
}

void Config::addSetting(string in_name, int in_value) {
  deleteSetting(in_name);
  VARIABLE_TYPE tyyppi = VARIABLE_INTEGER;
  Setting * set = new SettingString();
  static_cast<SettingInt*>(set)->name = in_name;
  static_cast<SettingInt*>(set)->type = tyyppi;
  static_cast<SettingInt*>(set)->value = in_value;
  static_cast<SettingInt*>(set)->file = m_strFilename;
  static_cast<SettingInt*>(set)->section = m_strCurrentSection;
  settingData.push_back(set);
}

void Config::addSetting(string in_file, string in_section, string in_name, string in_value) {
  deleteSetting(in_name);
  VARIABLE_TYPE tyyppi = VARIABLE_STRING;
  Setting * set = new SettingString();
  static_cast<SettingString*>(set)->name = in_name;
  static_cast<SettingString*>(set)->type = tyyppi;
  static_cast<SettingString*>(set)->value = in_value;
  static_cast<SettingString*>(set)->file = in_file;
  static_cast<SettingString*>(set)->section = in_section;
  settingData.push_back(set);
}

void Config::addSetting(string in_file, string in_section, string in_name, int in_value) {
  deleteSetting(in_name);
  VARIABLE_TYPE tyyppi = VARIABLE_INTEGER;
  Setting * set = new SettingString();
  static_cast<SettingInt*>(set)->name = in_name;
  static_cast<SettingInt*>(set)->type = tyyppi;
  static_cast<SettingInt*>(set)->value = in_value;
  static_cast<SettingInt*>(set)->file = in_file;
  static_cast<SettingInt*>(set)->section = in_section;
  settingData.push_back(set);
}


void Config::deleteSetting(string in_name) {
  for (vector<Setting*>::iterator iter = settingData.begin(); iter!=settingData.end(); ++iter) {
    if((*iter)->name == in_name) {
      //cout << "poistetaan : " << (*iter)->name << endl;
      settingData.erase(iter);
      break; //ei voida enää iteroida koska iteraattori tuhottiin samalla kuin vektorialkio johon se viittasi :D
    }
  }
}

void Config::deleteSetting(string in_filename, string in_name) {
  for (vector<Setting*>::iterator iter = settingData.begin(); iter!=settingData.end(); ++iter) {
    if((*iter)->name == in_name && (*iter)->file == in_filename) {
      settingData.erase(iter);
      break;
    }
  }
}

void Config::deleteSetting(string in_filename, string in_section, string in_name) {
  for (vector<Setting*>::iterator iter = settingData.begin(); iter!=settingData.end(); ++iter) {
    if((*iter)->name == in_name && (*iter)->file == in_filename && (*iter)->section == in_section) {
      settingData.erase(iter);
      break;
    }
  }
}

bool Config::settingExists(string in_name) {
  for (vector<Setting*>::iterator iter = settingData.begin(); iter!=settingData.end(); ++iter) {
    if((*iter)->name == in_name) {
      return true;
    }
  }
  return false;
}

bool Config::settingExists(string in_filename, string in_name) {
  for (vector<Setting*>::iterator iter = settingData.begin(); iter!=settingData.end(); ++iter) {
    if((*iter)->name == in_name && (*iter)->file == in_filename) {
      return true;
    }
  }
  return false;
}

bool Config::settingExists(string in_filename, string in_section, string in_name) {
  for (vector<Setting*>::iterator iter = settingData.begin(); iter!=settingData.end(); ++iter) {
    if((*iter)->name == in_name && (*iter)->file == in_filename && (*iter)->section == in_section) {
      return true;
    }
  }
  return false;
}


bool Config::parseRow(string row) {
  row = trim(row);  
  if(row.length() == 0) {
    //tyhjä rivi, skipataan
    return true;
  }
  else if(row.substr(0,1) == "[") {
    //[osasto]
    m_strCurrentSection = row.substr(1,row.length()-2);
    return true;
  }
  else if(row.substr(0,1) == "#") {
    //#kommentti
    return true;
  }
  else {
  }
  int i;
  for(i = 0;row.substr(i, 1) != "=";i++) {
    if(i > (int)row.length()) {
      return false; //row did not contain = character -> our ini file is not well formed
    }
  }
  string name = trim(row.substr(0, i++));
  string value = trim(row.substr(i, row.length()-i));
  if(isNumeric(value)) {
    addSetting(m_strFilename, m_strCurrentSection, name, stringToInteger(value));
  }
  else {
    addSetting(m_strFilename, m_strCurrentSection, name, value);
  }
  return true;
}

void Config::loadDefaults(void)
{
#pragma region defaultsettings
    addSetting("camera pan up", 200);
    addSetting("camera pan down", 208);
    addSetting("camera pan right", 205);
    addSetting("camera pan left", 203);
    addSetting("camera zoom in", 30);
    addSetting("camera zoom out", 44);
    addSetting("camera reset", 14);
    addSetting("generate new terrain", 57);
    addSetting("generate passability terrain", 2);
    addSetting("toggle wireframe", 15);
    addSetting("terrain detail up", 49);
    addSetting("terrain detail down", 50);
    addSetting("mouse drag button", 1);
    addSetting("mouse pick button", 0);
    addSetting("mouse rotate button", 2);
    addSetting("modify mouse panning horizontal", 10);
    addSetting("modify mouse panning vertical", 10);
    addSetting("modify mouse rotation horizontal", -10);
    addSetting("modify mouse rotation vertical", 10);
    addSetting("modify mouse zoom", 10);
    addSetting("toggle sound", 63);
    addSetting("toggle music", 64);
    addSetting("master volume up", 78);
    addSetting("master volume down", 74);
    addSetting("sound enabled", 1);
    addSetting("music enabled", 1);
    addSetting("master volume", 0);
    addSetting("music volume", 0);
#pragma endregion
}