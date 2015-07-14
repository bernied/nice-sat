#ifndef SUPPORT_BIT_WRITER_TESTS_H_
#define SUPPORT_BIT_WRITER_TESTS_H_

#include <cppunit/extensions/HelperMacros.h>

#include <sstream>
#include "Support/BitWriter.H"
using namespace nicesat;

class BitWriterTests : public CppUnit::TestFixture {
public:
  void setUp();
  void tearDown();
  
  CPPUNIT_TEST_SUITE(BitWriterTests);
  CPPUNIT_TEST(writeSingleBits);
  CPPUNIT_TEST(writeThreeBits);
  CPPUNIT_TEST(writeSingleBytes);
  CPPUNIT_TEST(writeThreeBytes);
  CPPUNIT_TEST(writeVariety);
  CPPUNIT_TEST_SUITE_END();
  
private:
  std::stringstream* _outStream;
  BitWriter*         _writer;

  void runTestConstSize(int len, int chunkSize, int chunkArray[]);
  void runTestNonConstSize(int len, int chunkSizes[], int chunkArray[]);
  
  void writeSingleBits();
  void writeThreeBits();
  void writeSingleBytes();
  void writeThreeBytes();
  void writeVariety();
};

#endif//NICESAT_INPUT_BIT_WRITER_TEST_HPP_
