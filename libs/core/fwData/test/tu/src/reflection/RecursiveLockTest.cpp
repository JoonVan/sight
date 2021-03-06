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

#include "reflection/RecursiveLockTest.hpp"

#include <fwData/Composite.hpp>
#include <fwData/Image.hpp>

#include <fwDataCamp/visitor/RecursiveLock.hpp>

#include <fwTest/generator/Image.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION( ::fwDataCamp::ut::RecursiveLockTest );

namespace fwData
{
namespace reflection
{
namespace ut
{

//-----------------------------------------------------------------------------

// Set up context before running a test.
void RecursiveLockTest::setUp()
{
}

//-----------------------------------------------------------------------------

void RecursiveLockTest::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

void RecursiveLockTest::lockTest()
{
    {
        ::fwData::Image::sptr image = ::fwData::Image::New();
        ::fwTest::generator::Image::generateRandomImage(image, ::fwTools::Type::create("int16"));
        ::fwMemory::BufferObject::sptr bo = image->getBufferObject();

        {
            ::fwData::reflection::visitor::RecursiveLock visitor(image);

            CPPUNIT_ASSERT(!image->getMutex().try_lock());
            CPPUNIT_ASSERT(!bo->getMutex().try_lock());
        }

        CPPUNIT_ASSERT(image->getMutex().try_lock());
        CPPUNIT_ASSERT(bo->getMutex().try_lock());
    }

    {
        ::fwData::Image::sptr img1 = ::fwData::Image::New();
        ::fwTest::generator::Image::generateRandomImage(img1, ::fwTools::Type::create("int16"));
        ::fwData::Image::sptr img2 = ::fwData::Image::New();
        ::fwTest::generator::Image::generateRandomImage(img2, ::fwTools::Type::create("uint8"));
        ::fwData::Composite::sptr composite = ::fwData::Composite::New();
        composite->getContainer()["img1"]   = img1;
        composite->getContainer()["img2"]   = img2;

        ::fwMemory::BufferObject::sptr bo1 = img1->getBufferObject();
        ::fwMemory::BufferObject::sptr bo2 = img2->getBufferObject();

        {
            ::fwData::reflection::visitor::RecursiveLock visitor(composite);

            CPPUNIT_ASSERT(!composite->getMutex().try_lock());
            CPPUNIT_ASSERT(!img1->getMutex().try_lock());
            CPPUNIT_ASSERT(!img2->getMutex().try_lock());
            CPPUNIT_ASSERT(!bo1->getMutex().try_lock());
            CPPUNIT_ASSERT(!bo2->getMutex().try_lock());
        }

        CPPUNIT_ASSERT(composite->getMutex().try_lock());
        CPPUNIT_ASSERT(img1->getMutex().try_lock());
        CPPUNIT_ASSERT(img2->getMutex().try_lock());
        CPPUNIT_ASSERT(bo1->getMutex().try_lock());
        CPPUNIT_ASSERT(bo2->getMutex().try_lock());
    }
}

//-----------------------------------------------------------------------------

} // namespace ut
} // namespace reflection
} // namespace fwData
