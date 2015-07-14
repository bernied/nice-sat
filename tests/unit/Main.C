#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

int main(int argc, char** argv) {
  CppUnit::TextUi::TestRunner runner;
  runner.setOutputter(new CppUnit::CompilerOutputter(&runner.result(),
                                                     std::cerr));
  CppUnit::TestFactoryRegistry &registry =
    CppUnit::TestFactoryRegistry::getRegistry();
  runner.addTest(registry.makeTest());
  bool wasSuccessful = runner.run("", false);
  return !wasSuccessful;
}
