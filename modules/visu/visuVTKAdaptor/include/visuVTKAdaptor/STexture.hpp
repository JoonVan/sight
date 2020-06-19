/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "visuVTKAdaptor/config.hpp"

#include <fwRenderVTK/IAdaptor.hpp>

namespace fwData
{
class Material;
}

namespace visuVTKAdaptor
{

/**
 * @brief Adaptor to map a texture on a mesh.
 *
 * This is done via ::visuVTKAdaptor::SMaterial. However in the configuration we don't specify the material adaptor
 * since it is automatically created by the ::visuVTKAdaptor::SMesh adaptor. Thus we rather try to specify the mesh
 * adaptor.
 *
 * To use this adaptor, you need to connect it to the SMesh::textureApplied signal
 *
 * @code{.xml}
    <connect>
        <signal>meshAdaptor/textureApplied</signal>
        <slot>textureAdaptor/applyTexture</slot>
    </connect>
   @endcode
 *
 * @section Slots Slots
 * - \b applyTexture(SPTR(::fwData::Material)): update image slice type
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
    <service type="::visuVTKAdaptor::STexture" autoConnect="yes">
       <inout key="texture" uid="..." />
       <config texture="texture" filtering="linear" wrapping="repeat" />
   </service>
   @endcode
 *
 *  * @subsection In-Out In-Out
 * - \b texture [::fwData::Image]: image to use as texture.
 *
 * @subsection Configuration Configuration:
 * - \b config(mandatory) : contains the adaptor configuration
 *    - \b filtering (optional) : filtering of the texture, "nearest" or "linear"
 *    - \b wrapping (optional) : wrapping of the texture, "clamp" or "repeat"
 *    - \b lighting (optional) : enable the lighting, "yes" or "no" default yes
 */
class VISUVTKADAPTOR_CLASS_API STexture : public ::fwRenderVTK::IAdaptor
{

public:

    fwCoreServiceMacro(STexture, ::fwRenderVTK::IAdaptor);

    /**
     * @name Slots API
     * @{
     */
    VISUVTKADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_APPLY_TEXTURE_SLOT;
    typedef ::fwCom::Slot< void ( SPTR(::fwData::Material) ) > ApplyTextureSlotType;
    /** @} */

    /// Constructor
    VISUVTKADAPTOR_API STexture() noexcept;
    /// Destructor
    VISUVTKADAPTOR_API virtual ~STexture() noexcept;

protected:

    VISUVTKADAPTOR_API void configuring() override;
    VISUVTKADAPTOR_API void starting() override;
    VISUVTKADAPTOR_API void updating() override;
    VISUVTKADAPTOR_API void stopping() override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Image::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect Image::s_BUFFER_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    VISUVTKADAPTOR_API virtual KeyConnectionsMap getAutoConnections() const override;

    /// Slot called when a texture must be applied on a material.
    void applyTexture( SPTR(::fwData::Material) _material);

    /// Contains all mesh adaptors that currently have this texture applied. Needed when image is updated.
    std::set< SPTR(::fwData::Material) > m_materialSet;

    /// How to filter this texture
    std::string m_filtering;

    /// How to wrap the texture
    std::string m_wrapping;

    /// enable or not the lighting (default true)
    bool m_lighting;
};

} //namespace visuVTKAdaptor
