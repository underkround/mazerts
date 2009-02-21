/**
 * IUpdatable
 *
 * Interface for updatable objects
 *
 * $Revision$
 * $Date$
 * $Id$
 */
 
#ifndef __IUPDATABLE_H__
#define __IUPDATABLE_H__

class IUpdatable
{
public:

    // response codes for update method
    static const char RESPONSE_OK           = 0; // normal update call return flag
    static const char RESPONSE_DESTROYME    = 2; // indication that the subject needs to be destroyed

    //Not instantiable
    IUpdatable(void)
    {
    }
    virtual ~IUpdatable(void)
    {
    }

    /**
     * Updates the object
     * @param deltaT Time passed since last update call in seconds
     * @return Boolean, meaning depends on the implementing object
     */
    virtual char update(const float deltaT) = 0;
   
};

#endif