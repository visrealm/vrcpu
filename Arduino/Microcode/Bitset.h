/*
 * Bitset class. Pack an array of booleans into bits.
 * 
 * Copyright (c) 2019 Troy Schrapel
 * 
 * This code is licensed under the MIT license
 * 
 * https://github.com/visrealm/vrcpu
 *
 */


template <size_t BITS>
class Bitset
{
  public:
    Bitset()
    {
      clear();
    }
    
    /*
     * clear the bitset - set all values to true or false
     */
    void clear(bool value = false)
    {
      memset(m_bytes, value ? 0xffffffff : 0x00, bytes());
    }

    /*
     * set a single bit to true or false
     */
    void set(size_t b, bool value)
    {
      size_t by = byteIndex(b);
      if (by != (size_t)-1)
      {
        if (value)
          bitSet(m_bytes[by], bitIndex(b));
        else
          bitClear(m_bytes[by], bitIndex(b));
      }      
    }

    /*
     * query a single bit
     */
    bool isSet(size_t b) const
    {
      size_t by = byteIndex(b);
      if (by != (size_t)-1)
      {
        return bitRead(m_bytes[by], bitIndex(b));
      }
      return false;
    }

    /*
     * size of storage in bytes
     */
    size_t bytes() const { return sizeof(m_bytes); }

  private:
    size_t byteIndex(size_t b) const
    {
      if (b < BITS)
      {
        return b / 8;
      }
      return (size_t)-1;
    }

    size_t bitIndex(size_t b) const
    {
      return b % 8;
    }

  private:
    byte m_bytes[(BITS >> 3) + 1];  
  
};
