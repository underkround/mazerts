#ifndef __XFILELOADER_H__
#define __XFILELOADER_H__


#include <tchar.h>
#include "C3DResourceContainer.h"
#include "C3DObject.h"


class CXFileLoader
{
public:
    const static float SCALEFACTOR;

    CXFileLoader(void);
    virtual ~CXFileLoader(void);

    /**
     * Load
     * load a 3d object from a x-file
     * function loads mesh, materials and textures
     * and adds them to given resource container
     * @param strFilename x-file to load
     * @param container reference to resource container
     * @param    pObject pointer to object to initialise with
     *            x-file resources, or NULL if not required
     * @return S_OK or an error code
     */
    static HRESULT Load(    LPCTSTR strFilename,
                            C3DResourceContainer& container,
                            C3DObject* pObject);

    /**
     * Scales the given mesh by given amount
     * @param pMesh Pointer to mesh
     * @param scale Scale-factor
     */
    static HRESULT scaleMesh(ID3DXMesh* pMesh, const float scale);
};

#endif