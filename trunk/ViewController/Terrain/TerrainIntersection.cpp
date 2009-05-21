#include "TerrainIntersection.h"
#include "UITerrain.h"
#include "../../Model/Terrain/Terrain.h"

#include <tchar.h>

#include "../3DDebug/UI3DDebug.h"


D3DXVECTOR3* TerrainIntersection::pickTerrain(D3DXVECTOR3 rayOrigin, D3DXVECTOR3 rayDir)
{
    //If the ray is traveling upwards (= positive z) or along the plane, no collision check occurs
    if(rayDir.z <= 0.0f)
    {
        return NULL;
    }

    D3DXVECTOR3* result = NULL;

    result = getCollisionPoint(NULL, rayOrigin, rayDir);

    return result;


    //Other stuff, not used atm, probably not really even needed =)

    //Get two vectors that define points in the plane above and below the terrain
    /*D3DXVECTOR3* clippedRay = getPointsFromPlaneClippedRay(rayOrigin, rayDir);

    if(clippedRay == NULL)
    {
        return NULL;
    }

    UI3DDebug::addLine(clippedRay[0].x, clippedRay[0].y, clippedRay[0].z, clippedRay[1].x, clippedRay[1].y, clippedRay[1].z, 0.3f, 5.0f);
    
    float factor = 1.0f/UITerrain::PATCHSIZE;
    int x0 = (int)(clippedRay[0].x * factor);
    int y0 = (int)(clippedRay[0].y * factor);
    int x1 = (int)(clippedRay[1].x * factor);
    int y1 = (int)(clippedRay[1].y * factor);

    delete [] clippedRay;

    int maxSize = UITerrain::getInstance()->m_Patches - 1;
    DoubleLinkedList<INDICES*>* patches = getSquaresBetween(x0, y0, x1, y1, maxSize);
    
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
    }*/

    //Go through remaining patches and find intersection triangle
}

D3DXVECTOR3* TerrainIntersection::getPointsFromPlaneClippedRay(D3DXVECTOR3 rayOrigin, D3DXVECTOR3 rayDir)
{
    //Z-values of the upper and lower xy-planes
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

DoubleLinkedList<TerrainIntersection::INDICES*>* TerrainIntersection::getSquaresBetween(int x0, int y0, int x1, int y1, int maxSize)
{
/*
Based on http://www.cs.yorku.ca/~amana/research/grid.pdf
*/

//Sign-macro
#define sgn(a) ((a > 0) ? 1 : (a < 0) ? -1 : 0)

    DoubleLinkedList<INDICES*>* result = new DoubleLinkedList<INDICES*>();
    
    //Initialization: stepping directions and startpoint
    int xDiff = x1 - x0;
    int yDiff = y1 - y0;
    int stepX = sgn(xDiff);
    int stepY = sgn(yDiff);
    int x = x0;
    int y = y0;
    //Delta-values: how much the x and y have to be incremented to hit next grid square
    //DeltaX = dir.x, deltaY = dir.y        
    D3DXVECTOR2 delta((float)(x1 - x0), (float)(y1 - y0));
    //D3DXVec2Normalize(&delta, &delta);
    if(delta.x < 0.0f)
    {
        delta.x = -delta.x;
    }
    if(delta.y < 0.0f)
    {
        delta.y = -delta.y;
    }

    //delta * distance = 1  -> distance = 1 / delta
    delta.x = 1.0f / delta.x;
    delta.y = 1.0f / delta.y;

    //tMax-values,used to decide whether to increment X or Y
    //In the original paper, the initial t-values are the length of delta-vector from start position to next intersection with axial edge
    //of the square on each axis, but in this case we give the starting positions as integers any way
    float tMaxX = delta.x;
    float tMaxY = delta.y;

    //Push first square (if within map) to list, because the first step will already move away from it
    if(x + stepX >= 0 && x + stepX < maxSize && y + stepY >= 0 && y + stepY < maxSize)
    {
        result->pushTail(new INDICES(x, y));
        result->pushTail(new INDICES(x + stepX, y));
        result->pushTail(new INDICES(x, y + stepY));
    }

    //Repeat until target square is reached
    while(!(x == x1 && y == y1) )
    {
        if(tMaxX < tMaxY)
        {            
            tMaxX += delta.x;
            x += stepX;
        }
        else
        {
            tMaxY += delta.y;
            y += stepY;
        }
        if(x >= 0 && x < maxSize && y >= 0 && y < maxSize)
        {
            result->pushTail(new INDICES(x, y));
            
            //Push also the other, in some (somewhat rare) cases,
            //the correct square would not end up in the list (probably should fix the initial t-values, eh? =P)
            if(tMaxX < tMaxY)
            {
                result->pushTail(new INDICES(x, y + stepY));
            }
            else
            {
                result->pushTail(new INDICES(x + stepX, y));                
            }
        }
    }

    return result;
}

D3DXVECTOR3* TerrainIntersection::getCollisionPoint(DoubleLinkedList<INDICES*>* patches, D3DXVECTOR3 rayOrigin, D3DXVECTOR3 rayDir)
{

    const unsigned char* const* ppVData = Terrain::getInstance()->getTerrainVertexHeightData();

    //Get the clipped ray
    D3DXVECTOR3* clippedRay = getPointsFromPlaneClippedRay(rayOrigin, rayDir);

    DoubleLinkedList<INDICES*>* squares = NULL;

    int maxSize = Terrain::getInstance()->getSize() - 1;

    if(clippedRay)
    {
        //Get square indices from clipped ray
        squares = getSquaresBetween((int)clippedRay[0].x, (int)clippedRay[0].y, (int)clippedRay[1].x, (int)clippedRay[1].y, maxSize);
        delete [] clippedRay;
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

    //Transfer ray-z's to Model-side heights (so I could drop the HEIGHTFACTOR-multiplications from ppVData)
    rayOrigin.z /= UITerrain::HEIGHTFACTOR;
    rayDir.z /= UITerrain::HEIGHTFACTOR;

    //Store the result here
    D3DXVECTOR3* result = new D3DXVECTOR3;

    //Used to check at the end if any point was found
    result->z = -1000000.0f;

    //Go through the squares
    while(!squares->empty())
    {
        INDICES* sq = squares->popHead();

        if(sq->x >= 0 && sq->y >= 0 && sq->y < maxSize && sq->y < maxSize)
        {
            V1.x = (float)sq->x;
            V1.y = (float)sq->y;
            V1.z = (float)-ppVData[sq->y][sq->x];
            
            V2.x = (float)sq->x + 1;
            V2.y = (float)sq->y;
            V2.z = (float)-ppVData[sq->y][sq->x + 1];
            
            V3.x = (float)sq->x;
            V3.y = (float)sq->y + 1;
            V3.z = (float)-ppVData[sq->y + 1][sq->x];
            
            V4.x = (float)sq->x + 1;
            V4.y = (float)sq->y + 1;
            V4.z = (float)-ppVData[sq->y + 1][sq->x + 1];
            
            //Uncomment to see which squares were searched
            //UI3DDebug::addSphere(V1.x, V1.y, V1.z * UITerrain::HEIGHTFACTOR, 0.3f, 10.0f);
                    
            //Search both triangles within square, no distance needed
            if(D3DXIntersectTri(&V1, &V3, &V2, &rayOrigin, &rayDir, NULL, NULL, NULL) ||
                D3DXIntersectTri(&V2, &V3, &V4, &rayOrigin, &rayDir, NULL, NULL, NULL))
            {
                result->x = V1.x;
                result->y = V1.y;
                result->z = V1.z;
                
                //The squares go away from camera, no need to check for farther hits, just clean up        
                delete sq;            
                break;
            }
        }

        delete sq;
    }

    //If a hit was found, the squares-list might not be empty    
    ListNode<INDICES*>* node = squares->headNode();
    while(node)
    {
        delete node->item;
        node = node->next;
    }
    delete squares;

    //Check if an intersection was found
    if(result->z > -1000000.0f)
    {
        result->z *= UITerrain::HEIGHTFACTOR;
        //UI3DDebug::addSphere(result->x, result->y, result->z, 1.0f, 10.0f);
        return result;
    }

    //No hit
    delete result;
    return NULL;
}