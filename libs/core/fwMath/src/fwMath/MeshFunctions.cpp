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

#include "fwMath/MeshFunctions.hpp"

#include "fwMath/VectorFunctions.hpp"

#include <boost/unordered_map.hpp>

#include <glm/glm.hpp>

#include <list>
#include <map>
#include <set>

namespace fwMath
{

//-----------------------------------------------------------------------------

bool intersect_triangle(fwVec3d _orig, fwVec3d _dir, fwVec3d _vert0,
                        fwVec3d _vert1, fwVec3d _vert2,
                        double& _t, double& _u, double& _v)
{
    const double Epsilon = 0.000001;

    fwVec3d edge1, edge2, tvec, pvec, qvec;

    /* find vectors for two edges sharing vert0 */
    edge1 = _vert1 - _vert0;
    edge2 = _vert2 - _vert0;

    /* begin calculating determinant - also used to calculate U parameter */
    pvec = ::fwMath::cross(_dir, edge2);

    /* if determinant is near zero, ray lies in plane of triangle */
    const double Det = ::fwMath::dot(edge1, pvec);

    if (Det > -Epsilon && Det < Epsilon)
    {
        return false;
    }
    const double Inv_det = 1.0 / Det;

    /* calculate distance from vert0 to ray origin */
    tvec = _orig - _vert0;

    /* calculate U parameter and test bounds */
    _u = Inv_det * ::fwMath::dot(tvec, pvec);
    if (_u < 0.0 || _u > 1.0)
    {
        return false;
    }

    /* prepare to test V parameter */
    qvec = ::fwMath::cross(tvec, edge1);

    /* calculate V parameter and test bounds */
    _v = Inv_det * ::fwMath::dot(_dir, qvec);
    if (_v < 0.0 || _u + _v > 1.0)
    {
        return false;
    }

    /* calculate t, ray intersects triangle */
    _t = Inv_det * ::fwMath::dot(edge2, qvec);
    return true;
}

//------------------------------------------------------------------------------

bool IsInclosedVolume(const fwVertexPosition& _vertex, const fwVertexIndex& _vertexIndex, const fwVec3d& _p)
{
    const unsigned int X = 0, Y = 1, Z = 2;
    const size_t ElementNbr = _vertexIndex.size();
    if ( ElementNbr == 0 )
    {
        return false;
    }

    // on regarde tous les triangles du maillage
    unsigned int intersectionNbr = 0;
    for ( size_t i = 0; i < ElementNbr; ++i )
    {
        //recuperation des trois sommets du triangle
        const fwVec3d P1 =
        {_vertex[ _vertexIndex[i][0] ][0], _vertex[ _vertexIndex[i][0] ][1], _vertex[ _vertexIndex[i][0] ][2]};
        const fwVec3d P2 =
        {_vertex[ _vertexIndex[i][1] ][0], _vertex[ _vertexIndex[i][1] ][1], _vertex[ _vertexIndex[i][1] ][2]};
        const fwVec3d P3 =
        {_vertex[ _vertexIndex[i][2] ][0], _vertex[ _vertexIndex[i][2] ][1], _vertex[ _vertexIndex[i][2] ][2]};

        //on enleve les triangles s'ils sont situes au dessus du point
        if ( !(P1[Z] > _p[Z] && P2[Z] > _p[Z] && P3[Z] > _p[Z] ) ) //trianglePotentiallyWellPositionned
        {
            //on teste la presence des vertex de part et d'autre des 3 axes.
            //Si P1[X] > P[X] alors il faut necessairement P2[X] < P[X] ou P3[X] < P[X], idem pour les 2 autres axes
            //En outre cela permet d'exclure les points qui sont situes sur les axes
            bool stop = false;
            for ( unsigned int axe = X; axe <= Y && !stop; ++axe )
            {
                const double Delta1 = P1[axe] - _p[axe];
                const double Delta2 = P2[axe] - _p[axe];
                const double Delta3 = P3[axe] - _p[axe];

                if ( Delta1 >= 0.f && Delta2 >= 0.f && Delta3 >= 0.f )
                {
                    stop = true; break;
                }
                if ( Delta1 < 0.f && Delta2 < 0.f && Delta3 < 0.f )
                {
                    stop = true; break;
                }
            }
            if ( !stop )
            {

                fwVec3d orig = {_p[0], _p[1], _p[2]};

                fwVec3d dir = { 0.f, 0.f, 1.f};
                fwVec3d vert0 = { P1[0], P1[1], P1[2]};
                fwVec3d vert1 = { P2[0], P2[1], P2[2]};
                fwVec3d vert2 = { P3[0], P3[1], P3[2]};
                double t, u, v;
                if ( intersect_triangle(orig, dir, vert0, vert1, vert2, t, u, v) )
                {
                    //on ne garde que les points situes en dessous du point _p selon l'axe (Oz)
                    if (t < 0.f)
                    {
                        ++intersectionNbr;
                    }
                }
            }
        }
    }
    return ( intersectionNbr%2 == 1 );
}

//-----------------------------------------------------------------------------

bool isBorderlessSurface(const fwVertexIndex& _vertexIndex)
{
    typedef std::pair< int, int >  Edge; // always Edge.first < Edge.second !!
    typedef ::boost::unordered_map< Edge, int >  EdgeHistogram;
    EdgeHistogram edgesHistogram;
    bool isBorderless = true;

    for(const fwVertexIndex::value_type& vertex :  _vertexIndex)
    {
        SLM_ASSERT("Invalid vertex size: "<< vertex.size(), vertex.size() > 2 );
        ++edgesHistogram[std::make_pair(std::min(vertex[0], vertex[1]), std::max(vertex[0], vertex[1]) )];
        ++edgesHistogram[std::make_pair(std::min(vertex[0], vertex[2]), std::max(vertex[0], vertex[2]) )];
        ++edgesHistogram[std::make_pair(std::min(vertex[2], vertex[1]), std::max(vertex[2], vertex[1]) )];
    }

    for(const EdgeHistogram::value_type& histo :  edgesHistogram)
    {
        if (histo.second < 2)
        {
            isBorderless = false;
            break;
        }
    }

    return isBorderless;
}

//-----------------------------------------------------------------------------

// container of connected component
void findBorderEdges( const fwVertexIndex& _vertexIndex,
                      std::vector< std::vector<  std::pair< int, int  > > >& contours)
{
    typedef std::pair< int, int  >  Edge;
    typedef std::vector< Edge > Contour; // at Border
    typedef std::vector< Contour> Contours;

    std::map< Edge, int > edgesHistogram;
    for ( fwVertexIndex::value_type vertex : _vertexIndex)
    {
        assert(vertex.size() > 2 );
        int i1 = vertex[0];
        int i2 = vertex[1];
        int i3 = vertex[2];
        edgesHistogram[std::make_pair(std::min(i1, i2), std::max(i1, i2) )]++;
        edgesHistogram[std::make_pair(std::min(i1, i3), std::max(i1, i3) )]++;
        edgesHistogram[std::make_pair(std::min(i3, i2), std::max(i3, i2) )]++;
    }

    for ( const std::map< Edge, int >::value_type& elt1 : edgesHistogram )
    {
        if (elt1.second < 2) // an orphan found
        {
            Contour contour;
            contour.reserve(1000);
            std::list< Edge > fifo;
            Edge orphan = elt1.first;

            fifo.push_back(orphan);
            while( !fifo.empty() )
            {
                Edge current = fifo.front();
                contour.push_back( current );
                fifo.pop_front();
                edgesHistogram[current] = 2; // to mark it processed;

                // search neighbor at border and insert in fifo
                for ( const std::map< Edge, int >::value_type& elt2 : edgesHistogram )
                {
                    Edge candidate = elt2.first;
                    if ( elt2.second < 2 ) // at border
                    {
                        if ( candidate.first == current.first ||  candidate.second == current.second || // neighbor
                             candidate.first == current.second ||  candidate.second == current.first
                             )
                        {
                            edgesHistogram[candidate] = 2; // mark processed;
                            fifo.push_back( candidate );
                        }
                    }
                }
            }
            // all neighbor processed
            contours.push_back( contour );
        }
    }
}

//-----------------------------------------------------------------------------

bool closeSurface(  fwVertexPosition& _vertex, fwVertexIndex& _vertexIndex )
{
    typedef std::pair< int, int  >  Edge;
    typedef std::vector< Edge > Contour; // at Border
    typedef std::vector< Contour> Contours;

    Contours contours;
    findBorderEdges( _vertexIndex, contours);
    bool closurePerformed = !contours.empty();
    // close each hole
    for( const Contours::value_type& contour : contours )
    {
        size_t newVertexIndex = _vertex.size();
        // create gravity point & insert new triangle
        std::vector< float > massCenter(3, 0);

        for ( const Contour::value_type& edge : contour )
        {
            for (int i = 0; i < 3; ++i )
            {
                massCenter[i] += _vertex[edge.first][i];
                massCenter[i] += _vertex[edge.second][i];
            }
            // create new Triangle
            std::vector< int > triangleIndex(3);
            triangleIndex[0] = edge.first;
            triangleIndex[1] = edge.second;
            triangleIndex[2] = newVertexIndex;
            _vertexIndex.push_back( triangleIndex ); // TEST
        }
        for (int i = 0; i < 3; ++i )
        {
            massCenter[i] /= contour.size()*2;
        }
        _vertex.push_back( massCenter ); // normalize barycenter
    }
    return closurePerformed;
}

//-----------------------------------------------------------------------------

bool removeOrphanVertices( fwVertexPosition& _vertex, fwVertexIndex& _vertexIndex )
{
    fwVertexPosition newVertex;
    newVertex.reserve(  _vertex.size() );

    std::set< int > indexPointToKeep;

    for( const fwVertexIndex::value_type& vertex : _vertexIndex )
    {
        indexPointToKeep.insert( vertex[0] );
        indexPointToKeep.insert( vertex[1] );
        indexPointToKeep.insert( vertex[2] );
    }

    bool orphanFound = indexPointToKeep.size() != _vertex.size();

    if (orphanFound)
    {
        // rebuild index table according to element suppression
        int idx = 0;
        std::map< int, int > translate; // map oldIndex -> newIndex (to take into account removal
        std::set< int >::iterator idxIter;
        for (int indexPt : indexPointToKeep )
        {
            translate[ indexPt ] = idx++;
            newVertex.push_back(  _vertex[ indexPt ] );
        }

        for (fwVertexIndex::value_type& vertex : _vertexIndex )
        {
            vertex[0] = translate[ vertex[0]  ];
            vertex[1] = translate[ vertex[1]  ];
            vertex[2] = translate[ vertex[2]  ];
        }
        _vertex.swap(newVertex);
    }
    return orphanFound;
}

//-----------------------------------------------------------------------------

::glm::dvec3 toBarycentricCoord(const glm::dvec3& _P, const glm::dvec3& _A, const glm::dvec3& _B, const glm::dvec3& _C)
{
    ::glm::dvec3 baryCoord;

    const ::glm::dvec3 v0 = _B - _A; // AB Vector
    const ::glm::dvec3 v1 = _C - _A; // AC Vector
    const ::glm::dvec3 v2 = _P - _A; // AP Vector

    // Precompute some dot products.
    const double d00 = ::glm::dot(v0, v0);
    const double d01 = ::glm::dot(v0, v1);
    const double d11 = ::glm::dot(v1, v1);
    const double d20 = ::glm::dot(v2, v0);
    const double d21 = ::glm::dot(v2, v1);

    const double div = ((d00 * d11) - (d01 * d01));

    // Don't test the case in release to avoid performance issue.
    SLM_ASSERT("Degenerate triangle case leads to zero division.", !(div >= -10e-16 && div <= 10e-16));

    // Inverse the denominator to speed up computation of v & w.
    const double invdenom = 1. / div;

    // Barycentric coordinates
    const double v = ((d11 * d20) - (d01* d21)) * invdenom;
    const double w = ((d00 * d21) - (d01 * d20)) * invdenom;
    const double u = 1. - v - w; // deduce last coordinate from the two others.

    baryCoord.x = u;
    baryCoord.y = v;
    baryCoord.z = w;

    return baryCoord;
}

//-----------------------------------------------------------------------------

::glm::dvec4 toBarycentricCoord(const ::glm::dvec3& _P, const ::glm::dvec3& _A, const ::glm::dvec3& _B,
                                const ::glm::dvec3& _C,
                                const ::glm::dvec3& _D)
{

    /*
       In general, a point with barycentric coordinates (u, v, w,h) is inside (or on) the tetrahedron(ABCD)
          if and only if 0 ≤ u, v, w, h ≤ 1, or alternatively
          if and only if 0 ≤ v ≤ 1, 0 ≤ w ≤ 1, 0 ≤ h ≤ 1, and v + w + h ≤ 1.

       The main idea of the barycentric volume coordinate is a proportionality with the ratio of the sub-tetrahedron
          volumes over the whole volume. Considering one of the four vertexes (_A, _B, _C, _D), the associated
             barycentric
          coordinate are equal to the volume of the tetrahedron build with the three other vertexes and P,
          divided by the total tetrahedron volume.

       As a result, the principle in the present algorithm, is to compute the three tetrahedron (_A,_B,_C,_P)
          (_A,_B,_D_P) (_A,_C,_D,_P) volume and the (_A,_B,_C,_D) volume. Then the ratio for respectively,
          _D, _C, _B vertexes are computed, and the last barycentric coordinate is obtained by the formula
          u + v + w + h = 1
     */

    ::glm::dvec4 baryCoord;

    const ::glm::dvec3 vab = _B - _A; // AB Vector
    const ::glm::dvec3 vac = _C - _A; // AC Vector
    const ::glm::dvec3 vad = _D - _A; // AD Vector

    const ::glm::dvec3 vap = _P - _A; // AP Vector

    const double volumeB = ::glm::dot(vap, ::glm::cross(vac, vad));
    const double volumeC = ::glm::dot(vap, ::glm::cross(vad, vab));
    const double volumeD = ::glm::dot(vap, ::glm::cross(vab, vac));

    const double volumeTot = ::glm::dot(vab, ::glm::cross(vac, vad));

    // Don't test the case in release to avoid performance issue.
    SLM_ASSERT("Degenerate triangle case leads to zero division.", volumeTot != 0.);

    // Inverse the denominator to speed up computation of v & w.
    const double invdenom = 1. / volumeTot;

    // Barycentric coordinates
    const double v = volumeB * invdenom;
    const double w = volumeC * invdenom;
    const double h = volumeD * invdenom;
    const double u = 1. - v - w - h; // deduce last coordinate from the two others.

    baryCoord[0] = u;
    baryCoord[1] = v;
    baryCoord[2] = w;
    baryCoord[3] = h;

    return baryCoord;

}

//-----------------------------------------------------------------------------

::glm::dvec3 fromBarycentricCoord(const ::glm::dvec3& _baryCoord, const ::glm::dvec3& _A, const ::glm::dvec3& _B,
                                  const ::glm::dvec3& _C)
{
    ::glm::dvec3 worldCoordinates;

    // Use standard notation for clarity.
    const double u = _baryCoord[0];
    const double v = _baryCoord[1];
    const double w = _baryCoord[2];

    [[maybe_unused]] const double sum = u + v + w; // Only used in the following assertion.

    // Don't test in release to avoid performance issue.
    SLM_ASSERT("Wrong barycentric coordinates.(u + v + w = " + std::to_string( sum ) + ")"
               , sum < 1. + 10e-9 && sum > 1. - 10e-9);

    worldCoordinates = u * _A + v * _B + w * _C;

    return worldCoordinates;
}

//-----------------------------------------------------------------------------

::glm::dvec3 fromBarycentricCoord(const ::glm::dvec4& _baryCoord, const ::glm::dvec3& _A, const ::glm::dvec3& _B,
                                  const ::glm::dvec3& _C,
                                  const ::glm::dvec3& _D)
{
    /*
       General formula (if [u, v, w, h] is normalized).
       x = (u * _A.x + v * _B.x + w * _C.x + h * _D.x)
       y = (u * _A.y + v * _B.y + w * _C.y + h * _D.y)
       z = (u * _A.z + v * _B.z + w * _C.z + h * _D.z)
     */

    // Use standard notation for clarity.
    const double u = _baryCoord[0];
    const double v = _baryCoord[1];
    const double w = _baryCoord[2];
    const double h = _baryCoord[3];

    [[maybe_unused]] const double sum = u + v + w + h; // Only used in the following assertion.

    // Don't test in release to avoid performance issue.
    SLM_ASSERT("Wrong barycentric coordinates.(u + v + w = " + std::to_string( sum ) + ")"
               , sum < 1. + 10e-9 && sum > 1. - 10e-9);

    return u * _A + v * _B + w * _C + h * _D;

}

//------------------------------------------------------------------------------

bool isInsideTetrahedron(const ::glm::dvec3& _P, const ::glm::dvec3& _A,
                         const ::glm::dvec3& _B, const ::glm::dvec3& _C, const ::glm::dvec3& _D)
{

    /*
       There are several ways to determine if a point is inside a tetrahedron.
       The present algorithm make use of the barycentric coordinates.
       It first computes the barycentric coordinate of the point inside the tetrahedron, and then checks if all of them
          are
          in between 0 and 1.
     */
    const ::glm::dvec4 barycentricCoord = toBarycentricCoord(_P, _A, _B, _C, _D);
    return isInsideTetrahedron(barycentricCoord);
}
//------------------------------------------------------------------------------

bool isInsideTetrahedron( const ::glm::dvec4 barycentricCoordPInsideABCD)
{

    /*
       There are several ways to determine if a point is inside a tetrahedron.
       The present algorithm make use of the barycentric coordinates.
       It checks if all of the barycentric coordinates are in between 0 and 1.

     */
    return 0 <= barycentricCoordPInsideABCD[0] &&  barycentricCoordPInsideABCD[0] <= 1
           &&  0 <= barycentricCoordPInsideABCD[1] &&  barycentricCoordPInsideABCD[1] <= 1
           &&  0 <= barycentricCoordPInsideABCD[2] &&  barycentricCoordPInsideABCD[2] <= 1
           &&  0 <= barycentricCoordPInsideABCD[3] &&  barycentricCoordPInsideABCD[3] <= 1;
}

//-----------------------------------------------------------------------------

} // namespace fwMath
