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
   * Käytetään merkin numeerisuuden tarkistamiseen
   *
   * @param merkki tutkittava merkki
   * @return oliko merkki numeerinen
   */
  bool isNumeric(string str);

  /**
   * trim
   *
   * Poistaa whitespacet merkkijonon edestä ja takaa.
   *
   * TODO: ei tajua tabeja :(
   */
  string trim(string str);

  /**
   * stringToInteger
   *
   * Muuntaa merkkijonon kokonaisluvuksi
   *
   * @param str   muutettava merkkijono
   * @return      muutettu kokonaisluku
   */
  int Config::stringToInteger (string str);

  /**
   * printSettings
   *
   * Tulostaa kaikki asetukset ruudulle. @see printSetting(Setting * set)
   */
  void printSettings();

  /**
   * readFile
   *
   * Lukee asetukset tiedostosta
   */
  void readFile();

  inline void setFilename(string filename) { m_strFilename = filename; };

  /**
   * getValueAsInt
   *
   * @return Palauttaa halutun muuttujan arvon kokonaislukumuodossa.
   */
  int getValueAsInt(string in_name);

  /**
   * getValueAsBool
   *
   * @return Palauttaa halutun muuttujan arvon totuusarvomuodossa.
   */
  bool getValueAsBool(string in_name);

  /**
   * getValueAsString
   *
   * @return Palauttaa halutun muuttujan arvon merkkijonomuodossa.
   */
  string getValueAsString(string in_name);

  private:
    //** muuttujat ja vakiot **
    string m_strFilename;
    vector<Setting*> settingData;
	static Config* pInstance;


    //** metodit **

    /**
     * addSetting
     *
     * Lisää uuden asetuksen, jos saman niminen asetus on jo olemassa, se korvataan
     */
    void addSetting(string in_name, string in_value);
    void addSetting(string in_name, int in_value);
public:
    /**
     * deleteSetting
     *
     * Poistaa asetuksen
     */
    void deleteSetting(string name);

    /**
     * settingExists
     *
     * Etsii onko halutun niminen asetus olemassa
     */
    bool settingExists(string name);

    /**
     * printSetting
     *
     * Tulostaa halutun asetuksen muotoiltuna ruudulle
     *
     * @param set tulostettava asetus
     */
    void printSetting(Setting * set);

    /**
     * parseRow
     *
     * Muuttaa asetustiedostosta (tai jostain muualtakin) luetun rivin asetukseksi.
     *
     * @return oliko rivi hyvin muodostettu?
     */
    bool parseRow(string row);

};
#endif //__CONFIG_H__
