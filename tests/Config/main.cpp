#include "./Config.h"

int main(void) {
  SConfig * settings = &SConfig::getInstance();
  settings->setFilename("testi.ini");
  settings->readFile();
  settings->printSettings();
  return 0;
}
