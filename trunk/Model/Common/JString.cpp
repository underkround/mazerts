/**
 * Copyright (c) Jani Immonen
 * Date 2.4.2008
 *
 * Formerly CSuperString
 *
 */

#include "JString.h"

JString::JString(void)
{
	m_pBuffer = NULL;
	m_iCount = 0;
}

JString::JString(const char* pString)
{
	m_pBuffer = NULL;
	m_iCount = 0;
	Set(pString);
}

JString::JString(const JString& string) 
{
	m_pBuffer = NULL;
	m_iCount = 0;
	Set(string);
}


JString::~JString(void)
{
	Reset();
}

void JString::Set(const char* pString)
{
	Reset();

	if(pString)
	{
		const int count = ComputeLength(pString) + 1; //+1 = nollamerkki mukaan
		if(AllocateBuffer(count))
		{
			//copy 'pString' to our buffer
			::memcpy(m_pBuffer, pString, count);
		}
	}
}

void JString::Set(const JString& string)
{
	Reset();

	const int count = ComputeLength(string) + 1;    // ylikuormitettu operaattori hanskaa ihanan n‰pp‰r‰sti
							                        // t‰m‰n ComputeLengthin t‰ss‰
	if(AllocateBuffer(count))
	{
		//copy 'pString' to our buffer
		::memcpy(m_pBuffer, string, count);
	}
}

void JString::Reset(void)
{
	if(m_pBuffer)
	{
		delete [] m_pBuffer;
		m_pBuffer = NULL;
	}
	m_iCount = 0;
}

char* JString::AllocateBuffer(const int iSize)
{
	Reset();

	if(iSize > 0)
	{
		m_pBuffer = new char[iSize];
		if(m_pBuffer)
		{
			m_pBuffer[iSize -1] = 0; //hyv‰‰ varmistelua, lopetusmerkki
			m_iCount = iSize;
		}
	}
	return m_pBuffer;
}

int JString::ComputeLength(const char* pString)
{
	if(pString == NULL)
	{
		return 0;
	}
	int i = 0;
	int length = 0;
	int safetycheck = 500000;
	
	while(pString[i++] != 0 && i < safetycheck)	 
	{
		length++;
	}
	return length;
}


int JString::Compare(const JString& string) const
{
	if(m_pBuffer == NULL) //strcmp ei arvosta nullia
	{
		return -1;
	}
	else if (string.m_pBuffer == NULL) //strcmp ei arvosta nullia
	{
		return 1;
	}
	else
	{
		return ::strcmp(m_pBuffer, string.m_pBuffer);
	}
}

int JString::GetLength(void) const
{
	if(m_pBuffer && m_iCount >0) 
	{
		return m_iCount - 1;
	}
	else
	{
		return 0;
	}
}

void JString::Add(const JString& string)
{
	//compute needed length
	const int count = GetLength() + string.GetLength() + 1;
	if(count < 1) //sanitycheck
	{
		return;
	}
	//copy contents of our buffer into the temporary string
	JString temp(m_pBuffer);

	Reset();

	if(AllocateBuffer(count))
	{
		//copy the first part into the new buffer
		::memcpy(m_pBuffer, temp.m_pBuffer, temp.GetLength());

		//copy the new part into the new buffer
		::memcpy(	&m_pBuffer[temp.GetLength()],
					string.m_pBuffer,
					string.GetLength());
	}

}

const char* JString::GetBuffer() const
{
	return m_pBuffer;
}

/**
 * ------------------------------------------------------
 * Overridden operators
 */

JString::operator const char*() const
{
	return m_pBuffer;
}

JString& JString::operator = (const JString& string)
{
	Set(string);
	return *this;
}

const char JString::operator [] (const int iIndex) const
{
	if(m_pBuffer && iIndex < m_iCount)
	{
		return m_pBuffer[iIndex];
	}
	else
	{
		return 0;
	}
}

JString& JString::operator += (const JString& string)
{
	Add(string);
	return *this;
}

bool JString::operator == (const JString& string) const
{
	return (Compare(string) == 0) ? true : false;
}

bool JString::operator != (const JString& string) const
{
	return (Compare(string) != 0) ? true : false;
}

