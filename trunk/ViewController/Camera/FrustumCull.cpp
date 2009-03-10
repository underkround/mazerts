#include "FrustumCull.h"

FrustumCull::TFrustumPlane* FrustumCull::frustumPlanes = new FrustumCull::TFrustumPlane[6];

void FrustumCull::rebuildFrustum(D3DXMATRIX* pMatView, D3DXMATRIX* pMatProj)
{
    // Get combined matrix
    D3DXMATRIX matComb;
    D3DXMatrixMultiply(&matComb, pMatView, pMatProj);

    // Left clipping plane
    frustumPlanes[0].m_Normal.x = matComb._14 + matComb._11;
    frustumPlanes[0].m_Normal.y = matComb._24 + matComb._21;
    frustumPlanes[0].m_Normal.z = matComb._34 + matComb._31;
    frustumPlanes[0].m_Distance = matComb._44 + matComb._41;

    // Right clipping plane
    frustumPlanes[1].m_Normal.x = matComb._14 - matComb._11;
    frustumPlanes[1].m_Normal.y = matComb._24 - matComb._21;
    frustumPlanes[1].m_Normal.z = matComb._34 - matComb._31;
    frustumPlanes[1].m_Distance = matComb._44 - matComb._41;

    // Top clipping plane
    frustumPlanes[2].m_Normal.x = matComb._14 - matComb._12;
    frustumPlanes[2].m_Normal.y = matComb._24 - matComb._22;
    frustumPlanes[2].m_Normal.z = matComb._34 - matComb._32;
    frustumPlanes[2].m_Distance = matComb._44 - matComb._42;

    // Bottom clipping plane
    frustumPlanes[3].m_Normal.x = matComb._14 + matComb._12;
    frustumPlanes[3].m_Normal.y = matComb._24 + matComb._22;
    frustumPlanes[3].m_Normal.z = matComb._34 + matComb._32;
    frustumPlanes[3].m_Distance = matComb._44 + matComb._42;

    // Near clipping plane
    frustumPlanes[4].m_Normal.x = matComb._13;
    frustumPlanes[4].m_Normal.y = matComb._23;
    frustumPlanes[4].m_Normal.z = matComb._33;
    frustumPlanes[4].m_Distance = matComb._43;

    // Far clipping plane
    frustumPlanes[5].m_Normal.x = matComb._14 - matComb._13;
    frustumPlanes[5].m_Normal.y = matComb._24 - matComb._23;
    frustumPlanes[5].m_Normal.z = matComb._34 - matComb._33;
    frustumPlanes[5].m_Distance = matComb._44 - matComb._43; 

    for(int i = 0; i < 6; i++)
    {
        frustumPlanes[i].normalise();
    }

}


bool FrustumCull::cullAABB(const D3DXVECTOR3& AABBMin, const D3DXVECTOR3& AABBMax)
{
 
      //bool intersect = FALSE;
      //WORD result=0;
      D3DXVECTOR3 minExtreme, maxExtreme;

      for (WORD i=0;i<6;i++)
      {
          if (frustumPlanes[i].m_Normal.x <= 0)
          {
             minExtreme.x = AABBMin.x;
             maxExtreme.x = AABBMax.x;
          }
          else
          {
             minExtreme.x = AABBMax.x;
             maxExtreme.x = AABBMin.x;
          }

          if (frustumPlanes[i].m_Normal.y <= 0)
          {
             minExtreme.y = AABBMin.y;
             maxExtreme.y = AABBMax.y;
          }
          else
          {
             minExtreme.y = AABBMax.y;
             maxExtreme.y = AABBMin.y;
          }

          if (frustumPlanes[i].m_Normal.z <= 0)
          {
             minExtreme.z = AABBMin.z;
             maxExtreme.z = AABBMax.z;
          }
          else
          {
             minExtreme.z = AABBMax.z;
             maxExtreme.z = AABBMin.z;
          }
         
          if (frustumPlanes[i].distanceToPoint(minExtreme) < 0)
          {
              return false;
          }

          /*if (frustumPlanes[i].distanceToPoint(maxExtreme) >= 0)
               intersect = TRUE;*/
      }

      /*if (intersect)
           result = 1; //Intersecting
      else
           result = 2; //All in*/

      return true;

}