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

#include "SeriesDBReaderTest.hpp"

#include <fwGdcmIO/reader/SeriesDB.hpp>

#include <fwData/Color.hpp>
#include <fwData/Image.hpp>
#include <fwData/Material.hpp>
#include <fwData/Mesh.hpp>
#include <fwData/PointList.hpp>
#include <fwData/Reconstruction.hpp>
#include <fwData/String.hpp>
#include <fwData/Vector.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>

#include <fwLog/Logger.hpp>

#include <fwMedData/DicomSeries.hpp>
#include <fwMedData/Equipment.hpp>
#include <fwMedData/ImageSeries.hpp>
#include <fwMedData/ModelSeries.hpp>
#include <fwMedData/Patient.hpp>
#include <fwMedData/SeriesDB.hpp>
#include <fwMedData/Study.hpp>

#include <fwMemory/BufferManager.hpp>

#include <fwTest/Data.hpp>
#include <fwTest/DicomReaderTest.hpp>
#include <fwTest/Slow.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/compute/detail/sha1.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/exceptions.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include <cppunit/extensions/HelperMacros.h>

CPPUNIT_TEST_SUITE_REGISTRATION( ::fwGdcmIO::ut::SeriesDBReaderTest );

namespace fwGdcmIO
{
namespace ut
{

//------------------------------------------------------------------------------

std::string getValue(const ::boost::property_tree::ptree& node, const std::string& name,
                     const std::filesystem::path& filePath )
{
    std::string value;
    try
    {
        value = node.get< std::string >(name);

        // Remove leading and trailing spaces
        size_t first = value.find_first_not_of(" ");
        if (first != std::string::npos)
        {
            size_t last = value.find_last_not_of(" ");
            value = value.substr(first, last - first + 1);
        }
    }
    catch( ::boost::property_tree::ptree_bad_path& )
    {
        SLM_WARN(name + " information are missing in '"+ filePath.string() +"'.");
        value = "";
    }

    return value;
}

//------------------------------------------------------------------------------

/**
 * @brief Verify tag values according to JSON files generated from DICOM dump
 * @param filename DICOM folder name
 * @param seriesDB SeriesDB object containing DICOM series
 */
void verifyTagValues(const std::string& filename, const ::fwMedData::SeriesDB::sptr& seriesDB)
{
    const double delta                    = 0.001;
    const std::filesystem::path dicomPath = ::fwTest::Data::dir() / "sight/Patient/Dicom/DicomDB";
    const std::filesystem::path metaPath  = dicomPath / "META";

    for(::fwMedData::SeriesDB::iterator sIt = seriesDB->begin(); sIt != seriesDB->end(); ++sIt)
    {
        ::fwMedData::ImageSeries::sptr series = ::fwMedData::ImageSeries::dynamicCast(*sIt);

        // Parse META File
        const std::string metaName           = filename + "/" + series->getInstanceUID() + ".json";
        const std::filesystem::path metaFile = metaPath / metaName;
        const std::string mf                 = metaFile.string();
        ::boost::property_tree::ptree root;
        ::boost::property_tree::json_parser::read_json(mf, root);

        // Series
        CPPUNIT_ASSERT_EQUAL(getValue(root, "SeriesInstanceUID", mf), series->getInstanceUID());
        CPPUNIT_ASSERT_EQUAL(getValue(root, "Modality", mf), series->getModality());
        CPPUNIT_ASSERT_EQUAL(getValue(root, "SeriesDate", mf), series->getDate());
        CPPUNIT_ASSERT_EQUAL(getValue(root, "SeriesTime", mf), series->getTime());
        CPPUNIT_ASSERT_EQUAL(getValue(root, "SeriesDescription", mf), series->getDescription());

        ::fwMedData::DicomValuesType performingPhysiciansName = series->getPerformingPhysiciansName();
        std::string performingPhysiciansNameStr = "";
        for(::fwMedData::DicomValuesType::iterator i = performingPhysiciansName.begin();
            i != performingPhysiciansName.end(); ++i)
        {
            performingPhysiciansNameStr += *i;
            if((i+1) != performingPhysiciansName.end())
            {
                performingPhysiciansNameStr += "\\";
            }
        }
        CPPUNIT_ASSERT_EQUAL(getValue(root, "PerformingPhysiciansName", mf), performingPhysiciansNameStr);

        // Patient
        ::fwMedData::Patient::sptr patient = series->getPatient();
        CPPUNIT_ASSERT_EQUAL(getValue(root, "PatientID", mf), patient->getPatientId());
        CPPUNIT_ASSERT_EQUAL(getValue(root, "PatientName", mf), patient->getName());
        CPPUNIT_ASSERT_EQUAL(getValue(root, "PatientBirthDate", mf), patient->getBirthdate());
        CPPUNIT_ASSERT_EQUAL(getValue(root, "PatientSex", mf), patient->getSex());

        // Study
        ::fwMedData::Study::sptr study = series->getStudy();
        CPPUNIT_ASSERT_EQUAL(getValue(root, "StudyInstanceUID", mf), study->getInstanceUID());
        CPPUNIT_ASSERT_EQUAL(getValue(root, "StudyDate", mf), study->getDate());
        CPPUNIT_ASSERT_EQUAL(getValue(root, "StudyTime", mf), study->getTime());
        CPPUNIT_ASSERT_EQUAL(getValue(root, "ReferringPhysicianName", mf), study->getReferringPhysicianName());
        CPPUNIT_ASSERT_EQUAL(getValue(root, "StudyDescription", mf), study->getDescription());
        CPPUNIT_ASSERT_EQUAL(getValue(root, "PatientAge", mf), study->getPatientAge());

        // Equipment
        ::fwMedData::Equipment::sptr equipment = series->getEquipment();
        CPPUNIT_ASSERT_EQUAL(getValue(root, "InstitutionName", mf), equipment->getInstitutionName());

        // Image
        ::fwData::Image::sptr image = series->getImage();

        // PixelSpacing - Not checked as the image could be rotated

        // SliceThickness - This value is recomputed using the SliceThicknessModifier filter.
//        std::string spacingZ = getValue(root, "SliceThickness", mf);
//        CPPUNIT_ASSERT_DOUBLES_EQUAL(::boost::lexical_cast< double >(spacingZ), image->getSpacing()[2], 0.0001);

        // Origin - Not checked as the image could be rotated

        // Size - Not checked as the image could be rotated

        // Window Center
        const std::string windowCenter = getValue(root, "WindowCenter", mf);
        if(!windowCenter.empty())
        {
            std::vector< std::string > windowCenterValues;
            ::boost::split(windowCenterValues, windowCenter, boost::is_any_of("\\"));
            CPPUNIT_ASSERT_DOUBLES_EQUAL(::boost::lexical_cast< double >(windowCenterValues[0]),
                                         image->getWindowCenter(), delta);
        }
        else
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL(0, image->getWindowCenter(), delta);
        }

        // Window Width
        const std::string windowWidth = getValue(root, "WindowWidth", mf);
        if(!windowWidth.empty())
        {
            std::vector< std::string > windowWidthValues;
            ::boost::split(windowWidthValues, windowWidth, boost::is_any_of("\\"));
            CPPUNIT_ASSERT_DOUBLES_EQUAL(::boost::lexical_cast< double >(windowWidthValues[0]),
                                         image->getWindowWidth(), delta);
        }
        else
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL(0, image->getWindowWidth(), delta);
        }

        // Number of components
        const std::string photometricInterpretation = getValue(root, "PhotometricInterpretation", mf);
        size_t nbComponents                         = 0;
        if(photometricInterpretation == "MONOCHROME2")
        {
            nbComponents = 1;
        }
        else if(photometricInterpretation == "RGB" || photometricInterpretation == "YBR")
        {
            nbComponents = 3;
        }
        else if(photometricInterpretation == "ARGB" || photometricInterpretation == "CMYK")
        {
            nbComponents = 4;
        }
        CPPUNIT_ASSERT_EQUAL(nbComponents, image->getNumberOfComponents());

    }

}

//------------------------------------------------------------------------------

void SeriesDBReaderTest::setUp()
{
    // Set up context before running a test.
    if(::fwTest::Slow::ignoreSlowTests())
    {
        std::cout << std::endl << "Ignoring slow " << std::endl;
    }
    else
    {
        std::cout << std::endl << "Executing slow tests.." << std::endl;
    }
}

//------------------------------------------------------------------------------

void SeriesDBReaderTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void SeriesDBReaderTest::readJMSSeriesDBTest()
{
    if(::fwTest::Slow::ignoreSlowTests())
    {
        return;
    }
    ::fwMemory::BufferManager::getDefault()->setLoadingMode(::fwMemory::BufferManager::DIRECT);
    this->readJMSSeries();
}

//------------------------------------------------------------------------------

void SeriesDBReaderTest::readCTSeriesDBTest()
{
    if(::fwTest::Slow::ignoreSlowTests())
    {
        return;
    }
    ::fwMemory::BufferManager::getDefault()->setLoadingMode(::fwMemory::BufferManager::DIRECT);
    this->readCTSeries();
}

//------------------------------------------------------------------------------

void SeriesDBReaderTest::readMRSeriesDBTest()
{
    if(::fwTest::Slow::ignoreSlowTests())
    {
        return;
    }
    ::fwMemory::BufferManager::getDefault()->setLoadingMode(::fwMemory::BufferManager::DIRECT);
    this->readMRSeries();
}

//------------------------------------------------------------------------------

void SeriesDBReaderTest::readOTSeriesDBTest()
{
    if(::fwTest::Slow::ignoreSlowTests())
    {
        return;
    }
    ::fwMemory::BufferManager::getDefault()->setLoadingMode(::fwMemory::BufferManager::DIRECT);
    this->readOTSeries();
}

//------------------------------------------------------------------------------

void SeriesDBReaderTest::readSEGSeriesDBTest()
{
    if(::fwTest::Slow::ignoreSlowTests())
    {
        return;
    }
    ::fwMemory::BufferManager::getDefault()->setLoadingMode(::fwMemory::BufferManager::DIRECT);
    this->readSEGSeries();
}

//------------------------------------------------------------------------------

void SeriesDBReaderTest::readSFSeriesDBTest()
{
    if(::fwTest::Slow::ignoreSlowTests())
    {
        return;
    }
    ::fwMemory::BufferManager::getDefault()->setLoadingMode(::fwMemory::BufferManager::DIRECT);
    this->readSFSeries();
}

//------------------------------------------------------------------------------

void SeriesDBReaderTest::readSRSeriesDBTest()
{
    if(::fwTest::Slow::ignoreSlowTests())
    {
        return;
    }
    ::fwMemory::BufferManager::getDefault()->setLoadingMode(::fwMemory::BufferManager::DIRECT);
    this->readSRSeries();
}

//------------------------------------------------------------------------------

void SeriesDBReaderTest::read3DSRSeriesDBTest()
{
    if(::fwTest::Slow::ignoreSlowTests())
    {
        return;
    }
    ::fwMemory::BufferManager::getDefault()->setLoadingMode(::fwMemory::BufferManager::DIRECT);
    this->read3DSRSeries();
}

//------------------------------------------------------------------------------

void SeriesDBReaderTest::readDisabledSeriesDBTest()
{
    if(::fwTest::Slow::ignoreSlowTests())
    {
        return;
    }
    ::fwMemory::BufferManager::getDefault()->setLoadingMode(::fwMemory::BufferManager::DIRECT);
    this->readDisabledSeries();
}

//------------------------------------------------------------------------------

void SeriesDBReaderTest::readMRSeriesWithDicomDirDBTest()
{
    if(::fwTest::Slow::ignoreSlowTests())
    {
        return;
    }
    ::fwMemory::BufferManager::getDefault()->setLoadingMode(::fwMemory::BufferManager::DIRECT);
    this->readMRSeriesWithDicomDir();
}

//------------------------------------------------------------------------------

void SeriesDBReaderTest::readMultipleRescaleSeriesDBTest()
{
    if(::fwTest::Slow::ignoreSlowTests())
    {
        return;
    }
    ::fwMemory::BufferManager::getDefault()->setLoadingMode(::fwMemory::BufferManager::DIRECT);
    this->readMultipleRescaleSeries();
}

//------------------------------------------------------------------------------

void SeriesDBReaderTest::readCTWithSurviewSeriesDBTest()
{
    if(::fwTest::Slow::ignoreSlowTests())
    {
        return;
    }
    ::fwMemory::BufferManager::getDefault()->setLoadingMode(::fwMemory::BufferManager::DIRECT);
    this->readCTWithSurviewSeries();
}

//------------------------------------------------------------------------------

void SeriesDBReaderTest::readMRWithTemporalPositionSeriesDBTest()
{
    if(::fwTest::Slow::ignoreSlowTests())
    {
        return;
    }
    ::fwMemory::BufferManager::getDefault()->setLoadingMode(::fwMemory::BufferManager::DIRECT);
    this->readMRWithTemporalPositionSeries();
}

//------------------------------------------------------------------------------

void SeriesDBReaderTest::readCTSeriesDBIssue01Test()
{
    if(::fwTest::Slow::ignoreSlowTests())
    {
        return;
    }
    ::fwMemory::BufferManager::getDefault()->setLoadingMode(::fwMemory::BufferManager::DIRECT);
    this->readCTSeriesDBIssue01();
}

//------------------------------------------------------------------------------

void SeriesDBReaderTest::readJMSSeries()
{
    ::fwMedData::SeriesDB::sptr seriesDB = ::fwMedData::SeriesDB::New();

    const std::filesystem::path path = ::fwTest::Data::dir() / "sight/Patient/Dicom/JMSGenou";

    CPPUNIT_ASSERT_MESSAGE("The dicom directory '" + path.string() + "' does not exist",
                           std::filesystem::exists(path));

    ::fwGdcmIO::reader::SeriesDB::sptr reader = ::fwGdcmIO::reader::SeriesDB::New();
    reader->setObject(seriesDB);
    reader->setFolder(path);
    reader->setDicomFilterType("::fwDicomIOFilter::custom::DefaultDicomFilter");

    CPPUNIT_ASSERT_NO_THROW(reader->read());

    CPPUNIT_ASSERT_EQUAL( size_t( 1 ), seriesDB->size());
    ::fwMedData::ImageSeries::sptr series = ::fwMedData::ImageSeries::dynamicCast(seriesDB->front());

    // Check trimmed values
    CPPUNIT_ASSERT( ::fwTest::DicomReaderTest::checkSeriesJMSGenouTrimmed( series ) );

    // Read image in lazy mode
    const auto dumpLock = series->getImage()->lock();

}

//------------------------------------------------------------------------------

void SeriesDBReaderTest::readCTSeries()
{
    ::fwMedData::SeriesDB::sptr seriesDB = ::fwMedData::SeriesDB::New();

    const std::string filename       = "01-CT-DICOM_LIVER";
    const std::filesystem::path path = ::fwTest::Data::dir() / "sight/Patient/Dicom/DicomDB" / filename;

    CPPUNIT_ASSERT_MESSAGE("The dicom directory '" + path.string() + "' does not exist",
                           std::filesystem::exists(path));

    ::fwGdcmIO::reader::SeriesDB::sptr reader = ::fwGdcmIO::reader::SeriesDB::New();
    reader->setObject(seriesDB);
    reader->setFolder(path);
    reader->setDicomFilterType("::fwDicomIOFilter::custom::DefaultDicomFilter");

    // Read DICOM
    CPPUNIT_ASSERT_NO_THROW(reader->read());

    const double delta = 0.00001;

    // Check number of series
    CPPUNIT_ASSERT_EQUAL( size_t( 1 ), seriesDB->size());

    // Read image buffer
    ::fwMedData::ImageSeries::sptr series = ::fwMedData::ImageSeries::dynamicCast(seriesDB->front());
    ::fwData::Image::sptr image           = series->getImage();
    const auto dumpLock = image->lock();

    // Check number of dimensions
    CPPUNIT_ASSERT_EQUAL( size_t( 3 ), image->getNumberOfDimensions());

    // Check size
    const ::fwData::Image::Size size = image->getSize2();
    CPPUNIT_ASSERT_EQUAL( size_t( 512 ), size[0]);
    CPPUNIT_ASSERT_EQUAL( size_t( 512 ), size[1]);
    CPPUNIT_ASSERT_EQUAL( size_t( 129 ), size[2]);

    // Check spacing
    const ::fwData::Image::Spacing spacing = image->getSpacing2();
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 0.57 ), spacing[0], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 0.57 ), spacing[1], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 1.6 ), spacing[2], delta);

    // Check origin
    const ::fwData::Image::Origin origin = image->getOrigin2();
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 0 ), origin[0], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 0 ), origin[1], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 0 ), origin[2], delta);

    // Check window center
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 0 ), image->getWindowCenter(), delta);

    // Check window width
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 0 ), image->getWindowWidth(), delta);

    // Check image type
    CPPUNIT_ASSERT_EQUAL(::fwTools::Type::s_INT16, image->getType());

    // Verify tag values according to json file
    verifyTagValues(filename, seriesDB);
}

//------------------------------------------------------------------------------

void SeriesDBReaderTest::readMRSeries()
{
    ::fwMedData::SeriesDB::sptr seriesDB = ::fwMedData::SeriesDB::New();

    const std::string filename       = "46-MR-BARRE-MONO2-12-shoulder";
    const std::filesystem::path path = ::fwTest::Data::dir() / "sight/Patient/Dicom/DicomDB" / filename;

    CPPUNIT_ASSERT_MESSAGE("The dicom directory '" + path.string() + "' does not exist",
                           std::filesystem::exists(path));

    ::fwGdcmIO::reader::SeriesDB::sptr reader = ::fwGdcmIO::reader::SeriesDB::New();
    reader->setObject(seriesDB);
    reader->setFolder(path);
    reader->setDicomFilterType("::fwDicomIOFilter::custom::DefaultDicomFilter");

    // Read DICOM
    CPPUNIT_ASSERT_NO_THROW(reader->read());

    const double delta = 0.01;

    // Check number of series
    CPPUNIT_ASSERT_EQUAL( size_t( 1 ), seriesDB->size());

    // Read image buffer
    ::fwMedData::ImageSeries::sptr series = ::fwMedData::ImageSeries::dynamicCast(seriesDB->front());
    ::fwData::Image::sptr image           = series->getImage();
    const auto dumpLock = image->lock();

    // Check number of dimensions - FIXME Should be 2 but when creating an image with 2D size, the visualization
    // crashes...
    CPPUNIT_ASSERT_EQUAL( size_t( 3 ), image->getNumberOfDimensions());

    // Check size
    const ::fwData::Image::Size size = image->getSize2();
    CPPUNIT_ASSERT_EQUAL( size_t( 1 ), size[0]);
    CPPUNIT_ASSERT_EQUAL( size_t( 1024 ), size[1]);
    CPPUNIT_ASSERT_EQUAL( size_t( 1024 ), size[2]);

    // Check spacing
    const ::fwData::Image::Spacing spacing = image->getSpacing2();
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 6 ), spacing[0], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 0.2 ), spacing[1], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 0.2 ), spacing[2], delta);

    // Check origin
    const ::fwData::Image::Origin origin = image->getOrigin2();
    SLM_WARN("ORIGIN : " << origin[0] << " " << origin[1] << " " << origin[2]);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( -112.828 ), origin[0], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( -180.058 ), origin[1], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 97.1478 ), origin[2], delta);

    // Check window center
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 1000 ), image->getWindowCenter(), delta);

    // Check window width
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 2000 ), image->getWindowWidth(), delta);

    // Check image type
    CPPUNIT_ASSERT_EQUAL(::fwTools::Type::s_DOUBLE, image->getType());

    // Verify tag values according to json file
    verifyTagValues(filename, seriesDB);

}

//------------------------------------------------------------------------------

void SeriesDBReaderTest::readOTSeries()
{
    ::fwMedData::SeriesDB::sptr seriesDB = ::fwMedData::SeriesDB::New();

    const std::string filename       = "42-OT-BARRE-MONO2-8-colon";
    const std::filesystem::path path = ::fwTest::Data::dir() / "sight/Patient/Dicom/DicomDB" / filename;

    CPPUNIT_ASSERT_MESSAGE("The dicom directory '" + path.string() + "' does not exist",
                           std::filesystem::exists(path));

    ::fwGdcmIO::reader::SeriesDB::sptr reader = ::fwGdcmIO::reader::SeriesDB::New();
    reader->setObject(seriesDB);
    reader->setFolder(path);
    reader->setDicomFilterType("::fwDicomIOFilter::custom::DefaultDicomFilter");

    // Read DICOM
    CPPUNIT_ASSERT_NO_THROW(reader->read());

    const double delta = 0.01;

    // Check number of series
    CPPUNIT_ASSERT_EQUAL( size_t( 1 ), seriesDB->size());

    // Read image buffer
    ::fwMedData::ImageSeries::sptr series = ::fwMedData::ImageSeries::dynamicCast(seriesDB->front());
    ::fwData::Image::sptr image           = series->getImage();
    const auto dumpLock = image->lock();

    // Check number of dimensions - FIXME Should be 2 but when creating an image with 2D size, the visualization
    // crashes...
    CPPUNIT_ASSERT_EQUAL( size_t( 3 ), image->getNumberOfDimensions());

    // Check size
    ::fwData::Image::Size size = image->getSize2();
    CPPUNIT_ASSERT_EQUAL( size_t( 512 ), size[0]);
    CPPUNIT_ASSERT_EQUAL( size_t( 512 ), size[1]);
    CPPUNIT_ASSERT_EQUAL( size_t( 1 ), size[2]);

    // Check spacing
    ::fwData::Image::Spacing spacing = image->getSpacing2();
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 1 ), spacing[0], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 1 ), spacing[1], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 1 ), spacing[2], delta);

    // Check origin
    ::fwData::Image::Origin origin = image->getOrigin2();
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 0 ), origin[0], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 0 ), origin[1], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 0 ), origin[2], delta);

    // Check window center
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 0 ), image->getWindowCenter(), delta);

    // Check window width
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 0 ), image->getWindowWidth(), delta);

    // Check image type
    CPPUNIT_ASSERT_EQUAL(::fwTools::Type::s_UINT8, image->getType());

    // Verify tag values according to json file
    verifyTagValues(filename, seriesDB);

}

//------------------------------------------------------------------------------

void SeriesDBReaderTest::readSEGSeries()
{
    ::fwMedData::SeriesDB::sptr seriesDB = ::fwMedData::SeriesDB::New();

    const std::string filename       = "71-CT-DICOM_SEG";
    const std::filesystem::path path = ::fwTest::Data::dir() / "sight/Patient/Dicom/DicomDB" / filename;

    CPPUNIT_ASSERT_MESSAGE("The dicom directory '" + path.string() + "' does not exist",
                           std::filesystem::exists(path));

    ::fwGdcmIO::reader::SeriesDB::sptr reader = ::fwGdcmIO::reader::SeriesDB::New();
    reader->setObject(seriesDB);
    reader->setFolder(path);
    reader->setDicomFilterType("::fwDicomIOFilter::custom::DefaultDicomFilter");

    // Read DICOM
    CPPUNIT_ASSERT_NO_THROW(reader->read());

    const double delta = 0.01;

    CPPUNIT_ASSERT_EQUAL( size_t( 2 ), seriesDB->size());

    //Retrieve ImageSeries
    ::fwMedData::ModelSeries::sptr series = ::fwMedData::ModelSeries::dynamicCast((*seriesDB)[1]);
    CPPUNIT_ASSERT(series);

    ::fwMedData::ModelSeries::ReconstructionVectorType reconstructionDB = series->getReconstructionDB();
    CPPUNIT_ASSERT_EQUAL( size_t( 1 ), reconstructionDB.size());

    // Check reconstruction
    ::fwData::Reconstruction::sptr reconstruction = reconstructionDB[0];
    CPPUNIT_ASSERT_EQUAL( std::string("Liver"), reconstruction->getOrganName());

    // Check mesh
    ::fwData::Mesh::sptr mesh = reconstruction->getMesh();
    CPPUNIT_ASSERT_EQUAL( ::fwData::Mesh::Size( 2498 ), mesh->getNumberOfPoints());
    CPPUNIT_ASSERT_EQUAL( ::fwData::Mesh::Size( 5000 ), mesh->getNumberOfCells());
    CPPUNIT_ASSERT_EQUAL( ::fwData::Mesh::Size( 15000 ), mesh->getCellDataSize());

    // Check material
    ::fwData::Material::sptr material = reconstruction->getMaterial();
    ::fwData::Color::sptr color       = material->diffuse();
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 0.8 ), color->red(), delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 0.2 ), color->green(), delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 0.2 ), color->blue(), delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 1 ), color->alpha(), delta);
}

//------------------------------------------------------------------------------

void SeriesDBReaderTest::readSFSeries()
{
    ::fwMedData::SeriesDB::sptr seriesDB = ::fwMedData::SeriesDB::New();

    const std::string filename       = "71-CT-DICOM_SF";
    const std::filesystem::path path = ::fwTest::Data::dir() / "sight/Patient/Dicom/DicomDB" / filename;

    CPPUNIT_ASSERT_MESSAGE("The dicom directory '" + path.string() + "' does not exist",
                           std::filesystem::exists(path));

    ::fwGdcmIO::reader::SeriesDB::sptr reader = ::fwGdcmIO::reader::SeriesDB::New();
    reader->setObject(seriesDB);
    reader->setFolder(path);
    reader->setDicomFilterType("::fwDicomIOFilter::custom::DefaultDicomFilter");

    // Read DICOM
    CPPUNIT_ASSERT_NO_THROW(reader->read());

    const double delta = 0.01;

    CPPUNIT_ASSERT_EQUAL( size_t( 1 ), seriesDB->size());

    // Retrieve ImageSeries
    ::fwMedData::ImageSeries::sptr series = ::fwMedData::ImageSeries::dynamicCast(seriesDB->front());
    ::fwData::Image::sptr image           = series->getImage();
    const auto dumpLock = image->lock();

    // Retrieve landmarks
    ::fwData::PointList::sptr pointList =
        image->getField< ::fwData::PointList >(::fwDataTools::fieldHelper::Image::m_imageLandmarksId);

    // Verify first landmark
    const ::fwData::Point::sptr& pointA = pointList->getPoints()[0];
    const std::string labelA            =
        pointA->getField< ::fwData::String >(::fwDataTools::fieldHelper::Image::m_labelId)->value();
    CPPUNIT_ASSERT_EQUAL(std::string("Label1"), labelA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 80.89 ), pointA->getCoord()[0], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 102.16 ), pointA->getCoord()[1], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 153 ), pointA->getCoord()[2], delta);

    // Verify second landmark
    const ::fwData::Point::sptr& pointB = pointList->getPoints()[1];
    const std::string labelB            =
        pointB->getField< ::fwData::String >(::fwDataTools::fieldHelper::Image::m_labelId)->value();
    CPPUNIT_ASSERT_EQUAL(std::string("Label2"), labelB);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 281.63 ), pointB->getCoord()[0], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 326.52 ), pointB->getCoord()[1], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 276 ), pointB->getCoord()[2], delta);
}

//------------------------------------------------------------------------------

void SeriesDBReaderTest::readSRSeries()
{
    ::fwMedData::SeriesDB::sptr seriesDB = ::fwMedData::SeriesDB::New();

    const std::string filename       = "71-CT-DICOM_SR";
    const std::filesystem::path path = ::fwTest::Data::dir() / "sight/Patient/Dicom/DicomDB" / filename;

    CPPUNIT_ASSERT_MESSAGE("The dicom directory '" + path.string() + "' does not exist",
                           std::filesystem::exists(path));

    ::fwGdcmIO::reader::SeriesDB::sptr reader = ::fwGdcmIO::reader::SeriesDB::New();
    reader->setObject(seriesDB);
    reader->setFolder(path);
    reader->setDicomFilterType("::fwDicomIOFilter::custom::DefaultDicomFilter");

    // Read DICOM
    CPPUNIT_ASSERT_NO_THROW(reader->read());

    const double delta = 0.01;

    CPPUNIT_ASSERT_EQUAL( size_t( 1 ), seriesDB->size());

    // Retrieve ImageSeries
    ::fwMedData::ImageSeries::sptr series = ::fwMedData::ImageSeries::dynamicCast(seriesDB->front());
    CPPUNIT_ASSERT(series);
    ::fwData::Image::sptr image = series->getImage();
    const auto dumpLock = image->lock();

    // Retrieve landmarks
    ::fwData::PointList::sptr landmarkPointList =
        image->getField< ::fwData::PointList >(::fwDataTools::fieldHelper::Image::m_imageLandmarksId);

    // Verify first landmark
    const ::fwData::Point::sptr& pointA = landmarkPointList->getPoints()[0];
    const std::string labelA            =
        pointA->getField< ::fwData::String >(::fwDataTools::fieldHelper::Image::m_labelId)->value();
    CPPUNIT_ASSERT_EQUAL(std::string("Label1"), labelA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 80.89 ), pointA->getCoord()[0], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 102.16 ), pointA->getCoord()[1], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 153 ), pointA->getCoord()[2], delta);

    // Verify second landmark
    const ::fwData::Point::sptr& pointB = landmarkPointList->getPoints()[1];
    const std::string labelB            =
        pointB->getField< ::fwData::String >(::fwDataTools::fieldHelper::Image::m_labelId)->value();
    CPPUNIT_ASSERT_EQUAL(std::string("Label2"), labelB);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 281.63 ), pointB->getCoord()[0], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 326.52 ), pointB->getCoord()[1], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 276 ), pointB->getCoord()[2], delta);

    // Retrieve distances
    ::fwData::Vector::sptr distanceVector =
        image->getField< ::fwData::Vector >(::fwDataTools::fieldHelper::Image::m_imageDistancesId);

    // Verify first distance
    ::fwData::PointList::sptr distancePointList = ::fwData::PointList::dynamicCast(distanceVector->getContainer()[0]);
    const ::fwData::Point::sptr& pointC = distancePointList->getPoints()[0];
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 38.34 ), pointC->getCoord()[0], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 116.67 ), pointC->getCoord()[1], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 276 ), pointC->getCoord()[2], delta);
    const ::fwData::Point::sptr& pointD = distancePointList->getPoints()[1];
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 329.41 ), pointD->getCoord()[0], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 302.33 ), pointD->getCoord()[1], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 276 ), pointD->getCoord()[2], delta);
}

//------------------------------------------------------------------------------

void SeriesDBReaderTest::read3DSRSeries()
{
    ::fwMedData::SeriesDB::sptr seriesDB = ::fwMedData::SeriesDB::New();

    const std::string filename       = "71-CT-DICOM_3DSR";
    const std::filesystem::path path = ::fwTest::Data::dir() / "sight/Patient/Dicom/DicomDB" / filename;

    CPPUNIT_ASSERT_MESSAGE("The dicom directory '" + path.string() + "' does not exist",
                           std::filesystem::exists(path));

    ::fwGdcmIO::reader::SeriesDB::sptr reader = ::fwGdcmIO::reader::SeriesDB::New();
    reader->setObject(seriesDB);
    reader->setFolder(path);
    reader->setDicomFilterType("::fwDicomIOFilter::custom::DefaultDicomFilter");

    // Read DICOM
    CPPUNIT_ASSERT_NO_THROW(reader->read());

    const double delta = 0.01;

    CPPUNIT_ASSERT_EQUAL( size_t( 1 ), seriesDB->size());

    // Retrieve ImageSeries
    ::fwMedData::ImageSeries::sptr series = ::fwMedData::ImageSeries::dynamicCast(seriesDB->front());
    ::fwData::Image::sptr image           = series->getImage();
    const auto dumpLock = image->lock();

    // Retrieve landmarks
    ::fwData::PointList::sptr landmarkPointList =
        image->getField< ::fwData::PointList >(::fwDataTools::fieldHelper::Image::m_imageLandmarksId);

    // Verify first landmark
    const ::fwData::Point::sptr& pointA = landmarkPointList->getPoints()[0];
    const std::string labelA            =
        pointA->getField< ::fwData::String >(::fwDataTools::fieldHelper::Image::m_labelId)->value();
    CPPUNIT_ASSERT_EQUAL(std::string("Label1"), labelA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 80.89 ), pointA->getCoord()[0], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 102.16 ), pointA->getCoord()[1], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 153 ), pointA->getCoord()[2], delta);

    // Verify second landmark
    const ::fwData::Point::sptr& pointB = landmarkPointList->getPoints()[1];
    const std::string labelB            =
        pointB->getField< ::fwData::String >(::fwDataTools::fieldHelper::Image::m_labelId)->value();
    CPPUNIT_ASSERT_EQUAL(std::string("Label2"), labelB);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 281.63 ), pointB->getCoord()[0], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 326.52 ), pointB->getCoord()[1], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 276 ), pointB->getCoord()[2], delta);

    // Retrieve distances
    ::fwData::Vector::sptr distanceVector =
        image->getField< ::fwData::Vector >(::fwDataTools::fieldHelper::Image::m_imageDistancesId);

    // Verify first distance
    ::fwData::PointList::sptr distancePointList = ::fwData::PointList::dynamicCast(distanceVector->getContainer()[0]);
    const ::fwData::Point::sptr& pointC = distancePointList->getPoints()[0];
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 281.63 ), pointC->getCoord()[0], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 99.30 ), pointC->getCoord()[1], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 87.26 ), pointC->getCoord()[2], delta);
    const ::fwData::Point::sptr& pointD = distancePointList->getPoints()[1];
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 329.41 ), pointD->getCoord()[0], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 302.33 ), pointD->getCoord()[1], delta);
    CPPUNIT_ASSERT_DOUBLES_EQUAL( double( 276 ), pointD->getCoord()[2], delta);
}

//------------------------------------------------------------------------------

void SeriesDBReaderTest::readDisabledSeries()
{
    ::fwMedData::SeriesDB::sptr seriesDB = ::fwMedData::SeriesDB::New();

    const std::string filename       = "46-MR-BARRE-MONO2-12-shoulder";
    const std::filesystem::path path = ::fwTest::Data::dir() / "sight/Patient/Dicom/DicomDB" / filename;

    CPPUNIT_ASSERT_MESSAGE("The dicom directory '" + path.string() + "' does not exist",
                           std::filesystem::exists(path));

    std::vector< std::string > supportedSOPClassContainer;
    supportedSOPClassContainer.push_back("1.2.840.10008.5.1.4.1.1.2");   // CT Image Storage

    ::fwGdcmIO::reader::SeriesDB::sptr reader = ::fwGdcmIO::reader::SeriesDB::New();
    reader->setObject(seriesDB);
    reader->setFolder(path);
    reader->setDicomFilterType("::fwDicomIOFilter::custom::DefaultDicomFilter");
    reader->setSupportedSOPClassContainer(supportedSOPClassContainer);

    // Read DICOM
    CPPUNIT_ASSERT_NO_THROW(reader->read());

    // Verify that the reading has failed
    CPPUNIT_ASSERT_EQUAL( size_t( 0 ), seriesDB->size());
    CPPUNIT_ASSERT_EQUAL( size_t( 1 ), size_t(reader->getLogger()->count(::fwLog::Log::CRITICAL)));
}

//------------------------------------------------------------------------------

void SeriesDBReaderTest::readMRSeriesWithDicomDir()
{
    ::fwMedData::SeriesDB::sptr seriesDB = ::fwMedData::SeriesDB::New();

    const std::string filename       = "82-MR-SAGITTAL-KNEE-DICOMDIR";
    const std::filesystem::path path = ::fwTest::Data::dir() / "sight/Patient/Dicom/DicomDB" / filename;

    CPPUNIT_ASSERT_MESSAGE("The dicom directory '" + path.string() + "' does not exist",
                           std::filesystem::exists(path));

    ::fwGdcmIO::reader::SeriesDB::sptr reader = ::fwGdcmIO::reader::SeriesDB::New();
    reader->setObject(seriesDB);
    reader->setFolder(path);
    reader->setDicomFilterType("::fwDicomIOFilter::custom::DefaultDicomFilter");
    reader->setDicomdirActivated(true);

    // Read DICOM
    CPPUNIT_ASSERT_NO_THROW(reader->read());

    // Verify that the reading has succeed
    CPPUNIT_ASSERT_EQUAL( size_t( 1 ), seriesDB->size());
    CPPUNIT_ASSERT_EQUAL( size_t( 0 ), size_t(reader->getLogger()->count(::fwLog::Log::CRITICAL)));
}

//------------------------------------------------------------------------------

void SeriesDBReaderTest::readMultipleRescaleSeries()
{
    ::fwMedData::SeriesDB::sptr seriesDB = ::fwMedData::SeriesDB::New();

    const std::string filename       = "83-CT-MultipleRescale";
    const std::filesystem::path path = ::fwTest::Data::dir() / "sight/Patient/Dicom/DicomDB" / filename;

    CPPUNIT_ASSERT_MESSAGE("The dicom directory '" + path.string() + "' does not exist",
                           std::filesystem::exists(path));

    ::fwGdcmIO::reader::SeriesDB::sptr reader = ::fwGdcmIO::reader::SeriesDB::New();
    reader->setObject(seriesDB);
    reader->setFolder(path);
    reader->setDicomFilterType("::fwDicomIOFilter::custom::DefaultDicomFilter");

    // Read DICOM
    CPPUNIT_ASSERT_NO_THROW(reader->read());

    // Retrieve ImageSeries
    CPPUNIT_ASSERT_EQUAL( size_t( 1 ), seriesDB->size());
    ::fwMedData::ImageSeries::sptr series = ::fwMedData::ImageSeries::dynamicCast(seriesDB->front());
    ::fwData::Image::sptr image           = series->getImage();
    const auto dumpLock = image->lock();

    // Get internal buffer
    auto buffer = image->getBuffer();
    CPPUNIT_ASSERT(buffer);

    CPPUNIT_ASSERT(dumpLock.getBuffer());

    // Compute sha1 digest
    ::boost::uuids::detail::sha1 sha1;
    sha1.process_bytes(static_cast< char* >(dumpLock.getBuffer()), image->getSizeInBytes());
    ::boost::uuids::detail::sha1::digest_type digest = {0};
    sha1.get_digest(digest);

    // Check digests
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Multiple rescale image hash comparison failed ", 808070165u, digest[0]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Multiple rescale image hash comparison failed ", 1419762457u, digest[1]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Multiple rescale image hash comparison failed ", 664759744u, digest[2]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Multiple rescale image hash comparison failed ", 4220766428u, digest[3]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Multiple rescale image hash comparison failed ", 2226307254u, digest[4]);
}

//------------------------------------------------------------------------------

void SeriesDBReaderTest::readCTWithSurviewSeries()
{
    ::fwMedData::SeriesDB::sptr seriesDB = ::fwMedData::SeriesDB::New();

    const std::string filename       = "84-CT-Surview";
    const std::filesystem::path path = ::fwTest::Data::dir() / "sight/Patient/Dicom/DicomDB" / filename;

    CPPUNIT_ASSERT_MESSAGE("The dicom directory '" + path.string() + "' does not exist",
                           std::filesystem::exists(path));

    ::fwGdcmIO::reader::SeriesDB::sptr reader = ::fwGdcmIO::reader::SeriesDB::New();
    reader->setObject(seriesDB);
    reader->setFolder(path);
    reader->setDicomFilterType("::fwDicomIOFilter::custom::DefaultDicomFilter");

    // Read DICOM
    CPPUNIT_ASSERT_NO_THROW(reader->read());

    // Verify that the reading has failed
    CPPUNIT_ASSERT_EQUAL( size_t( 3 ), seriesDB->size());
    CPPUNIT_ASSERT_EQUAL( size_t( 0 ), size_t(reader->getLogger()->count(::fwLog::Log::CRITICAL)));
}

//------------------------------------------------------------------------------

void SeriesDBReaderTest::readMRWithTemporalPositionSeries()
{
    ::fwMedData::SeriesDB::sptr seriesDB = ::fwMedData::SeriesDB::New();

    const std::string filename       = "85-MR-TemporalPosition";
    const std::filesystem::path path = ::fwTest::Data::dir() / "sight/Patient/Dicom/DicomDB" / filename;

    CPPUNIT_ASSERT_MESSAGE("The dicom directory '" + path.string() + "' does not exist",
                           std::filesystem::exists(path));

    ::fwGdcmIO::reader::SeriesDB::sptr reader = ::fwGdcmIO::reader::SeriesDB::New();
    reader->setObject(seriesDB);
    reader->setFolder(path);
    reader->setDicomFilterType("::fwDicomIOFilter::custom::DefaultDicomFilter");

    // Read DICOM
    CPPUNIT_ASSERT_NO_THROW(reader->read());

    // Verify that the reading has failed
    CPPUNIT_ASSERT_EQUAL( size_t( 4 ), seriesDB->size());
    CPPUNIT_ASSERT_EQUAL( size_t( 0 ), size_t(reader->getLogger()->count(::fwLog::Log::CRITICAL)));
}

//------------------------------------------------------------------------------

void SeriesDBReaderTest::readCTSeriesDBIssue01()
{
    ::fwMedData::SeriesDB::sptr seriesDB = ::fwMedData::SeriesDB::New();

    const std::string filename       = "86-CT-Skull";
    const std::filesystem::path path = ::fwTest::Data::dir() / "sight/Patient/Dicom/DicomDB" / filename;

    CPPUNIT_ASSERT_MESSAGE("The dicom directory '" + path.string() + "' does not exist",
                           std::filesystem::exists(path));

    ::fwGdcmIO::reader::SeriesDB::sptr reader = ::fwGdcmIO::reader::SeriesDB::New();
    reader->setObject(seriesDB);
    reader->setFolder(path);
    reader->setDicomFilterType("::fwDicomIOFilter::custom::DefaultDicomFilter");

    // Read DICOM
    CPPUNIT_ASSERT_NO_THROW(reader->read());

    // Verify that the reading has failed
    CPPUNIT_ASSERT_EQUAL( size_t( 1 ), seriesDB->size());
    CPPUNIT_ASSERT_EQUAL( size_t( 0 ), size_t(reader->getLogger()->count(::fwLog::Log::CRITICAL)));
}

} // namespace ut

} // namespace fwGdcmIO
