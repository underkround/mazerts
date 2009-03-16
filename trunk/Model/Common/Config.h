#ifndef __CONFIG_H__
#define __CONFIG_H__

/**
 * Config.h
 *
 * $Id$
 * $Revision$
 *
 * This class originally written by Antti for Yazatrix project contains configurations
 *
 * Usage:
 * - get singleton
 * - access values with getValueAs*("name")
 *
 * it iterates thru vector with every access so if you want to optimize for speed keep to value in your own variables
 *
 */
#include <vector>
#include <string>
#include <iostream>

using namespace std;

class Config {

protected:
    Config(void);
    virtual ~Config(void);
    Config(const Config&);
    Config& operator= (const Config&);

  public:

      static Config* getInstance();

  /**
   * Variables used in config file
   */
  enum VARIABLE_TYPE { VARIABLE_STRING, VARIABLE_INTEGER };

  struct Setting {
    string name;
    string file;
    string section;
    VARIABLE_TYPE type;
  };

  struct SettingInt : Setting {
    int value;
  };

  struct SettingString : Setting {
    string value;
  };

  /**
   * isNumeric
   *
   * check if character is numeric
   *
   * @param str character to be checked
   * @return was given character numeric
   */
  bool isNumeric(string str);

  /**
   * trim
   *
   * trims spaces from front and behind of string
   *
   * TODO: still doesn't understand tabs
   */
  string trim(string str);

  /**
   * stringToInteger
   *
   * converts string to integer
   *
   * @param str   string to be converted
   * @return      converted integer
   */
  int Config::stringToInteger (string str);

  /**
   * printSettings
   *
   * prints all settings from file to screen @see printSetting(Setting * set)
   */
  void printSettings();

  /**
   * readFile
   *
   * reads settings from set filename
   */
  void readFile();

  inline void setFilename(string filename) { m_strFilename = filename; m_strFilepath = ""; };
  inline void setFilename(string path, string filename) { m_strFilename = filename; m_strFilepath = path; };
  inline void setCurrentSection(string section) { m_strCurrentSection = section; };


  /**
   * getValueAsInt
   *
   * @param in_name name of setting to be queried
   * @param defaultValue value that will be returned if setting is not found
   * @return setting as integer
   */
  int getValueAsInt(string in_name, const int defaultValue=0);
  int getValueAsInt(string in_filename, string in_name, const int defaultValue=0);
  int getValueAsInt(string in_filename, string in_section, string in_name, const int defaultValue=0);

  /**
   * Alternative method, update the given value, if setting for in_name
   * is found. Otherwise this leaves the value untouched.
   * Intented to used to override default values if found from configs.
   * @param value   Reference to the value that is updated, if requested setting
   *                is found. Otherwise it's left untouched (can contain default)
   * @return        true, if the value was altered by the config
   */
  bool updateInt(string in_name, int& value);
  bool updateInt(string in_filename, string in_name, int& value);
  bool updateInt(string in_filename, string in_section, string in_name, int& value);


  /**
   * getValueAsBool
   *
   * @param in_name name of setting to be queried
   * @param defaultValue value that will be returned if setting is not found
   * @return setting as boolean
   */
  bool getValueAsBool(string in_name, const bool defaultValue=false);
  bool getValueAsBool(string in_filename, string in_name, const bool defaultValue=false);
  bool getValueAsBool(string in_filename, string in_section, string in_name, const bool defaultValue=false);

  /**
   * Alternative method, update the given value, if setting for in_name
   * is found. Otherwise this leaves the value untouched.
   * Intented to used to override default values if found from configs.
   * @param value   Reference to the value that is updated, if requested setting
   *                is found. Otherwise it's left untouched (can contain default)
   * @return        true, if the value was altered by the config
   */
  bool updateBool(string in_name, bool& value);
  bool updateBool(string in_filename, string in_name, bool& value);
  bool updateBool(string in_filename, string in_section, string in_name, bool& value);


  /**
   * getValueAsString
   *
   * @param in_name name of setting to be queried
   * @param defaultValue value that will be returned if setting is not found
   * @return setting as string
   */
  string getValueAsString(string in_name, const string defaultValue="");
  string getValueAsString(string in_filename, string in_name, const string defaultValue="");
  string getValueAsString(string in_filename, string in_section, string in_name, const string defaultValue="");

  /**
   * Alternative method, update the given value, if setting for in_name
   * is found. Otherwise this leaves the value untouched.
   * Intented to used to override default values if found from configs.
   * @param value   Reference to the value that is updated, if requested setting
   *                is found. Otherwise it's left untouched (can contain default)
   * @return        true, if the value was altered by the config
   */
  bool updateString(string in_name, string& value);
  bool updateString(string in_filename, string in_name, string& value);
  bool updateString(string in_filename, string in_section, string in_name, string& value);

  /**
   * Wide string
   */
  wstring getValueAsWString(string in_name);
  wstring getValueAsWString(string in_filename, string in_name);
  wstring getValueAsWString(string in_filename, string in_section, string in_name);

  /**
   * mazerts specific configuration default values
   * remember to add any new configuration variables here also
   * Update:
   *    default settings are now given as normal member values within the
   *    subject classes, values from configs can override them, if set
   * @deprecated
   */
  //void loadDefaults(void);

  private:
    //** variables **
    string m_strFilename;
    string m_strFilepath;
    string m_strCurrentSection;
    vector<Setting*> settingData;
    static Config* pInstance;


    //** metodit **

    /**
     * addSetting
     *
     * adds a new setting, if it already existed, replaces that
     * @param in_name name of setting
     * @param in_value value of setting
     * @param in_file current filename
     * @param in_section current section
     */
    void addSetting(string in_name, string in_value);
    void addSetting(string in_name, int in_value);
    void addSetting(string in_file, string in_section, string in_name, string in_value);
    void addSetting(string in_file, string in_section, string in_name, int in_value);

public:
    /**
     * deleteSetting
     *
     * removes setting
     */
    void deleteSetting(string in_name);
    void deleteSetting(string in_filename, string in_name);
    void deleteSetting(string in_filename, string in_section, string in_name);

    /**
     * settingExists
     *
     * checks if setting exists
     *
     * @param name setting to be checked
     */
    bool settingExists(string in_name);
    bool settingExists(string in_filename, string in_name);
    bool settingExists(string in_filename, string in_section, string in_name);

    /**
     * printSetting
     *
     * prints setting to screen
     *
     * @param set setting to be printed
     */
    void printSetting(Setting * set);

    /**
     * parseRow
     *
     * parses single row into a setting
     *
     * @return was that row well formed
     */
    bool parseRow(string row);

};
#endif //__CONFIG_H__
