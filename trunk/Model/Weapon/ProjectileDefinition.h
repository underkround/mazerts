/**
 * Definition struct for projectile
 * Used to create projectile.
 *
 * $Revision$
 * $Date$
 * $Id$
 */

// STC (heavily) - exists just so that weapon.h compiles

#ifndef __PROJECTILEDEFINITION_H__
#define __PROJECTILEDEFINITION_H__

class ProjectileDefinition
{
public:

    enum Type
    {
        BULLET,
        SHELL
    };

    inline ProjectileDefinition(int projectileId) : id (projectileId) { }
    ~ProjectileDefinition();

/**
 * Public properties for stuct-style class.
 * Zero values usually equal infinite.
 */
    const int   id;
    Type        type;
    float       rateOfFire;
    int         clipSize;

};

#endif // __PROJECTILEDEFINITION_H__
