/**
 * Wraps button for build operation, and the operation (BuildTask) itself.
 *
 * $Revision$
 * $Date$
 * $Id$
 */
#ifndef __BUILDBUTTONWRAPPER_H__
#define __BUILDBUTTONWRAPPER_H__

// @TODO: should the button be registered as listener, or the uiassetcontroller?

class BuildButtonWrapper
{
public:

    /**
     * @param buttonPanel       panel into which attach the button
     * @param assetDef          def for what to build
     */
    BuildButtonWrapper(Player* pPlayer, UIContainer* buttonPanel, AssetDef* pAssetDef);

    ~BuildButtonWrapper();

    BasicButton* getButton()
    {
        return m_pButton;
    }

private:

    bool            m_CanBuild;
    UIContainer*    m_pPanel;
    AssetDef*       m_pAssetDef;
    BasicButton*    m_pButton;
    BuildTask*      m_pTask;

};

#endif // __BUILDBUTTONWRAPPER_H__
