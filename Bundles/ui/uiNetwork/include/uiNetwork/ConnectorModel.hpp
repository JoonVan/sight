/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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

#ifndef __UINETWORK_CONNECTORMODEL_HPP__
#define __UINETWORK_CONNECTORMODEL_HPP__

#include "uiNetwork/config.hpp"

#include <fwTools/Failed.hpp>
#include <fwRuntime/ConfigurationElement.hpp>
#include <fwData/Object.hpp>

#include <vector>
#include <string>
#include <map>

namespace uiNetwork
{
/**
 *
 * @brief A model used to retreive service configuration and network service in current F4S app
 */
class UINETWORK_CLASS_API ConnectorModel
{
public:
    /**
     * @typedef ServiceConfigurationContainer
     *
     * @brief map when each key have a configuration element used to store services configuration
     */
    typedef std::map<std::string, ::fwRuntime::ConfigurationElement::sptr> ServiceConfigurationContainer;

    /// Constructor
    ConnectorModel();

    /// Destructor
    ~ConnectorModel();

    /**
     * @brief parse the configuration element, retrieve all service configuration and retrieve all network service
     * registered
     */
    void initialize(::fwRuntime::ConfigurationElement::csptr configElement,
                    ::fwData::Object::csptr obj);

    /**
     * @brief getter for registered ioNetwork::INetworkListener in a F4S app
     *
     * @return a vector of service name in a F4S app
     */
    std::vector<std::string> const& getRegisteredListeners() const;

    /**
     * @brief getter for registered ioNetwork::INetworkSender in a F4S app
     *
     * @return  a vector of service name in a F4S app
     */
    std::vector<std::string> const& getRegisteredSenders() const;

    /**
     * @brief getter for services configuration
     *
     * @return @see ServiceConfigurationContainer
     */
    ServiceConfigurationContainer const& getServiceConfigurations() const;


    bool isRegisteredService(std::string const& str) const;
private:
    /// registered listeners name
    std::vector<std::string> m_registeredListeners;

    /// registered senders name
    std::vector<std::string> m_registeredSenders;

    /// services configuration
    ServiceConfigurationContainer m_serviceConfigs;

};
} //namespace uiNetwork

#endif // __UINETWORK_CONNECTORMODEL_HPP__
