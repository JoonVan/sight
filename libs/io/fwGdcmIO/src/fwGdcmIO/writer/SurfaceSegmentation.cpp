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

#include "fwGdcmIO/writer/SurfaceSegmentation.hpp"

#include "fwGdcmIO/writer/iod/SurfaceSegmentationIOD.hpp"

#include <fwData/Image.hpp>
#include <fwData/PointList.hpp>
#include <fwData/Vector.hpp>

#include <fwDataIO/writer/registry/macros.hpp>

#include <fwJobs/Aggregator.hpp>
#include <fwJobs/Job.hpp>
#include <fwJobs/Observer.hpp>

#include <fwMedData/DicomSeries.hpp>
#include <fwMedData/ImageSeries.hpp>
#include <fwMedData/ModelSeries.hpp>
#include <fwMedData/Patient.hpp>
#include <fwMedData/Series.hpp>
#include <fwMedData/Study.hpp>

fwDataIOWriterRegisterMacro(::fwGdcmIO::writer::SurfaceSegmentation);

namespace fwGdcmIO
{
namespace writer
{

//------------------------------------------------------------------------------

SurfaceSegmentation::SurfaceSegmentation(::fwDataIO::writer::IObjectWriter::Key key) :
    ::fwData::location::enableSingleFile< ::fwDataIO::writer::IObjectWriter >(this),
    m_logger(::fwLog::Logger::New()),
    m_writerJob(::fwJobs::Observer::New("Writing DICOM file"))
{
}

//------------------------------------------------------------------------------

SurfaceSegmentation::~SurfaceSegmentation()
{
}

//------------------------------------------------------------------------------

void SurfaceSegmentation::write()
{
    const ::fwMedData::ModelSeries::csptr srcModelSeries        = this->getConcreteObject();
    const ::fwMedData::DicomSeries::csptr associatedDicomSeries = srcModelSeries->getDicomReference();

    SLM_ASSERT("::fwMedData::ModelSeries not instanced", srcModelSeries);

    if(!associatedDicomSeries)
    {
        m_logger->critical("Unable to retrieve information from the associated image series.");
        m_writerJob->done();
        m_writerJob->finish();
        return;
    }

    // Verify matching Patient's names
    const std::string& modelPatientName = srcModelSeries->getPatient()->getName();
    const std::string& imagePatientName = associatedDicomSeries->getPatient()->getName();
    if(modelPatientName != imagePatientName)
    {
        m_logger->warning("The patient's name of the model (\"" + modelPatientName + "\") "
                          "does not match the patient's name of the image (\"" + imagePatientName + "\").");
    }

    // Verify matching Patient ID
    const std::string& modelPatientID = srcModelSeries->getPatient()->getPatientId();
    const std::string& imagePatientID = associatedDicomSeries->getPatient()->getPatientId();
    if(modelPatientID != imagePatientID)
    {
        m_logger->warning("The patient ID of the model (\"" + modelPatientID + "\") "
                          "does not match the patient ID of the image (\"" + imagePatientID + "\").");
    }

    // Verify matching Study Instance UID
    const std::string& modelStudyInstanceUID = srcModelSeries->getStudy()->getInstanceUID();
    const std::string& imageStudyInstanceUID = associatedDicomSeries->getStudy()->getInstanceUID();
    if(modelStudyInstanceUID != imageStudyInstanceUID)
    {
        m_logger->warning("The study instance UID of the model (\"" + modelStudyInstanceUID + "\") "
                          "does not match the study instance UID of the image (\"" + imageStudyInstanceUID + "\").");
    }

    // Complete Model Series with information from associated Image Series
    const ::fwMedData::ModelSeries::sptr modelSeries = ::fwMedData::ModelSeries::New();
    modelSeries->shallowCopy(srcModelSeries);
    modelSeries->setPatient(associatedDicomSeries->getPatient());
    modelSeries->setStudy(associatedDicomSeries->getStudy());

    SPTR(::fwGdcmIO::container::DicomInstance) associatedDicomInstance =
        std::make_shared< ::fwGdcmIO::container::DicomInstance >(associatedDicomSeries, m_logger);

    SPTR(::fwGdcmIO::container::DicomInstance) modelInstance =
        std::make_shared< ::fwGdcmIO::container::DicomInstance >(modelSeries, m_logger, false);

    m_writerJob->doneWork(0);
    m_writerJob->setTotalWorkUnits(modelSeries->getReconstructionDB().size());

    ::fwGdcmIO::writer::iod::SurfaceSegmentationIOD iod(modelInstance,
                                                        associatedDicomInstance,
                                                        this->getFile(),
                                                        m_logger,
                                                        m_writerJob->progressCallback(),
                                                        m_writerJob->cancelRequestedCallback());
    try
    {
        iod.write(modelSeries);
    }
    catch (const ::fwGdcmIO::exception::Failed& e)
    {
        m_logger->critical(e.what());
    }

    m_writerJob->done();
    m_writerJob->finish();
}

//------------------------------------------------------------------------------

std::string SurfaceSegmentation::extension()
{
    return std::string("");
}

//------------------------------------------------------------------------------

SPTR(::fwJobs::IJob) SurfaceSegmentation::getJob() const
{
    return m_writerJob;
}

//------------------------------------------------------------------------------

SPTR(::fwLog::Logger) SurfaceSegmentation::getLogger() const
{
    return m_logger;
}

//------------------------------------------------------------------------------

} // namespace writer
} // namespace fwGdcmIO
