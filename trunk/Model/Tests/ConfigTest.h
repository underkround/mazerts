#include "../Common/Config.h"
#include <assert.h>

void testConfig() {
    Config & c = * Config::getInstance();

    c.setFilename("..\\Model\\Tests\\testi.ini");
    c.readFile();
    c.printSettings();
    assert(c.getValueAsBool("bool") == true);
    assert(c.getValueAsInt("int") == 2);
    assert(c.getValueAsString("str") == "testi");
}