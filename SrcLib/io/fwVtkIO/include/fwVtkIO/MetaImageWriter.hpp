/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWVTKIO_METAIMAGEWRITER_HPP__
#define __FWVTKIO_METAIMAGEWRITER_HPP__

#include <boost/filesystem/path.hpp>

#include <fwDataIO/writer/GenericObjectWriter.hpp>
#include <fwData/location/SingleFile.hpp>
#include <fwData/Image.hpp>

#include "fwVtkIO/config.hpp"

namespace fwJobs
{
class Observer;
}

namespace fwVtkIO
{

/**
 * @brief   Write a MetaImage.
 * @class   MetaImageWriter
 *
 * Write a MetaImage using the VTK lib
 */
class MetaImageWriter : public ::fwDataIO::writer::GenericObjectWriter< ::fwData::Image >,
                        public ::fwData::location::enableSingleFile< ::fwDataIO::writer::IObjectWriter >
{

public:

    fwCoreClassDefinitionsWithFactoryMacro(
        (MetaImageWriter)( ::fwDataIO::writer::GenericObjectWriter< ::fwData::Image >),
        (()),
        ::fwDataIO::writer::factory::New< MetaImageWriter >
        );
    fwCoreAllowSharedFromThis();

    //! @brief Constructor.
    FWVTKIO_API MetaImageWriter(::fwDataIO::writer::IObjectWriter::Key key);

    //! @brief Destructor.
    FWVTKIO_API ~MetaImageWriter();

    //! @brief Writing operator.
    FWVTKIO_API void write();

    /// @return ".mhd"
    FWVTKIO_API std::string extension();

    /// @return internal job
    FWVTKIO_API SPTR(::fwJobs::IJob) getJob() const;

private:

    ///Internal job
    SPTR(::fwJobs::Observer) m_job;

};

} // namespace fwVtkIO

#endif // __FWVTKIO_METAIMAGEWRITER_HPP__
