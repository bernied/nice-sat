#include <cppunit/extensions/TestFactoryRegistry.h>
#include "Support/BitWriterTests.H"
CPPUNIT_TEST_SUITE_REGISTRATION(BitWriterTests);

#include <string>

void BitWriterTests::setUp() {
  _outStream = new std::stringstream();
  _writer = new BitWriter(*_outStream);
}

void BitWriterTests::tearDown() {
  delete _writer;
  delete _outStream;
}

void BitWriterTests::runTestConstSize(int len,
                                      int chunkSize,
                                      int chunkArray[]) {
  for (int i = 0; i < len; i++) {
    _writer->putBits(chunkArray[i], chunkSize);
  }
  CPPUNIT_ASSERT_EQUAL(std::string("Hello World "), _outStream->str());
}

void BitWriterTests::runTestNonConstSize(int len,
                                         int chunkSizes[],
                                         int chunkArray[]) {
  for (int i = 0; i < len; i++) {
    _writer->putBits(chunkArray[i], chunkSizes[i]);
  }
  CPPUNIT_ASSERT_EQUAL(std::string("Hello World "), _outStream->str());
}

void BitWriterTests::writeSingleBits() {
  int bitArray[] = { 0, 1, 0, 0, 1, 0, 0, 0,
                     0, 1, 1, 0, 0, 1, 0, 1,
                     0, 1, 1, 0, 1, 1, 0, 0,
                     0, 1, 1, 0, 1, 1, 0, 0,
                     0, 1, 1, 0, 1, 1, 1, 1,
                     0, 0, 1, 0, 0, 0, 0, 0, 
                     0, 1, 0, 1, 0, 1, 1, 1,
                     0, 1, 1, 0, 1, 1, 1, 1,
                     0, 1, 1, 1, 0, 0, 1, 0,
                     0, 1, 1, 0, 1, 1, 0, 0,
                     0, 1, 1, 0, 0, 1, 0, 0,
                     0, 0, 1, 0, 0, 0, 0, 0 };
  runTestConstSize(sizeof(bitArray) / sizeof(int), 1, bitArray);
}

void BitWriterTests::writeThreeBits() {
  int bitArray[] = { 2, 2, 0, 6, 2, 5, 5, 4,
                     3, 3, 0, 6, 7, 4, 4, 0, 
                     2, 5, 6, 6, 7, 5, 6, 2,
                     3, 3, 0, 6, 2, 0, 4, 0 };
  runTestConstSize(sizeof(bitArray) / sizeof(int), 3, bitArray);
}

void BitWriterTests::writeSingleBytes() {
  int byteArray[] = { 0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x20,
                      0x57, 0x6f, 0x72, 0x6c, 0x64, 0x20 };
  runTestConstSize(sizeof(byteArray) / sizeof(int), 8, byteArray);
}

void BitWriterTests::writeThreeBytes() {
  int chunkArray[] = { 0x48656c, 0x6c6f20, 0x576f72, 0x6c6420 };
  runTestConstSize(sizeof(chunkArray) / sizeof(int), 24, chunkArray);
}

void BitWriterTests::writeVariety() {
  int chunkSizes[] = { 8, 4, 12, 4, 28, 8, 16, 4, 12};
  int chunkArray[] = { 0x48, 0x6, 0x56c, 0x6, 0xc6f2057,
                       0x6f, 0x726c, 0x6, 0x420 };
  runTestNonConstSize(sizeof(chunkArray) / sizeof(int),
                      chunkSizes, chunkArray);
}
