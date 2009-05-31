#ifndef __BUILDQUEUE_H__
#define __BUILDQUEUE_H__

#include "../../Model/Common/DoubleLinkedList.h"
#include "../../Model/Asset/IAsset.h"
#include "../../Model/Asset/IAssetListener.h"
#include "../../Model/Command/BuildTask.h"

#include "../UIComponent/UIContainer.h"
#include "../UIComponent/BasicButton.h"
#include "../UIComponent/IButtonListener.h"

class BuildQueue : public IAssetListener, public IButtonListener
{
public:

    BuildQueue(IAsset* pAsset);

    ~BuildQueue();

    IAsset* getAsset() const
    {
        return m_pAsset;
    }

    int getCount() const
    {
        return m_Queue.count();
    }

    /**
     * Get queue count by type
     */
    int getCount(int assetTag);

    void update(bool updateVisual);

    BasicButton* getButton(const int buttonId);

    void fillPanel(UIContainer* pPanel);

// ===== Listener methods

    virtual void handleAssetStateChange(IAsset* pAsset, IAsset::State newState);

    virtual void handleAssetReleased(IAsset* pAsset);

// ===== IButtonListener methods

    virtual void onButtonClick(BasicButton* pSrc);

    virtual void onButtonAltClick(BasicButton* pSrc);

private:

    bool add(AssetDef* pAssetDef);

    bool cancel(AssetDef* pAssetDef);

    void createBuildingButton(AssetDef* pAssetDef);

    unsigned short                  m_PosX;
    unsigned short                  m_PosY;

    IAsset*                         m_pAsset;

    IAsset*                         m_pCurrentBuild;
    DoubleLinkedList<AssetDef*>     m_Queue;
    DoubleLinkedList<BasicButton*>  m_Buttons;

    int                             m_CacheTmp;

};

#endif // __BUILDQUEUE_H__
