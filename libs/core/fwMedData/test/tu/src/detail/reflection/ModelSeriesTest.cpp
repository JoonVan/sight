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

#include "detail/reflection/ModelSeriesTest.hpp"

#include "detail/reflection/DataCampHelper.hpp"

#include <fwMedData/Equipment.hpp>
#include <fwMedData/ModelSeries.hpp>
#include <fwMedData/Patient.hpp>
#include <fwMedData/Study.hpp>

#include <fwData/Reconstruction.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwMedData::detail::reflection::ut::ModelSeriesTest );

namespace fwMedData
{
namespace detail
{
namespace reflection
{
namespace ut
{

//------------------------------------------------------------------------------

void ModelSeriesTest::setUp()
{
}

//------------------------------------------------------------------------------

void ModelSeriesTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void ModelSeriesTest::propertiesTest()
{
    const std::string modality                          = "CT";
    const std::string instance_uid                      = "123456789";
    const std::string number                            = "2";
    const std::string laterality                        = "L";
    const std::string date                              = "20130214";
    const std::string time                              = "143328";
    const std::string performing_physicians_name        = "John Doe";
    const std::string protocolName                      = "elios";
    const std::string description                       = "description";
    const std::string bodyPartExamined                  = "ABDOMEN";
    const std::string patientPosition                   = "unknown";
    const std::string anatomicalOrientationType         = "BIPED";
    const std::string performdedProcedureStepId         = "45896";
    const std::string performedProcedureStepStartDate   = "115548";
    const std::string performedProcedureStepStartTime   = "115535";
    const std::string performedProcedureStepEndDate     = "115945";
    const std::string performedProcedureStepEndTime     = "115944";
    const std::string performedProcedureStepDescription = "step description";
    const std::string performedProcedureComments        = "comments";

    const ::DataCampHelper::PropertiesNameType dataProperties = { { "fields" },
                                                                  { "patient" },
                                                                  { "study" },
                                                                  { "equipment" },
                                                                  { "modality" },
                                                                  { "instance_uid" },
                                                                  { "number" },
                                                                  { "laterality" },
                                                                  { "date" },
                                                                  { "time" },
                                                                  { "performing_physicians_name" },
                                                                  { "protocolName" },
                                                                  { "description" },
                                                                  { "body_part_examined" },
                                                                  { "patient_position" },
                                                                  { "anatomical_orientation_type" },
                                                                  { "performded_procedure_step_id" },
                                                                  { "performed_procedure_step_start_date" },
                                                                  { "performed_procedure_step_start_time" },
                                                                  { "performed_procedure_step_end_date" },
                                                                  { "performed_procedure_step_end_time" },
                                                                  { "performed_procedure_step_description" },
                                                                  { "performed_procedure_comments" },

                                                                  { "dicom_reference" },
                                                                  { "reconstruction_db" }, };

    ::fwMedData::ModelSeries::ReconstructionVectorType recDB;
    recDB.push_back(::fwData::Reconstruction::New());
    recDB.push_back(::fwData::Reconstruction::New());
    ::fwMedData::DicomValuesType performing_physicians_names;
    performing_physicians_names.push_back(performing_physicians_name);
    ::fwMedData::ModelSeries::sptr obj = ::fwMedData::ModelSeries::New();

    obj->setModality(modality);
    obj->setInstanceUID(instance_uid);
    obj->setNumber(number);
    obj->setLaterality(laterality);
    obj->setDate(date);
    obj->setTime(time);
    obj->setPerformingPhysiciansName(performing_physicians_names);
    obj->setProtocolName(protocolName);
    obj->setDescription(description);
    obj->setBodyPartExamined(bodyPartExamined);
    obj->setPatientPosition(patientPosition);
    obj->setAnatomicalOrientationType(anatomicalOrientationType);
    obj->setPerformedProcedureStepID(performdedProcedureStepId);
    obj->setPerformedProcedureStepStartDate(performedProcedureStepStartDate);
    obj->setPerformedProcedureStepStartTime(performedProcedureStepStartTime);
    obj->setPerformedProcedureStepEndDate(performedProcedureStepEndDate);
    obj->setPerformedProcedureStepEndTime(performedProcedureStepEndTime);
    obj->setPerformedProcedureStepDescription(performedProcedureStepDescription);
    obj->setPerformedProcedureComments(performedProcedureComments);

    obj->setReconstructionDB(recDB);

    ::DataCampHelper::visitProperties(obj->getClassname(), dataProperties);
    ::DataCampHelper::compareObjectPropertyValue(obj, "@patient", obj->getPatient());
    ::DataCampHelper::compareObjectPropertyValue(obj, "@study", obj->getStudy());
    ::DataCampHelper::compareObjectPropertyValue(obj, "@equipment", obj->getEquipment());
    ::DataCampHelper::compareSimplePropertyValue(obj, "@modality", modality);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@instance_uid", instance_uid);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@number", number);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@laterality", laterality);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@date", date);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@time", time);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@performing_physicians_name.0", performing_physicians_names[0]);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@protocolName", protocolName);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@description", description);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@body_part_examined", bodyPartExamined);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@patient_position", patientPosition);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@anatomical_orientation_type", anatomicalOrientationType);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@performded_procedure_step_id", performdedProcedureStepId);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@performed_procedure_step_start_date",
                                                 performedProcedureStepStartDate);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@performed_procedure_step_start_time",
                                                 performedProcedureStepStartTime);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@performed_procedure_step_end_date",
                                                 performedProcedureStepEndDate);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@performed_procedure_step_end_time",
                                                 performedProcedureStepEndTime);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@performed_procedure_step_description",
                                                 performedProcedureStepDescription);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@performed_procedure_comments", performedProcedureComments);

    ::DataCampHelper::compareObjectPropertyValue(obj, "@reconstruction_db.0", recDB[0]);
    ::DataCampHelper::compareObjectPropertyValue(obj, "@reconstruction_db.1", recDB[1]);
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace reflection
} //namespace detail
} //namespace fwMedData
