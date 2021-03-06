/************************************************************************
 *
 * Copyright (C) 2020 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include "uiGenericQt/action/SDisplayTestNotifications.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwServices/macros.hpp>

#include <QApplication>
#include <QFile>
#include <QString>
#include <QTextStream>

namespace uiGenericQt
{
namespace action
{

static const ::fwCom::Slots::SlotKeyType s_SET_ENUM_PARAMETER_SLOT = "setEnumParameter";
static const ::fwCom::Slots::SlotKeyType s_SET_BOOL_PARAMETER_SLOT = "setBoolParameter";

fwServicesRegisterMacro( ::fwGui::IActionSrv, ::uiGenericQt::action::SDisplayTestNotifications)

//------------------------------------------------------------------------------

SDisplayTestNotifications::SDisplayTestNotifications( ) noexcept
{
    newSlot(s_SET_ENUM_PARAMETER_SLOT, &SDisplayTestNotifications::setEnumParameter, this);
    newSlot(s_SET_BOOL_PARAMETER_SLOT, &SDisplayTestNotifications::setBoolParameter, this);
}

//------------------------------------------------------------------------------

SDisplayTestNotifications::~SDisplayTestNotifications() noexcept
{
}

//------------------------------------------------------------------------------

void SDisplayTestNotifications::setEnumParameter(std::string _val, std::string _key)
{
    if(_key == "position")
    {
        m_displayAll = false;
        if(_val == "ALL")
        {
            m_displayAll = true;
        }
        else if(_val == "TOP_LEFT")
        {
            m_position = ::dial::NotificationDialog::Position::TOP_LEFT;
        }
        else if(_val == "TOP_RIGHT")
        {
            m_position = ::dial::NotificationDialog::Position::TOP_RIGHT;
        }
        else if(_val == "CENTERED_TOP")
        {
            m_position = ::dial::NotificationDialog::Position::CENTERED_TOP;
        }
        else if(_val == "CENTERED")
        {
            m_position = ::dial::NotificationDialog::Position::CENTERED;
        }
        else if(_val == "BOTTOM_LEFT")
        {
            m_position = ::dial::NotificationDialog::Position::BOTTOM_LEFT;
        }
        else if(_val == "BOTTOM_RIGHT")
        {
            m_position = ::dial::NotificationDialog::Position::BOTTOM_RIGHT;
        }
        else if(_val == "CENTERED_BOTTOM")
        {
            m_position = ::dial::NotificationDialog::Position::CENTERED_BOTTOM;
        }
        else
        {
            SLM_ERROR("Value '" + _val + "' is not handled for key " + _key);
        }
    }
    else if(_key == "type")
    {
        if(_val == "SUCCESS")
        {
            m_type = ::dial::NotificationDialog::Type::SUCCESS;
        }
        else if (_val == "INFO")
        {
            m_type = ::dial::NotificationDialog::Type::INFO;
        }
        else if (_val == "FAILURE")
        {
            m_type = ::dial::NotificationDialog::Type::FAILURE;
        }
        else
        {
            SLM_ERROR("Value '" + _val + "' is not handled for key " + _key);
        }
    }
    else
    {
        SLM_ERROR("Key '" + _key + "' is not handled." );
    }
}

//------------------------------------------------------------------------------

void SDisplayTestNotifications::setBoolParameter(bool _val, std::string _key)
{
    if(_key == "useSNotifier")
    {
        m_useSNotifier = _val;
    }
    else
    {
        SLM_ERROR("Key '" + _key + "' is not handled." );
    }
}

//------------------------------------------------------------------------------

void SDisplayTestNotifications::info(std::ostream& _sstream )
{
    _sstream << "SDisplayTestNotifications" << std::endl;
}

//------------------------------------------------------------------------------

void SDisplayTestNotifications::configuring()
{
    this->::fwGui::IActionSrv::initialize();
}

//------------------------------------------------------------------------------

void SDisplayTestNotifications::updating( )
{
    if(m_useSNotifier)
    {
        // Mode 1: You use the SNotifier service that will display for you the notifications, you need to emit the
        // proper signal.
        // Notification will always be displayed at the same place,
        // and will be queued if several notifications are displayed at the same time.

        if(m_type == ::dial::NotificationDialog::Type::SUCCESS)
        {
            auto notif = this->signal< ::fwServices::IService::SuccessNotifiedSignalType >(
                ::fwServices::IService::s_SUCCESS_NOTIFIED_SIG);

            notif->asyncEmit("Notification Test !");
        }
        else if(m_type == ::dial::NotificationDialog::Type::FAILURE)
        {
            auto notif = this->signal< ::fwServices::IService::FailureNotifiedSignalType >(
                ::fwServices::IService::s_FAILURE_NOTIFIED_SIG);

            notif->asyncEmit("Notification Test !");
        }
        else
        {
            auto notif = this->signal< ::fwServices::IService::InfoNotifiedSignalType >(
                ::fwServices::IService::s_INFO_NOTIFIED_SIG);

            notif->asyncEmit("Notification Test !");
        }
    }
    else
    {
        // Mode 2: Standalone, you decide where to pop the notification by calling directly the NotificationDialog.
        if(m_displayAll)
        {
            ::dial::NotificationDialog::show("Notification Test !", m_type,
                                             ::dial::NotificationDialog::Position::TOP_LEFT );
            ::dial::NotificationDialog::show("Notification Test !", m_type,
                                             ::dial::NotificationDialog::Position::TOP_RIGHT );
            ::dial::NotificationDialog::show("Notification Test !", m_type,
                                             ::dial::NotificationDialog::Position::CENTERED_TOP );
            ::dial::NotificationDialog::show("Notification Test !", m_type,
                                             ::dial::NotificationDialog::Position::CENTERED );
            ::dial::NotificationDialog::show("Notification Test !", m_type,
                                             ::dial::NotificationDialog::Position::BOTTOM_LEFT );
            ::dial::NotificationDialog::show("Notification Test !", m_type,
                                             ::dial::NotificationDialog::Position::BOTTOM_RIGHT );
            ::dial::NotificationDialog::show("Notification Test !", m_type,
                                             ::dial::NotificationDialog::Position::CENTERED_BOTTOM );
        }
        else
        {
            ::dial::NotificationDialog::show("Notification Test !", m_type, m_position );
        }
    }
}

//------------------------------------------------------------------------------

void SDisplayTestNotifications::starting()
{
    this->::fwGui::IActionSrv::actionServiceStarting();
}

//------------------------------------------------------------------------------

void SDisplayTestNotifications::stopping()
{
    this->::fwGui::IActionSrv::actionServiceStopping();
}

//------------------------------------------------------------------------------

} // namespace action
} // namespace uiGenericQt
