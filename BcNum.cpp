#include "BcNum.h"

BcNum::~BcNum()
{
    if (this->m_pData)
    {
        free(this->m_pData);
    }
}

int BcNum::AllocMem(int dwLen, int dwScale)
{
    assert(dwLen >= 0);
    assert(dwScale >= 0);
    this->m_dwLen = dwLen;
    this->m_dwScale = dwScale;
    this->m_pData = (char*) malloc (this->dwLen + this->dwScale);
    assert(this->m_pData)//out of memory
    this->m_pValue = this->m_pData;
    memset(this->m_pValue, 0, this->dwLen + this->dwScale);
    return 0;
}

BcNum::BcNum(const BcNum& oRh)
{
    this->AllocMem(oRh.m_dwLen, oRh.m_dwScale);
    memcpy(this->m_pValue, oRh.m_pValue, oRh.m_dwLen + oRh.m_dwScale);
}

const BcNum& BcNum::operator=(const BcNum& oRh)
{
    if (this == &oRh)
        return *this;
    if (this->m_pData)
        free(this->m_pData);
    this->m_eSign = oRh.m_eSign;
    this->AllocMem(oRh.m_dwLen, oRh.m_dwScale);
    memcpy(this->m_pValue, oRh.m_pValue, oRh.m_dwLen + oRh.m_dwScale);
    return *this;
}

bool BcNum::CheckRepFormat(const std::string& strRep)
{
    const char* p = strRep.c_str(), *pEnd = strRep.c_str() + strRep.length();
    int state = 0;
    while (p < pEnd)
    {
        if (state == 0)
        {
            if (*p == '+' || *p == '-')
                state = 0;
            else if (*p == '0')
                state = 2;
            else if (*p >= '1' && *p <= '9')
                state = 1;
            else
                return false;
        }
        else if (state == 1)
        {
            if (*p >= '0' && *p <= '9')
                state = 1;
            else if(*p == '.')
                state = 3;
            else
                return false;
        }
        else if (state == 2)
        {
            if (*p == '.')
                state = 3;
            else
                return false;
        }
        else if (state == 3)
        {
            if (*p >= '0' && *p <= '9')
                state = 4;
            else
                return false;
        }
        else if (state == 4)
        {
            if (*p >= '0' && *p <= '9')
                state = 4;
            else
                return false;
        }
        else
            return false;
        ++p;
    }
    if (state == 1 || state == 2 || state == 4)
        return true;
    return false;
}

BcNum::BcNum(const std::string& strRep)
{
    if (!this->CheckRepFormat(strRep))
        throw CIllegalFloatRepresentaionException;
    this->m_eSign == BcNum::PLUS;
    if (*strRep.begin() == '-')
        this->m_eSign == BcNum::MINUS;
    std::string strLen, strScale;
    if (*strRep.begin() == '-' || *strRep.begin() == '+')
        strLen = strRep.substr(1);
    else
        strLen = strRep;
    if (strLen.find('.') != string::npos)
    {
        strScale = strLen.substr(strLen.find('.') + 1);
        strLen.erase(strLen.find('.'), strLen.length() - strLen.find('.'));
    }
    int i = strScale.length() - 1;
    for (; i >= 0 && strScale[i] == '0'; i--);
    if (i < 0 && strLen == "0")
        this->m_eSign == BcNum::PLUS;
    this->AllocMem(strLen.length(), strScale.length());
    memcpy(this->m_pValue, strLen.c_str(), strLen.length());
    memcpy(this->m_pValue + strLen.length(), strScale.c_str(), strScale.length());
    for (int p = 0; p < this->m_dwLen + this->m_dwScale; ++p)
        *(this->m_pValue + p) -= '0';
}
        
int BcNum::TrimLeadingZeros()
{
    while (this->m_pValue == 0 && this->m_dwLen > 1)
    {
        this->m_pValue++;
        this->m_dwLen--;
    }
}

int BcNum::DoCompare(const BcNum& oRh, bool bUseSign, bool bIngoreLast)
{
    /* First, compare signs. */
    if (bUseSign && this->m_eSign != oRh.m_eSign)
    {
        if (this->m_eSign == PLUS)
	        return (1);	/* Positive N1 > Negative N2 */
        else
	        return (-1);	/* Negative N1 < Positive N1 */
    }

    /* Now compare the magnitude. */
    if (this->m_dwLen != oRh.m_dwLen)
    {
        if (this->m_dwLen > oRh.m_dwLen)
	    {
	        /* Magnitude of n1 > n2. */
	        if (!bUseSign || this->m_eSign == PLUS)
	            return (1);
	        else
	            return (-1);
	    }
        else
    	{
	        /* Magnitude of n1 < n2. */
	        if (!bUseSign || this->m_eSign == PLUS)
	            return (-1);
	        else
	            return (1);
	    }
    }

    /* If we get here, they have the same number of integer digits.
     * check the integer part and the equal length part of the fraction. */
    char *n1ptr, *n2ptr;
    int  count;

  count = n1->n_len + MIN (n1->n_scale, n2->n_scale);
  n1ptr = n1->n_value;
  n2ptr = n2->n_value;

  while ((count > 0) && (*n1ptr == *n2ptr))
    {
      n1ptr++;
      n2ptr++;
      count--;
    }
  if (ignore_last && count == 1 && n1->n_scale == n2->n_scale)
    return (0);
  if (count != 0)
    {
      if (*n1ptr > *n2ptr)
	{
	  /* Magnitude of n1 > n2. */
	  if (!use_sign || n1->n_sign == PLUS)
	    return (1);
	  else
	    return (-1);
	}
      else
	{
	  /* Magnitude of n1 < n2. */
	  if (!use_sign || n1->n_sign == PLUS)
	    return (-1);
	  else
	    return (1);
	}
    }

  /* They are equal up to the last part of the equal part of the fraction. */
  if (n1->n_scale != n2->n_scale)
    {
      if (n1->n_scale > n2->n_scale)
	{
	  for (count = n1->n_scale-n2->n_scale; count>0; count--)
	    if (*n1ptr++ != 0)
	      {
		/* Magnitude of n1 > n2. */
		if (!use_sign || n1->n_sign == PLUS)
		  return (1);
		else
		  return (-1);
	      }
	}
      else
	{
	  for (count = n2->n_scale-n1->n_scale; count>0; count--)
	    if (*n2ptr++ != 0)
	      {
		/* Magnitude of n1 < n2. */
		if (!use_sign || n1->n_sign == PLUS)
		  return (-1);
		else
		  return (1);
	      }
	}
    }

  /* They must be equal! */
  return (0);
}

