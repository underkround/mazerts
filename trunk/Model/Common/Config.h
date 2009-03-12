#ifndef __CONFIG_H__
#define __CONFIG_H__

/**
 * Config.h
 *
 * $Id: Config.h 136 2008-05-06 13:48:14Z antsy $
 * $Revision: 136 $
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
   * @return setting as integer
   */
  int getValueAsInt(string in_name);
  int getValueAsInt(string in_filename, string in_name);
  int getValueAsInt(string in_filename, string in_section, string in_name);

  /**
   * getValueAsBool
   *
   * @param in_name name of setting to be queried
   * @return setting as boolean
   */
  bool getValueAsBool(string in_name);
  bool getValueAsBool(string in_filename, string in_name);
  bool getValueAsBool(string in_filename, string in_section, string in_name);

  /**
   * getValueAsString
   *
   * @param in_name name of setting to be queried
   * @return setting as string
   */
  string getValueAsString(string in_name);
  string getValueAsString(string in_filename, string in_name);
  string getValueAsString(string in_filename, string in_section, string in_name);

  wstring getValueAsWString(string in_name);
  wstring getValueAsWString(string in_filename, string in_name);
  wstring getValueAsWString(string in_filename, string in_section, string in_name);

  /**
   * mazerts specific configuration default values
   * remember to add any new configuration variables here also
   */
  void loadDefaults(void);

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
