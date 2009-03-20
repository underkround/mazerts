/**
 * Config.h
 *
 * $Id$
 * $Revision$
 *
 * This class originally written by Antti for Yazatrix project contains configurations
 *
 * ===== Info
 *
 * Usage:
 * - get singleton
 * - access values with getValueAs*("key")
 * or
 * - override already defined variables with updateXxx("key", variable) if
 *   setting for "key" is set, otherwise leaves variable untouched
 *
 * it iterates thru vector with every access so if you want to optimize for speed keep to value in your own variables
 *
 *
 * ===== Known bugs & issues
 *
 * - TODO: arrays do not have type-checking, if you define array values
 *   for same key with different types, i really don't know what can happen!
 *
 * - not optimized in any way :D
 *
 *
 * ===== Supported .ini format
 *
 * General:
 *
 *      ###
 *      # These are comment-lines
 *      #
 *      # they can
 *       # be surrounded by
 *        # whitespaces, but cannot
 *      # be on same line with variable defines
 *      #
 *
 *      [some section]
 *      ; this is another style to comment (.ini -default)
 *      ; comment for this "some int value" variable
 *      some int value = 55
 *      ; this will override the previous
 *      some int value = 66
 *
 *      keys can contain whitespaces = string values too
 *
 *      ; floats need to have comma as separator
 *      this_special_float = 5.12312
 *
 *      ;
 *      ; arrays are defined like this
 *      ;
 *      my array value[] = 1
 *      my array value[] = 2
 *      my array value[] = 3
 *
 */
#ifndef __CONFIG_H__
#define __CONFIG_H__

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
    enum VARIABLE_TYPE {
        VARIABLE_STRING,
        VARIABLE_INTEGER,
        VARIABLE_FLOAT
    };

    struct Setting {
        string name;
        string file;
        string section;
        VARIABLE_TYPE type;
        int count; // count of values, >1 for array types, 1 for scalars
    };

    // nodes

    struct IntNode {
        IntNode(int val, IntNode* nextNode) : value(val), next(nextNode) { }
        int value;
        IntNode* next;
    };

    struct FloatNode {
        FloatNode(float val, FloatNode* nextNode) : value(val), next(nextNode) { }
        float value;
        FloatNode* next;
    };

    struct StringNode {
        StringNode(string val, StringNode* nextNode) : value(val), next(nextNode) { }
        string value;
        StringNode* next;
    };

    // settiing types

    struct SettingInt : Setting {
        //int value;
        IntNode* valueNode;
    };

    struct SettingString : Setting {
        //string value;
        StringNode* valueNode;
    };

    struct SettingFloat : Setting {
        //float value;
        FloatNode* valueNode;
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
     * isFloat
     *
     * check if string contains floating point value
     *
     * @param str string to be checked
     * @return true, if str contains floating point value
     */
    bool isFloat(string str);

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
    int Config::stringToInteger(string str);

    /**
     * stringToFloat
     *
     * converts string to float or returns 0.0f if conversion failed.
     *
     * @param str     string to be converted
     * @return        converted float
     */
    float stringToFloat(string str);

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

// ===== INTEGER

    /**
     * getValueAsInt
     *
     * @param in_name       name of setting to be queried
     * @param in_filename   optional filter for filename (give "" to ignore)
     * @param in_section    optional filter for section (give "" to ignore)
     * @param defaultValue  value that will be returned if setting is not found
     * @return              setting as integer
     */
    int getValueAsInt(const string in_name, const int defaultValue=0);
    int getValueAsInt(const string in_filename, const string in_section, const string in_name, const int defaultValue=0);

    /**
     * Alternative method, update the given value, if setting for in_name
     * is found. Otherwise this leaves the value untouched.
     * Intented to used to override default values if found from configs.
     *
     * @param value         Reference to the value that is updated, if requested
     *                      setting is found. Otherwise it's left untouched
     *                      (can contain default)
     * @param in_filename   optional filter for filename (give "" to ignore)
     * @param in_section    optional filter for section (give "" to ignore)
     * @return              true, if the value was altered by the config
     */
    bool updateInt(const string in_name, int& value);
    bool updateInt(const string in_filename, const string in_section, const string in_name, int& value);

    /**
     * Get nodes,
     * If the key exits but the type is other than requested, NULL is returned.
     * If the key does not exit, NULL is returned.
     *
     * @param in_name       name/key of the setting
     * @param in_filename   optional filter for filename (give "" to ignore)
     * @param in_section    optional filter for section (give "" to ignore)
     * @return              first node to the setting value list
     */
    IntNode* getIntNode(const string in_name);
    IntNode* getIntNode(const string in_filename, const string in_section, const string in_name);

// ===== FLOATING POINT

    /**
     * @see getValueAsInt
     */
    float getValueAsFloat(const string in_name, const float defaultValue=0.0f);
    float getValueAsFloat(const string in_filename, const string in_section, const string in_name, const float defaultValue=0.0f);

    /**
     * @see updateInt
     */
    bool updateFloat(const string in_name, float& value);
    bool updateFloat(const string in_filename, const string in_section, const string in_name, float& value);

    /**
     * @see getIntNode
     */
    FloatNode* getFloatNode(const string in_name);
    FloatNode* getFloatNode(const string in_file, const string in_section, const string in_name);

// ===== STRING

    /**
     * getValueAsString
     *
     * @param in_name       name of setting to be queried
     * @param in_filename   optional filter for filename (give "" to ignore)
     * @param in_section    optional filter for section (give "" to ignore)
     * @param defaultValue  value that will be returned if setting is not found
     * @return              setting as string
     */
    string getValueAsString(const string in_name, const string defaultValue="");
    string getValueAsString(const string in_filename, const string in_section, const string in_name, const string defaultValue="");

    /**
     * Alternative method, update the given value, if setting for in_name
     * is found. Otherwise this leaves the value untouched.
     * Intented to used to override default values if found from configs.
     *
     * @param value         Reference to the value that is updated, if requested
     *                      setting is found. Otherwise it's left untouched (can
     *                      contain default)
     * @param in_filename   optional filter for filename (give "" to ignore)
     * @param in_section    optional filter for section (give "" to ignore)
     * @return              true, if the value was altered by the config
     */
    bool updateString(const string in_name, string& value);
    bool updateString(const string in_filename, const string in_section, const string in_name, string& value);

    /**
     * Get nodes,
     * If the key exits but the type is other than requested, NULL is returned.
     * If the key does not exit, NULL is returned.
     *
     * @param in_name       name/key of the setting
     * @param in_filename   optional filter for filename (give "" to ignore)
     * @param in_section    optional filter for section (give "" to ignore)
     * @return              first node to the setting value list
     */
    StringNode* getStringNode(const string in_name);
    StringNode* getStringNode(const string in_file, const string in_section, const string in_name);

// ===== BOOLEAN

    /**
     * getValueAsBool
     *
     * Return value found from config, or defaultValue if not found
     *
     * @param in_name       name of setting to be queried
     * @param in_filename   optional filter for filename (give "" to ignore)
     * @param in_section    optional filter for section (give "" to ignore)
     * @param defaultValue  value that will be returned if setting is not found
     * @return              setting as boolean
     */
    bool getValueAsBool(const string in_name, const bool defaultValue=false);
    bool getValueAsBool(const string in_filename, const string in_section, const string in_name, const bool defaultValue=false);

    /**
     * Alternative method, update the given value, if setting for in_name
     * is found. Otherwise this leaves the value untouched.
     * Intented to used to override default values if found from configs.
     *
     * @param value         Reference to the value that is updated, if requested
     *                      setting is found. Otherwise it's left untouched
     *                      (can contain default)
     * @param in_filename   optional filter for filename (give "" to ignore)
     * @param in_section    optional filter for section (give "" to ignore)
     * @return              true, if the value was altered by the config
     */
    bool updateBool(const string in_name, bool& value);
    bool updateBool(const string in_filename, const string in_section, const string in_name, bool& value);

// ===== WIDE STRINGS

    /**
     * Wide string
     *
     * @param in_name       keyword for the setting
     * @param in_filename   optional filter for filename (give "" to ignore)
     * @param in_section    optional filter for section (give "" to ignore)
     * @return              value, if any found
     */
    wstring getValueAsWString(const string in_name);
    wstring getValueAsWString(const string in_filename, const string in_section, const string in_name);

private:
    //** variables **
    string m_strFilename;
    string m_strFilepath;
    string m_strCurrentSection;
    vector<Setting*> settingData;
//  static Config* pInstance;


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
    void addSetting(string in_name, string in_value, bool isArray=false);
    void addSetting(string in_name, float in_value, bool isArray=false);
    void addSetting(string in_name, int in_value, bool isArray=false);
    void addSetting(string in_file, string in_section, string in_name, string in_value, bool isArray=false);
    void addSetting(string in_file, string in_section, string in_name, float in_value, bool isArray=false);
    void addSetting(string in_file, string in_section, string in_name, int in_value, bool isArray=false);

    /**
     * declaredAsArray
     *
     * checks if setting key (name) is declared as array
     *
     * @param trimmedStr setting key to check for array marks as trimmed string
     * @return true, if key is declared as array
     */
    bool declaredAsArray(string trimmedStr);

    /**
     * cleanArrayKeyname
     *
     * cleans array declaration marks from setting key (name) string
     *
     * @param trimmedStr setting key to clean as trimmed string
     * @return cleaned string
     */
    string cleanArrayKeyname(string trimmedStr);

    /**
     * deleteValueNodes
     *
     * removes and deletes the nodes holding the values for setting
     */
    void deleteValueNodes(Setting* setting);

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
