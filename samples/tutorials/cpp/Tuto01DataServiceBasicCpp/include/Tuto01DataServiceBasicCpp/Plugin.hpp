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

#pragma once

#include "Tuto01DataServiceBasicCpp/config.hpp"

#include <fwData/Image.hpp>

#include <fwRuntime/Plugin.hpp>

#include <fwServices/IService.hpp>

namespace Tuto01DataServiceBasicCpp
{

/// This class is started when the module is loaded.
class TUTO01DATASERVICEBASICCPP_CLASS_API Plugin final : public ::fwRuntime::Plugin
{

public:

    /// Destroys the plugin.
    TUTO01DATASERVICEBASICCPP_API ~Plugin() noexcept override;

    /// Starts the plugin, does nothing here.
    TUTO01DATASERVICEBASICCPP_API void start() override;

    /// Stops the plugin, does nothing here.
    TUTO01DATASERVICEBASICCPP_API void stop() noexcept override;

    /// Creates the whole application..
    TUTO01DATASERVICEBASICCPP_API void initialize() override;

    /// Destroys the whole application..
    TUTO01DATASERVICEBASICCPP_API void uninitialize() override;

private:

    ::fwData::Image::sptr m_image;

    ::fwServices::IService::sptr m_frameSrv;

    ::fwServices::IService::sptr m_renderSrv;

    ::fwServices::IService::sptr m_interactorSrv;

    ::fwServices::IService::sptr m_negatoSrv;

    ::fwServices::IService::sptr m_readerSrv;

};

} // namespace Tuto01DataServiceBasicCpp.
