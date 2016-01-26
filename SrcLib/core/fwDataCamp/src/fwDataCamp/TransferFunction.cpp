/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/camp/mapper.hpp>

#include <fwCamp/UserObject.hpp>
#include "fwDataCamp/TransferFunction.hpp"

fwCampImplementDataMacro((fwData)(TransferFunction))
{
    builder
    .tag("object_version", "1")
    .tag("lib_name", "fwData")
    .base< ::fwData::Object>()
    .property("level", &::fwData::TransferFunction::m_level)
    .property("window", &::fwData::TransferFunction::m_window)
    .property("name", &::fwData::TransferFunction::m_name)
    .property("background_color", &::fwData::TransferFunction::m_backgroundColor)
    .property("tf_data", &::fwData::TransferFunction::m_tfData)
    .property("interpolation_mode", &::fwData::TransferFunction::m_interpolationMode)
    .property("is_clamped", &::fwData::TransferFunction::m_isClamped)
    ;
}

fwCampImplementEnumMacro((fwData)(TransferFunction)(InterpolationMode))
{
    builder
    .value("LINEAR",  ::fwData::TransferFunction::LINEAR)
    .value("NEAREST",  ::fwData::TransferFunction::NEAREST)
    ;
}
