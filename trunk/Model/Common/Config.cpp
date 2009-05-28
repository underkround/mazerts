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

    cout << " <" << set->file << ">\t[" << set->section << "]\t\"" << set->name << "\"" << endl;

    // values
    ValueNode* node = set->node;
    while(node) {
        switch(node->type) {
            case VARIABLE_INTEGER:
                cout << "\t(int)\t" << node->getInt() << endl;
                break;
            case VARIABLE_FLOAT:
                cout << "\t(float)\t" << node->getFloat() << endl;
                break;
            case VARIABLE_STRING:
                cout << "\t(str)\t" << node->getString() << endl;
                break;
        }
        node = node->next;
    }

    cout << endl;
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


const bool Config::hasSection(const string sectionName)
{
    for(vector<Setting*>::iterator iter = settingData.begin(); iter != settingData.end(); ++iter) {
        if((*iter)->section == sectionName) {
            return true; // found value with requested section
        }
    }
    return false;
}


//////////////////////////////////////////////////////////////////////////////
// Value nodes to iterate through array-settings


Config::ValueNode* Config::getNode(const string in_name) {
    return getNode("", in_name);
}


Config::ValueNode* Config::getNode(const string in_section, const string in_name)
{
    Setting* s;
    for (vector<Setting*>::iterator iter = settingData.begin(); iter!=settingData.end(); ++iter) {
        s = (*iter);
        if(s->name != in_name)
            continue;
        if(in_section != "" && s->section != in_section)
            continue;
        return s->node;
    }
    return NULL;
}


//////////////////////////////////////////////////////////////////////////////
// Int - method #1

int Config::getValueAsInt(const string in_name, const int defaultValue) {
    ValueNode* node = getNode("", in_name);
    return (node) ? node->getInt() : defaultValue;
}

int Config::getValueAsInt(const string in_section, string in_name, const int defaultValue){
    ValueNode* node = getNode(in_section, in_name);
    return (node) ? node->getInt() : defaultValue;
}

// Int - method #2

bool Config::updateInt(const string in_name, int& value) {
    ValueNode* node = getNode("", in_name);
    if (node) {
        value = node->getInt();
        return true;
    }
    return false;
}

bool Config::updateInt(const string in_section, const string in_name, int& value) {
    ValueNode* node = getNode(in_section, in_name);
    if (node) {
        value = node->getInt();
        return true;
    }
    return false;
}


//////////////////////////////////////////////////////////////////////////////
// Float - method #1

float Config::getValueAsFloat(const string in_name, const float defaultValue) {
    ValueNode* node = getNode("", in_name);
    return (node) ? node->getFloat() : defaultValue;
}

float Config::getValueAsFloat(const string in_section, const string in_name, const float defaultValue){
    ValueNode* node = getNode(in_section, in_name);
    return (node) ? node->getFloat() : defaultValue;
}

// Float - method #2

bool Config::updateFloat(const string in_name, float& value) {
    ValueNode* node = getNode("", in_name);
    if (node) {
        value = node->getFloat();
        return true;
    }
    return false;
}

bool Config::updateFloat(const string in_section, const string in_name, float& value) {
    ValueNode* node = getNode(in_section, in_name);
    if (node) {
        value = node->getFloat();
        return true;
    }
    return false;
}


//////////////////////////////////////////////////////////////////////////////
// String - method #1

string Config::getValueAsString(const string in_name) {
    ValueNode* node = getNode("", in_name);
    return (node) ? node->getString() : ""; // defaultValue;
}

string Config::getValueAsString(const string in_section, const string in_name, const string defaultValue) {
    ValueNode* node = getNode(in_section, in_name);
    return (node) ? node->getString() : defaultValue;
}

// String - method #2

bool Config::updateString(const string in_name, string& value) {
    ValueNode* node = getNode("", in_name);
    if (node) {
        value = node->getString();
        return true;
    }
    return false;
}

bool Config::updateString(const string in_section, const string in_name, string& value) {
    ValueNode* node = getNode(in_section, in_name);
    if (node) {
        value = node->getString();
        return true;
    }
    return false;
}


//////////////////////////////////////////////////////////////////////////////
// Bool - method #1

bool Config::getValueAsBool(const string in_name, const bool defaultValue) {
    ValueNode* node = getNode("", in_name);
    return (node) ? node->getBool() : defaultValue;
}

bool Config::getValueAsBool(const string in_section, const string in_name, const bool defaultValue) {
    ValueNode* node = getNode(in_section, in_name);
    return (node) ? node->getBool() : defaultValue;
}

// Bool - method #2

bool Config::updateBool(const string in_name, bool& value) {
    ValueNode* node = getNode("", in_name);
    if (node) {
        value = node->getBool();
        return true;
    }
    return false;
}

bool Config::updateBool(const string in_section, const string in_name, bool& value) {
    ValueNode* node = getNode(in_section, in_name);
    if (node) {
        value = node->getBool();
        return true;
    }
    return false;
}


//////////////////////////////////////////////////////////////////////////////
// Wide String

wstring Config::getValueAsWString(const string in_name) {
    ValueNode* node = getNode("", in_name);
    string tmpname = (node) ? node->getString() : "";
    std::wstring wstrname;
    wstrname.assign(tmpname.begin(), tmpname.end());
    return wstrname;
}

wstring Config::getValueAsWString(string in_section, string in_name, const string defaultValue) {
    ValueNode* node = getNode(in_section, in_name);
    string tmpname = (node) ? node->getString() : defaultValue;
    std::wstring wstrname;
    wstrname.assign(tmpname.begin(), tmpname.end());
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


Config::Setting* Config::getSetting(string in_section, string in_name) {
    Setting* s = NULL;
    for(vector<Setting*>::iterator iter = settingData.begin(); iter != settingData.end(); ++iter) {
        s = (*iter);
        if(s->name == in_name && s->section == in_section) {
            return s; // found match, break from loop
        }
    }
    return NULL;
}


// ===== ADDING


// add int
void Config::addSetting(string in_name, int in_value, bool asArray) {
    //addSetting(m_strFilename, m_strCurrentSection, in_name, in_value, asArray);
    addSetting("",      "",         in_name, new ValueNodeInt(in_value, NULL), asArray);
}
void Config::addSetting(string in_file, string in_section, string in_name, int in_value, bool asArray) {
    addSetting(in_file, in_section, in_name, new ValueNodeInt(in_value, NULL), asArray);
}

// add float
void Config::addSetting(string in_name, float in_value, bool asArray) {
    //addSetting(m_strFilename, m_strCurrentSection, in_name, in_value, asArray);
    addSetting("",      "",         in_name, new ValueNodeFloat(in_value, NULL), asArray);
}
void Config::addSetting(string in_file, string in_section, string in_name, float in_value, bool asArray) {
    addSetting(in_file, in_section, in_name, new ValueNodeFloat(in_value, NULL), asArray);
}

// add string
void Config::addSetting(string in_name, string in_value, bool asArray) {
    //addSetting(m_strFilename, m_strCurrentSection, in_name, in_value, asArray);
    addSetting("",      "",         in_name, new ValueNodeString(in_value, NULL), asArray);
}
void Config::addSetting(string in_file, string in_section, string in_name, string in_value, bool asArray) {
    addSetting(in_file, in_section, in_name, new ValueNodeString(in_value, NULL), asArray);
}


// add general value node to setting
void Config::addSetting(string in_file, string in_section, string in_name, ValueNode* valueNode, bool asArray) {
    Setting* set = NULL;
    // delete old setting - if any - if setting is not declared as array
    if( !asArray ) {
        deleteSetting(in_section, in_name);
    }
    // search pre-existing setting - if any - to add value
    else {
        set = getSetting(in_section, in_name);
    }

    // add value to pre-existing setting
    if(set) {
        // add node to setting
        valueNode->next = set->node;
        set->node = valueNode;
        // update filename (that is not used anywhere)
        set->file = in_file;
        set->count++;
        return;
    }
    set = new Setting();
    set->count = 1;
    set->name = in_name;
    set->file = in_file;
    set->section = in_section;
    valueNode->next = NULL; // this is the only node
    set->node = valueNode;
    settingData.push_back(set);
}


// =====


void Config::deleteValueNodes(Setting* setting) {
    // remove value nodes
    ValueNode* node = setting->node;
    ValueNode* temp;
    while(node) {
        temp = node;
        node = node->next;
        delete temp;
    }
    setting->count = 0;
    setting->node = NULL;
}

void Config::deleteSetting(string in_name) {
    deleteSetting("", in_name);
}

void Config::deleteSetting(string in_section, string in_name) {
    Setting* s;
    for (vector<Setting*>::iterator iter = settingData.begin(); iter!=settingData.end(); ++iter) {
        s = (*iter);
        if(s->name != in_name)
            continue;
        if(in_section != "" && s->section != in_section)
            continue;
        deleteValueNodes((*iter));
        settingData.erase(iter);
        return;
    }
}

void Config::deleteSettingSection(string in_section) {
    Setting* s;
    for (vector<Setting*>::iterator iter = settingData.begin(); iter!=settingData.end(); ++iter) {
        s = (*iter);

        if(s->section == "" || s->section != in_section)
            continue;
        deleteValueNodes((*iter));
        settingData.erase(iter);
        return;
    }
}

bool Config::settingExists(string in_name) {
    return settingExists("", in_name);
}

bool Config::settingExists(string in_section, string in_name) {
  Setting* s;
  for (vector<Setting*>::iterator iter = settingData.begin(); iter!=settingData.end(); ++iter) {
    s = (*iter);
    if(s->name != in_name)
      continue;
    if(in_section != "" && s->section != in_section)
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
