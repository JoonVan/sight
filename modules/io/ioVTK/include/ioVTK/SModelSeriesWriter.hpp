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

#pragma once

#include "ioVTK/config.hpp"

#include <fwCom/Signal.hpp>

#include <fwData/Mesh.hpp>

#include <fwIO/IWriter.hpp>

#include <filesystem>
#include <string>

namespace fwData
{
class Mesh;
}

namespace fwJobs
{
class IJob;
}

namespace ioVTK
{

/**
 * @brief   VTK Model series writer.
 *
 * Service writing a model series as .vtk files using the fwVtkIO lib.
 *
 * @section Signals Signals
 * - \b jobCreated(SPTR(::fwJobs::IJob)): emitted to display a progress bar while the image is written (it should be
 * connected to a SJobBar).
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::ioVTK::SModelSeriesWriter">
       <in key="data" uid="..." />
       <folder>...</folder>
       <extension>vtk</extension>
   </service>
   @endcode
 * @subsection Input Input
 * - \b data [::fwMedData::ModelSeries]: model to save.
 * @subsection Configuration Configuration
 * - \b folder (optional): path of the folder, if it is not defined, 'openLocationDialog()' should be called to define
 * the path.
 * - \b extension (optional): extension to use when writing files (vtk, vtp, obj, stl, ply). If nothing is set
 * a popup will ask to user to choose one.
 * Accepted extensions are: "vtk", "vtp", "obj" "stl" "ply"
 * Extensions aren't case sensitive but make sure there isn't a dot "." before extension name.
 */
class IOVTK_CLASS_API SModelSeriesWriter : public ::fwIO::IWriter
{

public:

    SModelSeriesWriter() noexcept;

    ~SModelSeriesWriter() noexcept
    {
    }

    fwCoreServiceMacro(SModelSeriesWriter,  ::fwIO::IWriter)

    typedef ::fwCom::Signal< void ( SPTR(::fwJobs::IJob) ) > JobCreatedSignalType;

    /**
     * @brief Configure the mesh path.
     *
     * This method is used to find the file path  using a file selector.
     * @deprecated Will be removed in sight 22.0. Use openLocationDialog() instead.
     */
    [[deprecated("Will be removed in sight 22.0. Use openLocationDialog() instead.")]]
    IOVTK_API virtual void configureWithIHM() override;

    /**
     * @brief Configure the mesh path.
     *
     * This method is used to find the file path  using a file selector.
     */
    IOVTK_API virtual void openLocationDialog() override;

protected:

    IOVTK_API virtual ::fwIO::IOPathType getIOPathType() const override;

    /**
     * @brief Starting method.
     *
     * This method is used to initialize the service.
     */
    IOVTK_API virtual void starting() override;

    /**
     * @brief Stopping method.
     *
     * The stopping method is empty for this service.
     */
    IOVTK_API virtual void stopping() override;

    /**
     * @brief Configuring method.
     *
     * The configuring method only calls the configuring method from the base class
     */
    IOVTK_API virtual void configuring() override;

    /**
     * @brief Updating method.
     *
     * This method is used to update the service.
     * The mesh is read.
     */
    IOVTK_API void updating() override;

    /**
     * @brief Info method.
     *
     * This method is used to give
     * informations about the service.
     */
    IOVTK_API void info(std::ostream& _sstream ) override;

private:

    /**
     * @brief write the mesh on disk, using _filename
     * @param[in] _filename : name of the file to write with extension. extension will be used to choose the writer.
     * @param[in] _mesh : const pointer of the data mesh.
     */
    void writeMesh(const std::filesystem::path& _filename, const ::fwData::Mesh::csptr _mesh);

    /// Job Created signal
    SPTR(JobCreatedSignalType) m_sigJobCreated;

    ///  Stores the selectedExtension from dialog.
    std::string m_selectedExtension;

};

} // namespace ioVTK
