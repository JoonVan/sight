/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#ifndef __FWTOOLS_OS_HPP__
#define __FWTOOLS_OS_HPP__

#include <string>

#include "fwTools/config.hpp"


namespace fwTools
{

/**
 * @brief   Namespace fwTools::os contains tools methods which depend on os like get user application data directory.
 *
 * @date    2009-2010.
 */
namespace os
{


/**
 * @brief Returns a environment variable value
 *
 * @param name environment variable 'name'
 * @param[out] ok boolean set to true if variable exists
 *
 * @return The environment variable content if it exists, else an empty string
 */
FWTOOLS_API std::string getEnv(const std::string &name, bool *ok = NULL);


/**
 * @brief Returns a environment variable value
 *
 * @param name environment variable 'name'
 * @param defaultValue Value returned if variable 'name' doesn't exist
 */
FWTOOLS_API std::string getEnv(const std::string &name, const std::string &defaultValue);

/**
 * @brief   Return the users's application data directory
 *
 * @param company The company name
 * @param appName The application name
 * @param createDirectory if true, create the returned directory if it don't exist
 * @date    2007-2009.
 *
 * Return the application data directory. If company or appName is not empty, append them to
 * the path. Under unix, XDG conventions are repected.
 * For example, the UserDataDir under linux will be "~/.config/company/appName"
 */
FWTOOLS_API std::string  getUserDataDir(
    std::string company = "",
    std::string appName = "",
    bool createDirectory = false
    );


} // namespace os

} // namespace fwTools

#endif //__FWTOOLS_OS_HPP__
