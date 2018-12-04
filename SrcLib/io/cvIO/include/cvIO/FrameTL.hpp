/************************************************************************
 *
 * Copyright (C) 2017 IRCAD France
 * Copyright (C) 2017 IHU Strasbourg
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

#ifndef __CVIO_FRAMETL_HPP__
#define __CVIO_FRAMETL_HPP__

#include "cvIO/config.hpp"

#include <arData/FrameTL.hpp>

#include <opencv2/core.hpp>

namespace cvIO
{
/**
 * @brief Helper to manage conversion from/to OpenCV to/from ::arData::FrameTL buffer
 *
 * @note There is no method to move a OpenCV image into a ::arData::FrameTL buffer. This is because it is tough and not
 * really recommended to detach a buffer allocated by a ::cv::Mat, cause we may not free it in the same way than OpenCV.
 * Instead you can allocate a ::arData::FrameTL buffer of the correct size/type, and then use moveToCv() to wrap the
 * image with a ::cv::Mat. This way every modification made to the buffer will be applied to the ::arData::FrameTL
 * buffer, and you don't need an extra stage to get back your modified image.
 */
class CVIO_CLASS_API FrameTL
{

public:

    /**
     * @brief Convert a timeline buffer into an OpenCV image.
     *
     * The resulting ::cv::Mat does not allocate a new buffer and points to the ::arData::FrameTL::BufferType.
     * That's why the source buffer is not const, because the _cvImage may alter the existing buffer.
     */
    CVIO_API static void moveToCv(const ::arData::FrameTL::csptr& _timeline,
                                  ::arData::FrameTL::BufferType::ElementType* _buffer,
                                  ::cv::Mat& _cvImage);

    /**
     * @brief Convert a timeline buffer into an OpenCV image.
     *
     * The resulting ::cv::Mat does not allocate a new buffer and points to the ::arData::FrameTL::BufferType.
     * The returned cvMat is const because the input buffer is const as well.
     */
    CVIO_API static const ::cv::Mat moveToCv(const ::arData::FrameTL::csptr& _timeline,
                                             const ::arData::FrameTL::BufferType::ElementType* _buffer);

    /**
     * @brief Copy a OpenCV image into our internal image data.
     */
    CVIO_API static void copyFromCv(const ::arData::FrameTL::csptr& _timeline,
                                    ::arData::FrameTL::BufferType::ElementType* _buffer,
                                    const ::cv::Mat& _cvImage);

    /**
     * @brief Copy an image data into an OpenCV image.
     *
     * @note The cvImage allocates its own buffer and has the ownership.
     */
    CVIO_API static void copyToCv(const ::arData::FrameTL::csptr& _timeline,
                                  const ::arData::FrameTL::BufferType::ElementType* _buffer,
                                  ::cv::Mat& _cvImage);

};

}// namespace cvIO

#endif //__CVIO_FRAMETL_HPP__
