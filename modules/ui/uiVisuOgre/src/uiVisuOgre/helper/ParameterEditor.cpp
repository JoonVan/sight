/************************************************************************
 *
 * Copyright (C) 2017-2020 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
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

#include "uiVisuOgre/helper/ParameterEditor.hpp"

#include <fwData/Array.hpp>
#include <fwData/Color.hpp>
#include <fwData/Float.hpp>
#include <fwData/Integer.hpp>

namespace uiVisuOgre
{

namespace helper
{

//-----------------------------------------------------------------------------

template <typename T>
std::pair<T, T> getRange(T _value)
{
    static const T range = 10;
    T max;
    T min;

    // For _value > 1, we use [-_value * range;+_value *range]
    if(_value > 1)
    {
        max = +_value * range;
        min = -_value * range;
    }
    else if(_value < -1)
    // For _value < 0, we use [_value * range;-_value *range]
    {
        max = -_value * range;
        min = +_value * range;
    }
    else if(_value > 0 || _value < 0)
    // For -1 < value < 0 < _value < 1, we use [_value / range;+_value * range]
    {
        max = _value * range;
        min = _value / range;
    }
    else
    // For _value == 0, we use [0; 1]
    {
        max = 0;
        min = 1;
    }
    return std::make_pair(min, max);
}

//-----------------------------------------------------------------------------

fwServices::IService::ConfigType ParameterEditor::createConfig(const ::fwRenderOgre::IParameter::csptr& _adaptor,
                                                               const ::fwServices::IService::csptr& _paramSrv,
                                                               ::fwCom::helper::SigSlotConnection& _connections)
{
    ::fwServices::IService::ConfigType paramConfig;

    /// Getting associated object infos
    const ::fwData::Object::csptr shaderObj =
        _adaptor->getInOut< ::fwData::Object>(::fwRenderOgre::IParameter::s_PARAMETER_INOUT);

    const auto& objType = shaderObj->getClassname();

    if(objType == "::fwData::Boolean")
    {
        _connections.connect(_paramSrv, "boolChanged", _adaptor, "setBoolParameter");

        paramConfig.add("<xmlattr>.type", "bool");
        paramConfig.add("<xmlattr>.name", _adaptor->getParamName());
        paramConfig.add("<xmlattr>.key", _adaptor->getParamName());
        paramConfig.add("<xmlattr>.defaultValue", false);
    }
    else if(objType == "::fwData::Color")
    {
        _connections.connect(_paramSrv, "colorChanged", _adaptor, "setColorParameter");

        auto colorValue = ::fwData::Color::dynamicCast(shaderObj);

        int r = static_cast< unsigned char >(colorValue->red() * 255);
        int g = static_cast< unsigned char >(colorValue->green() * 255);
        int b = static_cast< unsigned char >(colorValue->blue() * 255);
        int a = static_cast< unsigned char >(colorValue->alpha() * 255);

        std::stringstream hexStr;
        hexStr << "#" << std::hex;
        hexStr << ((r < 0x10) ? "0" : "") << r;
        hexStr << ((g < 0x10) ? "0" : "") << g;
        hexStr << ((b < 0x10) ? "0" : "") << b;
        hexStr << ((a < 0x10) ? "0" : "") << a;

        paramConfig.add("<xmlattr>.type", "color");
        paramConfig.add("<xmlattr>.name", _adaptor->getParamName());
        paramConfig.add("<xmlattr>.key", _adaptor->getParamName());
        paramConfig.add("<xmlattr>.defaultValue", hexStr.str());
    }
    else if(objType == "::fwData::Float")
    {
        _connections.connect(_paramSrv, "doubleChanged", _adaptor, "setDoubleParameter");

        auto floatValue           = ::fwData::Float::dynamicCast(shaderObj);
        const double defaultValue = static_cast<double>(floatValue->value());
        const auto minmax         = getRange(defaultValue);
        const double min          = minmax.first;
        const double max          = minmax.second;

        paramConfig.add("<xmlattr>.type", "double");
        paramConfig.add("<xmlattr>.name", _adaptor->getParamName());
        paramConfig.add("<xmlattr>.key", _adaptor->getParamName());
        paramConfig.add("<xmlattr>.defaultValue", std::to_string(defaultValue));
        paramConfig.add("<xmlattr>.min", min);
        paramConfig.add("<xmlattr>.max", max);
    }
    else if(objType == "::fwData::Integer")
    {
        _connections.connect(_paramSrv, "intChanged", _adaptor, "setIntParameter");

        auto intValue          = ::fwData::Integer::dynamicCast(shaderObj);
        const int defaultValue = intValue->value();
        const auto minmax      = getRange(defaultValue);
        const int min          = minmax.first;
        const int max          = minmax.second;

        paramConfig.add("<xmlattr>.type", "int");
        paramConfig.add("<xmlattr>.name", _adaptor->getParamName());
        paramConfig.add("<xmlattr>.key", _adaptor->getParamName());
        paramConfig.add("<xmlattr>.defaultValue", std::to_string(defaultValue));
        paramConfig.add("<xmlattr>.min", min);
        paramConfig.add("<xmlattr>.max", max);
    }
    else if(objType == "::fwData::Array")
    {
        auto arrayObject         = ::fwData::Array::dynamicCast(shaderObj);
        const auto numComponents = arrayObject->getSize()[0];
        if(numComponents <= 3)
        {
            std::string strSize = std::to_string(numComponents);

            if( arrayObject->getType() == ::fwTools::Type::s_FLOAT ||
                arrayObject->getType() == ::fwTools::Type::s_DOUBLE)
            {
                _connections.connect(_paramSrv, "double" + strSize + "Changed",
                                     _adaptor, "setDouble" + strSize + "Parameter");

                // We can't give a default value for each component to SParameters :/
                // For now fill it with the first one
                const auto dumpLock = arrayObject->lock();

                double defaultValue;
                if(arrayObject->getType() == ::fwTools::Type::s_FLOAT)
                {
                    defaultValue = static_cast<double>(arrayObject->at< float >(0));
                }
                else
                {
                    defaultValue = arrayObject->at< double >(0);
                }
                const auto minmax = getRange(defaultValue);
                const double min  = minmax.first;
                const double max  = minmax.second;

                paramConfig.add("<xmlattr>.type", "double" + strSize);
                paramConfig.add("<xmlattr>.name", _adaptor->getParamName());
                paramConfig.add("<xmlattr>.key", _adaptor->getParamName());
                paramConfig.add("<xmlattr>.defaultValue", std::to_string(defaultValue));
                paramConfig.add("<xmlattr>.min", min);
                paramConfig.add("<xmlattr>.max", max);
            }
            else if( arrayObject->getType() == ::fwTools::Type::s_INT32)
            {
                _connections.connect(_paramSrv, "int" + strSize + "Changed",
                                     _adaptor, "setInt" + strSize + "Parameter");
                const auto dumpLock = arrayObject->lock();

                const int defaultValue = arrayObject->at< std::int32_t >(0);
                const auto minmax      = getRange(defaultValue);
                const int min          = minmax.first;
                const int max          = minmax.second;

                paramConfig.add("<xmlattr>.type", "int" + strSize);
                paramConfig.add("<xmlattr>.name", _adaptor->getParamName());
                paramConfig.add("<xmlattr>.key", _adaptor->getParamName());
                paramConfig.add("<xmlattr>.defaultValue", std::to_string(defaultValue));
                paramConfig.add("<xmlattr>.min", min);
                paramConfig.add("<xmlattr>.max", max);
            }
            else
            {
                SLM_ERROR("Array type not handled: " << arrayObject->getType());
            }
        }
        else
        {
            SLM_ERROR("Array size not handled: " << numComponents);
        }
    }
    else
    {
        SLM_ERROR("No editor found for the object of type " << objType);
    }

    return paramConfig;
}

//-----------------------------------------------------------------------------

} // namespace helper

} // namespace uiVisuOgre
