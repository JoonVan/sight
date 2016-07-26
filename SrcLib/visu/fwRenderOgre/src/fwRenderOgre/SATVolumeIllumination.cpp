#include "fwRenderOgre/SATVolumeIllumination.hpp"

#include <OGRE/OgreCompositor.h>
#include <OGRE/OgreCompositorChain.h>
#include <OGRE/OgreCompositorInstance.h>
#include <OGRE/OgreCompositorManager.h>
#include <OGRE/OgreHardwarePixelBuffer.h>
#include <OGRE/OgreMaterial.h>
#include <OGRE/OgreMaterialManager.h>
#include <OGRE/OgreTechnique.h>
#include <OGRE/OgreTextureManager.h>
#include <OGRE/OgreRenderTarget.h>
#include <OGRE/OgreRenderTexture.h>
#include <OGRE/OgreViewport.h>

//-----------------------------------------------------------------------------

namespace fwRenderOgre
{

//-----------------------------------------------------------------------------

class VolIllumCompositorListener : public ::Ogre::CompositorInstance::Listener
{
public:

    VolIllumCompositorListener(int& currentSliceIndex, int nbShells, int shellRadius) :
        m_currentSliceIndex(currentSliceIndex),
        m_nbShells(nbShells),
        m_shellRadius(shellRadius)
    {
    }

    virtual void notifyMaterialRender(::Ogre::uint32 pass_id, ::Ogre::MaterialPtr& mat)
    {
        ::Ogre::Pass *pass                                   = mat->getTechnique(0)->getPass(0);
        ::Ogre::GpuProgramParametersSharedPtr volIllumParams = pass->getFragmentProgramParameters();

        volIllumParams->setNamedConstant("u_sliceIndex", m_currentSliceIndex);
    }

    virtual void notifyMaterialSetup(::Ogre::uint32 pass_id, ::Ogre::MaterialPtr& mat)
    {
        ::Ogre::Pass *pass                                   = mat->getTechnique(0)->getPass(0);
        ::Ogre::GpuProgramParametersSharedPtr volIllumParams = pass->getFragmentProgramParameters();

        volIllumParams->setNamedConstant("u_nbShells", m_nbShells);
        volIllumParams->setNamedConstant("u_shellRadius", m_shellRadius);
    }

private:

    int& m_currentSliceIndex;

    int m_nbShells;
    int m_shellRadius;
};

//-----------------------------------------------------------------------------

SATVolumeIllumination::SATVolumeIllumination(std::string parentId, ::Ogre::SceneManager *sceneManager,
                                             float satSizeRatio, int nbShells, int shellRadius) :
    m_sat(parentId, sceneManager, satSizeRatio),
    m_nbShells(nbShells),
    m_shellRadius(shellRadius)
{

}

//-----------------------------------------------------------------------------

SATVolumeIllumination::~SATVolumeIllumination()
{

}

//-----------------------------------------------------------------------------

void SATVolumeIllumination::updateSAT(float _satSizeRatio)
{
    m_sat.updateSatFromRatio(_satSizeRatio);
}

//-----------------------------------------------------------------------------

void SATVolumeIllumination::updateVolIllum(Ogre::TexturePtr _img, Ogre::TexturePtr _tf)
{
    // Update SAT.
    m_sat.computeParallel(_img, _tf);

    m_illuminationVolume = m_sat.getSpareTexture();

    const int depth = m_illuminationVolume->getDepth();

    ::Ogre::CompositorManager& compositorManager = ::Ogre::CompositorManager::getSingleton();

    VolIllumCompositorListener volIllumListener(m_currentSliceIndex, m_nbShells, m_shellRadius);

    ::Ogre::MaterialPtr volIllumMtl       = ::Ogre::MaterialManager::getSingleton().getByName("VolumeAO");
    ::Ogre::Pass *pass                    = volIllumMtl->getTechnique(0)->getPass(0);
    ::Ogre::TextureUnitState *satImgState = pass->getTextureUnitState("sat");

    satImgState->setTexture(m_sat.getTexture());

    // Update illumination volume.
    for(m_currentSliceIndex = 0; m_currentSliceIndex < depth; ++m_currentSliceIndex)
    {
        ::Ogre::RenderTarget *rt = m_illuminationVolume->getBuffer()->getRenderTarget(m_currentSliceIndex);
        ::Ogre::Viewport *vp     = rt->getViewport(0);

        // Add compositor.
        compositorManager.addCompositor(vp, "VolIllum");
        compositorManager.setCompositorEnabled(vp, "VolIllum", true);

        ::Ogre::CompositorInstance *compInstance = compositorManager.getCompositorChain(vp)->getCompositor("VolIllum");
        compInstance->addListener(&volIllumListener);

        // Compute volillum.
        rt->update(false);

        // Remove compositor.
        compInstance->removeListener(&volIllumListener);
        compositorManager.setCompositorEnabled(vp, "VolIllum", false);
        compositorManager.removeCompositor(vp, "VolIllum");
    }
}

//-----------------------------------------------------------------------------

}
