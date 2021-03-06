/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "fwGdcmIO/reader/tid/Fiducial.hpp"

#include "fwGdcmIO/container/sr/DicomSRImageNode.hpp"
#include "fwGdcmIO/container/sr/DicomSRSCoord3DNode.hpp"
#include "fwGdcmIO/container/sr/DicomSRSCoordNode.hpp"
#include "fwGdcmIO/container/sr/DicomSRTextNode.hpp"
#include "fwGdcmIO/helper/DicomDataTools.hpp"

#include <fwData/Boolean.hpp>
#include <fwData/PointList.hpp>
#include <fwData/String.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>

namespace fwGdcmIO
{
namespace reader
{
namespace tid
{

//------------------------------------------------------------------------------

Fiducial::Fiducial(const ::fwMedData::DicomSeries::csptr& dicomSeries,
                   const SPTR(::gdcm::Reader)& reader,
                   const ::fwGdcmIO::container::DicomInstance::sptr& instance,
                   const ::fwData::Image::sptr& image,
                   const ::fwLog::Logger::sptr& logger) :
    ::fwGdcmIO::reader::tid::TemplateID< ::fwData::Image >(dicomSeries, reader, instance, image, logger)
{
}

//------------------------------------------------------------------------------

Fiducial::~Fiducial()
{
}

//------------------------------------------------------------------------------

void Fiducial::readNode(const SPTR(::fwGdcmIO::container::sr::DicomSRNode)& node)
{
    if(node->getCodedAttribute() == ::fwGdcmIO::container::DicomCodedAttribute("122340", "DCM", "Fiducial feature") &&
       !node->getSubNodeContainer().empty())
    {
        std::string label = "";
        double x, y, z;
        bool foundLandmark = false;
        for(const SPTR(::fwGdcmIO::container::sr::DicomSRNode)& subNode : node->getSubNodeContainer())
        {
            // Read label
            if(subNode->getCodedAttribute() ==
               ::fwGdcmIO::container::DicomCodedAttribute("122369", "DCM", "Fiducial intent"))
            {
                SPTR(::fwGdcmIO::container::sr::DicomSRTextNode) intentNode =
                    std::dynamic_pointer_cast< ::fwGdcmIO::container::sr::DicomSRTextNode >(subNode);
                if(intentNode)
                {
                    label = intentNode->getTextValue();
                }
            }
            // 2D Coordinate
            else if(subNode->getType() == "SCOORD")
            {
                SPTR(::fwGdcmIO::container::sr::DicomSRSCoordNode) scoordNode =
                    std::dynamic_pointer_cast< ::fwGdcmIO::container::sr::DicomSRSCoordNode >(subNode);
                if(scoordNode && scoordNode->getGraphicType() == "POINT")
                {
                    // Retrieve coordinates
                    ::fwGdcmIO::container::sr::DicomSRSCoordNode::GraphicDataContainerType coordinates =
                        scoordNode->getGraphicDataContainer();

                    x = coordinates[0];
                    y = coordinates[1];

                    if(!scoordNode->getSubNodeContainer().empty())
                    {
                        SPTR(::fwGdcmIO::container::sr::DicomSRImageNode) imageNode =
                            std::dynamic_pointer_cast< ::fwGdcmIO::container::sr::DicomSRImageNode >(
                                *scoordNode->getSubNodeContainer().begin());
                        if(imageNode)
                        {
                            const int frameNumber = imageNode->getFrameNumber();
                            z = ::fwGdcmIO::helper::DicomDataTools::convertFrameNumberToZCoordinate(m_object,
                                                                                                    frameNumber);
                            foundLandmark = true;
                        }
                    }
                }
            }
            // 3D Coordinate
            else if(subNode->getType() == "SCOORD3D")
            {
                SPTR(::fwGdcmIO::container::sr::DicomSRSCoord3DNode) scoord3DNode =
                    std::dynamic_pointer_cast< ::fwGdcmIO::container::sr::DicomSRSCoord3DNode >(subNode);
                if(scoord3DNode && scoord3DNode->getGraphicType() == "POINT")
                {
                    // Retrieve coordinates
                    ::fwGdcmIO::container::sr::DicomSRSCoordNode::GraphicDataContainerType coordinates =
                        scoord3DNode->getGraphicDataContainer();
                    x             = coordinates[0];
                    y             = coordinates[1];
                    z             = coordinates[2];
                    foundLandmark = true;
                }
            }

        }

        if(foundLandmark)
        {
            this->addLandmark(x, y, z, label);
        }

    }
}

//------------------------------------------------------------------------------

void Fiducial::addLandmark(double x, double y, double z, const std::string& label)
{
    ::fwData::Point::sptr point = ::fwData::Point::New(x, y, z);
    point->setField(::fwDataTools::fieldHelper::Image::m_labelId, ::fwData::String::New(label));

    ::fwData::PointList::sptr pointList =
        m_object->getField< ::fwData::PointList >(::fwDataTools::fieldHelper::Image::m_imageLandmarksId);

    if(!pointList)
    {
        pointList = ::fwData::PointList::New();
        m_object->setField(::fwDataTools::fieldHelper::Image::m_imageLandmarksId, pointList);
    }

    pointList->getPoints().push_back(point);
    m_object->setField("ShowLandmarks", ::fwData::Boolean::New(true));
}

//------------------------------------------------------------------------------

} // namespace tid
} // namespace reader
} // namespace fwGdcmIO
