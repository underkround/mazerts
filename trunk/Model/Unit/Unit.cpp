#include "Unit.h"

Unit::Unit(void)
{
    // by default set unit size to 1*1 cell
    m_Width = 1;
    m_Height = 1;
}


Unit::~Unit(void)
{
}


bool Unit::update(float deltaT)
{
    return true;
}