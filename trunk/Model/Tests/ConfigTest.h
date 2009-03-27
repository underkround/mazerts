
#ifndef DEBUG
#define DEBUG 1
#endif

#include "../Common/Config.h"
#include <assert.h>

void testConfig() {
    Config & c = * Config::getInstance();

    c.setFilename("..\\Model\\Tests\\", "testi.ini");
    c.readFile();
#ifdef DEBUG
    c.printSettings();
#endif
    assert(c.getValueAsBool("bool") == true);
    assert(c.getValueAsBool("int") == true);
    assert(c.getValueAsInt("int") == 2);
    assert(c.getValueAsInt("negative int") == -66);
    assert(c.getValueAsString("str") == "testi");

    float f = (c.getValueAsFloat("joku floatti") - 1.23f);
    assert( (-0.00001f < f && f < 0.00001f) );

    f = (c.getValueAsFloat("joku negaileva floatti") + 3.21f);
    assert( (-0.00001f < f && f < 0.00001f) );

    // update int
    int i = 312;
    c.updateInt("int", i);
    assert(i == 2);

    // update failing int, should return default
    i = 112;
    c.updateInt("minua ei ole olemassa", i);
    assert(i == 112);
    i = 911;
    i = c.getValueAsInt("minua ei ole olemassa", i);
    assert(i = 911);
    i = c.getValueAsInt("minua ei ole olemassa", 555);
    assert(i = 555);

    // update float
    f = 55.55f;
    c.updateFloat("joku floatti", f);
    assert( (1.22999f < f && f < 1.23001f) );

    // testing for conditional filename or section
    // right value is 5

    // these should return 5
    assert(c.getValueAsInt("integeri muut-sektionissa") == 5); // fetch only by value
    assert(c.getValueAsInt("", "integeri muut-sektionissa") == 5); // another way to say previous line
    assert(c.getValueAsInt("muut", "integeri muut-sektionissa") == 5); // filter by section
    // this has wrong section and should not return 5
    assert(c.getValueAsInt("bar", "integeri muut-sektionissa") != 5); // wrong section

    cout << " value: \"int\" as float: " << c.getValueAsFloat("int") << endl;

//    cout << c.getValueAsInt("", "sekt1", "olen monessa sectionissa") << endl;
}
