/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderOgre/vr/ImportanceDrivenVolumeRenderer.hpp"

#include "fwRenderOgre/Layer.hpp"

#include <OGRE/OgreCompositionPass.h>
#include <OGRE/OgreCompositionTargetPass.h>
#include <OGRE/OgreCompositor.h>
#include <OGRE/OgreCompositorChain.h>
#include <OGRE/OgreCompositorManager.h>
#include <OGRE/OgreException.h>
#include <OGRE/OgreGpuProgramManager.h>
#include <OGRE/OgreHighLevelGpuProgram.h>
#include <OGRE/OgreHighLevelGpuProgramManager.h>
#include <OGRE/OgreMaterial.h>
#include <OGRE/OgreTexture.h>
#include <OGRE/OgreTextureManager.h>
#include <OGRE/OgreViewport.h>

#include <string>

namespace fwRenderOgre
{
namespace vr
{

class ImportanceDrivenVolumeRenderer::JFACompositorListener : public ::Ogre::CompositorInstance::Listener
{
public:

    JFACompositorListener(float passIndex, float nbPasses, float& blurWeight) :
        m_passIndex(passIndex),
        m_nbPasses(nbPasses),
        m_blurWeight(blurWeight)
    {

    }

    //------------------------------------------------------------------------------

    virtual void notifyMaterialSetup(::Ogre::uint32, ::Ogre::MaterialPtr& mtl)
    {
        if(mtl->getName().find("JFA") != std::string::npos)
        {
            ::Ogre::Technique* tech = mtl->getBestTechnique();

            for(short unsigned int i = 0; i < tech->getNumPasses(); i++ )
            {
                ::Ogre::GpuProgramParametersSharedPtr vrParams = tech->getPass(i)->getFragmentProgramParameters();

                vrParams->setNamedConstant("u_passIndex", m_passIndex);
                vrParams->setNamedConstant("u_nbPasses", m_nbPasses);
            }
        }
    }

    //------------------------------------------------------------------------------

    virtual void notifyMaterialRender(::Ogre::uint32, ::Ogre::MaterialPtr& mtl)
    {
        if(mtl->getName().find("Blur") != std::string::npos)
        {
            ::Ogre::Technique* tech = mtl->getBestTechnique();

            for(short unsigned int i = 0; i < tech->getNumPasses(); i++ )
            {
                ::Ogre::GpuProgramParametersSharedPtr fParams = tech->getPass(i)->getFragmentProgramParameters();
                fParams->setNamedConstant("u_blurWeight", m_blurWeight);
            }
        }
    }

private:

    float m_passIndex;
    float m_nbPasses;

    float& m_blurWeight;
};

//------------------------------------------------------------------------------

const std::string s_NONE  = "None";
const std::string s_MIMP  = "MImP";
const std::string s_AIMC  = "AImC";
const std::string s_VPIMC = "VPImC";

const std::string s_MIMP_COMPOSITOR         = "IDVR_MImP_Comp";
const std::string s_AIMC_COMPOSITOR         = "IDVR_AImC_Comp";
const std::string s_VPIMC_COMPOSITOR        = "IDVR_VPImC_Comp";
const std::string s_JFAINIT_COMPOSITOR      = "JFAInit";
const std::string s_JFAPING_COMPOSITOR      = "JFAPingComp";
const std::string s_JFAPONG_COMPOSITOR      = "JFAPongComp";
const std::string s_JFAFINALPING_COMPOSITOR = "JFAFinalPingComp";
const std::string s_JFAFINALPONG_COMPOSITOR = "JFAFinalPongComp";

const std::string s_MIMP_DEFINE  = "IDVR=1";
const std::string s_AIMC_DEFINE  = "IDVR=2";
const std::string s_VPIMC_DEFINE = "IDVR=3";

const std::string s_CSG_DEFINE                  = "CSG=1";
const std::string s_CSG_BORDER_DEFINE           = "CSG_BORDER=1";
const std::string s_CSG_DISABLE_CONTEXT_DEFINE  = "CSG_DISABLE_CONTEXT=1";
const std::string s_CSG_OPACITY_DECREASE_DEFINE = "CSG_OPACITY_DECREASE=1";
const std::string s_CSG_DEPTH_LINES_DEFINE      = "CSG_DEPTH_LINES=1";

const std::string s_CSG_MOD_GRAYSCALE_AVERAGE_DEFINE    = "CSG_MODULATION=1";
const std::string s_CSG_MOD_GRAYSCALE_LIGHTNESS_DEFINE  = "CSG_MODULATION=2";
const std::string s_CSG_MOD_GRAYSCALE_LUMINOSITY_DEFINE = "CSG_MODULATION=3";
const std::string s_CSG_MOD_COLOR1_DEFINE               = "CSG_MODULATION=4";
const std::string s_CSG_MOD_COLOR2_DEFINE               = "CSG_MODULATION=5";
const std::string s_CSG_MOD_COLOR3_DEFINE               = "CSG_MODULATION=6";
const std::string s_CSG_MOD_COLOR4_DEFINE               = "CSG_MODULATION=7";

const std::string s_IMPORTANCE_COMPOSITING_TEXTURE = "IC";
const std::string s_JUMP_FLOOD_ALGORITHM_TEXTURE   = "JFA";

//-----------------------------------------------------------------------------

ImportanceDrivenVolumeRenderer::ImportanceDrivenVolumeRenderer(std::string parentId,
                                                               Layer::sptr layer,
                                                               ::Ogre::SceneNode* parentNode,
                                                               ::Ogre::TexturePtr imageTexture,
                                                               ::Ogre::TexturePtr maskTexture,
                                                               TransferFunction& gpuTF,
                                                               PreIntegrationTable& preintegrationTable,
                                                               bool ambientOcclusion,
                                                               bool colorBleeding,
                                                               bool shadows,
                                                               double aoFactor,
                                                               double colorBleedingFactor) :
    fwRenderOgre::vr::RayTracingVolumeRenderer(parentId, layer, parentNode, imageTexture, gpuTF, preintegrationTable,
                                               ambientOcclusion, colorBleeding, shadows, aoFactor, colorBleedingFactor),
    m_maskTexture(maskTexture),
    m_idvrMethod(s_NONE),
    m_idvrCSG(false),
    m_idvrCSGSlope(0.3f),
    m_idvrCSGBlurWeight(0.01f),
    m_idvrCSGBorder(false),
    m_idvrCSGDisableContext(false),
    m_idvrCSGBorderThickness(0.05f),
    m_idvrCSGBorderColor(::Ogre::ColourValue(1.f, 0.f, 0.f)),
    m_idvrCSGModulation(false),
    m_idvrCSGModulationMethod(IDVRCSGModulationMethod::COLOR1),
    m_idvrCSGModulationFactor(1.f),
    m_idvrCSGOpacityDecrease(false),
    m_idvrCSGOpacityDecreaseFactor(1.f),
    m_idvrCSGDepthLines(false),
    m_idvrCSGDepthLinesThreshold(0.07f),
    m_idvrAImCAlphaCorrection(0.05f),
    m_idvrVPImCAlphaCorrection(0.3f)
{
    m_RTVSharedParameters->addConstantDefinition("u_countersinkSlope", ::Ogre::GCT_FLOAT1);
    m_RTVSharedParameters->addConstantDefinition("u_csgBorderThickness", ::Ogre::GCT_FLOAT1);
    m_RTVSharedParameters->addConstantDefinition("u_colorModulationFactor", ::Ogre::GCT_FLOAT1);
    m_RTVSharedParameters->addConstantDefinition("u_opacityDecreaseFactor", ::Ogre::GCT_FLOAT1);
    m_RTVSharedParameters->addConstantDefinition("u_vpimcAlphaCorrection", ::Ogre::GCT_FLOAT1);
    m_RTVSharedParameters->addConstantDefinition("u_aimcAlphaCorrection", ::Ogre::GCT_FLOAT1);
    m_RTVSharedParameters->addConstantDefinition("u_depthLinesThreshold", ::Ogre::GCT_FLOAT1);
    m_RTVSharedParameters->addConstantDefinition("u_csgBorderColor", ::Ogre::GCT_FLOAT3);
    m_RTVSharedParameters->setNamedConstant("u_countersinkSlope", m_idvrCSGSlope);
    m_RTVSharedParameters->setNamedConstant("u_csgBorderThickness", m_idvrCSGBorderThickness);
    m_RTVSharedParameters->setNamedConstant("u_colorModulationFactor", m_idvrCSGModulationFactor);
    m_RTVSharedParameters->setNamedConstant("u_opacityDecreaseFactor", m_idvrCSGOpacityDecreaseFactor);
    m_RTVSharedParameters->setNamedConstant("u_csgBorderColor", m_idvrCSGBorderColor);
    m_RTVSharedParameters->setNamedConstant("u_aimcAlphaCorrection", m_idvrAImCAlphaCorrection);
    m_RTVSharedParameters->setNamedConstant("u_vpimcAlphaCorrection", m_idvrVPImCAlphaCorrection);
    m_RTVSharedParameters->setNamedConstant("u_depthLinesThreshold", m_idvrCSGDepthLinesThreshold);

    m_fragmentShaderAttachements.push_back("ColorFormats_FP");

    this->createIDVRTechnique();
}

//-----------------------------------------------------------------------------

ImportanceDrivenVolumeRenderer::~ImportanceDrivenVolumeRenderer()
{
    this->cleanCompositorChain();
}

//-----------------------------------------------------------------------------

void ImportanceDrivenVolumeRenderer::setIDVRMethod(std::string method)
{
    bool isSupported(false);

    if(method == s_NONE ||
       method == s_MIMP ||
       method == s_AIMC ||
       method == s_VPIMC)
    {
        isSupported = true;
    }

    SLM_FATAL_IF("IDVR method '" + method + "' isn't supported by the ray tracing volume renderer.", !isSupported);
    m_idvrMethod = method;

    this->createMaterialAndIDVRTechnique();
}

//-----------------------------------------------------------------------------

void ImportanceDrivenVolumeRenderer::initCompositors()
{
    // Start from an empty compositor chain
    this->cleanCompositorChain();

    this->buildICCompositors();
    this->getLayer()->requestRender();
}

//-----------------------------------------------------------------------------

void ImportanceDrivenVolumeRenderer::buildICCompositors()
{
    std::string vpPPDefines, fpPPDefines;
    size_t hash;
    std::tie(vpPPDefines, fpPPDefines, hash) = this->computeRayTracingDefines();

    ::Ogre::CompositorManager& compositorManager = ::Ogre::CompositorManager::getSingleton();
    auto viewport = this->getLayer()->getViewport();

    ::Ogre::CompositorInstance* compositorInstance = nullptr;

    if(m_idvrMethod == s_MIMP)
    {
        compositorInstance = compositorManager.addCompositor(viewport, s_MIMP_COMPOSITOR, 0);
        SLM_ASSERT("Compositor could not be initialized", compositorInstance);
        compositorInstance->setEnabled(true);

        const double numPasses =
            std::nearbyint(std::max(std::log(m_camera->getViewport()->getActualWidth()) / std::log(2.0),
                                    std::log(m_camera->getViewport()->getActualHeight()) / std::log(2.0)));

        int chainIndex = 1;
        compositorInstance = compositorManager.addCompositor(viewport, s_JFAINIT_COMPOSITOR, chainIndex++);
        SLM_ASSERT("Compositor could not be initialized", compositorInstance);
        compositorInstance->setEnabled(true);

        m_compositorListeners.push_back(new JFACompositorListener(static_cast<float>(0),
                                                                  static_cast<float>(numPasses),
                                                                  m_idvrCSGBlurWeight));
        compositorInstance->addListener(m_compositorListeners.back());

        int i = 0;
        for(i = 0; i < numPasses - 2; i++)
        {
            if(i % 2 == 0)
            {
                compositorInstance = compositorManager.addCompositor(viewport, s_JFAPING_COMPOSITOR, chainIndex++);
                SLM_ASSERT("Compositor could not be initialized", compositorInstance);
                compositorInstance->setEnabled(true);
            }
            else
            {
                compositorInstance = compositorManager.addCompositor(viewport, s_JFAPONG_COMPOSITOR, chainIndex++);
                SLM_ASSERT("Compositor could not be initialized", compositorInstance);
                compositorInstance->setEnabled(true);
            }

            m_compositorListeners.push_back(new JFACompositorListener(static_cast<float>(i + 1),
                                                                      static_cast<float>(numPasses),
                                                                      m_idvrCSGBlurWeight));
            compositorInstance->addListener(m_compositorListeners.back());
        }

        if(i % 2 == 0)
        {
            compositorInstance = compositorManager.addCompositor(viewport, s_JFAFINALPING_COMPOSITOR, chainIndex++);
            SLM_ASSERT("Compositor could not be initialized", compositorInstance);
            compositorInstance->setEnabled(true);
        }
        else
        {
            compositorInstance = compositorManager.addCompositor(viewport, s_JFAFINALPONG_COMPOSITOR, chainIndex++);
            SLM_ASSERT("Compositor could not be initialized", compositorInstance);
            compositorInstance->setEnabled(true);
        }

        m_compositorListeners.push_back(new JFACompositorListener(static_cast<float>(i),
                                                                  static_cast<float>(numPasses),
                                                                  m_idvrCSGBlurWeight));
        compositorInstance->addListener(m_compositorListeners.back());
    }
    else if(m_idvrMethod == s_AIMC)
    {
        compositorInstance = compositorManager.addCompositor(viewport, s_AIMC_COMPOSITOR, 0);
        SLM_ASSERT("Compositor could not be initialized", compositorInstance);
        compositorInstance->setEnabled(true);
    }
    else if(m_idvrMethod == s_VPIMC)
    {
        compositorInstance = compositorManager.addCompositor(viewport, s_VPIMC_COMPOSITOR, 0);
        SLM_ASSERT("Compositor could not be initialized", compositorInstance);
        compositorInstance->setEnabled(true);
    }
}

//-----------------------------------------------------------------------------

void ImportanceDrivenVolumeRenderer::cleanCompositorChain()
{
    ::Ogre::CompositorManager& compositorManager = ::Ogre::CompositorManager::getSingleton();
    auto viewport = this->getLayer()->getViewport();

    ::Ogre::CompositorChain* compChain = compositorManager.getCompositorChain(viewport);
    SLM_ASSERT("Can't find compositor chain", compChain);

    // Then clean the whole chain
    const size_t numCompositors = compChain->getNumCompositors();
    std::vector<size_t> removeCompositors;

    for(size_t i = 0; i < numCompositors; ++i)
    {
        ::Ogre::CompositorInstance* targetComp = compChain->getCompositor(i);
        SLM_ASSERT("Compositor instance is null", targetComp);
        if(targetComp->getCompositor()->getName() == s_MIMP_COMPOSITOR ||
           targetComp->getCompositor()->getName() == s_AIMC_COMPOSITOR||
           targetComp->getCompositor()->getName() == s_VPIMC_COMPOSITOR||
           targetComp->getCompositor()->getName() == s_JFAINIT_COMPOSITOR ||
           targetComp->getCompositor()->getName() == s_JFAPING_COMPOSITOR ||
           targetComp->getCompositor()->getName() == s_JFAPONG_COMPOSITOR ||
           targetComp->getCompositor()->getName() == s_JFAFINALPING_COMPOSITOR ||
           targetComp->getCompositor()->getName() == s_JFAFINALPONG_COMPOSITOR
           )
        {
            removeCompositors.push_back(i);
        }
    }

    for(auto it = removeCompositors.rbegin(); it != removeCompositors.rend(); ++it )
    {
        compChain->removeCompositor(static_cast<size_t>(*it));
    }

    // Remove all associated listeners from the compositor chain
    std::for_each(m_compositorListeners.begin(), m_compositorListeners.end(),
                  [](::Ogre::CompositorInstance::Listener* listener) { delete listener; }
                  );
    m_compositorListeners.clear();
}

//-----------------------------------------------------------------------------

void ImportanceDrivenVolumeRenderer::toggleIDVRCountersinkGeometry(bool CSG)
{
    m_idvrCSG = CSG;

    if(this->m_idvrMethod == s_MIMP)
    {
        this->createMaterialAndIDVRTechnique();
    }
}

//-----------------------------------------------------------------------------

void ImportanceDrivenVolumeRenderer::setIDVRCountersinkSlope(double slope)
{
    m_idvrCSGSlope = static_cast<float>(slope);

    if(m_idvrMethod == s_MIMP && m_idvrCSG)
    {
        m_RTVSharedParameters->setNamedConstant("u_countersinkSlope", m_idvrCSGSlope);
        this->getLayer()->requestRender();
    }
}

//-----------------------------------------------------------------------------

void ImportanceDrivenVolumeRenderer::setIDVRCSGBlurWeight(double blurWeight)
{
    m_idvrCSGBlurWeight = static_cast<float>(blurWeight);

    if(m_idvrMethod == s_MIMP && m_idvrCSG)
    {
        this->getLayer()->requestRender();
    }
}

//-----------------------------------------------------------------------------

void ImportanceDrivenVolumeRenderer::toggleIDVRCSGBorder(bool border)
{
    m_idvrCSGBorder = border;

    if(this->m_idvrMethod == s_MIMP && this->m_idvrCSG)
    {
        this->createMaterialAndIDVRTechnique();
    }
}

//-----------------------------------------------------------------------------

void ImportanceDrivenVolumeRenderer::toggleIDVRCSGDisableContext(bool discard)
{
    m_idvrCSGDisableContext = discard;

    if(this->m_idvrMethod == s_MIMP && this->m_idvrCSG)
    {
        this->createMaterialAndIDVRTechnique();
    }
}

//-----------------------------------------------------------------------------

void ImportanceDrivenVolumeRenderer::setIDVRCSGBorderThickness(double thickness)
{
    m_idvrCSGBorderThickness = static_cast<float>(thickness);

    if(m_idvrMethod == s_MIMP && this->m_idvrCSG && this->m_idvrCSGBorder)
    {
        m_RTVSharedParameters->setNamedConstant("u_csgBorderThickness", m_idvrCSGBorderThickness);
        this->getLayer()->requestRender();
    }
}

//-----------------------------------------------------------------------------

void ImportanceDrivenVolumeRenderer::setIDVRCSGBorderColor(std::array<std::uint8_t, 4> color)
{
    m_idvrCSGBorderColor.r = color[0] / 256.f;
    m_idvrCSGBorderColor.g = color[1] / 256.f;
    m_idvrCSGBorderColor.b = color[2] / 256.f;

    if(m_idvrMethod == s_MIMP && this->m_idvrCSG && this->m_idvrCSGBorder)
    {
        m_RTVSharedParameters->setNamedConstant("u_csgBorderColor", m_idvrCSGBorderColor);
        this->getLayer()->requestRender();
    }
}

//-----------------------------------------------------------------------------

void ImportanceDrivenVolumeRenderer::toggleIDVRCSGModulation(bool modulation)
{
    m_idvrCSGModulation = modulation;

    if(this->m_idvrMethod == s_MIMP && this->m_idvrCSG)
    {
        this->createMaterialAndIDVRTechnique();
    }
}

//-----------------------------------------------------------------------------

void ImportanceDrivenVolumeRenderer::setIDVRCSModulationMethod(IDVRCSGModulationMethod method)
{
    m_idvrCSGModulationMethod = method;

    if(this->m_idvrMethod == s_MIMP && this->m_idvrCSG && this->m_idvrCSGModulation)
    {
        this->createMaterialAndIDVRTechnique();
    }
}

//-----------------------------------------------------------------------------

void ImportanceDrivenVolumeRenderer::setIDVRCSGModulationFactor(double modulationFactor)
{
    m_idvrCSGModulationFactor = static_cast<float>(modulationFactor);

    if(m_idvrMethod == s_MIMP && this->m_idvrCSG)
    {
        m_RTVSharedParameters->setNamedConstant("u_colorModulationFactor", m_idvrCSGModulationFactor);
        this->getLayer()->requestRender();
    }
}

//-----------------------------------------------------------------------------

void ImportanceDrivenVolumeRenderer::toggleIDVRCSGOpacityDecrease(bool opacityDecrease)
{
    m_idvrCSGOpacityDecrease = opacityDecrease;

    if(m_idvrMethod == s_MIMP && this->m_idvrCSG)
    {
        this->createMaterialAndIDVRTechnique();
    }
}

//-----------------------------------------------------------------------------

void ImportanceDrivenVolumeRenderer::setIDVRCSGOpacityDecreaseFactor(double opacityDecrease)
{
    m_idvrCSGOpacityDecreaseFactor = static_cast<float>(opacityDecrease);

    if(m_idvrMethod == s_MIMP && this->m_idvrCSG)
    {
        m_RTVSharedParameters->setNamedConstant("u_opacityDecreaseFactor", m_idvrCSGOpacityDecreaseFactor);
        this->getLayer()->requestRender();
    }
}

//-----------------------------------------------------------------------------

void ImportanceDrivenVolumeRenderer::toggleIDVRDepthLines(bool depthLines)
{
    m_idvrCSGDepthLines = depthLines;

    if(m_idvrMethod == s_MIMP && this->m_idvrCSG && this->m_idvrCSGBorder)
    {
        this->createMaterialAndIDVRTechnique();
    }
}

//-----------------------------------------------------------------------------

void ImportanceDrivenVolumeRenderer::setIDVRCSGDepthLinesThreshold(double threshold)
{
    m_idvrCSGDepthLinesThreshold = static_cast<float>(threshold);

    if(m_idvrMethod == s_MIMP && this->m_idvrCSG && this->m_idvrCSGBorder && this->m_idvrCSGDepthLines)
    {
        m_RTVSharedParameters->setNamedConstant("u_depthLinesThreshold", m_idvrCSGDepthLinesThreshold);
        this->getLayer()->requestRender();
    }
}

//-----------------------------------------------------------------------------

void ImportanceDrivenVolumeRenderer::setIDVRAImCAlphaCorrection(double alphaCorrection)
{
    m_idvrAImCAlphaCorrection = static_cast<float>(alphaCorrection);

    if(m_idvrMethod == s_AIMC)
    {
        m_RTVSharedParameters->setNamedConstant("u_aimcAlphaCorrection", m_idvrAImCAlphaCorrection);
        this->getLayer()->requestRender();
    }
}

//-----------------------------------------------------------------------------

void ImportanceDrivenVolumeRenderer::setIDVRVPImCAlphaCorrection(double alphaCorrection)
{
    m_idvrVPImCAlphaCorrection = static_cast<float>(alphaCorrection);

    if(m_idvrMethod == s_VPIMC)
    {
        m_RTVSharedParameters->setNamedConstant("u_vpimcAlphaCorrection", m_idvrVPImCAlphaCorrection);
        this->getLayer()->requestRender();
    }
}

//-----------------------------------------------------------------------------

std::tuple<std::string, std::string, size_t> ImportanceDrivenVolumeRenderer::computeRayTracingDefines() const
{
    std::string vpPPDefines, fpPPDefines;
    size_t hash;

    // Compute regular ray tracing defines.
    std::tie(vpPPDefines, fpPPDefines, hash) = this->RayTracingVolumeRenderer::computeRayTracingDefines();

    std::ostringstream vpPPDefs, fpPPDefs;
    vpPPDefs << vpPPDefines;
    fpPPDefs << fpPPDefines;

    // Add IDVR defines.
    if(m_idvrMethod != s_NONE)
    {
        if(m_idvrMethod == s_MIMP)
        {
            fpPPDefs << (fpPPDefs.str() == "" ? "" : ",") << s_MIMP_DEFINE;
            if(m_idvrCSG)
            {
                fpPPDefs << (fpPPDefs.str() == "" ? "" : ",") << s_CSG_DEFINE;

                if(m_idvrCSGBorder)
                {
                    fpPPDefs << (fpPPDefs.str() == "" ? "" : ",") << s_CSG_BORDER_DEFINE;
                }
                if(m_idvrCSGDisableContext)
                {
                    fpPPDefs << (fpPPDefs.str() == "" ? "" : ",") << s_CSG_DISABLE_CONTEXT_DEFINE;
                }
                if(m_idvrCSGOpacityDecrease)
                {
                    fpPPDefs << (fpPPDefs.str() == "" ? "" : ",") << s_CSG_OPACITY_DECREASE_DEFINE;
                }
                if(m_idvrCSGDepthLines)
                {
                    fpPPDefs << (fpPPDefs.str() == "" ? "" : ",") << s_CSG_DEPTH_LINES_DEFINE;
                }

                if(m_idvrCSGModulation)
                {
                    switch(m_idvrCSGModulationMethod)
                    {
                        case IDVRCSGModulationMethod::AVERAGE_GRAYSCALE:
                            fpPPDefs << (fpPPDefs.str() == "" ? "" : ",") << s_CSG_MOD_GRAYSCALE_AVERAGE_DEFINE;
                            break;
                        case IDVRCSGModulationMethod::LIGHTNESS_GRAYSCALE:
                            fpPPDefs << (fpPPDefs.str() == "" ? "" : ",") << s_CSG_MOD_GRAYSCALE_LIGHTNESS_DEFINE;
                            break;
                        case IDVRCSGModulationMethod::LUMINOSITY_GRAYSCALE:
                            fpPPDefs <<
                            (fpPPDefs.str() == "" ? "" : ",") << s_CSG_MOD_GRAYSCALE_LUMINOSITY_DEFINE;
                            break;
                        case IDVRCSGModulationMethod::COLOR1:
                            fpPPDefs << (fpPPDefs.str() == "" ? "" : ",") << s_CSG_MOD_COLOR1_DEFINE;
                            break;
                        case IDVRCSGModulationMethod::COLOR2:
                            fpPPDefs << (fpPPDefs.str() == "" ? "" : ",") << s_CSG_MOD_COLOR2_DEFINE;
                            break;
                        case IDVRCSGModulationMethod::COLOR3:
                            fpPPDefs << (fpPPDefs.str() == "" ? "" : ",") << s_CSG_MOD_COLOR3_DEFINE;
                            break;
                        case IDVRCSGModulationMethod::COLOR4:
                            fpPPDefs << (fpPPDefs.str() == "" ? "" : ",") << s_CSG_MOD_COLOR4_DEFINE;
                            break;
                    }

                }
            }
        }
        else if(m_idvrMethod == s_AIMC)
        {
            fpPPDefs << (fpPPDefs.str() == "" ? "" : ",") << s_AIMC_DEFINE;
        }
        else if(m_idvrMethod == s_VPIMC)
        {
            fpPPDefs << (fpPPDefs.str() == "" ? "" : ",") << s_VPIMC_DEFINE;
        }
    }

    return std::make_tuple(vpPPDefs.str(), fpPPDefs.str(), std::hash<std::string>{} (vpPPDefs.str() + fpPPDefs.str()));
}

//-----------------------------------------------------------------------------

void ImportanceDrivenVolumeRenderer::setRayCastingPassTextureUnits(Ogre::Pass* _rayCastingPass,
                                                                   const std::string& _fpPPDefines) const
{
    this->RayTracingVolumeRenderer::setRayCastingPassTextureUnits(_rayCastingPass, _fpPPDefines);

    int nbTexUnits = int(_rayCastingPass->getTextureUnitStates().size());

    ::Ogre::GpuProgramParametersSharedPtr fpParams = _rayCastingPass->getFragmentProgramParameters();
    ::Ogre::TextureUnitState* texUnitState;

    // Importance Compositing texture: MImP | AImC | VPImC
    if(m_idvrMethod != s_NONE)
    {
        const std::string compositorRef = m_idvrMethod == s_MIMP ? s_MIMP_COMPOSITOR :
                                          m_idvrMethod == s_AIMC ? s_AIMC_COMPOSITOR : s_VPIMC_COMPOSITOR;
        texUnitState = _rayCastingPass->createTextureUnitState();
        texUnitState->setTextureFiltering(::Ogre::TFO_BILINEAR);
        texUnitState->setTextureAddressingMode(::Ogre::TextureUnitState::TAM_CLAMP);
        texUnitState->setContentType(::Ogre::TextureUnitState::CONTENT_COMPOSITOR);
        texUnitState->setCompositorReference(compositorRef, "IC", 0);

        fpParams->setNamedConstant("u_" + s_IMPORTANCE_COMPOSITING_TEXTURE, nbTexUnits++);
    }

    // JFA texture: MImP
    if(m_idvrMethod == s_MIMP)
    {
        texUnitState = _rayCastingPass->createTextureUnitState();
        texUnitState->setTextureFiltering(::Ogre::TFO_BILINEAR);
        texUnitState->setTextureAddressingMode(::Ogre::TextureUnitState::TAM_CLAMP);
        texUnitState->setContentType(::Ogre::TextureUnitState::CONTENT_COMPOSITOR);
        texUnitState->setCompositorReference(s_JFAINIT_COMPOSITOR, "JFAFinal", 0);

        fpParams->setNamedConstant("u_" + s_JUMP_FLOOD_ALGORITHM_TEXTURE, nbTexUnits++);
    }

    // Alpha Correction: AImC | VPImC
    if(m_idvrMethod == s_AIMC)
    {
        m_RTVSharedParameters->setNamedConstant("u_aimcAlphaCorrection", m_idvrAImCAlphaCorrection);
    }
    else if(m_idvrMethod == s_VPIMC)
    {
        m_RTVSharedParameters->setNamedConstant("u_vpimcAlphaCorrection", m_idvrVPImCAlphaCorrection);
    }
}

//-----------------------------------------------------------------------------

void ImportanceDrivenVolumeRenderer::createIDVRTechnique()
{
    std::string vpPPDefines, fpPPDefines;
    size_t hash;
    std::tie(vpPPDefines, fpPPDefines, hash) = this->computeRayTracingDefines();

    ::Ogre::MaterialManager& mm = ::Ogre::MaterialManager::getSingleton();

    ::Ogre::String vpName("RTV_VP_" + std::to_string(hash));

    ::Ogre::MaterialPtr mat = mm.getByName(m_currentMtlName, ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

    ///////////////////////////////////////////////////////////////////////////
    /// Create the technique that is used in ImportanceCompositing.compositor
    if(m_idvrMethod != s_NONE )
    {
        auto tech = mat->createTechnique();
        SLM_ASSERT("Can't create a new technique", tech);

        auto pass = tech->createPass();
        SLM_ASSERT("Can't create a new pass", pass);

        pass->setCullingMode(::Ogre::CullingMode::CULL_ANTICLOCKWISE);
        pass->setSceneBlending(::Ogre::SceneBlendType::SBT_TRANSPARENT_ALPHA);
        pass->setVertexProgram(vpName);

        ::Ogre::GpuProgramParametersSharedPtr vpParams = pass->getVertexProgramParameters();
        vpParams->setNamedAutoConstant("u_worldViewProj", ::Ogre::GpuProgramParameters::ACT_WORLDVIEWPROJ_MATRIX);

        if(m_idvrMethod == s_MIMP)
        {
            tech->setName("IDVR_MImP_Mat");
            tech->setSchemeName("IDVR_MImP_Mat");
            pass->setFragmentProgram("IDVR_MImP_FP");
        }
        else if(m_idvrMethod == s_AIMC)
        {
            tech->setName("IDVR_AImC_Mat");
            tech->setSchemeName("IDVR_AImC_Mat");
            pass->setFragmentProgram("IDVR_AImC_FP");
        }
        else if(m_idvrMethod == s_VPIMC)
        {
            tech->setName("IDVR_VPImC_Mat");
            tech->setSchemeName("IDVR_VPImC_Mat");
            pass->setFragmentProgram("IDVR_VPImC_FP");
        }

        ::Ogre::TextureUnitState* texUnitState = pass->createTextureUnitState();
        texUnitState->setTextureName(m_maskTexture->getName(), ::Ogre::TEX_TYPE_3D);
        texUnitState->setTextureFiltering(::Ogre::TFO_BILINEAR);
        texUnitState->setTextureAddressingMode(::Ogre::TextureUnitState::TAM_CLAMP);

        texUnitState = pass->createTextureUnitState();
        texUnitState->setName("entryPoints");
        texUnitState->setTexture(m_entryPointsTextures[0]);
        texUnitState->setTextureFiltering(::Ogre::TFO_NONE);
        texUnitState->setTextureAddressingMode(::Ogre::TextureUnitState::TAM_CLAMP);
    }

    this->initCompositors();
}

//-----------------------------------------------------------------------------

void ImportanceDrivenVolumeRenderer::createMaterialAndIDVRTechnique()
{
    this->createRayTracingMaterial();
    this->createIDVRTechnique();
}

//-----------------------------------------------------------------------------

} // namespace vr
} // namespace fwRenderOgre

