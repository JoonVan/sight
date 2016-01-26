/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwVtkIO/vtk.hpp"
#include "fwVtkIO/VtiImageWriter.hpp"
#include "fwVtkIO/helper/vtkLambdaCommand.hpp"

#include <fwCore/base.hpp>

#include <fwDataIO/writer/registry/macros.hpp>

#include <fwJobs/IJob.hpp>
#include <fwJobs/Observer.hpp>

#include <vtkImageData.h>
#include <vtkXMLImageDataWriter.h>
#include <vtkZLibDataCompressor.h>
#include <vtkSmartPointer.h>


fwDataIOWriterRegisterMacro( ::fwVtkIO::VtiImageWriter );


namespace fwVtkIO
{
//------------------------------------------------------------------------------

VtiImageWriter::VtiImageWriter(::fwDataIO::writer::IObjectWriter::Key key) :
    ::fwData::location::enableSingleFile< ::fwDataIO::writer::IObjectWriter >(this),
    m_job(::fwJobs::Observer::New("VTK Image Writer"))
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

VtiImageWriter::~VtiImageWriter()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void VtiImageWriter::write()
{
    using namespace fwVtkIO::helper;

    assert( !m_object.expired() );
    assert( m_object.lock() );

    ::fwData::Image::sptr pImage = getConcreteObject();

    vtkSmartPointer< vtkXMLImageDataWriter > writer = vtkSmartPointer< vtkXMLImageDataWriter >::New();
    vtkSmartPointer< vtkImageData > vtkImage        = vtkSmartPointer< vtkImageData >::New();
    ::fwVtkIO::toVTKImage( pImage, vtkImage );
    writer->SetInputData( vtkImage );
    writer->SetFileName( this->getFile().string().c_str() );
    writer->SetDataModeToAppended();

    vtkSmartPointer< vtkZLibDataCompressor > compressor = vtkSmartPointer< vtkZLibDataCompressor >::New();
    compressor->SetCompressionLevel(1);
    writer->SetCompressor( compressor );
    writer->EncodeAppendedDataOff();

    vtkSmartPointer<vtkLambdaCommand> progressCallback;
    progressCallback = vtkSmartPointer<vtkLambdaCommand>::New();
    progressCallback->SetCallback([this](vtkObject* caller, long unsigned int, void* )
        {
            auto filter = static_cast<vtkXMLImageDataWriter*>(caller);
            m_job->doneWork( filter->GetProgress()*100 );
        });

    writer->AddObserver(vtkCommand::ProgressEvent, progressCallback);
    m_job->addSimpleCancelHook( [&]()
        {
            writer->AbortExecuteOn();
        });
    writer->Write();
    m_job->finish();
}

//------------------------------------------------------------------------------

std::string VtiImageWriter::extension()
{
    return ".vti";
}

//------------------------------------------------------------------------------

::fwJobs::IJob::sptr VtiImageWriter::getJob() const
{
    return m_job;
}

} // namespace fwVtkIO
