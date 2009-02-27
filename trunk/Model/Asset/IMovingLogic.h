/**
 * Interface for unit's logic that takes commands
 *
 * $Revision$
 * $Date$
 * $Id$
 */
#ifndef __IMOVINGLOGIC_H__
#define __IMOVINGLOGIC_H__

class IUnitLogic
{
public:

    void attach(IAsset* host);

    /**
     * @return true if this component is to be attached
     */
    bool release();

    /**
     * GetActions
private:

};

#endif // __IMOVINGLOGIC_H__
