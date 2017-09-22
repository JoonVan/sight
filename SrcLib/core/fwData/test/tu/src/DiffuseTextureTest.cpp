/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "DiffuseTextureTest.hpp"

#include <fwData/DiffuseTexture.hpp>

#include <vector>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwData::ut::DiffuseTextureTest );

namespace fwData
{
namespace ut
{

//------------------------------------------------------------------------------

void DiffuseTextureTest::setUp()
{
    // Set up context before running a test.

}
//------------------------------------------------------------------------------

void DiffuseTextureTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void DiffuseTextureTest::textureSetupTest()
{
    //-----------test values
    ::fwData::Image::sptr img = ::fwData::Image::New();

    const size_t nDim = 2;
    ::fwTools::Type type = ::fwTools::Type::create("int8");
    double spacing = 2.5;
    std::vector<double> vectorSpacing(nDim, spacing);
    double origin = 2.7;
    std::vector<double> vectorOrigin(nDim, origin);
    ::boost::int32_t size = 42;
    ::fwData::Image::SizeType vectorSize(nDim, size);

    img->setType(::fwTools::Type::create("int8"));
    img->setSpacing(vectorSpacing);
    img->setOrigin(vectorOrigin);
    img->setSize(vectorSize);

    ::fwData::DiffuseTexture::sptr diffuseTexture = ::fwData::DiffuseTexture::New();

    diffuseTexture->setFiltering(::fwData::DiffuseTexture::FilteringType::LINEAR);
    diffuseTexture->setWrapping(::fwData::DiffuseTexture::WrappingType::REPEAT);
    diffuseTexture->setBlending(::fwData::DiffuseTexture::BlendingType::MODULATE);

    diffuseTexture->setImage(img);

    // Check texture parameters
    {
        CPPUNIT_ASSERT_EQUAL(diffuseTexture->getFiltering(), ::fwData::DiffuseTexture::FilteringType::LINEAR);
        CPPUNIT_ASSERT_EQUAL(diffuseTexture->getWrapping(), ::fwData::DiffuseTexture::WrappingType::REPEAT);
        CPPUNIT_ASSERT_EQUAL(diffuseTexture->getBlending(), ::fwData::DiffuseTexture::BlendingType::MODULATE);

        CPPUNIT_ASSERT(diffuseTexture->getImage() != nullptr);

        CPPUNIT_ASSERT_EQUAL(diffuseTexture->getImage()->getNumberOfDimensions(), nDim);
        CPPUNIT_ASSERT(diffuseTexture->getImage()->getType() == type);
        CPPUNIT_ASSERT(diffuseTexture->getImage()->getSpacing() == vectorSpacing);
        CPPUNIT_ASSERT(diffuseTexture->getImage()->getOrigin() == vectorOrigin);
        CPPUNIT_ASSERT(diffuseTexture->getImage()->getSize() == vectorSize);
    }

    {
        // Check deep copy
        ::fwData::DiffuseTexture::csptr diffuseTextureCopy = ::fwData::Object::copy(diffuseTexture);
        CPPUNIT_ASSERT(diffuseTextureCopy);

        CPPUNIT_ASSERT_EQUAL(diffuseTextureCopy->getFiltering(), ::fwData::DiffuseTexture::FilteringType::LINEAR);
        CPPUNIT_ASSERT_EQUAL(diffuseTextureCopy->getWrapping(), ::fwData::DiffuseTexture::WrappingType::REPEAT);
        CPPUNIT_ASSERT_EQUAL(diffuseTextureCopy->getBlending(), ::fwData::DiffuseTexture::BlendingType::MODULATE);

        CPPUNIT_ASSERT(diffuseTextureCopy->getImage() != nullptr);

        CPPUNIT_ASSERT_EQUAL(diffuseTextureCopy->getImage()->getNumberOfDimensions(), nDim);
        CPPUNIT_ASSERT(diffuseTextureCopy->getImage()->getType() == type);
        CPPUNIT_ASSERT(diffuseTextureCopy->getImage()->getSpacing() == vectorSpacing);
        CPPUNIT_ASSERT(diffuseTextureCopy->getImage()->getOrigin() == vectorOrigin);
        CPPUNIT_ASSERT(diffuseTextureCopy->getImage()->getSize() == vectorSize);
    }

    {
        // Check shallow copy
        ::fwData::DiffuseTexture::sptr diffuseTextureCopy = ::fwData::DiffuseTexture::New();
        diffuseTextureCopy->shallowCopy(diffuseTexture);
        CPPUNIT_ASSERT(diffuseTextureCopy);

        CPPUNIT_ASSERT_EQUAL(diffuseTextureCopy->getFiltering(), ::fwData::DiffuseTexture::FilteringType::LINEAR);
        CPPUNIT_ASSERT_EQUAL(diffuseTextureCopy->getWrapping(), ::fwData::DiffuseTexture::WrappingType::REPEAT);
        CPPUNIT_ASSERT_EQUAL(diffuseTextureCopy->getBlending(), ::fwData::DiffuseTexture::BlendingType::MODULATE);

        CPPUNIT_ASSERT(diffuseTextureCopy->getImage() != nullptr);

        CPPUNIT_ASSERT_EQUAL(diffuseTextureCopy->getImage()->getNumberOfDimensions(), nDim);
        CPPUNIT_ASSERT(diffuseTextureCopy->getImage()->getType() == type);
        CPPUNIT_ASSERT(diffuseTextureCopy->getImage()->getSpacing() == vectorSpacing);
        CPPUNIT_ASSERT(diffuseTextureCopy->getImage()->getOrigin() == vectorOrigin);
        CPPUNIT_ASSERT(diffuseTextureCopy->getImage()->getSize() == vectorSize);
    }
}

} //namespace ut
} //namespace fwData
