/**
 * MeshFileNames
 * Contains mesh filenames and offsets for different units and buildings
 */


#ifndef __MESHFILENAMES_H__
#define __MESHFILENAMES_H__

#include <tchar.h>

static const int NUMBER_OF_UNITS_WITH_MESHES = 6;

/**
 * Contains filenames for different meshes, 3 for every unit (leave unused parts as _T("") )
 */
static const TCHAR g_ppUnitMeshNames[NUMBER_OF_UNITS_WITH_MESHES][3][256] = 
{
    _T("../data/mesh/car_base.x"),              //Car, no separater barrel
    _T("../data/mesh/car_gun.x"),
    _T(""),                                     //DO NOT USE NULL for empty parts (they won't be included 
                                                //in the array at all, and the indices will be fucked up)

    _T("../data/mesh/basictank_base.x"),        //Basictank base
    _T("../data/mesh/basictank_turret.x"),      //turret
    _T(""),


    _T("../data/mesh/supertank_base.x"),        //Supertank base
    _T("../data/mesh/supertank_turret.x"),      //turret
    _T("../data/mesh/supertank_cannon.x"),      //barrels

    _T(""),                                     //Missiletank base
    _T(""),                                     //turret
    _T(""),                                     //barrel

    _T("../data/mesh/nuker_base.x"),            //Nukecannon base
    _T("../data/mesh/nuker_turret.x"),          //turret
    _T("../data/mesh/nuker_cannon.x"),          //barrel

    _T("../data/mesh/truck.x"),                 //Resource truck base
    _T(""),                                     //turret
    _T("")                                      //barrel

};

/**
 * Contains offsets for different meshes, bases is relative to world, gun is relative to base, barrel is relative to gun
 */
static const float g_ppUnitMeshOffsets[NUMBER_OF_UNITS_WITH_MESHES][3][3] =
{
    0, 0, 0,                //Car base
    0, 1.0f, 0.65f,         //Car gun
    0, 0, 0,                //Car barrel (not used)

    0, 0, 0,         //Basictank
    0, 0.65f, 0.45f,
    0, 0, 0,

    0, 0, 0,         //Supertank
    0, 1.4f, 0.90f,
    0, 0.4f, -0.65f,

    0, 0, 0,         //Missiletank
    0, 0, 0,
    0, 0, 0,

    0, 0, 0,         //Nukecannon
    0, 0.8f, 0.4f,
    0, 1.2f, 1.6f,

    0, 0, 0,         //Mining/resource truck
    0, 0, 0,
    0, 0, 0
};

//For firing effects
static const float g_pUnitBarrelLength[NUMBER_OF_UNITS_WITH_MESHES] =
{
    0.8f,   //Car
    1.75f,  //Basic tank
    3.2f,   //Supertank
    0.0f,   //Missiletank
    4.0f,   //Nukecannon
    0.0f    //Mining truck
};


static const int NUMBER_OF_BUILDINGS_WITH_MESHES = 3;

/**
 * Contains filenames for different meshes, 3 for every building (leave unused parts as _T("") )
 */
static const TCHAR g_ppBuildingMeshNames[NUMBER_OF_BUILDINGS_WITH_MESHES][3][256] = 
{
    _T("../data/mesh/ore_mine.x"),
    _T(""),                                     //DO NOT USE NULL for empty parts (they won't be included 
                                                //in the array at all, and the indices will be fucked up)
    _T(""),

    _T("../data/mesh/tehdas.x"),
    _T(""),                                                
    _T(""),

    _T("../data/mesh/yard.x"),
    _T(""),                                                
    _T("")

};

/**
 * Contains offsets for different meshes, bases is relative to world, gun is relative to base
 */
static const float g_ppBuildingMeshOffsets[NUMBER_OF_BUILDINGS_WITH_MESHES][3][3] =
{
    0, 0, 0,                //Ore mine base
    0, 0, 0,                //No gun, so values won't matter, have to be here anyway for indice correctness
    0, 0, 0,

    0, 0, 0,                //Factory base
    0, 0, 0,
    0, 0, 0,

    0, 0, 0,                //Resource yard base
    0, 0, 0,
    0, 0, 0
};





static const int NUMBER_OF_TEXTURES = 2;

enum Textures
{
    BALLTEXTURE,
    STARTEXTURE
};

static const TCHAR g_ppTextureNames[NUMBER_OF_TEXTURES][256] = 
{
    _T("../data/textures/16ball.png"),
    _T("../data/textures/16star.png")
};

#endif
