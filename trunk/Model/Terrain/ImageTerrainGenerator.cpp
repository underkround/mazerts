#include "ImageTerrainGenerator.h"
#include <fstream>

using namespace std;

#define BITMAP_DATA_OFFSET 0x436
#define BITMAP_SIZE_OFFSET 0x12

//#define BITMAP_PADDING_BYTES 2

ImageTerrainGenerator::ImageTerrainGenerator()
{
    m_pFilename = NULL;
    initialize(DEFAULT_FILENAME);
}

ImageTerrainGenerator::ImageTerrainGenerator(const char* filename)
{
    m_pFilename = NULL;
    initialize(filename);
}

void ImageTerrainGenerator::initialize(const char* filename)
{
    //m_pFilename = filename; // no longer working since i change the param to const -z

    // copy the const string into our member string
    if(filename)
    {
        const int ccount = strlen(filename) + 1;
        m_pFilename = new char[ccount];
        if(m_pFilename)
        {
            m_pFilename[ccount -1] = 0;
            ::memcpy(m_pFilename, filename, ccount);
        }
    }


    ifstream file(m_pFilename, ios::in|ios::binary|ios::beg);
    if (file.is_open())
    {
        char* check = new char[2];
        file.read(check, 2);
        if (check[0] != 'B' || check[1] != 'M')
        {
            // not a bmp file
            m_Size = DEFAULT_SIZE;
            m_pFilename = NULL;
        }
        else
        {
            char* width = new char[4];
            file.seekg(BITMAP_SIZE_OFFSET, ios::beg);
            file.read(width, 4);
            file.close();

            m_Size = (width[3] << 24) + (width[2] << 16) + (width[1] << 8) + width[0];
            delete[] width;
        }
        delete[] check;
    }
    else
    {
        // TODO: Handle error?
        m_Size = DEFAULT_SIZE;
        m_pFilename = NULL;
    }
}
    
void ImageTerrainGenerator::generateHeightmap(unsigned char** ppVertexHeightData, const unsigned short terrainSize)
{
    if (!m_pFilename) return;
    char* data = new char[m_Size * m_Size];

    ifstream file(m_pFilename, ios::in|ios::binary|ios::beg);
    if (file.is_open())
    {
        file.seekg(BITMAP_DATA_OFFSET, ios::beg);
        // file.read(data, BITMAP_SIZE * (BITMAP_SIZE + BITMAP_PADDING_BYTES));
        file.read(data, m_Size * m_Size);
        file.close();

        for (int y = 0; y < m_Size; y++)
            for (int x = 0; x < m_Size; x++) {
                //int i = y * (BITMAP_SIZE + BITMAP_PADDING_BYTES) + x;
                int i = y * m_Size + x;
                unsigned char uc = (unsigned char)data[i];
                //ppVertexHeightData[BITMAP_SIZE - 1 - y][x] = uc; // "right" y-coordinate
                ppVertexHeightData[y][x] = uc; // reversed y-coordinate
            }

        delete[] data;
    }
    else
    {
        // TODO: Handle error?
    }
}

