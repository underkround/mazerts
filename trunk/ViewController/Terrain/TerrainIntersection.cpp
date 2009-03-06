#include "TerrainIntersection.h"
#include "UITerrain.h"
#include "../../Model/Terrain/Terrain.h"

#include <tchar.h>

#include "../3DDebug/UI3DDebug.h"


D3DXVECTOR3* TerrainIntersection::pickTerrain(D3DXVECTOR3 rayOrigin, D3DXVECTOR3 rayDir)
{
    
    D3DXVECTOR3* result = NULL;

    result = getCollisionPoint(NULL, rayOrigin, rayDir);

    return result;


    //Other stuff =)

    //Get two vectors that define points in the plane above and below the terrain
    D3DXVECTOR3* clippedRay = getPointsFromPlaneClippedRay(rayOrigin, rayDir);

    if(clippedRay == NULL)
    {
        return NULL;
    }

    /*TCHAR msg[1000];
    _stprintf_s(msg, _T("Clipped:  %d : %d  ->  %d : %d \n"), (int)clippedRay[0].x, (int)clippedRay[0].y, (int)clippedRay[1].x, (int)clippedRay[1].y);
    ::OutputDebugString(msg);*/
    UI3DDebug::addLine(clippedRay[0].x, clippedRay[0].y, clippedRay[0].z, clippedRay[1].x, clippedRay[1].y, clippedRay[1].z, 0.3f, 5.0f);
    
    float factor = 1.0f/UITerrain::PATCHSIZE;
    int x0 = (int)(clippedRay[0].x * factor);
    int y0 = (int)(clippedRay[0].y * factor);
    int x1 = (int)(clippedRay[1].x * factor);
    int y1 = (int)(clippedRay[1].y * factor);

    delete [] clippedRay;

    int maxSize = UITerrain::getInstance()->m_Patches - 1;
    DoubleLinkedList<INDICES*>* patches = getPatchesBetween(x0, y0, x1, y1, maxSize);
    
    if(patches == NULL)
    {
        //Delete the clipped ray
        delete [] clippedRay;
        return NULL;
    }

    //Cull based on AABB
    patches = getAABBCulledPatches(patches, rayOrigin, rayDir);

    if(patches->empty())
    {
        //All culled
        return NULL;
    }
}

D3DXVECTOR3* TerrainIntersection::getPointsFromPlaneClippedRay(D3DXVECTOR3 rayOrigin, D3DXVECTOR3 rayDir)
{
    //Clip the ray between upper and lower xy-planes, for both planes the
    //normal will be -1
    const float UPPERPLANEZ = -255 * UITerrain::HEIGHTFACTOR;
    const float LOWERPLANEZ = 0.0f;

    //Plane-normal always points towards negative z => 0, 0, -1    
    //Denominator for equation (direction dot normal) becomes 
    //dir.x * 0 + dir.y * 0 + dir.z * -1
    float denom = -rayDir.z;

    //If the direction is parallel or points away, there can be no intersection
    if(denom >= 0.0f)
    {
        return NULL;
    }

    D3DXVECTOR3* result = new D3DXVECTOR3[2];    

    float t = (rayOrigin.z - UPPERPLANEZ) / denom;
    
    result[0].z = UPPERPLANEZ;
    result[0].x = rayOrigin.x + rayDir.x * t;
    result[0].y = rayOrigin.y + rayDir.y * t;

    //Lower plane
    t = (rayOrigin.z - LOWERPLANEZ) / denom;
    result[1].z = LOWERPLANEZ;
    result[1].x = rayOrigin.x + rayDir.x * t;
    result[1].y = rayOrigin.y + rayDir.y * t;
    
    return result;
}

DoubleLinkedList<TerrainIntersection::INDICES*>* TerrainIntersection::getPatchesBetween(int x0, int y0, int x1, int y1, int maxSize)
{

    DoubleLinkedList<INDICES*>* result = new DoubleLinkedList<INDICES*>();

    //Modified from premade Bresenham
    bool steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep) 
    {
        int temp = x0;
        x0 = y0;
        y0 = temp;
        
        temp = x1;
        x1 = y1;
        y1 = temp;
    }
    if (x0 > x1) 
    {
        int temp = x0;
        x0 = x1;
        x1 = temp;

        temp = y0;
        y0 = y1;
        y1 = temp;
    }

    int deltax = x1 - x0;
    int deltay = abs(y1 - y0);
    int error = 0;
    int ystep;
    int y = y0;
    if (y0 < y1)
    {
        ystep = 1;
    }
    else
    {
        ystep = -1;
    }
    for (int x = x0; x <= x1; x++) 
    {
        //Need to continue the loop, the start of the ray might be outside map
        if(x < 0 || y < 0 || x > (maxSize) || y > (maxSize))
        {
            continue;
        }

        if (steep)
        {
            result->pushTail(new INDICES(y, x));
        }
        else
        {
            result->pushTail(new INDICES(x, y));
        }
        error += deltay;
        
        if (2 * error >= deltax) 
        {
            y += ystep;
            error -= deltax;
        }
    }

    return result;
}

DoubleLinkedList<TerrainIntersection::INDICES*>* TerrainIntersection::getAABBCulledPatches(DoubleLinkedList<INDICES*>* patches, D3DXVECTOR3 rayOrigin, D3DXVECTOR3 rayDir)
{
    UITerrain* pTerrain = UITerrain::getInstance();
    ListNode<INDICES*>* node = patches->headNode();

    DoubleLinkedList<INDICES*>* result = new DoubleLinkedList<INDICES*>;

    while(node)
    {        

        D3DXVECTOR3 AABB1 = pTerrain->m_pppPatchAABBs[node->item->y][node->item->x][0];
        D3DXVECTOR3 AABB2 = pTerrain->m_pppPatchAABBs[node->item->y][node->item->x][1];
        
        //Check against box
        if(D3DXBoxBoundProbe(&AABB1, &AABB2, &rayOrigin, &rayDir))
        {
            result->pushTail(node->item);
        }
        else
        {
            /*TCHAR msg[1000];
            _stprintf_s(msg, _T("Removing:  X: %d  Y: %d\n"), node->item->x, node->item->y);
            ::OutputDebugString(msg);  */ 
            delete node->item;
        }
        node = node->next;
    }
    
    delete patches;

    return result;
}

D3DXVECTOR3* TerrainIntersection::getCollisionPoint(DoubleLinkedList<INDICES*>* patches, D3DXVECTOR3 rayOrigin, D3DXVECTOR3 rayDir)
{

    D3DXVECTOR3* result = NULL;
    const unsigned char* const* ppVData = Terrain::getInstance()->getTerrainVertexHeightData();

    //Get the clipped ray, shouldn't return NULL anymore at this point (or something went very badly wrong before)
    D3DXVECTOR3* clippedRay = getPointsFromPlaneClippedRay(rayOrigin, rayDir);

    DoubleLinkedList<INDICES*>* squares;

    if(clippedRay)
    {
        //Get square indices from clipped ray
        squares = getPatchesBetween((int)clippedRay[0].x, (int)clippedRay[0].y, (int)clippedRay[1].x, (int)clippedRay[1].y,
                    Terrain::getInstance()->getSize() - 1);
    }
    else
    {
        return NULL;
    }
    
    /*
    Vertices in quad:
       V3 -- V4
       | \   |
       |   \ |
       V1 -- V2
    */
    
    D3DXVECTOR3 V1;
    D3DXVECTOR3 V2;
    D3DXVECTOR3 V3;
    D3DXVECTOR3 V4;

    float dist = 0.0f;
    float smallestDist = 1000000.0f;;
    //TODO: Listan siivous patches-datan pohjalta, syvyysjärjestys?

    //Go through the triangles
    while(!squares->empty())
    {
        INDICES* sq = squares->popHead();

        V1.x = sq->x;
        V1.y = sq->y;
        V1.z = -ppVData[sq->y][sq->x] * UITerrain::HEIGHTFACTOR;
        
        V2.x = sq->x + 1;
        V2.y = sq->y;
        V2.z = -ppVData[sq->y][sq->x + 1] * UITerrain::HEIGHTFACTOR;
        
        V3.x = sq->x;
        V3.y = sq->y + 1;
        V3.z = -ppVData[sq->y + 1][sq->x] * UITerrain::HEIGHTFACTOR;
        
        V4.x = sq->x + 1;
        V4.y = sq->y + 1;
        V4.z = -ppVData[sq->y + 1][sq->x + 1] * UITerrain::HEIGHTFACTOR;
        
        //TODO: Pick the one with lowest distance, build VECTOR and return
        if(D3DXIntersectTri(&V1, &V3, &V2, &rayOrigin, &rayDir, NULL, NULL, &dist))
        {
            if(dist < smallestDist)
            {
                UI3DDebug::addSphere(V1.x, V1.y, V1.z, 0.5f, 10.0f);
                smallestDist = dist;
                result = &V1;
            }
        }
        else
        {
            if(D3DXIntersectTri(&V2, &V3, &V4, &rayOrigin, &rayDir, NULL, NULL, &dist))
            {
                if(dist < smallestDist)
                {
                    UI3DDebug::addSphere(V1.x, V1.y, V1.z, 0.5f, 10.0f);
                    smallestDist = dist;
                    result = &V1;
                }
            }
        }
        
        delete sq;
    }
    
    delete squares;

    return result;
}