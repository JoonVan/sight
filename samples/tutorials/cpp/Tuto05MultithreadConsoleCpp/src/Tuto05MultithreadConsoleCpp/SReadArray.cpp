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

#include "Tuto05MultithreadConsoleCpp/SReadArray.hpp"

#include <fwCom/Signal.hxx>

#include <fwData/Array.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwServices/macros.hpp>

fwServicesRegisterMacro( ::fwServices::IController, ::Tuto05MultithreadConsoleCpp::SReadArray, ::fwData::Array )

namespace Tuto05MultithreadConsoleCpp
{

static const std::string s_ARRAY_INOUT = "array";

//------------------------------------------------------------------------------

SReadArray::SReadArray() noexcept
{
}

//------------------------------------------------------------------------------

SReadArray::~SReadArray() noexcept
{
}

//------------------------------------------------------------------------------

void SReadArray::configuring()
{

}

//------------------------------------------------------------------------------

void SReadArray::starting()
{
}

//------------------------------------------------------------------------------

void SReadArray::updating()
{
    const auto array = this->getInOut< ::fwData::Array >(s_ARRAY_INOUT);

    // Initialize the array size and type.
    const int arraySize = 10;
    ::fwData::Array::SizeType size(1, arraySize);
    array->resize(size, ::fwTools::Type::s_UINT32);

    // Fill the array values.
    const auto dumpLock = array->lock();

    auto itr = array->begin< unsigned int >();

    for(unsigned int i = 0; i < arraySize; i++, ++itr)
    {
        *itr = i;
    }

    // Notify that the array is modified.
    const auto sig = array->signal< ::fwData::Object::ModifiedSignalType>( ::fwData::Object::s_MODIFIED_SIG );
    sig->asyncEmit();
}

//------------------------------------------------------------------------------

void SReadArray::stopping()
{
}

//------------------------------------------------------------------------------

} // namespace Tuto05MultithreadConsoleCpp.
