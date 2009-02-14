#include "Terrain.h"

Terrain* Terrain::pTerrain = NULL;

Terrain& Terrain::createNew(const int seed, const short size)
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
    //Initialize array
    m_ppTerrainHeightData = new unsigned char*[m_Size];

    //Initialize sub-arrays
    for(int i = 0; i < m_Size; i++)
    {
        m_ppTerrainHeightData[i] = new unsigned char[m_Size];
    }

    //Initialize vertex-height array
    m_ppVertexHeightData = new unsigned char*[m_Size + 1];

    //Initialize sub-arrays
    for(int i = 0; i < (m_Size+1); i++)
    {
        m_ppVertexHeightData[i] = new unsigned char[m_Size + 1];
    }
    
    flattenMap(DEFAULT_FLATHEIGHT);
    generateMap();
    calculateHeightMapFromVertices();
}

void Terrain::destroy()
{
    //Destroy data for vertices
    for(int i = 0; i < m_Size; i++)
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
    if(targetX < 0 ||targetX > m_Size || targetY < 0 ||targetY > m_Size)
    {
        return -1;
    }
    else
    {
        short moveCost = m_ppTerrainHeightData[targetY][targetX] - m_ppTerrainHeightData[y][x];
        /*if(moveCost < 0)
        {
            moveCost = -moveCost;
        }*/
        return moveCost;
    }
}


void Terrain::setTerrainVertexHeight(const short x, const short y, const unsigned char height)
{
    //Bounds checking
    if(x < 0 ||x > m_Size+1 || y < 0 ||y > m_Size+1)
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


void Terrain::generateMap()
{
    //TODO: TBD, this should modify VERTEXDATA
}

void Terrain::flattenMap(const unsigned char heightValue)
{
    //Flatten tile heights
    for(int i = 0; i < m_Size; i++)
    {
        for(int j = 0; j < m_Size; j++)
        {
            m_ppTerrainHeightData[i][j] = heightValue;
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
}
