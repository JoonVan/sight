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

#include "registrationCV/config.hpp"

#include <fwServices/IRegisterer.hpp>

#include <opencv2/core.hpp>

#include <array>

namespace registrationCV
{
/**
 * @brief This service estimates the object pose given a set of object points, their corresponding image projections,
 *  as well as the camera matrix and the distortion coefficients.
 *
 * @section Slots Slots
 * - \b computeRegistration(::fwCore::HiResClock::HiResClockType): compute the registration using ::cv::solvePnP.
 *
 * @section XML XML Configuration
 * @code{.xml}
        <service type="::registrationCV::SSolvePnP">
            <in key="pointList2d" uid="..." />
            <in key="pointList3d" uid="..." />
            <in key="calibration" uid="..." />
            <inout key="matrix" uid="..." />
            <config videoRef="center" inverse="false"/>
       </service>
   @endcode
 * @subsection Input Input:
 * - \b pointList2d [::fwData::PointList]: list of 2d points clicked on image
 * - \b pointList3d [::fwData::PointList]: list of corresponding 3d points clicked on 3d model
 * - \b calibration [::arData::Camera]: camera parameters (intrinsic & distorison coefficients)
 * @subsection In-Out In-Out:
 * - \b matrix [::fwData::TransformationMatrix3D]: output registration matrix
 * (updated when calling 'computeRegistration' slot), in-out is used since matrix is not created in this service.
 * @subsection Configuration Configuration:
 *      Values are "top_left" or "center" (default: "top_left")
 * - \b inverse (optionnal): reverse output matrix. If 'inverse' is "true" then the camera pose is computed,
 *      object pose is computed otherwise.(default: "false").
 */
class REGISTRATIONCV_CLASS_API SSolvePnP : public ::fwServices::IRegisterer
{
public:

    fwCoreServiceMacro(SSolvePnP, ::fwServices::IRegisterer)

    /// Constructor.
    REGISTRATIONCV_API SSolvePnP() noexcept;

    /// Destructor.
    REGISTRATIONCV_API virtual ~SSolvePnP() noexcept
    {
    }

    /**
     * @brief computeRegistration: compute the camera pose from 'pointList2d' and corresponding 'pointList3d'.
     * Update 'matrix' with the camera pose (or object pose if 'inverse' is "true")
     * @param _timestamp: not used by the method.
     */
    REGISTRATIONCV_API virtual void computeRegistration(::fwCore::HiResClock::HiResClockType _timestamp) override;

protected:
    /// configures the service
    REGISTRATIONCV_API virtual void configuring() override;
    /// starts the service and call initialize()
    REGISTRATIONCV_API virtual void starting() override;
    /// does nothing
    REGISTRATIONCV_API virtual void stopping() override;
    /// calls computeRegistration with fake timestamp.
    REGISTRATIONCV_API virtual void updating() override;

private:

    /**
     * @brief Camera structure: handles intrinsic parameters and distorsion coefficients in OpenCV format.
     */
    struct Camera
    {
        /// Size of the calibration images (width, height)
        ::cv::Size imageSize;
        /// Intrisic parameters 3x3 matrix:
        /**  | fx, 0,  cx |
         *   | 0,  fy, cy |
         *   | 0,  0,   1 |
         **/
        ::cv::Mat intrinsicMat;
        /// Distorsion coefficients 5x1 matrix: | k1, k2, p1, p2, k3 |
        ::cv::Mat distCoef;
    };

    /// reverse or not output matrix (camera pose vs object pose)
    bool m_reverseMatrix = {false};

};

} // namespace registrationCV
