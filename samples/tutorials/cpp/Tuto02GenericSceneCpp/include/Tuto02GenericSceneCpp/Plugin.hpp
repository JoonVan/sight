/************************************************************************
 *
 * Copyright (C) 2018-2020 IRCAD France
 * Copyright (C) 2018-2020 IHU Strasbourg
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

#include "Tuto02GenericSceneCpp/config.hpp"

#include <fwRuntime/Plugin.hpp>

#include <fwServices/AppManager.hpp>

namespace Tuto02GenericSceneCpp
{

/// This class is started when the module is loaded.
class TUTO02GENERICSCENECPP_CLASS_API Plugin final : public ::fwRuntime::Plugin
{

public:

    /// Destroys the plugin.
    TUTO02GENERICSCENECPP_API ~Plugin() noexcept override;

    /// Starts the plugin, does nothing here.
    TUTO02GENERICSCENECPP_API void start() override;

    /// Stops the plugin, does nothing here.
    TUTO02GENERICSCENECPP_API void stop() noexcept override;

    /// Creates the whole application.
    TUTO02GENERICSCENECPP_API void initialize() override;

    /// Destroys the whole application.
    TUTO02GENERICSCENECPP_API void uninitialize() override;

private:

    std::unique_ptr< ::fwServices::AppManager > m_appManager;

};

} // namespace Tuto02GenericSceneCpp.
