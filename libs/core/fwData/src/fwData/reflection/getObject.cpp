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

#include "fwData/reflection/getObject.hpp"

#include "fwData/Object.hpp"
#include "fwData/reflection/exception/NullPointer.hpp"
#include "fwData/reflection/exception/ObjectNotFound.hpp"
#include "fwData/reflection/visitor/GetObject.hpp"

#include <fwCore/base.hpp>

namespace fwData
{
namespace reflection
{
//------------------------------------------------------------------------------

::fwData::Object::sptr getObject( ::fwData::Object::csptr object,
                                  const std::string& path,
                                  bool raiseException )
{
    SLM_FATAL_IF( "Path for desired object is empty.", path.empty() );
    SLM_FATAL_IF( "Path for desired object doesn't start with '@'.", path[0] != '@' );
    const std::string objectPath = path.substr( 1 );
    ::fwData::reflection::visitor::GetObject visitor( object, objectPath );
    ::fwData::Object::sptr subObject;
    try
    {
        subObject = visitor.get();
    }
    catch(const ::fwData::reflection::exception::NullPointer& np)
    {
        FW_FORWARD_EXCEPTION_IF(np, raiseException);
    }

    FW_RAISE_EXCEPTION_IF(
        ::fwData::reflection::exception::ObjectNotFound("Object '" + path + "' not found."),
        !visitor.objectsFound() && raiseException);

    return subObject;
}

} // namespace reflection
} // namespace fwData
