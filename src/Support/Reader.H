#ifndef NICESAT_SUPPORT_READER_HPP_
#define NICESAT_SUPPORT_READER_HPP_

#include <string>
using namespace std;

#include <stdio.h>

#define READER_BUFFER_LEN 256

namespace nicesat {
  class Reader {
  public:
    /**
     * Opens the file and sets the reader up for reading.
     * @param filename The name of the file to open
     */
    Reader(const char* filename);
    
    /**
     * Destructor
     */
    ~Reader();

    const char* getFileName() const { return _filename; }
    
    /**
     * Read a string from the file.  Reads exactly the string specified.
     */
    void readExpected(const char* str);
    
    /**
     * Read a string ended by 'sep'.  Consumes the separator but does
     * not include it in the string.
     * @param sep The end of the string to be parsed
     * @return The string that was read from the file.
     */
    string readString(char sep);
    
    /**
     * Read a string ended by 'sep'.  Consumes the separator but does
     * not include it in the string.
     * @param seps Characters to treat as separators
     * @return The string that was read from the file.
     */
    string readString(const char* seps);
    
    /**
     * Read a number from the string, ended by the separator.  Consumes
     * the separator but does not include it in the string.
     * @param sep The end of the number to be parsed.
     * @return The number that was read from the file.
     */
    int readNum(char sep);
    
    /**
     * Read a number from the string, ended by the separator.  Consumes
     * the separator but does not include it in the string.
     * @param sep The end of the number to be parsed.
     * @return The number that was read from the file.
     */
    int readNum(const char* sep);
    
    /**
     * Read a number from the file in the AIGER variable-width integer
     * format.
     * @return The number that was read from the file.
     */
    unsigned int readNumBinary();
    
  private:
    /**
     * Read a line from the buffer
     */
    void fillBuff();
    
    /**
     * Function to get a single character from the file.
     * @return The character read from the file.
     */
    unsigned char getNonEofChar();
    
    /**
     * The filename of the file that is being read from.
     */
    char*  _filename;
    
    /**
     * The FILE pointer being read from.
     */
    FILE* _in;
    
    /**
     * A buffer of the information read in from the file.
     */
    char _buffer[READER_BUFFER_LEN + 1];
    
    /**
     * The current position in the buffer
     */
    size_t _bufPos;

    /**
     * The current length of the bufefr
     */
    size_t _bufLen;
  };
}

#endif//NICESAT_SUPPORT_READER_HPP_
