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

#include "fwMedData/ModelSeries/V2ToV3Test.hpp"

#include <fwStructuralPatch/fwMedData/ModelSeries/V2ToV3.hpp>

#include <fwAtoms/Object.hpp>
#include <fwAtoms/Sequence.hpp>
#include <fwAtoms/String.hpp>

#include <fwAtomsPatch/helper/functions.hpp>
#include <fwAtomsPatch/helper/Object.hpp>

#include <fwTest/Exception.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION( ::fwStructuralPatch::ut::fwMedData::ModelSeries::V2ToV3Test );

namespace fwStructuralPatch
{
namespace ut
{
namespace fwMedData
{
namespace ModelSeries
{

//------------------------------------------------------------------------------

void V2ToV3Test::setUp()
{
}

//------------------------------------------------------------------------------

void V2ToV3Test::tearDown()
{
}

//------------------------------------------------------------------------------

void V2ToV3Test::applyPatchTest()
{
    ::fwAtoms::Object::sptr modelSeriesV2 = ::fwAtoms::Object::New();
    ::fwAtoms::Object::sptr modelSeriesV3;

    ::fwAtomsPatch::helper::setClassname(modelSeriesV2, "::fwMedData::ModelSeries");
    ::fwAtomsPatch::helper::setVersion(modelSeriesV2, "2");

    ::fwAtomsPatch::helper::Object helper(modelSeriesV2);

    helper.addAttribute("patient", ::fwAtoms::Object::sptr());
    helper.addAttribute("study", ::fwAtoms::Object::sptr());
    helper.addAttribute("equipment", ::fwAtoms::Object::sptr());
    helper.addAttribute("instance_uid", ::fwAtoms::String::New(""));
    helper.addAttribute("modality", ::fwAtoms::String::New(""));
    helper.addAttribute("date", ::fwAtoms::String::New(""));
    helper.addAttribute("time", ::fwAtoms::String::New(""));
    helper.addAttribute("performing_physicians_name", ::fwAtoms::Sequence::New());
    helper.addAttribute("description", ::fwAtoms::String::New(""));
    helper.addAttribute("dicom_reference", ::fwAtoms::Object::sptr());
    helper.addAttribute("reconstruction_db", ::fwAtoms::Object::sptr());

    {
        modelSeriesV3 = ::fwAtoms::Object::dynamicCast(modelSeriesV2->clone());

        ::fwAtomsPatch::IPatch::NewVersionsType newVersions;
        newVersions[modelSeriesV2] = modelSeriesV3;

        ::fwStructuralPatch::fwMedData::ModelSeries::V2ToV3::sptr v2ToV3Patch;
        v2ToV3Patch = ::fwStructuralPatch::fwMedData::ModelSeries::V2ToV3::New();
        CPPUNIT_ASSERT_NO_THROW(v2ToV3Patch->apply(modelSeriesV2, modelSeriesV3, newVersions));

        CPPUNIT_ASSERT(modelSeriesV3);
        CPPUNIT_ASSERT_EQUAL(std::string("3"), ::fwAtomsPatch::helper::getVersion(modelSeriesV3));
        const auto attributes = modelSeriesV3->getAttributes();
        CPPUNIT_ASSERT(attributes.find("number") != attributes.end());
        CPPUNIT_ASSERT(attributes.find("laterality") != attributes.end());
        CPPUNIT_ASSERT(attributes.find("protocolName") != attributes.end());
        CPPUNIT_ASSERT(attributes.find("body_part_examined") != attributes.end());
        CPPUNIT_ASSERT(attributes.find("patient_position") != attributes.end());
        CPPUNIT_ASSERT(attributes.find("anatomical_orientation_type") != attributes.end());
        CPPUNIT_ASSERT(attributes.find("performded_procedure_step_id") != attributes.end());
        CPPUNIT_ASSERT(attributes.find("performed_procedure_step_start_date") != attributes.end());
        CPPUNIT_ASSERT(attributes.find("performed_procedure_step_start_time") != attributes.end());
        CPPUNIT_ASSERT(attributes.find("performed_procedure_step_end_date") != attributes.end());
        CPPUNIT_ASSERT(attributes.find("performed_procedure_step_end_time") != attributes.end());
        CPPUNIT_ASSERT(attributes.find("performed_procedure_step_description") != attributes.end());
        CPPUNIT_ASSERT(attributes.find("performed_procedure_comments") != attributes.end());
    }
}

//------------------------------------------------------------------------------
} //namespace ModelSeries
} //namespace fwMedData
} //namespace ut
} //namespace fwStructuralPatch
