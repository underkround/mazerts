#include "Terrain.h"
//#include "DefaultTerrainGenerator.h"

//Terrain* Terrain::pTerrain = NULL;
//ITerrainGenerator* Terrain::pTerrainGenerator = new DefaultTerrainGenerator();

Terrain* Terrain::getInstance()
{
    static Terrain instance;
    return &instance;
}


Terrain::Terrain()
{
    m_Size          = Terrain::DEFAULT_MAPSIZE;
    m_WaterLevel    = Terrain::DEFAULT_WATERLEVEL;
    m_pTerrainGenerator = NULL; // no generator by default, results flat
    m_Initialized = false;
    initialize(); // initialize flat with defaults
}


Terrain::~Terrain()
{
    releaseGenerator(); // release and delete the generator
    if(m_Initialized)
    {
        destroy();
    }
}


void Terrain::initialize()
{
    // if this is not the first time initializing, cleanup old data
    if(m_Initialized)
    {
        destroy();
    }

    // if generator is set, get preferred size from it
    if(m_pTerrainGenerator)
    {
        // if the generator passes 0 as preferred size, it means we'll use our size
        if(m_pTerrainGenerator->getPreferredSize() > 0)
        {
            m_Size = m_pTerrainGenerator->getPreferredSize();
        }
    }

    //Initialize arrays
    m_ppTerrainHeightData = new unsigned char*[m_Size];
    m_ppPassableTile = new bool*[m_Size];

    // initialize sub-arrays
    for(int i = 0; i < m_Size; i++)
    {
        m_ppTerrainHeightData[i] = new unsigned char[m_Size];
        m_ppPassableTile[i] = new bool[m_Size];
    }

    // initialize vertex-height array
    m_ppVertexHeightData = new unsigned char*[m_Size + 1];

    // initialize sub-arrays
    for(int i = 0; i < (m_Size+1); i++)
    {
        m_ppVertexHeightData[i] = new unsigned char[m_Size + 1];
    }

    // flatten map and reset values
    flattenMap(DEFAULT_FLATHEIGHT);

    // give the vertex data to generator, if any
    if(m_pTerrainGenerator)
    {
        m_pTerrainGenerator->generateHeightmap(m_ppVertexHeightData, m_Size);
    }

    // calculate tile heights and mark as passable/non-passable
    calculateHeightMapFromVertices();

    m_Initialized = true; // set the flag
}


void Terrain::initialize(const unsigned short size)
{
    if(m_Initialized)
    {
        // cleanup before changing members, since destroy uses m_Size
        // so it cannot be changed before cleanup
        destroy();
    }
    m_Size = size;
    initialize(); // setup flat
}


void Terrain::initialize(ITerrainGenerator* generator)
{
    // delete the previously used generator, if any
    releaseGenerator();
    // cleanup before changing members, since destroy uses m_Size
    // so it cannot be changed before cleanup
    destroy();
    // set the new generator
    m_pTerrainGenerator = generator;
    // initialize uses the generator if it's set
    initialize();
}


void Terrain::releaseGenerator()
{
    if(m_pTerrainGenerator)
    {
        m_pTerrainGenerator->release();
        delete m_pTerrainGenerator;
        m_pTerrainGenerator = NULL;
    }
}


void Terrain::release()
{
    // temporarily store the generator so initialization does not use it
    ITerrainGenerator* gen = m_pTerrainGenerator;
    m_pTerrainGenerator = NULL;
    destroy();
    initialize(1);
    m_pTerrainGenerator = gen;
}


void Terrain::destroy()
{
    // sanity check
    if(!m_Initialized)
        return;

    // do NOT destroy generator here, since this is called between the
    // initializations, and same generator could be used with them

    // destroy passable-array
    for(int i = 0; i < m_Size; i++)
    {
        delete [] m_ppPassableTile[i];
    }

    // destroy array
    delete [] m_ppPassableTile;

    // destroy data for vertices
    for(int i = 0; i < (m_Size + 1); i++)
    {
        delete [] m_ppVertexHeightData[i];
    }
    delete [] m_ppVertexHeightData;
    m_ppVertexHeightData = NULL;


    // destroy data for heights
    for(int i = 0; i < m_Size; i++)
    {
        delete [] m_ppTerrainHeightData[i];
    }

    // destroy array
    delete [] m_ppTerrainHeightData;
    
    m_ppTerrainHeightData = NULL;
    m_Initialized = false;
}


short Terrain::getMoveCost(const short x, const short y, const signed char dirX, const signed char dirY) const
{
    short targetX = x + dirX;
    short targetY = y + dirY;
    
    //Bounds check
    if(targetX < 0 ||targetX >= m_Size || targetY < 0 ||targetY >= m_Size)
    {
        return MOVE_OUTOFBOUNDS;
    }
    else
    {
        //Check if the tile is passable
        if(!m_ppPassableTile[targetY][targetX])
        {
            return MOVE_ILLEGAL;
        }
        else
        {
            //Calculate offsetted value
            short moveCost = m_ppTerrainHeightData[targetY][targetX] - m_ppTerrainHeightData[y][x] + MOVECOST_OFFSET;

            if(moveCost > MOVECOST_MAX || moveCost < MOVECOST_MIN)
            {
                return MOVE_ILLEGAL;
            }

            return moveCost;
        }
    }
}


void Terrain::setTerrainVertexHeight(const short x, const short y, const unsigned char height)
{
    //Bounds checking
    if(x < 0 ||x > m_Size || y < 0 ||y > m_Size)
    {
        return;
    }
    else
    {
        m_ppVertexHeightData[y][x] = height;

        //Calculate changes in tileheights
        //The calculateTileHeight takes care of bounds checking
        calculateTileHeight(x, y);
        calculateTileHeight(x - 1, y);
        calculateTileHeight(x, y - 1);
        calculateTileHeight(x - 1, y - 1);
    }
    
}

void Terrain::flattenMap(const unsigned char heightValue)
{
    //Flatten tile heights and mark all as passable
    for(int i = 0; i < m_Size; i++)
    {
        for(int j = 0; j < m_Size; j++)
        {
            m_ppTerrainHeightData[i][j] = heightValue;
            m_ppPassableTile[i][j] = true;
        }
    }

    //Flatten vertices
    for(int i = 0; i < (m_Size+1); i++)
    {
        for(int j = 0; j < (m_Size+1); j++)
        {
            m_ppVertexHeightData[i][j] = heightValue;
        }
    }

}


void Terrain::calculateHeightMapFromVertices()
{
    for(int y = 0; y < m_Size; y++)
    {
        for(int x = 0; x < m_Size; x++)
        {
            calculateTileHeight(x, y);
        }
    }
}


void Terrain::calculateTileHeight(const short x, const short y)
{
    if(x < 0 ||x >= m_Size || y < 0 ||y >= m_Size)
    {
        return;
    }
    m_ppTerrainHeightData[y][x] = 
    (m_ppVertexHeightData[y][x] + 
    m_ppVertexHeightData[y][x+1] + 
    m_ppVertexHeightData[y+1][x] + 
    m_ppVertexHeightData[y+1][x+1]) >> 2;

    //If both triangles of the tile (quad) aren't on the same plane, it's unpassable
    //TODO: Note to self:
    //Integer-calculation actually causes a 0.5-rounding error, which means that
    //tiles with slightly different triangle normals fall through also (+-1 difference
    //in he�ght-difference of the vertex-pairs)
    //Have to check later if this is an issue, and if need be, convert to floating point
    unsigned short line1aver = (m_ppVertexHeightData[y][x] + 
                        m_ppVertexHeightData[y+1][x+1]) >> 1;

    unsigned short line2aver = (m_ppVertexHeightData[y+1][x] + 
                        m_ppVertexHeightData[y][x+1]) >> 1;

    if(line1aver == line2aver)
    {
        m_ppPassableTile[y][x] = true;
    }
    else
    {
        m_ppPassableTile[y][x] = false;
    }
}
