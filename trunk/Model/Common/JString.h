/**
 * JString.h header file
 * Copyright (c) Jani Immonen
 * Date 2.4.2008
 *
 * Formerly CSuperString
 *
 * This class implements character string class
 * to demonstrate usage of the overridden operators
 */

#ifndef __JSTRING_H__
#define __JSTRING_H__

#include <string.h> //vissiin ainakin NULL:in define tulee täältä

class JString
{
public:
    JString(void);
    JString(const char* pString);
    JString(const JString& string);
    virtual ~JString(void);

    /**
     * Set
     *
     * Initialize string from character buffer
     * @param pString buffer to initialize from
     */
    void Set(const char* pString);

    /**
     * Set
     *
     * Initialize string from an another JString
     * @param string string to initialize from
     */
    void Set(const JString& string);


    /**
     * Add
     * add string to another
     * @param string string to add
     */
    void Add(const JString& string);

    /**
     * Compare
     *
     * compare strings
     * @param string
     * @return 0 if strings are identical
     *           > 0 if this string is greater than parameter
     *           < 0 if this string is less than parameter
     */
    int Compare(const JString& string) const;
    
    /**
     * GetLength
     * @return number of characters in this string
     */
    int GetLength() const;


    /**
     * Reset
     *
     * empties the string and releases the memory
     */
    void Reset(void);

    /**
     * GetBuffer
     *
     * access to strings character buffer
     * @return character buffer
     */
    const char* GetBuffer() const;

    /**
     * ------------------------------------------------------
     * Overridden operators
     */

    /**
     * operator const char*
     *
     * @return pointer to string data
     */
    operator const char*() const;

    /**
     * operator =
     *
     * string copt operator
     * @param string string to copy from
     * @return reference to copied string
     */
    JString& operator = (const JString& string);
    //JString& operator = (const char* string); //tätä ei voi tehdä koska kääntäjä luulee että se on jo olemassa

    /**
     * operator []
     *
     * @param iIndex
     * @return character at 'iIndex'
     */
    const char operator [] (const int iIndex) const; //const tyyppinen että tällä operaattorilla ei voida muokata
                            //merkkijonoamme ettei se mene rikki väärästä indeksistä

    /**
     * operator +=
     * adds string to another
     * @param stirng string to copy ot
     * @return reference to plah plaa
     */
    JString& operator += (const JString& string);

    /**
     * operator ==
     * compare two strings
     * @param string string to compare to
     * @return true if strings are identical, false other
     */
    bool operator == (const JString& string) const;

    /**
     * operator !=
     * compare two strings
     * @param string string to compare to
     * @return false if strings are identical, true other
     */
    bool operator != (const JString& string) const;

    /**
     * ------------------------------------------------------
     */

private:
    /**
     * AllocateBuffer
     *
     * Allocate memory for the string
     * @param iSize needed length of the string
     * @return pointer to allocated memory
     */
    char* AllocateBuffer(const int iSize);

    /**
     * ComputeLength
     *
     * @param pString string to compute length from
     * @return number of characters in string
     */
    static int ComputeLength(const char* pString);

    char*    m_pBuffer;
    int        m_iCount;
};

#endif //__JSTRING_H__

