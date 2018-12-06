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

#include "fwGdcmIO/writer/ie/FrameOfReference.hpp"

#include "fwGdcmIO/helper/DicomDataWriter.hxx"

#include <fwMedData/Series.hpp>

namespace fwGdcmIO
{
namespace writer
{
namespace ie
{

//------------------------------------------------------------------------------

FrameOfReference::FrameOfReference(const SPTR(::gdcm::Writer)& writer,
                                   const SPTR(::fwGdcmIO::container::DicomInstance)& instance,
                                   const ::fwMedData::Series::csptr& series,
                                   const ::fwLog::Logger::sptr& logger,
                                   ProgressCallback progress,
                                   CancelRequestedCallback cancel) :
    ::fwGdcmIO::writer::ie::InformationEntity< ::fwMedData::Series >(writer, instance, series,
                                                                     logger, progress, cancel)
{
}

//------------------------------------------------------------------------------

FrameOfReference::~FrameOfReference()
{
}

//------------------------------------------------------------------------------

void FrameOfReference::writeFrameOfReferenceModule()
{
    // Retrieve dataset
    ::gdcm::DataSet& dataset = m_writer->GetFile().GetDataSet();

    // Frame of Reference UID
    const std::string frameOfReferenceUID = m_instance->getFrameOfReferenceUID();
    ::fwGdcmIO::helper::DicomDataWriter::setTagValue< 0x0020, 0x0052 >(frameOfReferenceUID, dataset);

    // Position Reference Indicator - Type 2
    ::fwGdcmIO::helper::DicomDataWriter::setEmptyTagValue< 0x0020, 0x1040 >(dataset);

}

//------------------------------------------------------------------------------

} // namespace ie
} // namespace writer
} // namespace fwGdcmIO
