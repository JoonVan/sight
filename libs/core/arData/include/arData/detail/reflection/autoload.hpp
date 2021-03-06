/************************************************************************
 *
 * Copyright (C) 2014-2020 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include "arData/CalibrationInfo.hpp"
#include "arData/Camera.hpp"
#include "arData/CameraSeries.hpp"
#include "arData/config.hpp"
#include "arData/detail/reflection/Camera.hpp"
#include "arData/detail/reflection/CameraSeries.hpp"

#include <fwCamp/macros.hpp>

namespace ardata
{
namespace detail
{
namespace reflection
{
struct runner
{
    runner()
    {
        localDeclarearDataCameraSourceType();
        localDeclarearDataCameraPixelFormat();
        localDeclarearDataCamera();
        localDeclarearDataCameraSeries();
        localDeclarearDataCalibrationInfo();
    }

    static runner r;
};
} // namespace reflection
} // namespace detail
} //end namespace ardata
