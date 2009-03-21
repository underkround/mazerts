#include "Config.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

//*** Protected ***

Config::Config(void) {
}

//Config* Config::pInstance = 0;// initialize pointer

Config* Config::getInstance () 
{
/*
 * NOTE: this singleton style leaks memory if not explicitly released :(
 *
    if (pInstance == 0)  // is it the first call?
    {  
        pInstance = new Config;
    }
    return pInstance;
 */
    // ..this does not :)
    static Config instance;
    return &instance;
}


Config::~Config(void) {
    Setting* set;
    for (vector<Setting*>::iterator iter = settingData.begin(); iter!=settingData.end(); ++iter) {
        set = (*iter);
        deleteValueNodes(set);
        delete *iter;
    }
}

//#ifdef DEBUG
void Config::printSettings() {
  for (vector<Setting*>::iterator iter = settingData.begin(); iter!=settingData.end(); ++iter) {
    printSetting(*iter);
  }
}
//#endif

//#ifdef DEBUG
void Config::printSetting(Setting * set) {
    cout << "[" << set->section << "] Nimi: " << set->name << "\tTiedostosta: " << set->file << endl;
    switch(set->type) {

        case VARIABLE_INTEGER: {
            SettingInt* temp = static_cast<SettingInt*>(set);
            if(set->count == 1) {
                cout << "Tyyppi:\t" << "kokonaisluku" << endl;
                cout << "Arvo:\t" << temp->valueNode->value << endl << endl;
            } else {
                cout << "Tyyppi:\t" << "kokonaislukutaulukko" << endl;
                cout << "Arvot:" << endl;
                IntNode* node = temp->valueNode;
                while(node) {
                    cout << "\t" << node->value << endl;
                    node = node->next;
                }
                cout << endl;
            }
            break;
        }

        case VARIABLE_STRING: {
            SettingString* temp = static_cast<SettingString*>(set);
            if(set->count == 1) {
                cout << "Tyyppi:\t" << "merkkijono" << endl;
                cout << "Arvo:\t" << temp->valueNode->value << endl << endl;
            } else {
                cout << "Tyyppi:\t" << "merkkijonotaulukko" << endl;
                cout << "Arvot:" << endl;
                StringNode* node = temp->valueNode;
                while(node) {
                    cout << "\t" << node->value << endl;
                    node = node->next;
                }
                cout << endl;
            }
            break;
        }

        case VARIABLE_FLOAT: {
            SettingFloat* temp = static_cast<SettingFloat*>(set);
            if(set->count == 1) {
                cout << "Tyyppi:\t" << "liukuluku" << endl;
                cout << "Arvo:\t" << temp->valueNode->value << endl << endl;
            } else {
                cout << "Tyyppi:\t" << "liukulukutaulukko" << endl;
                cout << "Arvot:" << endl;
                FloatNode* node = temp->valueNode;
                while(node) {
                    cout << "\t" << node->value << endl;
                    node = node->next;
                }
                cout << endl;
            }
            break;
        }

    } // switch
}
//#endif // DEBUG




//*** Public ***

void Config::readFile(void) {
    m_strCurrentSection = "";
    string rivi;
    string filename = m_strFilepath.append(m_strFilename);
    ifstream kahva (filename.c_str());
    if (kahva.is_open()) {
        while (! kahva.eof() ) {
            getline(kahva, rivi);
            parseRow(rivi);
        }
        kahva.close();
    }
#ifdef DEBUG
    else cout << "Configuration file \"" << m_strFilename << "\" could not be loaded."; //TODO: where to log errors?
#endif
    m_strCurrentSection = "";
}

//////////////////////////////////////////////////////////////////////////////
// Int - method #1

int Config::getValueAsInt(const string in_name, const int defaultValue) {
    IntNode* node = getIntNode("", "", in_name);
    return (node) ? node->value : defaultValue;
}

int Config::getValueAsInt(const string in_filename, const string in_section, string in_name, const int defaultValue){
    IntNode* node = getIntNode(in_filename, in_section, in_name);
    return (node) ? node->value : defaultValue;
}

// Int - method #2

bool Config::updateInt(const string in_name, int& value) {
    IntNode* node = getIntNode("", "", in_name);
    if (node) {
        value = node->value;
        return true;
    }
    return false;
}

bool Config::updateInt(const string in_filename, const string in_section, const string in_name, int& value) {
    IntNode* node = getIntNode(in_filename, in_section, in_name);
    if (node) {
        value = node->value;
        return true;
    }
    return false;
}

// Int - node methods

Config::IntNode* Config::getIntNode(const string in_name) {
    return getIntNode("", "", in_name);
}

Config::IntNode* Config::getIntNode(const string in_filename, const string in_section, const string in_name) {
    Setting* s;
    for (vector<Setting*>::iterator iter = settingData.begin(); iter!=settingData.end(); ++iter) {
        s = (*iter);
        if(s->name != in_name)
            continue;
        if(in_section != "" && s->section != in_section)
            continue;
        if(in_filename != "" && s->file != in_filename)
            continue;
        return static_cast<SettingInt*>(s)->valueNode;
    }
    return NULL;
}


//////////////////////////////////////////////////////////////////////////////
// Float - method #1

float Config::getValueAsFloat(const string in_name, const float defaultValue) {
    FloatNode* node = getFloatNode("", "", in_name);
    return (node) ? node->value : defaultValue;
}

float Config::getValueAsFloat(const string in_filename, const string in_section, const string in_name, const float defaultValue){
    FloatNode* node = getFloatNode(in_filename, in_section, in_name);
    return (node) ? node->value : defaultValue;
}

// Float - method #2

bool Config::updateFloat(const string in_name, float& value) {
    FloatNode* node = getFloatNode("", "", in_name);
    if (node) {
        value = node->value;
        return true;
    }
    return false;
}

bool Config::updateFloat(const string in_filename, const string in_section, const string in_name, float& value) {
    FloatNode* node = getFloatNode("", "", in_name);
    if (node) {
        value = node->value;
        return true;
    }
    return false;
}

// Float - node methods

Config::FloatNode* Config::getFloatNode(const string in_name) {
    return getFloatNode("", "", in_name);
}

Config::FloatNode* Config::getFloatNode(const string in_filename, const string in_section, const string in_name) {
    Setting* s;
    for (vector<Setting*>::iterator iter = settingData.begin(); iter!=settingData.end(); ++iter) {
        s = (*iter);
        if(s->name != in_name)
            continue;
        if(in_section != "" && s->section != in_section)
            continue;
        if(in_filename != "" && s->file != in_filename)
            continue;
        return static_cast<SettingFloat*>(s)->valueNode;
    }
    return NULL;
}


//////////////////////////////////////////////////////////////////////////////
// String - method #1

string Config::getValueAsString(const string in_name, const string defaultValue) {
  for (vector<Setting*>::iterator iter = settingData.begin(); iter!=settingData.end(); ++iter) {
    if((*iter)->name == in_name) {
      return static_cast<SettingString*>(*iter)->valueNode->value;
    }
  }
  return defaultValue;
}

string Config::getValueAsString(const string in_filename, const string in_section, const string in_name, const string defaultValue) {
  Setting* s;
  for (vector<Setting*>::iterator iter = settingData.begin(); iter!=settingData.end(); ++iter) {
    s = (*iter);
    if(s->name != in_name)
      continue;
    if(in_section != "" && s->section != in_section)
      continue;
    if(in_filename != "" && s->file != in_filename)
      continue;
    return static_cast<SettingString*>(*iter)->valueNode->value;
  }
  return defaultValue;
}

// String - method #2

bool Config::updateString(const string in_name, string& value) {
  for (vector<Setting*>::iterator iter = settingData.begin(); iter!=settingData.end(); ++iter) {
    if((*iter)->name == in_name) {
      value = static_cast<SettingString*>(*iter)->valueNode->value;
      return true; // setting was found and value updated
    }
  }
  return false; // setting was not found
}

bool Config::updateString(const string in_filename, const string in_section, const string in_name, string& value) {
  Setting* s;
  for (vector<Setting*>::iterator iter = settingData.begin(); iter!=settingData.end(); ++iter) {
    s = (*iter);
    if(s->name != in_name)
      continue;
    if(in_section != "" && s->section != in_section)
      continue;
    if(in_filename != "" && s->file != in_filename)
      continue;
    value = static_cast<SettingString*>(*iter)->valueNode->value;
    return true; // setting was found and value updated
  }
  return false; // setting was not found
}

// String - node methods

Config::StringNode* Config::getStringNode(const string in_name) {
  return NULL;
}

Config::StringNode* Config::getStringNode(const string in_file, const string in_section, const string in_name) {
  return NULL;
}


//////////////////////////////////////////////////////////////////////////////
// Bool - method #1

bool Config::getValueAsBool(const string in_name, const bool defaultValue) {
  for (vector<Setting*>::iterator iter = settingData.begin(); iter!=settingData.end(); ++iter) {
    if((*iter)->name == in_name) {
      return (((SettingInt*)(*iter))->valueNode->value) ? true : false;
    }
  }
  return defaultValue;
}

bool Config::getValueAsBool(const string in_filename, const string in_section, const string in_name, const bool defaultValue) {
  Setting* s;
  for (vector<Setting*>::iterator iter = settingData.begin(); iter!=settingData.end(); ++iter) {
    s = (*iter);
    if(s->name != in_name)
      continue;
    if(in_section != "" && s->section != in_section)
      continue;
    if(in_filename != "" && s->file != in_filename)
      continue;
    return (((SettingInt*)(*iter))->valueNode->value) ? true : false;
  }
  return defaultValue;
}

// Bool - method #2

bool Config::updateBool(const string in_name, bool& value) {
  for (vector<Setting*>::iterator iter = settingData.begin(); iter!=settingData.end(); ++iter) {
    if((*iter)->name == in_name) {
      value = (((SettingInt*)(*iter))->valueNode->value) ? true : false;
      return true; // setting was found and value updated
    }
  }
  return false; // setting not found
}

bool Config::updateBool(const string in_filename, const string in_section, const string in_name, bool& value) {
  Setting* s;
  for (vector<Setting*>::iterator iter = settingData.begin(); iter!=settingData.end(); ++iter) {
    s = (*iter);
    if(s->name != in_name)
      continue;
    if(in_section != "" && s->section != in_section)
      continue;
    if(in_filename != "" && s->file != in_filename)
      continue;
    value = (((SettingInt*)(*iter))->valueNode->value) ? true : false;
    return true; // setting was found and value updated
  }
  return false; // setting not found
}


//////////////////////////////////////////////////////////////////////////////
// Wide String

wstring Config::getValueAsWString(const string in_name)
{
  std::wstring wstrname;
  for (vector<Setting*>::iterator iter = settingData.begin(); iter!=settingData.end(); ++iter) {
    if((*iter)->name == in_name) {
      string tmpname = static_cast<SettingString*>(*iter)->valueNode->value;
      wstrname.assign(tmpname.begin(), tmpname.end());
      return wstrname;
    }
  }
  return wstrname;
}

wstring Config::getValueAsWString(string in_filename, string in_section, string in_name) {
  Setting* s;
  std::wstring wstrname;
  for (vector<Setting*>::iterator iter = settingData.begin(); iter!=settingData.end(); ++iter) {
    s = (*iter);
    if(s->name != in_name)
      continue;
    if(in_section != "" && s->section != in_section)
      continue;
    if(in_filename != "" && s->file != in_filename)
      continue;
    string tmpname = static_cast<SettingString*>(*iter)->valueNode->value;
    wstrname.assign(tmpname.begin(), tmpname.end());
    return wstrname;
  }
  return wstrname;
}

//////////////////////////////////////////////////////////////////////////////
// ===== Parsing


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

bool Config::isFloat(string str) {
  char merkki;
  bool out = true;
  bool point = false;
  for(unsigned int i = 0;i < str.length();i++) {
    merkki = str.at(i);
    switch(merkki) {
      case '.':
      //case ',': // vain piste toimii näemmä iostreamin settien kanssa
        if(point)
          return false; // useampi kuin yksi erotin löydetty, epäkelpo arvo
        else
          point = true;
        break;
      case '-':
        if(i != 0) return false; //out = false;
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
        return false; //out = false;
        break;
    }
  }
  return out && point;
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

bool Config::declaredAsArray(string trimmedStr) {
  if(
    trimmedStr.at(trimmedStr.length()-2) == '[' &&
    trimmedStr.at(trimmedStr.length()-1) == ']'
  ) {
    return true;
  }
  return false;
}

string Config::cleanArrayKeyname(string trimmedStr) {
  string::size_type position = trimmedStr.find_first_of('[');
  if(position != string::npos) {
    trimmedStr.erase(position);
  }
  return trimmedStr;
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

float Config::stringToFloat(string str) {
  stringstream ss(str);
  float floatingPoint;
  if(ss >> floatingPoint) {
    return floatingPoint;
  }
  else {
    return 0.0f;
  }
}

//*** Private ***

void Config::addSetting(string in_name, string in_value, bool asArray) {
    //addSetting(m_strFilename, m_strCurrentSection, in_name, in_value, asArray);
    addSetting("", "", in_name, in_value, asArray);
}

void Config::addSetting(string in_name, float in_value, bool asArray) {
    //addSetting(m_strFilename, m_strCurrentSection, in_name, in_value, asArray);
    addSetting("", "", in_name, in_value, asArray);
}

void Config::addSetting(string in_name, int in_value, bool asArray) {
    //addSetting(m_strFilename, m_strCurrentSection, in_name, in_value, asArray);
    addSetting("", "", in_name, in_value, asArray);
}

//

void Config::addSetting(string in_file, string in_section, string in_name, string in_value, bool asArray) {
  if(!asArray) {
    deleteSetting(in_file, in_section, in_name);
  } else {
    // search for pre-existing value and add to it
    for (vector<Setting*>::iterator iter = settingData.begin(); iter!=settingData.end(); ++iter) {
      if((*iter)->name == in_name && (*iter)->file == in_file && (*iter)->section == in_section) {
        // add new value node to existing setting
        // check that we add only same type
        if((*iter)->type == VARIABLE_STRING) {
          SettingString* item = static_cast<SettingString*>(*iter);
          StringNode* node = new StringNode(in_value, item->valueNode);
          item->valueNode = node;
          item->count++;
        }
        return;
      }
    }
  }
  // this is the first value, add it
  VARIABLE_TYPE tyyppi = VARIABLE_STRING;
  SettingString * set = new SettingString();
  set->count = 1;
  set->name = in_name;
  set->type = tyyppi;
  set->valueNode = new StringNode(in_value, NULL);
  set->file = in_file;
  set->section = in_section;
  settingData.push_back(set);
}

void Config::addSetting(string in_file, string in_section, string in_name, float in_value, bool asArray) {
  if(!asArray) {
    deleteSetting(in_file, in_section, in_name);
  } else {
    // search for pre-existing value and add to it
    for (vector<Setting*>::iterator iter = settingData.begin(); iter!=settingData.end(); ++iter) {
      if((*iter)->name == in_name && (*iter)->file == in_file && (*iter)->section == in_section) {
        // add new value node to existing setting
        // check that we add only same type
        if((*iter)->type == VARIABLE_FLOAT) {
          SettingFloat* item = static_cast<SettingFloat*>(*iter);
          FloatNode* node = new FloatNode(in_value, item->valueNode);
          item->valueNode = node;
          item->count++;
        }
        return;
      }
    }
  }
  // this is the first value, add it
  VARIABLE_TYPE tyyppi = VARIABLE_FLOAT;
  SettingFloat * set = new SettingFloat();
  set->count = 1;
  set->name = in_name;
  set->type = tyyppi;
  set->valueNode = new FloatNode(in_value, NULL);
  set->file = in_file;
  set->section = in_section;
  settingData.push_back(set);
}

void Config::addSetting(string in_file, string in_section, string in_name, int in_value, bool asArray) {
  if(!asArray) {
    deleteSetting(in_file, in_section, in_name);
  } else {
    // search for pre-existing value and add to it
    for (vector<Setting*>::iterator iter = settingData.begin(); iter!=settingData.end(); ++iter) {
      if((*iter)->name == in_name && (*iter)->file == in_file && (*iter)->section == in_section) {
        // add new value node to existing setting
        // check that we add only same type
        if((*iter)->type == VARIABLE_INTEGER) {
          SettingInt* item = static_cast<SettingInt*>(*iter);
          IntNode* node = new IntNode(in_value, item->valueNode);
          item->valueNode = node;
          item->count++;
        }
        return;
      }
    }
  }
  // this is the first value, add it
  VARIABLE_TYPE tyyppi = VARIABLE_INTEGER;
  SettingInt * set = new SettingInt();
  set->count = 1;
  set->name = in_name;
  set->type = tyyppi;
  set->valueNode = new IntNode(in_value, NULL);
  set->file = in_file;
  set->section = in_section;
  settingData.push_back(set);
}

// =====

void Config::deleteValueNodes(Setting* setting) {
    // remove value nodes
    switch( setting->type ) {
        case VARIABLE_INTEGER: {
            SettingInt* set = static_cast<SettingInt*>(setting);
            IntNode* node = set->valueNode;
            IntNode* temp;
            while(node) {
                temp = node;
                node = node->next;
                delete temp;
            }
            set->valueNode = NULL;
            set->count = 0;
            break;
        }
        case VARIABLE_FLOAT: {
            SettingFloat* set = static_cast<SettingFloat*>(setting);
            FloatNode* node = set->valueNode;
            FloatNode* temp;
            while(node) {
                temp = node;
                node = node->next;
                delete temp;
            }
            set->valueNode = NULL;
            set->count = 0;
            break;
        }
        case VARIABLE_STRING: {
            SettingString* set = static_cast<SettingString*>(setting);
            StringNode* node = set->valueNode;
            StringNode* temp;
            while(node) {
                temp = node;
                node = node->next;
                delete temp;
            }
            set->valueNode = NULL;
            set->count = 0;
            break;
        }
    }
}

void Config::deleteSetting(string in_name) {
    deleteSetting("", "", in_name);
}

void Config::deleteSetting(string in_filename, string in_section, string in_name) {
    Setting* s;
    for (vector<Setting*>::iterator iter = settingData.begin(); iter!=settingData.end(); ++iter) {
        s = (*iter);
        if(s->name != in_name)
            continue;
        if(in_section != "" && s->section != in_section)
            continue;
        if(in_filename != "" && s->file != in_filename)
            continue;
        deleteValueNodes((*iter));
        settingData.erase(iter);
        return;
    }
}

bool Config::settingExists(string in_name) {
  return settingExists("", "", in_name);
}

bool Config::settingExists(string in_filename, string in_section, string in_name) {
  Setting* s;
  for (vector<Setting*>::iterator iter = settingData.begin(); iter!=settingData.end(); ++iter) {
    s = (*iter);
    if(s->name != in_name)
      continue;
    if(in_section != "" && s->section != in_section)
      continue;
    if(in_filename != "" && s->file != in_filename)
      continue;
    return true;
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
  else if( (row.substr(0,1) == "#") || (row.substr(0,1) == ";") ) {
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
  // array
  bool isArray = declaredAsArray(name);
  if(isArray) {
    name = cleanArrayKeyname(name);
  }
  // float
  if(isFloat(value)) {
    addSetting(m_strFilename, m_strCurrentSection, name, stringToFloat(value), isArray);
  }
  // integer
  else if(isNumeric(value)) {
    addSetting(m_strFilename, m_strCurrentSection, name, stringToInteger(value), isArray);
  }
  // string
  else {
    addSetting(m_strFilename, m_strCurrentSection, name, value, isArray);
  }
  return true;
}
