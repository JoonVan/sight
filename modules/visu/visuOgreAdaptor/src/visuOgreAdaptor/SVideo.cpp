/************************************************************************
 *
 * Copyright (C) 2014-2020 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "visuOgreAdaptor/SVideo.hpp"

#include <fwCom/Slots.hxx>

#include <fwData/TransferFunction.hpp>

#include <fwRenderOgre/Utils.hpp>

#include <fwServices/macros.hpp>

#include <OGRE/OgreCamera.h>
#include <OGRE/OgreEntity.h>
#include <OGRE/OgreHardwarePixelBuffer.h>
#include <OGRE/OgreMaterial.h>
#include <OGRE/OgreMaterialManager.h>
#include <OGRE/OgreMesh.h>
#include <OGRE/OgreMeshManager.h>
#include <OGRE/OgreMovablePlane.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreSceneNode.h>
#include <OGRE/OgreTechnique.h>
#include <OGRE/OgreTextureManager.h>

namespace visuOgreAdaptor
{

static const ::fwCom::Slots::SlotKeyType s_UPDATE_TF_SLOT = "updateTF";
static const ::fwCom::Slots::SlotKeyType s_UPDATE_PL_SLOT = "updatePL";

static const ::fwServices::IService::KeyType s_IMAGE_INPUT = "image";
static const ::fwServices::IService::KeyType s_TF_INPUT    = "tf";
static const ::fwServices::IService::KeyType s_PL_INPUT    = "pointList";

static const std::string s_VISIBLE_CONFIG           = "visible";
static const std::string s_MATERIAL_TEMPLATE_CONFIG = "materialTemplate";
static const std::string s_TEXTURE_NAME_CONFIG      = "textureName";
static const std::string s_RADIUS_CONFIG            = "radius";
static const std::string s_DISPLAY_LABEL_CONFIG     = "displayLabel";
static const std::string s_LABEL_COLOR_CONFIG       = "labelColor";
static const std::string s_COLOR_CONFIG             = "color";
static const std::string s_FIXED_SIZE_CONFIG        = "fixedSize";
static const std::string s_QUERY_CONFIG             = "queryFlags";
static const std::string s_FONT_SOURCE_CONFIG       = "fontSource";
static const std::string s_FONT_SIZE_CONFIG         = "fontSize";

static const std::string s_VIDEO_MATERIAL_NAME            = "Video";
static const std::string s_VIDEO_WITHTF_MATERIAL_NAME     = "VideoWithTF";
static const std::string s_VIDEO_WITHTF_INT_MATERIAL_NAME = "VideoWithTF_Int";

//------------------------------------------------------------------------------

SVideo::SVideo() noexcept
{
    newSlot(s_UPDATE_TF_SLOT, &SVideo::updateTF, this);
    newSlot(s_UPDATE_PL_SLOT, &SVideo::updatePL, this);
}

//------------------------------------------------------------------------------

SVideo::~SVideo() noexcept
{
}

//------------------------------------------------------------------------------

void SVideo::configuring()
{
    this->configureParams();

    const ConfigType configType = this->getConfigTree();
    const ConfigType config     = configType.get_child("config.<xmlattr>");

    const bool visible = config.get<bool>(s_VISIBLE_CONFIG, m_isVisible);
    this->updateVisibility(visible);

    m_materialTemplateName = config.get<std::string>(s_MATERIAL_TEMPLATE_CONFIG, m_materialTemplateName);
    m_textureName          = config.get<std::string>(s_TEXTURE_NAME_CONFIG, m_textureName);
    m_radius               = config.get<std::string>(s_RADIUS_CONFIG, m_radius);
    m_displayLabel         = config.get<std::string>(s_DISPLAY_LABEL_CONFIG, m_displayLabel);
    m_labelColor           = config.get<std::string>(s_LABEL_COLOR_CONFIG, m_labelColor);
    m_color                = config.get<std::string>(s_COLOR_CONFIG, m_color);
    m_fixedSize            = config.get<std::string>(s_FIXED_SIZE_CONFIG, m_fixedSize);
    m_queryFlags           = config.get<std::string>(s_QUERY_CONFIG, m_queryFlags);
    m_fontSource           = config.get<std::string>(s_FONT_SOURCE_CONFIG, m_fontSource);
    m_fontSize             = config.get<std::string>(s_FONT_SIZE_CONFIG, m_fontSize);
}

//------------------------------------------------------------------------------

void SVideo::starting()
{
    this->initialize();

    const auto plW = this->getWeakInput< ::fwData::PointList >(s_PL_INPUT);

    if(plW.lock())
    {
        m_pointList = ::fwData::PointList::New();

        updatePL();

        m_pointListAdaptor = this->registerService< ::fwRenderOgre::IAdaptor >("::visuOgreAdaptor::SPointList");

        m_pointListAdaptor->registerInput(m_pointList, s_PL_INPUT, true);

        ::fwServices::IService::ConfigType config;
        config.add("config.<xmlattr>.layer", this->getLayerID());
        config.add("config.<xmlattr>.autoresetcamera", "no");
        if(!m_materialTemplateName.empty())
        {
            config.add("config.<xmlattr>." + s_MATERIAL_TEMPLATE_CONFIG, m_materialTemplateName);
        }
        if(!m_textureName.empty())
        {
            config.add("config.<xmlattr>." + s_TEXTURE_NAME_CONFIG, m_textureName);
        }
        if(!m_radius.empty())
        {
            config.add("config.<xmlattr>." + s_RADIUS_CONFIG, m_radius);
        }
        if(!m_displayLabel.empty())
        {
            config.add("config.<xmlattr>." + s_DISPLAY_LABEL_CONFIG, m_displayLabel);
        }
        if(!m_labelColor.empty())
        {
            config.add("config.<xmlattr>." + s_LABEL_COLOR_CONFIG, m_labelColor);
        }
        if(!m_color.empty())
        {
            config.add("config.<xmlattr>." + s_COLOR_CONFIG, m_color);
        }
        if(!m_fixedSize.empty())
        {
            config.add("config.<xmlattr>." + s_FIXED_SIZE_CONFIG, m_fixedSize);
        }
        if(!m_queryFlags.empty())
        {
            config.add("config.<xmlattr>." + s_QUERY_CONFIG, m_queryFlags);
        }
        if(!m_fontSource.empty())
        {
            config.add("config.<xmlattr>." + s_FONT_SOURCE_CONFIG, m_fontSource);
        }
        if(!m_fontSize.empty())
        {
            config.add("config.<xmlattr>." + s_FONT_SIZE_CONFIG, m_fontSize);
        }
        m_pointListAdaptor->setConfiguration(config);

        m_pointListAdaptor->setRenderService(this->getRenderService());

        m_pointListAdaptor->configure();
        m_pointListAdaptor->start();

        SLM_ASSERT("SPointList is not started", m_pointListAdaptor->isStarted());

    }
}

//-----------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SVideo::getAutoConnections() const
{
    ::fwServices::IService::KeyConnectionsMap connections;
    connections.push(s_IMAGE_INPUT, ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_IMAGE_INPUT, ::fwData::Image::s_MODIFIED_SIG, s_UPDATE_SLOT);

    connections.push(s_TF_INPUT, ::fwData::TransferFunction::s_MODIFIED_SIG, s_UPDATE_TF_SLOT);
    connections.push(s_TF_INPUT, ::fwData::TransferFunction::s_POINTS_MODIFIED_SIG, s_UPDATE_TF_SLOT);
    connections.push(s_TF_INPUT, ::fwData::TransferFunction::s_WINDOWING_MODIFIED_SIG, s_UPDATE_TF_SLOT);

    connections.push(s_PL_INPUT, ::fwData::PointList::s_MODIFIED_SIG, s_UPDATE_PL_SLOT);
    connections.push(s_PL_INPUT, ::fwData::PointList::s_POINT_ADDED_SIG, s_UPDATE_PL_SLOT);
    connections.push(s_PL_INPUT, ::fwData::PointList::s_POINT_REMOVED_SIG, s_UPDATE_PL_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

void SVideo::updating()
{
    this->getRenderService()->makeCurrent();

    // Getting Sight Image
    const auto imageSight = this->getLockedInput< ::fwData::Image >(s_IMAGE_INPUT);

    auto type = imageSight->getType();

    if (!m_isTextureInit || type != m_previousType )
    {
        // /////////////////////////////////////////////////////////////////////
        // Create the appropriate material according to the texture format
        // /////////////////////////////////////////////////////////////////////
        if(!m_texture)
        {
            auto texture = ::Ogre::TextureManager::getSingletonPtr()->createOrRetrieve(
                this->getID() + "_VideoTexture",
                ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, true).first;

            m_texture = ::Ogre::dynamic_pointer_cast< ::Ogre::Texture>( texture );
        }

        auto& mtlMgr   = ::Ogre::MaterialManager::getSingleton();
        const auto tfW = this->getWeakInput< ::fwData::TransferFunction >(s_TF_INPUT);
        const auto tf  = tfW.lock();

        ::Ogre::MaterialPtr defaultMat;
        if(tf)
        {
            if(type == ::fwTools::Type::s_FLOAT || type == ::fwTools::Type::s_DOUBLE)
            {
                defaultMat = mtlMgr.getByName(s_VIDEO_WITHTF_MATERIAL_NAME);
            }
            else
            {
                defaultMat = mtlMgr.getByName(s_VIDEO_WITHTF_INT_MATERIAL_NAME);
            }
        }
        else
        {
            defaultMat = mtlMgr.getByName(s_VIDEO_MATERIAL_NAME);
        }

        // Duplicate the template material to create our own material
        auto material = ::Ogre::MaterialManager::getSingletonPtr()->createOrRetrieve(
            this->getID() + "_VideoMaterial",
            ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
            true).first;
        m_material = ::Ogre::dynamic_pointer_cast< ::Ogre::Material>( material );

        defaultMat->copyDetailsTo(m_material);

        // Set the texture to the main material pass
        ::Ogre::Pass* ogrePass = m_material->getTechnique(0)->getPass(0);
        ogrePass->getTextureUnitState("image")->setTexture(m_texture);

        if(tf)
        {
            // TF texture initialization
            m_gpuTF = std::make_unique< ::fwRenderOgre::TransferFunction>();
            m_gpuTF->createTexture(this->getID());

            this->updateTF();
        }
        m_previousType = type;
    }

    const ::fwData::Image::Size size = imageSight->getSize2();
    ::fwRenderOgre::Utils::loadOgreTexture(imageSight.get_shared(), m_texture, ::Ogre::TEX_TYPE_2D, true);

    if (!m_isTextureInit || size[0] != m_previousWidth || size[1] != m_previousHeight )
    {
        this->clearEntity();

        // /////////////////////////////////////////////////////////////////////
        // Create the plane entity
        // /////////////////////////////////////////////////////////////////////
        const std::string thisID        = this->getID();
        const std::string videoMeshName = thisID + "_VideoMesh";
        const std::string entityName    = thisID + "_VideoEntity";
        const std::string nodeName      = thisID + "_VideoSceneNode";

        ::Ogre::MovablePlane plane( ::Ogre::Vector3::UNIT_Z, 0 );

        ::Ogre::SceneManager* sceneManager = this->getSceneManager();
        ::Ogre::MeshManager& meshManager   = ::Ogre::MeshManager::getSingleton();

        m_mesh = meshManager.createPlane(videoMeshName, ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                                         plane, static_cast< ::Ogre::Real >(size[0]),
                                         static_cast< ::Ogre::Real >(size[1]));

        // Create Ogre Entity
        m_entity = sceneManager->createEntity(entityName, videoMeshName);
        m_entity->setMaterial(m_material);

        // Add the entity to the scene
        m_sceneNode = sceneManager->getRootSceneNode()->createChildSceneNode(nodeName);
        m_sceneNode->attachObject(m_entity);

        // Slightly offset the plane in Z to allow some space for other entities, thus they can be rendered on top
        m_sceneNode->setPosition(0, 0, -1);
        m_sceneNode->setVisible(m_isVisible);

        ::Ogre::Camera* cam = this->getLayer()->getDefaultCamera();
        SLM_ASSERT("Default camera not found", cam);
        cam->setProjectionType(::Ogre::PT_ORTHOGRAPHIC);
        cam->setOrthoWindowHeight(static_cast< ::Ogre::Real >(size[1]));

        m_isTextureInit = true;
    }

    m_previousWidth  = size[0];
    m_previousHeight = size[1];

    this->requestRender();
}

//------------------------------------------------------------------------------

void SVideo::stopping()
{
    this->getRenderService()->makeCurrent();

    if(m_pointListAdaptor)
    {
        this->unregisterService(m_pointListAdaptor);
        m_pointList.reset();
    }

    this->clearEntity();

    m_material.reset();
    m_texture.reset();
    m_gpuTF.reset();

    m_isTextureInit = false;
}

//-----------------------------------------------------------------------------

void SVideo::setVisible(bool _visible)
{
    if(m_entity)
    {
        m_entity->setVisible(_visible);

        this->requestRender();
    }
}

//------------------------------------------------------------------------------

void SVideo::updateTF()
{
    const auto tfW = this->getWeakInput< ::fwData::TransferFunction >(s_TF_INPUT);
    const auto tf  = tfW.lock();

    m_gpuTF->updateTexture(tf.get_shared());

    ::Ogre::Pass* ogrePass = m_material->getTechnique(0)->getPass(0);
    m_gpuTF->bind(ogrePass, "tf", ogrePass->getFragmentProgramParameters());

    this->requestRender();
}

//------------------------------------------------------------------------------

void SVideo::updatePL()
{
    const auto image = this->getLockedInput< ::fwData::Image >(s_IMAGE_INPUT);

    const auto pl = this->getLockedInput< ::fwData::PointList >(s_PL_INPUT);

    const ::fwData::PointList::PointListContainer& inPoints = pl->getPoints();

    ::fwData::PointList::PointListContainer& outPoints = m_pointList->getPoints();
    outPoints.clear();

    for (size_t i = 0; i < inPoints.size(); ++i)
    {
        const ::fwData::Point::PointCoordArrayType& point = inPoints[i]->getCoord();
        outPoints.push_back(::fwData::Point::New(point[0] - static_cast< double >(image->getSize2()[0]) * 0.5,
                                                 -(point[1] - static_cast< double >(image->getSize2()[1]) * 0.5),
                                                 point[2]));
    }

    // Send the signal:
    auto modifiedSig = m_pointList->signal< ::fwData::PointList::ModifiedSignalType >(
        ::fwData::PointList::s_MODIFIED_SIG );
    modifiedSig->asyncEmit();
}

//------------------------------------------------------------------------------

void SVideo::clearEntity()
{
    if(m_entity)
    {
        m_entity->detachFromParent();
        this->getSceneManager()->destroyEntity(m_entity);
        m_entity = nullptr;
    }
    if(m_sceneNode)
    {
        m_sceneNode->removeAndDestroyAllChildren();
        this->getSceneManager()->destroySceneNode(m_sceneNode);
        m_sceneNode = nullptr;
    }
    if (m_mesh)
    {
        ::Ogre::MeshManager& meshManager = ::Ogre::MeshManager::getSingleton();
        meshManager.remove(m_mesh->getName());
        m_mesh.reset();
    }
}

//------------------------------------------------------------------------------

} // namespace visuOgreAdaptor.
