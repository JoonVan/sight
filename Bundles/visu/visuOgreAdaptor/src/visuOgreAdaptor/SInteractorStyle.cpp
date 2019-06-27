/************************************************************************
 *
 * Copyright (C) 2014-2019 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include "visuOgreAdaptor/SInteractorStyle.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hxx>

#include <fwRenderOgre/interactor/IInteractor.hpp>
#include <fwRenderOgre/interactor/IMovementInteractor.hpp>
#include <fwRenderOgre/interactor/IPickerInteractor.hpp>
#include <fwRenderOgre/interactor/VRWidgetsInteractor.hpp>

#include <fwServices/macros.hpp>

fwServicesRegisterMacro(::fwRenderOgre::IAdaptor, ::visuOgreAdaptor::SInteractorStyle,
                        ::fwData::Object);

namespace visuOgreAdaptor
{

const ::fwCom::Signals::SignalKeyType visuOgreAdaptor::SInteractorStyle::s_PICKED_SIG = "picked";

const ::fwCom::Slots::SlotKeyType visuOgreAdaptor::SInteractorStyle::s_PICK_SLOT = "pick";

static const std::string s_CONFIG_PICKER   = "picker";
static const std::string s_CONFIG_MOVEMENT = "movement";

static const std::map<std::string, std::string> s_STYLES_PICKER = {
    { "Mesh", "::fwRenderOgre::interactor::MeshPickerInteractor"},
    { "Video", "::fwRenderOgre::interactor::VideoPickerInteractor"},
};

static const std::map<std::string, std::string> s_STYLES_MOVEMENT = {
    { "Trackball", "::fwRenderOgre::interactor::TrackballInteractor"},
    { "Fixed", "::fwRenderOgre::interactor::FixedStyleInteractor"},
    { "Negato2D", "::fwRenderOgre::interactor::Negato2DInteractor"},
    { "VR", "::fwRenderOgre::interactor::VRWidgetsInteractor"}
};

//------------------------------------------------------------------------------

SInteractorStyle::SInteractorStyle() noexcept
{
    newSlot( s_PICK_SLOT, &::visuOgreAdaptor::SInteractorStyle::picked, this );

    m_sigPicked = newSignal< PointClickedSigType >( s_PICKED_SIG );

    newSlot( "addPoint", &::visuOgreAdaptor::SInteractorStyle::addPointDeprecated, this );
    newSlot( "removePoint", &::visuOgreAdaptor::SInteractorStyle::removePointDeprecated, this );

    m_sigAddPointDeprecated    = newSignal< PointClickedSignalTypeDeprecated >( "pointAdded" );
    m_sigRemovePointDeprecated = newSignal< PointClickedSignalTypeDeprecated >( "pointRemoved" );
}

//------------------------------------------------------------------------------

SInteractorStyle::~SInteractorStyle() noexcept
{
}

//------------------------------------------------------------------------------

void SInteractorStyle::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");

    if(config.count(s_CONFIG_PICKER))
    {
        m_pickerStyle = config.get<std::string>(s_CONFIG_PICKER);
    }

    if(config.count(s_CONFIG_MOVEMENT))
    {
        m_movementStyle = config.get<std::string>(s_CONFIG_MOVEMENT);
    }

    m_queryFlags = config.get<std::uint32_t>("queryFlags", m_queryFlags);
}

//------------------------------------------------------------------------------

void SInteractorStyle::starting()
{
    this->initialize();

    this->setInteractorStyle();

    if(!m_pickerStyle.empty())
    {
        ::fwRenderOgre::interactor::IPickerInteractor::sptr pickerInteractor =
            this->getRenderService()->getLayer(m_layerID)->getSelectInteractor();

        if(pickerInteractor)
        {
            m_connections.connect(pickerInteractor, ::fwRenderOgre::interactor::IPickerInteractor::s_PICKED_SIG,
                                  this->getSptr(), ::visuOgreAdaptor::SInteractorStyle::s_PICK_SLOT);

            m_connections.connect(pickerInteractor, "addPoint", this->getSptr(), "addPoint");
            m_connections.connect(pickerInteractor,  "removePoint", this->getSptr(), "removePoint");
        }
    }
}

//------------------------------------------------------------------------------

void SInteractorStyle::updating()
{
}

//------------------------------------------------------------------------------

void SInteractorStyle::stopping()
{
    m_connections.disconnect();
}

//------------------------------------------------------------------------------

void SInteractorStyle::setInteractorStyle()
{
    if(!m_pickerStyle.empty())
    {
        if(s_STYLES_PICKER.count(m_pickerStyle))
        {
            const auto style = s_STYLES_PICKER.at(m_pickerStyle);

            m_selectInteractor = ::fwRenderOgre::interactorFactory::New(style, this->getSceneManager()->getName());
            SLM_ASSERT(this->getID() + " : Unknown picker interactor style : " + style, m_selectInteractor);

            auto layer            = this->getRenderService()->getLayer(m_layerID);
            auto pickerInteractor = ::fwRenderOgre::interactor::IPickerInteractor::dynamicCast(m_selectInteractor);
            pickerInteractor->initPicker();
            pickerInteractor->setQueryFlags(m_queryFlags);
            layer->setSelectInteractor(::fwRenderOgre::interactor::IPickerInteractor::dynamicCast(m_selectInteractor));
        }
        else
        {
            SLM_WARN(this->getID() + " : '" + s_CONFIG_PICKER +"' has an unknown value : '" + m_pickerStyle +"'");
        }
    }
    else
    {
        SLM_WARN(this->getID() + " : '" + s_CONFIG_PICKER +"' is not set.");
    }

    if(!m_movementStyle.empty())
    {
        if(s_STYLES_MOVEMENT.count(m_movementStyle))
        {
            const auto style = s_STYLES_MOVEMENT.at(m_movementStyle);

            m_moveInteractor = ::fwRenderOgre::interactorFactory::New(style, this->getSceneManager()->getName());
            SLM_ASSERT(this->getID() + " : Unknown movement interactor style : " + style, m_moveInteractor);

            auto layer = this->getRenderService()->getLayer(m_layerID);
            layer->setMoveInteractor(::fwRenderOgre::interactor::IMovementInteractor::dynamicCast(m_moveInteractor));
        }
        else
        {
            SLM_WARN(this->getID() + " : '" + s_CONFIG_MOVEMENT +"' has an unknown value : '" + m_movementStyle +"'");
        }
    }
    else
    {
        SLM_WARN(this->getID() + " : '" + s_CONFIG_MOVEMENT +"' is not set.");
    }

}

//------------------------------------------------------------------------------

void SInteractorStyle::picked(::fwDataTools::PickingInfo _info)
{
    m_sigPicked->asyncEmit(_info);
}

//------------------------------------------------------------------------------

void SInteractorStyle::addPointDeprecated(::fwData::Object::sptr _obj )
{
    if(m_sigAddPointDeprecated->getNumberOfConnections() > 0 )
    {
        FW_DEPRECATED_MSG(
            "This signal is deprecated. You should use `SInteractorStyle::pick(::fwDataTools::PickingInfo)`",
            "20.0");
        m_sigAddPointDeprecated->asyncEmit( _obj );
    }
}

//------------------------------------------------------------------------------

void SInteractorStyle::removePointDeprecated(::fwData::Object::sptr _obj )
{
    if(m_sigRemovePointDeprecated->getNumberOfConnections() > 0 )
    {
        FW_DEPRECATED_MSG(
            "This signal is deprecated. You should use `SInteractorStyle::pick(::fwDataTools::PickingInfo)`",
            "20.0");
        m_sigRemovePointDeprecated->asyncEmit( _obj );
    }
}

} //namespace visuOgreAdaptor
