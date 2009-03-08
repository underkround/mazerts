#pragma once
/**
 * Faction encapsulates player or AI in game, keeping track of its
 * resources and stuff.
 *
 * $Revision$
 * $Date$
 * $Id$
 */
class Faction
{
public:
    Faction(void);
    virtual ~Faction(void);

private:
    int m_Resource;
    int m_Energy;
};
