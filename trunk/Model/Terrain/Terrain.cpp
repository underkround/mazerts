#include "Terrain.h"
//#include "DefaultTerrainGenerator.h"
#include "../Asset/AssetCollection.h"

#include "../Weapon/Damage.h"

const float Terrain::DIAGONAL_MOVECOST_FACTOR = 1.4142f;

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
    m_ChangeCounter = 0;

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
        m_pTerrainGenerator->generateHeightmap(m_ppVertexHeightData, m_Size+1);
    }

    // calculate tile heights and mark as passable/non-passable
    calculateHeightMapFromVertices();

    //Alter changecounter
    m_ChangeCounter++;

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
    m_ppPassableTile = NULL;

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
            short baseCost = m_ppTerrainHeightData[targetY][targetX] - m_ppTerrainHeightData[y][x];
            
            //Penalize uphills
            /*if(baseCost > 0)
            {
                baseCost <<= 2;
            }*/

            //Calculate offsetted value
            short moveCost = baseCost + MOVECOST_OFFSET;

            if(moveCost > MOVECOST_MAX || moveCost < MOVECOST_MIN)
            {
                return MOVE_ILLEGAL;
            }

            //If there's a unit in the square, penalize movecost
            /*if(AssetCollection::getUnitsAt(NULL, targetX-2, targetY-2, 5, 5) != 0)
            {
                moveCost += 1000;
            }*/

            moveCost <<= MOVECOST_FACTOR;

            if(dirX != 0 && dirY != 0)
            {
                return (short)(moveCost * DIAGONAL_MOVECOST_FACTOR);
            }
            else
            {
                return moveCost;
            }
        }
    }
}

float Terrain::getUnitMoveSpeed(const short x, const short y, const signed char dirX, const signed char dirY) const
{

    short targetX = x + dirX;
    short targetY = y + dirY;
    
    //Bounds check
    if(targetX < 0 ||targetX >= m_Size || targetY < 0 ||targetY >= m_Size)
    {
        return 0.0f;
    }
    else
    {
        //Check if the tile is passable
        if(!m_ppPassableTile[targetY][targetX])
        {
            return 0.1f;
        }
        else
        {
            //Calculate speed factor
            float moveSpeed = (m_ppTerrainHeightData[y][x] - m_ppTerrainHeightData[targetY][targetX] + MOVECOST_THRESHOLD) / (MOVECOST_THRESHOLD * 2.0f);
            
            if(moveSpeed <= 0.1f)
            {
                moveSpeed = 0.1f;
            }
            if(moveSpeed > 1.0f)
            {
                moveSpeed = 1.0f;
            }

            return moveSpeed;
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
        m_ChangeCounter++;

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

    m_ChangeCounter++;
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
    //Bounds check
    if(x < 0 ||x >= m_Size || y < 0 ||y >= m_Size)
    {
        return;
    }
    m_ppTerrainHeightData[y][x] = 
    (m_ppVertexHeightData[y][x] + 
    m_ppVertexHeightData[y][x+1] + 
    m_ppVertexHeightData[y+1][x] + 
    m_ppVertexHeightData[y+1][x+1]) >> 2;


    //PASSABLE CHECKS
    //By default, a tile is passable, but the following checks can mark it as unpassable
    m_ppPassableTile[y][x] = true;

    //If the height difference between vertices is too high, the resulting quad
    //will be too steep for units to travel
    unsigned char min = 255;
    unsigned char max = 0;

    for(int i = 0; i < 2; i++)
    {
        for(int j = 0; j < 2; j++)
        {
            int xval = x+j;
            int yval = y+i;
            if(m_ppVertexHeightData[yval][xval] < min)
            {
                min = m_ppVertexHeightData[yval][xval];
            }

            if(m_ppVertexHeightData[yval][xval] > max)
            {
                max = m_ppVertexHeightData[yval][xval];
            }
        }
    }
    
    if((max-min) > MOVECOST_THRESHOLD)
    {
        m_ppPassableTile[y][x] = false;
        return;
    }

    //If both triangles of the tile (quad) aren't on the same plane, it's unpassable
    //TODO: Note to self:
    //Integer-calculation actually causes a 0.5-rounding error, which means that
    //tiles with slightly different triangle normals fall through also (+-1 difference
    //in heíght-difference of the vertex-pairs)
    //Have to check later if this is an issue, and if need be, convert to floating point
    /*unsigned short line1aver = (m_ppVertexHeightData[y][x] + 
                        m_ppVertexHeightData[y+1][x+1]) >> 1;

    unsigned short line2aver = (m_ppVertexHeightData[y+1][x] + 
                        m_ppVertexHeightData[y][x+1]) >> 1;

    if(line1aver != line2aver)
    {
        m_ppPassableTile[y][x] = false;
        return;
    }*/

    //Waterlevel-check: if the middle of the tile is below water, it is not passable
    if(m_WaterLevel > m_ppTerrainHeightData[y][x])
    {
        m_ppPassableTile[y][x] = false;
        return;
    }
}


void Terrain::smoothMap(int smoothPasses)
{
    int size = m_Size+1;

    //Smoothing cycles
    for(int i = 0; i < smoothPasses; i++)
    {
        //Create another array for the results
        //If the results were directly written to the vertices,
        //the smoothing would be affected by previous changes (last column/row)
        unsigned char** tempArray;

        tempArray = new unsigned char*[size];
        for(int i = 0; i < size; i++)
        {
            tempArray[i] = new unsigned char[size];
        }


        for(int x = 0; x < size; x++)
        {
            for(int y = 0; y < size; y++)
            {
                

                //Bounds checking, if the border is reached, we use a value
                //from the opposite side of the heightmap
                int xneg = x-1;
                if(xneg < 0) xneg = 0;//+= size;

                int yneg = y-1;
                if(yneg < 0) yneg = 0;//+= size;

                int xpos = x+1;
                if(xpos > m_Size) xpos = m_Size;// xpos -= size;

                int ypos = y+1;
                if(ypos > m_Size) ypos = m_Size; //ypos -= size;

                //Calculate sum from the diagonal vertices, divide by 16
                int diagonal = (m_ppVertexHeightData[ypos][xpos] + m_ppVertexHeightData[ypos][xneg]
                                +  m_ppVertexHeightData[yneg][xpos] + m_ppVertexHeightData[yneg][xneg]) >> 4;
                
                //Calculate the sum of adjacent vertices, and divide by 8                
                int adjacent = (m_ppVertexHeightData[ypos][x]%256 + m_ppVertexHeightData[y][xneg]%256
                                +  m_ppVertexHeightData[yneg][x]%256 + m_ppVertexHeightData[y][xpos]%256) >> 3;

                //Middle: the curren vertice, value is divided by 4
                int middle = m_ppVertexHeightData[y][x] >> 2;

                //The final smoothed result is got from summing the above values                
                tempArray[y][x] = diagonal + adjacent + middle;                                         
                                
            }
        }

        //Switch the heightmap values to the ones in the temporary array
        for(int x = 0; x < size; x++)
        {
            for(int y = 0; y < size; y++)
            {
                m_ppVertexHeightData[y][x] = tempArray[y][x];
            }
        }

        for(int i = 0; i < size; i++)
        {
            delete [] tempArray[i];
        }
        delete [] tempArray;

        //calculateHeightMapFromVertices();
    }//Smoothing cycle

    m_ChangeCounter++;
}

void Terrain::handleDamage(const unsigned short x, const unsigned short y, float damage)
{
    if(m_ppVertexHeightData[y][x] > 10 && damage > 1000)
    {
        const float sizeFactor = damage / 1000.0f;
        const int craterRad = (int)(7 * sizeFactor);
        const int craterDepth = (int)(10 * sizeFactor);

        unsigned short cornerX = x - craterRad;
        unsigned short cornerY = y - craterRad;
                
        int craterRadSquared = craterRad*craterRad;
        
        for(int i = 0; i < (2 * craterRad); i++)
        {
            for(int j = 0; j < (2 * craterRad); j++)
            {
                unsigned short curX = cornerX + j;
                unsigned short curY = cornerY + i;
                
                int dist = (x-curX)*(x-curX)+(y-curY)*(y-curY);
                if(dist < craterRadSquared)
                {
                    if(curX >= 0 && curX < m_Size && curY >= 0 && curY < m_Size)
                    {
                        int heightMod = (int)((craterDepth / 2)*(1 + cos(3.1415f * dist / craterRadSquared) ));
                        if(m_ppVertexHeightData[curY][curX] > heightMod)
                        {
                            m_ppVertexHeightData[curY][curX] -= heightMod;
                        }
                        else
                        {
                            m_ppVertexHeightData[curY][curX] = 0;
                        }
                    }
                }

            }
        }
        m_ChangeCounter++;
    }
}
