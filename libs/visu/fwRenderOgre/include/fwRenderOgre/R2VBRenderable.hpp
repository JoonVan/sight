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

#pragma once

#include "fwRenderOgre/config.hpp"
#include "fwRenderOgre/IAdaptor.hpp"

#include <fwData/Mesh.hpp>

#include <OGRE/OgreEntity.h>
#include <OGRE/OgreManualObject.h>
#include <OGRE/OgreNode.h>
#include <OGRE/OgreRenderToVertexBuffer.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreSimpleRenderable.h>
#include <OGRE/OgreSubEntity.h>

namespace fwRenderOgre
{

/**
 * @brief This object is used to implement a render-to-vertex-buffer (r2vb) process (GL_TRANSFORM_FEEDBACK).
 *
 * This objects holds a reference to the object used as input for the render-to-vertex-buffer process.
 * It also contains the output vertex buffer, that is used to be displayed like a "regular" object.
 */
class FWRENDEROGRE_CLASS_API R2VBRenderable : public ::Ogre::SimpleRenderable
{
public:
    [[deprecated("Will be removed in sight 22.0")]]
    FWRENDEROGRE_API static R2VBRenderable* New(const std::string& _name, ::Ogre::SubEntity* _sourceObject,
                                                ::Ogre::SceneManager* _sceneManager,
                                                ::fwData::Mesh::CellTypesEnum _primitiveType,
                                                const std::string& _mtlName );

    /// Create a new instance of R2VBRenderable
    FWRENDEROGRE_API static R2VBRenderable* New(const std::string& _name, ::Ogre::SubEntity* _sourceObject,
                                                ::Ogre::SceneManager* _sceneManager,
                                                ::fwData::Mesh::CellType _primitiveType,
                                                const std::string& _mtlName );

    FWRENDEROGRE_API R2VBRenderable(const ::Ogre::String& name);
    virtual FWRENDEROGRE_API ~R2VBRenderable();

    /// Set the maximum number of vertices in output, and adjust the size of the output buffer accordingly.
    /// It also updates the vertex declaration of the output buffer
    FWRENDEROGRE_API void setOutputSettings(size_t _vertexCount, bool _hasColor, bool _hasTexCoord,
                                            bool _hasNormals = true);

    /** @copydoc SimpleRenderable::_updateRenderQueue. */
    FWRENDEROGRE_API virtual void _updateRenderQueue(::Ogre::RenderQueue* _queue) override;

    /** @copydoc SimpleRenderable::getMovableType. */
    FWRENDEROGRE_API virtual const ::Ogre::String& getMovableType(void) const override;

    /** @copydoc SimpleRenderable::getRenderOperation. */
    FWRENDEROGRE_API virtual void getRenderOperation(::Ogre::RenderOperation& _op) override;

    /// Delegate to the subentity.
    FWRENDEROGRE_API virtual ::Ogre::Real getBoundingRadius(void) const override;

    /// @copydoc Renderable::getSquaredViewDepth
    FWRENDEROGRE_API virtual ::Ogre::Real getSquaredViewDepth(const Ogre::Camera* _cam) const override;

    /// Return the parent mesh.
    FWRENDEROGRE_API virtual const ::Ogre::MeshPtr& getMesh(void) const;

    /// Mark the output verex buffer as dirty, the r2vb process will be run on next update
    FWRENDEROGRE_API void setDirty();

    /// Runs the R2VB process.
    FWRENDEROGRE_API void manualUpdate();

    /// Set the material used to process the geometry pass.
    FWRENDEROGRE_API void setRenderToBufferMaterial(const std::string& _mtlName);

    ::fwRenderOgre::IAdaptor::wptr m_materialAdaptor;

    [[deprecated("Will be removed in sight 22.0")]]
    ::fwData::Mesh::CellTypesEnum getInputPrimitiveType() const;

    ::fwData::Mesh::CellType getInputPrimitiveType2() const;

protected:
    /// Source object of the r2vb process
    ::Ogre::SubEntity* m_srcObject;

    /// Buffer used as output
    ::Ogre::RenderToVertexBufferSharedPtr m_r2vbBuffer;

    /// Tells if the r2vb must be run on next update - typically we want this to be done, at worst, only once per frame.
    /// Thus we use this flag, depending on the technique to enable the r2vb only on the first rendering pass.
    bool m_dirty;

    /// Primitive type used as input
    ::fwData::Mesh::CellType m_inputPrimitiveType;

    /// Maximum number of vertices in output buffer
    size_t m_maxOutputVertexCount;
};

//-----------------------------------------------------------------------------
// Inline functions

//-----------------------------------------------------------------------------

inline R2VBRenderable::~R2VBRenderable()
{
    m_srcObject = nullptr;
}

//-----------------------------------------------------------------------------

inline Ogre::Real R2VBRenderable::getBoundingRadius() const
{
    return m_srcObject->getParent()->getBoundingRadius();
}

//-----------------------------------------------------------------------------

inline Ogre::Real R2VBRenderable::getSquaredViewDepth(const Ogre::Camera* _cam) const
{
    return this->getParentNode()->getSquaredViewDepth(_cam);
}

//-----------------------------------------------------------------------------

inline void R2VBRenderable::setDirty()
{
    m_dirty = true;
}

//-----------------------------------------------------------------------------

inline void R2VBRenderable::manualUpdate()
{
    m_r2vbBuffer->update(mParentSceneManager);
}

//-----------------------------------------------------------------------------

inline void R2VBRenderable::setRenderToBufferMaterial(const std::string& _mtlName)
{
    m_r2vbBuffer->setRenderToBufferMaterialName(_mtlName);
}

//------------------------------------------------------------------------------

inline ::fwData::Mesh::CellTypesEnum R2VBRenderable::getInputPrimitiveType() const
{
    return static_cast< ::fwData::Mesh::CellTypesEnum >(m_inputPrimitiveType);
}

//------------------------------------------------------------------------------

inline ::fwData::Mesh::CellType R2VBRenderable::getInputPrimitiveType2() const
{
    return m_inputPrimitiveType;
}

} // namespace fwRenderOgre
