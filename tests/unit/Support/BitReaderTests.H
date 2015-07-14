#ifndef SUPPORT_BIT_READER_TESTS_H_
#define SUPPORT_BIT_READER_TESTS_H_

#include <cppunit/extensions/HelperMacros.h>

#include <sstream>
#include "Support/BitReader.H"
using namespace nicesat;

class BitReaderTests : public CppUnit::TestFixture {
public:
  void setUp();
  void tearDown();
  
  CPPUNIT_TEST_SUITE(BitReaderTests);
  CPPUNIT_TEST(readSingleBits);
  CPPUNIT_TEST(readThreeBits);
  CPPUNIT_TEST(readSingleBytes);
  CPPUNIT_TEST(readThreeBytes);
  CPPUNIT_TEST(readVariety);
  CPPUNIT_TEST_SUITE_END();
  
private:
  std::stringstream* _inStream;
  BitReader* _reader;

  void runTestConstSize(int len, int chunkSize, int chunkArray[]);
  void runTestNonConstSize(int len, int chunkSizes[], int chunkArray[]);
  
  void readSingleBits();
  void readThreeBits();
  void readSingleBytes();
  void readThreeBytes();
  void readVariety();
};

#endif//NICESAT_INPUT_BIT_READER_TEST_HPP_
