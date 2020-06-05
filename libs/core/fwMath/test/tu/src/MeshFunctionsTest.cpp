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

#include "MeshFunctionsTest.hpp"

#include <fwMath/MeshFunctions.hpp>
#include <fwMath/VectorFunctions.hpp>

#include <fwCore/Profiling.hpp>

namespace fwMath
{
namespace ut
{

// To avoid using indexes in our vectors (for mathematical clarity).
// x, y , z.
#define X 0
#define Y 1
#define Z 2
// u, v, w.
#define U X
#define V Y
#define W Z

const static double s_EPSILON = 10e-9;

CPPUNIT_TEST_SUITE_REGISTRATION( ::fwMath::ut::MeshFunctionsTest );

//-----------------------------------------------------------------------------

void MeshFunctionsTest::setUp()
{

}

//-----------------------------------------------------------------------------

void MeshFunctionsTest::tearDown()
{

}

//-----------------------------------------------------------------------------

void MeshFunctionsTest::compare(const ::glm::dvec3& _expected, const ::glm::dvec3& _actual)
{
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Conversion world-barycentric-world error.", _expected[X], _actual[X],
                                         s_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Conversion world-barycentric-world error.", _expected[Y], _actual[Y],
                                         s_EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Conversion world-barycentric-world error.", _expected[Z], _actual[Z],
                                         s_EPSILON);
}

//-----------------------------------------------------------------------------

void MeshFunctionsTest::computeBarycenterABC2D()
{
    // With the same Z it correspond to a 2d case

    // Creating triangle points A, B, C.
    const ::glm::dvec3 A {9., 6., 1.};
    const ::glm::dvec3 B {9., 12., 1.};
    const ::glm::dvec3 C {2., 9., 1.};

    // Creating intersection Point P.
    const ::glm::dvec3 P {7., 9., 1.};

    const ::glm::dvec3 barycentric = ::fwMath::toBarycentricCoord(P, A, B, C);

    // Test if sum of barycentric coordinates are equal to 1.

    const double sum = (barycentric[U] + barycentric[V] + barycentric[W]);

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("u + v + w = 1", 1., sum, s_EPSILON);

    // Test if 0 ≤ v ≤ 1, 0 ≤ w ≤ 1, and v + w ≤ 1

    CPPUNIT_ASSERT_MESSAGE("0 ≤ v ≤ 1", barycentric[V] >= 0. && barycentric[V] <= 1. );
    CPPUNIT_ASSERT_MESSAGE("0 ≤ w ≤ 1", barycentric[W] >= 0. && barycentric[W] <= 1. );
    CPPUNIT_ASSERT_MESSAGE("v + w ≤ 1", (barycentric[V] + barycentric[W]) <= 1. );

    // Convert back to world coordinates.

    const ::glm::dvec3 P2 = ::fwMath::fromBarycentricCoord(barycentric, A, B, C);

    this->compare(P, P2);
}

//-----------------------------------------------------------------------------

void MeshFunctionsTest::computeBarycenterABC3D()
{
    // Second test in 3d.
    const ::glm::dvec3 A {1., 0., 0.};
    const ::glm::dvec3 B {0., 1., 0.};
    const ::glm::dvec3 C {0., 0., 1.};

    // Should be at the center of ABC
    const ::glm::dvec3 P {1./3., 1./3., 1./3.};

    const ::glm::dvec3 barycentric = ::fwMath::toBarycentricCoord(P, A, B, C);

    // Test if sum of barycentric coordinates are equal to 1.

    const double sum = (barycentric[U] + barycentric[V] + barycentric[W]);

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("u + v + w = 1", 1., sum, s_EPSILON);

    // Test if 0 ≤ v ≤ 1, 0 ≤ w ≤ 1, and v + w ≤ 1

    CPPUNIT_ASSERT_MESSAGE("0 ≤ v ≤ 1", barycentric[V] >= 0. && barycentric[V] <= 1. );
    CPPUNIT_ASSERT_MESSAGE("0 ≤ w ≤ 1", barycentric[W] >= 0. && barycentric[W] <= 1. );
    CPPUNIT_ASSERT_MESSAGE("v + w ≤ 1", (barycentric[V] + barycentric[W]) <= 1. );

    // Convert back to world coordinates.

    const ::glm::dvec3 P2 = ::fwMath::fromBarycentricCoord(barycentric, A, B, C);

    this->compare(P, P2);
}

//-----------------------------------------------------------------------------

void MeshFunctionsTest::computeBarycenterABCRealCoords()
{

    // Second test in 3d.
    const ::glm::dvec3 A {-0.5, 0., 3.};
    const ::glm::dvec3 B {0.5, -0.7071, 3.7071};
    const ::glm::dvec3 C {-0.5, -0.7071, 3.7071};

    // Should be at the center of ABC
    const ::glm::dvec3 P {-0.39, -0.30, 3.30};

    const ::glm::dvec3 barycentric = ::fwMath::toBarycentricCoord(P, A, B, C);

    // Test if sum of barycentric coordinates are equal to 1.

    const double sum = (barycentric[U] + barycentric[V] + barycentric[W]);

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("u + v + w = 1", 1., sum, s_EPSILON);

    // Test if 0 ≤ v ≤ 1, 0 ≤ w ≤ 1, and v + w ≤ 1

    CPPUNIT_ASSERT_MESSAGE("0 ≤ v ≤ 1", barycentric[V] >= 0. && barycentric[V] <= 1. );
    CPPUNIT_ASSERT_MESSAGE("0 ≤ w ≤ 1", barycentric[W] >= 0. && barycentric[W] <= 1. );
    CPPUNIT_ASSERT_MESSAGE("v + w ≤ 1", (barycentric[V] + barycentric[W]) <= 1. );

    // Convert back to world coordinates.

    const ::glm::dvec3 P2 = ::fwMath::fromBarycentricCoord(barycentric, A, B, C);

    this->compare(P, P2);
}

//-----------------------------------------------------------------------------

void MeshFunctionsTest::computeBarycenterOutsideABC()
{
    // Test with a point outside of the triangle.

    // Second test in 3d.
    const ::glm::dvec3 A {0., 0., 0.};
    const ::glm::dvec3 B {0., 1., 0.};
    const ::glm::dvec3 C {1., 0., 1.};

    const ::glm::dvec3 P {10., 20., 30.};

    const ::glm::dvec3 barycentric = ::fwMath::toBarycentricCoord(P, A, B, C);

    // Test if sum of barycentric coordinates are equal to 1.

    const double sum = (barycentric[U] + barycentric[V] + barycentric[W]);

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("u + v + w = 1", 1., sum, s_EPSILON);

    // Test if 0 ≤ v ≤ 1, 0 ≤ w ≤ 1, and v + w ≤ 1

    CPPUNIT_ASSERT_MESSAGE("0 ≤ v ≤ 1 " + std::to_string(
                               barycentric[V]), !(barycentric[V] >= 0. && barycentric[V] <= 1.));
    CPPUNIT_ASSERT_MESSAGE("0 ≤ w ≤ 1", !(barycentric[W] >= 0. && barycentric[W] <= 1. ));
    CPPUNIT_ASSERT_MESSAGE("v + w ≤ 1", !((barycentric[V] + barycentric[W]) <= 1.) );

}

//-----------------------------------------------------------------------------
} // namespace ut
} // namespace fwMath
