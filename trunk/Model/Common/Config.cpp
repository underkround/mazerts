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
  //TODO: tutustu voiko lukea tiedostosta jotenkin hienommin
  string rivi;
  ifstream kahva (m_strFilename.c_str());
  if (kahva.is_open())
  {
    while (! kahva.eof() )
    {
      getline(kahva, rivi);
      //cout << rivi << "\n";
      parseRow(rivi);
    }
    kahva.close();
  }
  else cout << "Configuration file \"" << m_strFilename << "\" could not be loaded."; //TODO: where to log errors?
}

int Config::getValueAsInt(string in_name) {
  for (vector<Setting*>::iterator iter = settingData.begin(); iter!=settingData.end(); ++iter) {
    if((*iter)->name == in_name) {
      return static_cast<SettingInt*>(*iter)->value;
    }
  }
  return 0;
}

bool Config::getValueAsBool(string in_name) {
  for (vector<Setting*>::iterator iter = settingData.begin(); iter!=settingData.end(); ++iter) {
    if((*iter)->name == in_name) {
      return (bool)static_cast<SettingInt*>(*iter)->value;
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
  cout << "Nimi:\t" << set->name << endl;
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
  settingData.push_back(set);
}

void Config::addSetting(string in_name, int in_value) {
  deleteSetting(in_name);
  VARIABLE_TYPE tyyppi = VARIABLE_INTEGER;
  Setting * set = new SettingString();
  static_cast<SettingInt*>(set)->name = in_name;
  static_cast<SettingInt*>(set)->type = tyyppi;
  static_cast<SettingInt*>(set)->value = in_value;
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

bool Config::settingExists(string in_name) {
  for (vector<Setting*>::iterator iter = settingData.begin(); iter!=settingData.end(); ++iter) {
    if((*iter)->name == in_name) {
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
    addSetting(name, stringToInteger(value));
  }
  else {
    addSetting(name, value);
  }
  return true;
}
