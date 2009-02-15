#include "Terrain.h"
#include "DefaultTerrainGenerator.h"

Terrain* Terrain::pTerrain = NULL;
ITerrainGenerator* Terrain::pTerrainGenerator = new DefaultTerrainGenerator();

Terrain& Terrain::createNew(const int seed, const unsigned short size)
{
    //Check for existing instance
    if(pTerrain != NULL)
    {
        Terrain::release();
    }

    Terrain::pTerrain = new Terrain(seed, size);

    return *pTerrain;
}

Terrain& Terrain::getInstance()
{
    if(!pTerrain)
    {
        pTerrain = new Terrain();
    }

    return *pTerrain;
}

void Terrain::release()
{
    if(Terrain::pTerrain)
    {
        delete Terrain::pTerrain;
        Terrain::pTerrain = NULL;
    }
}

void Terrain::initialize()
{
    //Initialize arrays
    m_ppTerrainHeightData = new unsigned char*[m_Size];
    m_ppPassableTile = new bool*[m_Size];

    //Initialize sub-arrays
    for(int i = 0; i < m_Size; i++)
    {
        m_ppTerrainHeightData[i] = new unsigned char[m_Size];
        m_ppPassableTile[i] = new bool[m_Size];
    }

    //Initialize vertex-height array
    m_ppVertexHeightData = new unsigned char*[m_Size + 1];

    //Initialize sub-arrays
    for(int i = 0; i < (m_Size+1); i++)
    {
        m_ppVertexHeightData[i] = new unsigned char[m_Size + 1];
    }
    
    int foo = (int)m_ppTerrainHeightData[4];

    //Flatten map and reset values
    flattenMap(DEFAULT_FLATHEIGHT);

    foo = (int)m_ppTerrainHeightData[4];    
    //Generate the terrain into vertex-data
    pTerrainGenerator->generateTerrain(m_Seed, m_Size, m_ppVertexHeightData);
    foo = (int)m_ppTerrainHeightData[4];

    //Calculate tile heights and mark as passable/non-passable
    calculateHeightMapFromVertices();
    foo = (int)m_ppTerrainHeightData[4];
}

void Terrain::destroy()
{
    //Destroy passable-array
    for(int i = 0; i < m_Size; i++)
    {
        delete [] m_ppPassableTile[i];
    }

    //Destroy array
    delete [] m_ppPassableTile;

    //Destroy data for vertices
    for(int i = 0; i < (m_Size + 1); i++)
    {
        delete [] m_ppVertexHeightData[i];
    }
    delete [] m_ppVertexHeightData;
    m_ppVertexHeightData = NULL;


    //Destroy data for heights
    for(int i = 0; i < m_Size; i++)
    {
        delete [] m_ppTerrainHeightData[i];
    }

    //Destroy array
    delete [] m_ppTerrainHeightData;
    
    m_ppTerrainHeightData = NULL;

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
    //in heíght-difference of the vertex-pairs)
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
