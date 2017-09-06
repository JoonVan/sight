/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#ifndef __BINDING_PLUGIN_HPP__
#define __BINDING_PLUGIN_HPP__


#include <fwRuntime/Plugin.hpp>

#include "binding/config.hpp"

namespace binding
{
/**
 * @brief   This class is started when the bundles is loaded.
 */

class BINDING_CLASS_API Plugin : public ::fwRuntime::Plugin
{
public:
    /**
     * @brief   destructor
     */
    BINDING_API ~Plugin() noexcept;

    // Overrides
    BINDING_API void start();

    // Overrides
    BINDING_API void stop() noexcept;

};


} // namespace binding


#endif //__BINDING_PLUGIN_HPP__
