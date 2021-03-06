/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "ioPacs/SPacsConfigurationInitializer.hpp"

#include <fwData/Composite.hpp>
#include <fwData/String.hpp>

#include <fwPreferences/helper.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <utility>

namespace ioPacs
{

template<typename T, typename CAST_T = T>
struct SetFromConfig
{
    //------------------------------------------------------------------------------

    void operator()(::fwData::Composite::sptr config, std::string const& confName, T& attribute)
    {
        if(config)
        {
            ::fwData::Composite::iterator it = config->find(confName);
            if(it != config->end())
            {
                ::fwData::String::sptr obj = ::fwData::String::dynamicCast(it->second);
                attribute = T(::boost::lexical_cast<CAST_T>(obj->value()));
            }
        }
    }
};

static const ::fwServices::IService::KeyType s_CONFIG_INOUT = "config";

fwServicesRegisterMacro(::fwServices::IController, ::ioPacs::SPacsConfigurationInitializer)

//------------------------------------------------------------------------------

SPacsConfigurationInitializer::SPacsConfigurationInitializer() noexcept
{
}

//------------------------------------------------------------------------------

SPacsConfigurationInitializer::~SPacsConfigurationInitializer() noexcept
{
}

//------------------------------------------------------------------------------

void SPacsConfigurationInitializer::configuring()
{
    const ConfigType configTree = this->getConfigTree();
    const auto config           = configTree.get_child("config.<xmlattr>");

    // Local application title.
    m_SCUAppEntityTitle = config.get< std::string >("localApplicationTitle");

    // Pacs host name.
    m_SCPHostName = config.get< std::string >("pacsHostName");

    // Pacs application title.
    m_SCPAppEntityTitle = config.get< std::string >("pacsApplicationTitle");

    // Pacs port.
    const std::string pacsApplicationPort = config.get< std::string >("pacsApplicationPort");
    m_SCPPort = static_cast < decltype(m_SCPPort) >(std::stoi(pacsApplicationPort));

    // Retrieve Method.
    const std::string retrieveMethod = config.get< std::string >("retrieveMethod");
    m_retrieveMethod = (retrieveMethod == "MOVE") ?
                       (::fwPacsIO::data::PacsConfiguration::MOVE_RETRIEVE_METHOD):
                       (::fwPacsIO::data::PacsConfiguration::GET_RETRIEVE_METHOD);

    // Move application title.
    m_moveAppEntityTitle = config.get<std::string>("moveApplicationTitle");

    // Move application port.
    const std::string moveApplicationPort = config.get< std::string >("moveApplicationPort");
    m_movePort = static_cast < decltype(m_movePort) >(std::stoi(moveApplicationPort));

    // Preference Key.
    m_preferenceKey = config.get< std::string >("preferenceKey", m_preferenceKey);
}

//------------------------------------------------------------------------------

void SPacsConfigurationInitializer::starting()
{
    const ::fwPacsIO::data::PacsConfiguration::sptr pacsConfiguration
        = this->getInOut< ::fwPacsIO::data::PacsConfiguration >(s_CONFIG_INOUT);
    SLM_ASSERT("input '" + s_CONFIG_INOUT +"' does not exist.", pacsConfiguration);

    // Set information from xml and update PacsConfiguration.
    if(!m_preferenceKey.empty())
    {
        ::fwData::Composite::sptr prefs = ::fwPreferences::getPreferences();
        if(prefs)
        {
            ::fwData::Composite::sptr config = ::fwData::Composite::dynamicCast((*prefs)[m_preferenceKey]);

            SetFromConfig< std::string > setFromConfig;
            SetFromConfig< unsigned short > setFromConfigShort;
            SetFromConfig< ::fwPacsIO::data::PacsConfiguration::RETRIEVE_METHOD, int > setFromConfigEnum;

            setFromConfig(config, "LocalApplicationTitle", m_SCUAppEntityTitle);
            setFromConfig(config, "PacsHostName", m_SCPHostName);
            setFromConfig(config, "PacsApplicationTitle", m_SCPAppEntityTitle);
            setFromConfigShort(config, "PacsApplicationPort", m_SCPPort);
            setFromConfig(config, "MoveApplicationTitle", m_moveAppEntityTitle);
            setFromConfigShort(config, "MoveApplicationPort", m_movePort);
            setFromConfigEnum(config, "RetrieveMethod", m_retrieveMethod);
        }
    }

    pacsConfiguration->setLocalApplicationTitle(m_SCUAppEntityTitle);
    pacsConfiguration->setPacsHostName(m_SCPHostName);
    pacsConfiguration->setPacsApplicationTitle(m_SCPAppEntityTitle);
    pacsConfiguration->setPacsApplicationPort(m_SCPPort);
    pacsConfiguration->setMoveApplicationTitle(m_moveAppEntityTitle);
    pacsConfiguration->setMoveApplicationPort(m_movePort);
    pacsConfiguration->setRetrieveMethod(m_retrieveMethod);
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SPacsConfigurationInitializer::getAutoConnections() const
{
    ::fwServices::IService::KeyConnectionsMap connections;

    connections.push(s_CONFIG_INOUT, ::fwPacsIO::data::PacsConfiguration::s_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

void SPacsConfigurationInitializer::updating()
{
    const ::fwPacsIO::data::PacsConfiguration::sptr pacsConfiguration
        = this->getInOut< ::fwPacsIO::data::PacsConfiguration >(s_CONFIG_INOUT);
    SLM_ASSERT("input '" + s_CONFIG_INOUT +"' does not exist.", pacsConfiguration);

    // Check if the user has changed the Pacs configuration and update the local var
    if(pacsConfiguration->getLocalApplicationTitle() != m_SCUAppEntityTitle
       || pacsConfiguration->getPacsHostName() != m_SCPHostName
       || pacsConfiguration->getPacsApplicationTitle() != m_SCPAppEntityTitle
       || pacsConfiguration->getPacsApplicationPort() != m_SCPPort
       || pacsConfiguration->getMoveApplicationTitle() != m_moveAppEntityTitle
       || pacsConfiguration->getMoveApplicationPort() != m_movePort
       || pacsConfiguration->getRetrieveMethod() != m_retrieveMethod
       )
    {
        m_SCUAppEntityTitle  = pacsConfiguration->getLocalApplicationTitle();
        m_SCPHostName        = pacsConfiguration->getPacsHostName();
        m_SCPAppEntityTitle  = pacsConfiguration->getPacsApplicationTitle();
        m_SCPPort            = pacsConfiguration->getPacsApplicationPort();
        m_moveAppEntityTitle = pacsConfiguration->getMoveApplicationTitle();
        m_movePort           = pacsConfiguration->getMoveApplicationPort();
        m_retrieveMethod     = pacsConfiguration->getRetrieveMethod();
    }

    // If a preference key is set, save the local var to the preferences
    if(!m_preferenceKey.empty())
    {
        ::fwData::Composite::sptr prefs = ::fwPreferences::getPreferences();
        if(prefs && (prefs->find(m_preferenceKey) == prefs->end() || !(*prefs)[m_preferenceKey]))
        {
            (*prefs)[m_preferenceKey] = ::fwData::Composite::New();
        }
        if(prefs)
        {
            ::fwData::Composite::sptr config   = ::fwData::Composite::dynamicCast((*prefs)[m_preferenceKey]);
            (*config)["LocalApplicationTitle"] = ::fwData::String::New(m_SCUAppEntityTitle);
            (*config)["PacsHostName"         ] = ::fwData::String::New(m_SCPHostName);
            (*config)["PacsApplicationTitle" ] = ::fwData::String::New(m_SCPAppEntityTitle);
            (*config)["PacsApplicationPort"  ] =
                ::fwData::String::New(::boost::lexical_cast<std::string>(m_SCPPort));
            (*config)["MoveApplicationTitle" ] = ::fwData::String::New(m_moveAppEntityTitle);
            (*config)["MoveApplicationPort"  ] =
                ::fwData::String::New(::boost::lexical_cast<std::string>(m_movePort));
            (*config)["RetrieveMethod"       ] =
                ::fwData::String::New(::boost::lexical_cast<std::string>(m_retrieveMethod));
        }
        ::fwPreferences::savePreferences();
    }
}

//------------------------------------------------------------------------------

void SPacsConfigurationInitializer::stopping()
{
    this->updating();
}

} // namespace ioPacs
