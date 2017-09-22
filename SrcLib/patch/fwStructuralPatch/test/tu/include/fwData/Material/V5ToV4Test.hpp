/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWSTRUCTURALPATCH_UT_FWDATA_MATERIAL_V5TOV4TEST_HPP__
#define __FWSTRUCTURALPATCH_UT_FWDATA_MATERIAL_V5TOV4TEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace fwStructuralPatch
{
namespace ut
{
namespace fwData
{
namespace Material
{

/**
 * @brief Test patch to convert fwData::Material from version 5 to version 4.
 */
class V5ToV4Test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( V5ToV4Test );
CPPUNIT_TEST( applyPatchTest );
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void applyPatchTest();
};

} //namespace Material
} //namespace fwData
} //namespace ut
} //namespace fwStructuralPatch

#endif //__FWSTRUCTURALPATCH_UT_FWDATA_MATERIAL_V5TOV4TEST_HPP__