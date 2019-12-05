/************************************************************************
 *
 * Copyright (C) 2019 IRCAD France
 * Copyright (C) 2019 IHU Strasbourg
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

#include "fwData/config.hpp"

#include <fwMemory/BufferObject.hpp>

#include <iterator>

namespace fwData
{
class Mesh;

namespace iterator
{

struct Point {
    float x;
    float y;
    float z;
};

struct Normal {
    float nx;
    float ny;
    float nz;
};

struct RGBA {
    std::uint8_t r;
    std::uint8_t g;
    std::uint8_t b;
    std::uint8_t a;
};

struct TexCoords {
    float u;
    float v;
};

/**
 * @brief Base class for mesh's point iterators
 *
 * Iterate through the point arrays and check if the index is not out of the bounds
 */
template<bool isConst>
class PointIteratorBase
{
public:

    typedef typename std::conditional<isConst, const Point, Point>::type point_value_type;
    typedef point_value_type& point_reference;

    typedef typename std::conditional<isConst, const RGBA, RGBA>::type color_value_type;
    typedef color_value_type& color_reference;

    typedef typename std::conditional<isConst, const Normal, Normal>::type normal_value_type;
    typedef normal_value_type& normal_reference;

    typedef typename std::conditional<isConst, const TexCoords, TexCoords>::type tex_value_type;
    typedef tex_value_type& tex_reference;

    struct PointInfo {
        point_value_type* point{nullptr};
        normal_value_type* normal{nullptr};
        color_value_type* color{nullptr};
        tex_value_type* tex{nullptr};
    };

    /**
     * @name Typedefs required by std::iterator_traits
     * @{
     */
    typedef typename std::conditional<isConst, const PointInfo, PointInfo>::type value_type;
    typedef value_type* pointer;
    typedef value_type& reference;
    typedef std::ptrdiff_t difference_type;
    typedef std::random_access_iterator_tag iterator_category;
    /// @}

    /// Constructor
    PointIteratorBase();
    /// Destructor
    virtual ~PointIteratorBase();
    /// Copy constructor
    PointIteratorBase(const PointIteratorBase& other) = default;

    PointIteratorBase& operator=(const PointIteratorBase& other) = default;

    /// Comparison operators
    bool operator==(const PointIteratorBase& other) const;
    bool operator!=(const PointIteratorBase& other) const;
    bool operator<(const PointIteratorBase& other) const;
    bool operator<=(const PointIteratorBase& other) const;
    bool operator>(const PointIteratorBase& other) const;
    bool operator>=(const PointIteratorBase& other) const;

    /// Increment/Decrement operators
    PointIteratorBase& operator++();
    PointIteratorBase operator++(int);
    PointIteratorBase operator+(difference_type index) const;
    PointIteratorBase& operator+=(difference_type index);
    PointIteratorBase& operator--();
    PointIteratorBase operator--(int);
    PointIteratorBase operator-(difference_type index) const;
    PointIteratorBase& operator-=(difference_type index);

    difference_type operator+(const PointIteratorBase& other) const;
    difference_type operator-(const PointIteratorBase& other) const;
    reference operator*();
    pointer operator->();

protected:

    friend class ::fwData::Mesh;
    friend class PointIterator;
    friend class ConstPointIterator;

    PointInfo m_pointInfo;
    std::vector< ::fwMemory::BufferObject::Lock > m_locks;
    difference_type m_idx{0};
    difference_type m_numberOfElements{0};
};

/**
 * @brief Iterator on Mesh points, point colors, point normals and/or point texture coordinates
 *
 * Iterate through the point buffers and check if the index is not out of the bounds
 *
 * Example:
 * @code{.cpp}
    ::fwData::Mesh::sptr mesh = ::fwData::Mesh::New();
    mesh->resize(25, 33, ::fwData::Mesh::TRIANGLE);
    auto iter    = mesh->begin< ::fwData::iterator::PointIterator >();
    const auto iterEnd = mesh->end< ::fwData::iterator::PointIterator >();
    float p[3] = {12.f, 16.f, 18.f};

   for (; iter != iterEnd; ++iter)
   {
       iter->point->x = p[0];
       iter->point->y = p[1];
       iter->point->z = p[2];
   }
   @endcode
 */
class FWDATA_CLASS_API PointIterator : public PointIteratorBase<false>
{
public:

    /// Constructor
    FWDATA_API PointIterator(Mesh* mesh);
    FWDATA_API PointIterator(const PointIterator& other) = default;
    FWDATA_API PointIterator(const PointIteratorBase& other);
    FWDATA_API virtual ~PointIterator() override;

    FWDATA_API PointIterator& operator=(const PointIterator& other) = default;
    FWDATA_API PointIterator& operator=(const PointIteratorBase& other);

    friend class ConstPointIterator;
};

/**
 * @brief Const iterator on Mesh points, point colors, point normals and/or point texture coordinates
 *
 * Iterate through the point buffers and check if the index is not out of the bounds
 *
 * Example:
 * @code{.cpp}
    ::fwData::Mesh::sptr mesh = ::fwData::Mesh::New();
    mesh->resize(25, 33, ::fwData::Mesh::TRIANGLE);
    auto iter    = mesh->begin< ::fwData::iterator::PointIterator >();
    const auto iterEnd = mesh->end< ::fwData::iterator::PointIterator >();
    float p[3];

   for (; iter != iterEnd; ++iter)
   {
       p[0] = iter->point->x;
       p[1] = iter->point->y;
       p[2] = iter->point->z;
   }
   @endcode
 */
class FWDATA_CLASS_API ConstPointIterator : public PointIteratorBase<true>
{
public:
    /// Constructor
    FWDATA_API ConstPointIterator(const Mesh* mesh);
    FWDATA_API ConstPointIterator(const ConstPointIterator& other) = default;
    FWDATA_API ConstPointIterator(const PointIteratorBase& other);
    FWDATA_API virtual ~ConstPointIterator() override;

    FWDATA_API ConstPointIterator& operator=(const ConstPointIterator& other) = default;
    FWDATA_API ConstPointIterator& operator=(const PointIteratorBase& other);
};

/**
 * @brief Base class for mesh's cells iterators
 *
 * Iterate through the cell arrays and check if the index is not out of the bounds
 */
template<bool isConst>
class CellIteratorBase
{
public:

    typedef typename std::conditional<isConst, const std::uint64_t, std::uint64_t>::type cell_data_value_type;
    typedef typename std::conditional<isConst, const std::uint8_t, std::uint8_t>::type cell_type_value_type;
    typedef typename std::conditional<isConst, const std::uint64_t, std::uint64_t>::type cell_offset_value_type;

    typedef typename std::conditional<isConst, const RGBA, RGBA>::type color_value_type;
    typedef color_value_type& color_reference;

    typedef typename std::conditional<isConst, const Normal, Normal>::type normal_value_type;
    typedef normal_value_type& normal_reference;

    typedef typename std::conditional<isConst, const TexCoords, TexCoords>::type tex_value_type;
    typedef tex_value_type& tex_reference;

    struct CellInfo {
        cell_data_value_type* pointIdx{nullptr};
        cell_type_value_type* type{nullptr};
        cell_offset_value_type* offset{nullptr};
        normal_value_type* normal{nullptr};
        color_value_type* color{nullptr};
        tex_value_type* tex{nullptr};
        size_t nbPoints{0};
    };

    /**
     * @name Typedefs required by std::iterator_traits
     * @{
     */
    typedef typename std::conditional<isConst, const CellInfo, CellInfo>::type value_type;
    typedef value_type* pointer;
    typedef value_type& reference;
    typedef std::ptrdiff_t difference_type;
    typedef std::random_access_iterator_tag iterator_category;

    /// @}

    /// Constructor
    CellIteratorBase();
    /// Destructor
    virtual ~CellIteratorBase();
    /// Copy constructor
    CellIteratorBase(const CellIteratorBase& other)            = default;
    CellIteratorBase& operator=(const CellIteratorBase& other) = default;

    /// Comparison operators
    bool operator==(const CellIteratorBase& other) const;
    bool operator!=(const CellIteratorBase& other) const;
    bool operator<(const CellIteratorBase& other) const;
    bool operator<=(const CellIteratorBase& other) const;
    bool operator>(const CellIteratorBase& other) const;
    bool operator>=(const CellIteratorBase& other) const;

    /// Increment/Decrement operators
    CellIteratorBase& operator++();
    CellIteratorBase operator++(int);
    CellIteratorBase operator+(difference_type index) const;
    CellIteratorBase& operator+=(difference_type index);
    CellIteratorBase& operator--();
    CellIteratorBase operator--(int);
    CellIteratorBase operator-(difference_type index) const;
    CellIteratorBase& operator-=(difference_type index);

    difference_type operator+(const CellIteratorBase& other) const;
    difference_type operator-(const CellIteratorBase& other) const;

    reference operator*();
    pointer operator->();

    cell_data_value_type& operator[](size_t index);
    size_t nbPoints() const;

protected:

    friend class ::fwData::Mesh;
    friend class CellIterator;
    friend class ConstCellIterator;

    CellInfo m_cellInfo;
    std::vector< ::fwMemory::BufferObject::Lock > m_locks;
    difference_type m_idx{0};
    difference_type m_numberOfElements{0};
    std::uint64_t m_cellDataSize{0};
    difference_type m_currentOffset{0};
};

/**
 * @brief Iterator on Mesh cells, cell colors, cell normals and/or cell texture coordinates
 *
 * Iterate through the buffers and check if the index is not out of the bounds
 *
 * Example:
 * @code{.cpp}
    ::fwData::Mesh::sptr mesh = ::fwData::Mesh::New();
    mesh->resize(25, 33, TRIANGLE);
    PointIteratorBase<PointIteration> iter    = img->begin<PointIteration>();
    const PointIteratorBase<PointIteration> iterEnd = img->end<PointIteration>();

    for (; iter != iterEnd; ++iter)
    {
        const size_t nbPoints = iter.nbPoints();
        for (int i = 0; i< nbPoints ; ++i)
        {
            const auto pointIdx = iter.pointIdx(i);
            //...
        }
    }
   @endcode
 */
class FWDATA_CLASS_API CellIterator : public CellIteratorBase<false>
{
public:

    friend class ConstCellIterator;

    /// Constructor
    FWDATA_API CellIterator(::fwData::Mesh* mesh);
    FWDATA_API CellIterator(const CellIterator& other) = default;
    FWDATA_API virtual ~CellIterator() override;

    FWDATA_API CellIterator& operator=(const CellIterator& other) = default;
    FWDATA_API CellIterator& operator=(const CellIteratorBase& other);
};

/**
 * @brief Iterator on Mesh cells, cell colors, cell normals and/or cell texture coordinates
 *
 * Iterate through the buffer and check if the index is not out of the bounds
 *
 * Example:
 * @code{.cpp}
    ::fwData::Mesh::sptr mesh = ::fwData::Mesh::New();
    mesh->resize(25, 33, TRIANGLE);
    PointIteratorBase<PointIteration> iter    = img->begin<PointIteration>();
    const PointIteratorBase<PointIteration> iterEnd = img->end<PointIteration>();

    for (; iter != iterEnd; ++iter)
    {
        const size_t nbPoints = iter.nbPoints();
        for (int i = 0; i< nbPoints ; ++i)
        {
            const auto pointIdx = iter.pointIdx(i);
            //...
        }
    }
   @endcode
 */
class FWDATA_CLASS_API ConstCellIterator : public CellIteratorBase<true>
{
public:

    /// Constructor
    FWDATA_API ConstCellIterator(const ::fwData::Mesh* mesh);
    FWDATA_API ConstCellIterator(const ConstCellIterator& other) = default;
    FWDATA_API ConstCellIterator(const CellIterator& other);
    FWDATA_API virtual ~ConstCellIterator() override;

    FWDATA_API ConstCellIterator& operator=(const ConstCellIterator& other) = default;
    FWDATA_API ConstCellIterator& operator=(const CellIteratorBase& other);
};

} // namespace iterator
} // namespace fwData

#include "fwData/iterator/MeshIterators.hxx"
