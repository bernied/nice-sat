#include <cppunit/extensions/TestFactoryRegistry.h>
#include "Support/BitReaderTests.H"
CPPUNIT_TEST_SUITE_REGISTRATION(BitReaderTests);

#include <stdio.h>

void BitReaderTests::setUp() {
  // Hello World =
  //  0100 1000 H
  //  0110 0101 e
  //  0110 1100 l
  //  0110 1100 l
  //  0110 1111 o
  //  0010 0000 
  //  0101 0111 W
  //  0110 1111 o
  //  0111 0010 r
  //  0110 1100 l
  //  0110 0100 d
  //  0010 0000
  _inStream = new std::stringstream("Hello World ");
  _reader = new BitReader(*_inStream);
}

void BitReaderTests::tearDown() {
  delete _reader;
  delete _inStream;
}

void BitReaderTests::runTestConstSize(int len,
                                      int chunkSize,
                                      int chunkArray[]) {
  char buf[80];
  for (int i = 0; i < len; i++) {
    sprintf(buf, "Chunk index %d", i);
    CPPUNIT_ASSERT_EQUAL_MESSAGE(buf, chunkArray[i],
                                 (int) _reader->getBits(chunkSize));
  }
}

void BitReaderTests::runTestNonConstSize(int len,
                                         int chunkSizes[],
                                         int chunkArray[]) {
  char buf[80];
  for (int i = 0; i < len; i++) {
    sprintf(buf, "Chunk index %d", i);
    CPPUNIT_ASSERT_EQUAL_MESSAGE(buf, chunkArray[i],
                                 (int) _reader->getBits(chunkSizes[i]));
  }
}

void BitReaderTests::readSingleBits() {
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

void BitReaderTests::readThreeBits() {
  int bitArray[] = { 2, 2, 0, 6, 2, 5, 5, 4,
                     3, 3, 0, 6, 7, 4, 4, 0, 
                     2, 5, 6, 6, 7, 5, 6, 2,
                     3, 3, 0, 6, 2, 0, 4, 0 };
  runTestConstSize(sizeof(bitArray) / sizeof(int), 3, bitArray);
}

void BitReaderTests::readSingleBytes() {
  int byteArray[] = { 0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x20,
                      0x57, 0x6f, 0x72, 0x6c, 0x64, 0x20 };
  runTestConstSize(sizeof(byteArray) / sizeof(int), 8, byteArray);
}

void BitReaderTests::readThreeBytes() {
  int chunkArray[] = { 0x48656c, 0x6c6f20, 0x576f72, 0x6c6420 };
  runTestConstSize(sizeof(chunkArray) / sizeof(int), 24, chunkArray);
}

void BitReaderTests::readVariety() {
  int chunkSizes[] = { 8, 4, 12, 4, 28, 8, 16, 4, 12};
  int chunkArray[] = { 0x48, 0x6, 0x56c, 0x6, 0xc6f2057,
                       0x6f, 0x726c, 0x6, 0x420 };
  runTestNonConstSize(sizeof(chunkArray) / sizeof(int),
                      chunkSizes, chunkArray);
}
