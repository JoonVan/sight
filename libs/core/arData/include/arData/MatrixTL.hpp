/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "arData/GenericTL.hpp"
#include "arData/GenericTL.hxx"
#include "arData/timeline/GenericObject.hpp"
#include "arData/timeline/GenericObject.hxx"

namespace arData
{

/**
 * @brief   This class defines a timeline that stores groups of matrices.
 */
class ARDATA_CLASS_API MatrixTL : public GenericTL< float[16] >
{
public:
    fwCoreClassMacro(MatrixTL, ::arData::TimeLine, ::fwData::factory::New< MatrixTL >);

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    MatrixTL( ::fwData::Object::Key key ) :
        GenericTL< float[16] >(key)
    {
    }
};

} // namespace arData
