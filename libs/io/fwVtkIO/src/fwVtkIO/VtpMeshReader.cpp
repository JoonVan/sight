/************************************************************************
 *
 * Copyright (C) 2020 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include "fwVtkIO/VtpMeshReader.hpp"

#include "fwVtkIO/helper/Mesh.hpp"
#include "fwVtkIO/helper/vtkLambdaCommand.hpp"

#include <fwCore/base.hpp>

#include <fwDataIO/reader/registry/macros.hpp>

#include <fwJobs/IJob.hpp>
#include <fwJobs/Observer.hpp>

#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkXMLGenericDataObjectReader.h>

fwDataIOReaderRegisterMacro( ::fwVtkIO::VtpMeshReader );

namespace fwVtkIO
{
//------------------------------------------------------------------------------

VtpMeshReader::VtpMeshReader(::fwDataIO::reader::IObjectReader::Key) :
    ::fwData::location::enableSingleFile< ::fwDataIO::reader::IObjectReader >(this),
    m_job(::fwJobs::Observer::New("VTP Mesh reader"))
{
}

//------------------------------------------------------------------------------

VtpMeshReader::~VtpMeshReader()
{
}

//------------------------------------------------------------------------------

void VtpMeshReader::read()
{
    SLM_ASSERT("Object pointer expired", !m_object.expired());

    [[maybe_unused]] const auto objectLock = m_object.lock();

    SLM_ASSERT("Object Lock null.", objectLock );

    const ::fwData::Mesh::sptr pMesh = getConcreteObject();

    using namespace fwVtkIO::helper;

    vtkSmartPointer< vtkXMLGenericDataObjectReader > reader = vtkSmartPointer< vtkXMLGenericDataObjectReader >::New();
    reader->SetFileName(this->getFile().string().c_str());

    vtkSmartPointer< vtkLambdaCommand > progressCallback;

    progressCallback = vtkSmartPointer< vtkLambdaCommand >::New();
    progressCallback->SetCallback(
        [&](vtkObject* caller, long unsigned int, void*)
        {
            const auto filter = static_cast< vtkXMLGenericDataObjectReader* >(caller);
            m_job->doneWork(static_cast<std::uint64_t>(filter->GetProgress() * 100.));
        }
        );
    reader->AddObserver(vtkCommand::ProgressEvent, progressCallback);

    m_job->addSimpleCancelHook([&] { reader->AbortExecuteOn(); });

    reader->Update();

    vtkDataObject* obj = reader->GetOutput();
    vtkPolyData* mesh  = vtkPolyData::SafeDownCast(obj);
    FW_RAISE_IF("VtpMeshReader cannot read VTK Mesh file : "<< this->getFile().string(), !mesh);
    ::fwVtkIO::helper::Mesh::fromVTKMesh(mesh, pMesh);

    m_job->finish();
}

//------------------------------------------------------------------------------

std::string VtpMeshReader::extension()
{
    return ".vtp";
}

//------------------------------------------------------------------------------

::fwJobs::IJob::sptr VtpMeshReader::getJob() const
{
    return m_job;
}

//------------------------------------------------------------------------------

} // namespace fwVtkIO
