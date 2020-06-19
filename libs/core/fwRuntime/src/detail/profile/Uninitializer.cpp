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

#include "fwRuntime/detail/profile/Uninitializer.hpp"

#include "fwRuntime/detail/Module.hpp"
#include "fwRuntime/detail/Runtime.hpp"

#include <fwCore/base.hpp>

namespace fwRuntime
{

namespace detail
{

namespace profile
{

//------------------------------------------------------------------------------

Uninitializer::Uninitializer( const std::string& identifier, const Version& version ) :
    m_identifier( identifier ),
    m_version( version )
{
}

//------------------------------------------------------------------------------

void Uninitializer::apply()
{
    auto module = detail::Runtime::get().findEnabledModule(m_identifier, m_version);
    SLM_FATAL_IF("Unable to start module " + Module::getModuleStr(m_identifier, m_version) + ": not found.",
                 module == nullptr);
    try
    {
        module->uninitialize();
    }
    catch( const std::exception& e )
    {
        SLM_FATAL("Unable to uninitialize module " + detail::Module::getModuleStr(m_identifier,
                                                                                  m_version) + ". " + e.what());
    }
}

//------------------------------------------------------------------------------

} // namespace profile

} // namespace detail

} // namespace fwRuntime
