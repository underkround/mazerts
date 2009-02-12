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
    //Not instantiable
    IUpdatable(void);
    virtual ~IUpdatable(void);

    /**
     * Updates the object
     * @param deltaT Time passed since last update call in seconds
     * @return Boolean, meaning depends on the implementing object
     */
    virtual bool update(float deltaT) = 0;

   
};

#endif