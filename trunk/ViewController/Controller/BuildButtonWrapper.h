/**
 * Wraps button for build operation, and the operation (BuildTask) itself.
 *
 * $Revision$
 * $Date$
 * $Id$
 */
#ifndef __BUILDBUTTONWRAPPER_H__
#define __BUILDBUTTONWRAPPER_H__

#include "../UIComponent/BasicButton.h"
#include "../UIComponent/IButtonListener.h"
#include "../../Model/Command/BuildTask.h"
#include "../../Model/Player/Player.h"

#define BUILDICON_DEFAULT_SIZE 64

// @TODO: should the button be registered as listener, or the uiassetcontroller?

class BuildButtonWrapper : public IButtonListener
{
public:

    /**
     * @param buttonPanel       panel into which attach the button
     * @param assetDef          def for what to build
     */
    BuildButtonWrapper(Player* pPlayer, UIContainer* buttonPanel, AssetDef* pAssetDef);

    ~BuildButtonWrapper();

    void setBuildTask(BuildTask* pTask);
    BuildTask* getBuildTask();
    bool hasBuildTask() const;

    BasicButton* getButton() {  return m_pButton; }

    AssetDef* getDef() {        return m_pAssetDef; }
    int getAssetTag() const {   return m_pAssetDef->tag; }

    void updateCanBuild();

    void update();

// ===== IButtonListener methods

    virtual void onButtonClick(BasicButton* pSrc);

    virtual void onButtonAltClick(BasicButton* pSrc);

private:

    bool            m_CanBuild;
    Player*         m_pPlayer;
    UIContainer*    m_pPanel;
    AssetDef*       m_pAssetDef;
    BasicButton*    m_pButton;
    BuildTask*      m_pTask;

};

#endif // __BUILDBUTTONWRAPPER_H__
