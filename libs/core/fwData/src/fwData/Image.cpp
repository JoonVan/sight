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

#include "fwData/Image.hpp"

#include "fwData/Exception.hpp"
#include "fwData/registry/macros.hpp"

#include <fwCom/Signal.hxx>

#include <fwTools/DynamicType.hpp>
#include <fwTools/DynamicTypeKeyTypeMapping.hpp>

#include <numeric>

//------------------------------------------------------------------------------

fwDataRegisterMacro( ::fwData::Image );

//------------------------------------------------------------------------------

namespace fwData
{

const ::fwCom::Signals::SignalKeyType Image::s_BUFFER_MODIFIED_SIG       = "bufferModified";
const ::fwCom::Signals::SignalKeyType Image::s_LANDMARK_ADDED_SIG        = "landmarkAdded";
const ::fwCom::Signals::SignalKeyType Image::s_LANDMARK_REMOVED_SIG      = "landmarkRemoved";
const ::fwCom::Signals::SignalKeyType Image::s_LANDMARK_DISPLAYED_SIG    = "landmarkDisplayed";
const ::fwCom::Signals::SignalKeyType Image::s_DISTANCE_ADDED_SIG        = "distanceAdded";
const ::fwCom::Signals::SignalKeyType Image::s_DISTANCE_REMOVED_SIG      = "distanceRemoved";
const ::fwCom::Signals::SignalKeyType Image::s_DISTANCE_DISPLAYED_SIG    = "distanceDisplayed";
const ::fwCom::Signals::SignalKeyType Image::s_SLICE_INDEX_MODIFIED_SIG  = "sliceIndexModified";
const ::fwCom::Signals::SignalKeyType Image::s_SLICE_TYPE_MODIFIED_SIG   = "sliceTypeModified";
const ::fwCom::Signals::SignalKeyType Image::s_VISIBILITY_MODIFIED_SIG   = "visibilityModified";
const ::fwCom::Signals::SignalKeyType Image::s_TRANSPARENCY_MODIFIED_SIG = "transparencyModified";

//------------------------------------------------------------------------------

Image::Image(::fwData::Object::Key) :
    m_dataArray( ::fwData::Array::New() )
{
    newSignal< BufferModifiedSignalType >(s_BUFFER_MODIFIED_SIG);
    newSignal< LandmarkAddedSignalType >(s_LANDMARK_ADDED_SIG);
    newSignal< LandmarkRemovedSignalType >(s_LANDMARK_REMOVED_SIG);
    newSignal< LandmarkDisplayedSignalType >(s_LANDMARK_DISPLAYED_SIG);
    newSignal< DistanceDisplayedSignalType >(s_DISTANCE_DISPLAYED_SIG);
    newSignal< DistanceAddedSignalType >(s_DISTANCE_ADDED_SIG);
    newSignal< DistanceRemovedSignalType >(s_DISTANCE_REMOVED_SIG);
    newSignal< SliceIndexModifiedSignalType >(s_SLICE_INDEX_MODIFIED_SIG);
    newSignal< SliceTypeModifiedSignalType >(s_SLICE_TYPE_MODIFIED_SIG);
    newSignal< VisibilityModifiedSignalType >(s_VISIBILITY_MODIFIED_SIG);
    newSignal< TransparencyModifiedSignalType >(s_TRANSPARENCY_MODIFIED_SIG);
}

//------------------------------------------------------------------------------

Image::~Image() noexcept
{
}

//-----------------------------------------------------------------------------

void Image::shallowCopy(const Object::csptr& _source )
{
    Image::csptr other = Image::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );
    this->fieldShallowCopy( _source );

    // Assign
    copyInformation( other );

    m_dataArray = other->m_dataArray;
}

//-----------------------------------------------------------------------------

void Image::cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache)
{
    Image::csptr other = Image::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !other );
    this->fieldDeepCopy( _source, cache );

    // Assign
    this->copyInformation( other );

    if( other->m_dataArray )
    {
        m_dataArray->cachedDeepCopy(other->m_dataArray, cache);
    }
}

//------------------------------------------------------------------------------

size_t Image::resize()
{
    if (!m_dataArray)
    {
        m_dataArray = ::fwData::Array::New();
    }

    SLM_ASSERT( "NumberOfComponents must be > 0", m_numberOfComponents > 0 );

    const size_t imageDims = this->getNumberOfDimensions();

    ::fwData::Array::SizeType arraySize(imageDims);

    /*
     * @todo replace the resizeTMP method by these line.
     * Array m_numberOfComponents attribute is deprecated, but to support the old API, we need to use it temporary
     *
     * The component dimension must be the first dimentsion to be registered in the array like RGBARGBARGBA.
     *
     * size_t count = 0;
     * if (m_numberOfComponents > 1)
     * {
     *     arraySize.resize(imageDims+1);
     *     arraySize[0] = m_numberOfComponents;
     *     count        = 1;
     * }
     * for (size_t i = 0; i < imageDims; ++i)
     * {
     *     arraySize[count] = m_size[i];
     *     ++count;
     * }
     */

    for (size_t i = 0; i < imageDims; ++i)
    {
        arraySize[i] = m_size[i];
    }
    return m_dataArray->resizeTMP(m_type, arraySize, m_numberOfComponents);
}

//------------------------------------------------------------------------------

size_t Image::resize(IndexType x, IndexType y,  IndexType z, const ::fwTools::Type& type, PixelFormat format)
{
    return resize({ x, y, z}, type, format);
}
//------------------------------------------------------------------------------

size_t Image::resize(const Size& size, const ::fwTools::Type& type, PixelFormat format)
{
    m_size        = size;
    m_type        = type;
    m_pixelFormat = format;

    switch (format)
    {
        case PixelFormat::GRAY_SCALE:
            m_numberOfComponents = 1;
            break;
        case PixelFormat::RGB:
        case PixelFormat::BGR:
            m_numberOfComponents = 3;
            break;
        case PixelFormat::RGBA:
        case PixelFormat::BGRA:
            m_numberOfComponents = 4;
            break;
        default:
            m_numberOfComponents = 1;
    }

    return resize();
}

//------------------------------------------------------------------------------

::fwTools::Type Image::getType() const
{
    return m_type;
}

//------------------------------------------------------------------------------

void Image::setType(::fwTools::Type type)
{
    m_type = type;
}

//------------------------------------------------------------------------------

void Image::setType(const std::string& type)
{
    m_type = ::fwTools::Type(type);
}

//------------------------------------------------------------------------------

void Image::copyInformation( Image::csptr _source )
{
    m_size               = _source->m_size;
    m_spacing            = _source->m_spacing;
    m_origin             = _source->m_origin;
    m_type               = _source->m_type;
    m_windowCenter       = _source->m_windowCenter;
    m_windowWidth        = _source->m_windowWidth;
    m_numberOfComponents = _source->m_numberOfComponents;
    m_pixelFormat        = _source->m_pixelFormat;
}

//------------------------------------------------------------------------------

size_t Image::getNumberOfDimensions() const
{
    size_t dims = 0;

    for (const auto& val: m_size)
    {
        if(val > 0)
        {
            ++dims;
        }
        else
        {
            break;
        }
    }

    return dims;
}

//------------------------------------------------------------------------------

size_t Image::getSizeInBytes() const
{
    size_t size       = 0;
    const size_t dims = this->getNumberOfDimensions();
    if (dims > 0)
    {
        size = std::accumulate(
            m_size.begin(), m_size.begin() + dims,
            static_cast<size_t>(m_type.sizeOf()) * m_numberOfComponents,
            std::multiplies< size_t > () );
    }
    return size;
}

//------------------------------------------------------------------------------

size_t Image::getAllocatedSizeInBytes() const
{
    size_t size = 0;
    if (m_dataArray)
    {
        size = m_dataArray->getSizeInBytes();
    }
    return size;
}

//------------------------------------------------------------------------------

::fwMemory::BufferObject::Lock Image::lock() const
{
    return m_dataArray->lock();
}

//------------------------------------------------------------------------------

void Image::lockBuffer(std::vector< ::fwMemory::BufferObject::Lock >& locks) const
{
    locks.push_back(this->lock());
}

//-----------------------------------------------------------------------------

void* Image::getBuffer()
{
    return m_dataArray->getBuffer();
}

//-----------------------------------------------------------------------------

void* Image::getBuffer() const
{
    return m_dataArray->getBuffer();
}

//------------------------------------------------------------------------------

void* Image::getPixelBuffer( IndexType index )
{
    const size_t imagePixelSize = m_type.sizeOf() * m_numberOfComponents;
    BufferType* buf             = static_cast < BufferType* >(this->getBuffer());
    const IndexType bufIndex    = index * imagePixelSize;
    return buf + bufIndex;
}

//------------------------------------------------------------------------------

void* Image::getPixelBuffer( IndexType index ) const
{
    const size_t imagePixelSize = m_type.sizeOf() * m_numberOfComponents;
    BufferType* buf             = static_cast < BufferType* >(this->getBuffer());
    const IndexType bufIndex    = index * imagePixelSize;
    return buf + bufIndex;
}

//------------------------------------------------------------------------------

void Image::setPixelBuffer( IndexType index, Image::BufferType* pixBuf)
{
    const size_t imagePixelSize = m_type.sizeOf() * m_numberOfComponents;
    BufferType* buf             = static_cast < BufferType* >(this->getPixelBuffer(index));

    std::copy(pixBuf, pixBuf+imagePixelSize, buf);
}
//------------------------------------------------------------------------------

const std::string Image::getPixelAsString(IndexType x,
                                          IndexType y,
                                          IndexType z ) const
{
    const IndexType offset = x + m_size[0]*y + z*m_size[0]*m_size[1];
    return m_type.toString(this->getPixelBuffer(offset));
}

//------------------------------------------------------------------------------

Image::Iterator<char> Image::begin()
{
    return Iterator<char>(this);
}

//------------------------------------------------------------------------------

Image::Iterator<char> Image::end()
{
    auto itr = Iterator<char>(this);
    itr += static_cast< typename Iterator<char>::difference_type>(this->getSizeInBytes());
    return itr;
}

//------------------------------------------------------------------------------

Image::ConstIterator<char> Image::begin() const
{
    return ConstIterator<char>(this);
}

//------------------------------------------------------------------------------

Image::ConstIterator<char> Image::end() const
{
    auto itr = ConstIterator<char>(this);
    itr += static_cast< typename Iterator<char>::difference_type>(this->getSizeInBytes());
    return itr;
}

//------------------------------------------------------------------------------

size_t Image::getNumElements() const
{
    size_t nbElts = 0;
    if (m_size[0] > 0)
    {
        nbElts = m_numberOfComponents;
        for (const auto& val: m_size)
        {
            if (val > 0)
            {
                nbElts *= val;
            }
            else
            {
                break;
            }
        }
    }
    return nbElts;
}

//------------------------------------------------------------------------------

void Image::setBuffer(
    void* buf,
    bool takeOwnership,
    const ::fwTools::Type& type,
    const ::fwData::Image::Size& size,
    ::fwMemory::BufferAllocationPolicy::sptr policy)
{
    m_type = type;
    m_size = size;
    this->resize();
    this->setBuffer(buf, takeOwnership, policy);
}

//------------------------------------------------------------------------------

void Image::setBuffer(void* buf, bool takeOwnership, ::fwMemory::BufferAllocationPolicy::sptr policy)
{
    if(m_dataArray->getIsBufferOwner())
    {
        if(!m_dataArray->getBufferObject()->isEmpty())
        {
            m_dataArray->getBufferObject()->destroy();
        }
    }
    else
    {
        ::fwMemory::BufferObject::sptr newBufferObject = ::fwMemory::BufferObject::New();
        ::fwMemory::BufferObject::sptr oldBufferObject = m_dataArray->getBufferObject();
        oldBufferObject->swap(newBufferObject);
    }
    m_dataArray->getBufferObject()->setBuffer(buf, (buf == NULL) ? 0 : m_dataArray->getSizeInBytes(), policy);
    m_dataArray->setIsBufferOwner(takeOwnership);
}

//------------------------------------------------------------------------------

::fwMemory::BufferObject::sptr Image::getBufferObject()
{
    return m_dataArray->getBufferObject();
}

//------------------------------------------------------------------------------

::fwMemory::BufferObject::csptr Image::getBufferObject() const
{
    return m_dataArray->getBufferObject();
}

//------------------------------------------------------------------------------

void Image::setIStreamFactory(const SPTR(::fwMemory::stream::in::IFactory)& factory,
                              const size_t size,
                              const std::filesystem::path& sourceFile,
                              const ::fwMemory::FileFormatType format,
                              const ::fwMemory::BufferAllocationPolicy::sptr& policy)
{
    const auto imageDims = this->getNumberOfDimensions();
    ::fwData::Array::SizeType arraySize(imageDims);
    size_t count = 0;
    if (m_numberOfComponents > 1)
    {
        arraySize.resize(imageDims+1);
        arraySize[0] = m_numberOfComponents;
        count        = 1;
    }
    for (size_t i = 0; i < imageDims; ++i)
    {
        arraySize[count] = m_size[i];
        ++count;
    }

    m_dataArray->resize(arraySize, m_type, false);
    m_dataArray->getBufferObject()->setIStreamFactory(factory, size, sourceFile, format, policy);
}

//------------------------------------------------------------------------------
// Deprecated API
//------------------------------------------------------------------------------

::fwData::Array::sptr Image::getDataArray() const
{
    return m_dataArray;
}

//------------------------------------------------------------------------------

void Image::setDataArray(::fwData::Array::sptr array, bool copyArrayInfo)
{
    if( !array )
    {
        array = ::fwData::Array::New();
    }
    m_dataArray = array;
    if (copyArrayInfo)
    {
        FW_RAISE_EXCEPTION_IF(::fwData::Exception("Data array must have a maximum of 3 dimensions"),
                              array->getNumberOfDimensions() > 3);

        const ::fwData::Array::SizeType arraySize = array->getSize();
        for (size_t i = 0; i < arraySize.size(); ++i)
        {
            m_size[i] = arraySize[i];
        }
        m_type = array->getType();
    }
}

//------------------------------------------------------------------------------

size_t Image::allocate()
{
    return this->resize();
}
//------------------------------------------------------------------------------

size_t Image::allocate(SizeType::value_type x, SizeType::value_type y,  SizeType::value_type z,
                       const ::fwTools::Type& type, size_t numberOfComponents)
{
    m_size               = { x, y, z};
    m_type               = type;
    m_numberOfComponents = numberOfComponents;
    return this->resize();
}

//------------------------------------------------------------------------------

size_t Image::allocate(const SizeType& size, const ::fwTools::Type& type, size_t numberOfComponents)
{
    this->setSize(size);
    m_type               = type;
    m_numberOfComponents = numberOfComponents;
    return this->resize();
}

//------------------------------------------------------------------------------

const Image::SpacingType& Image::getSpacing() const
{
    const size_t dims = this->getNumberOfDimensions();
    if (m_oldSpacing.size() != dims)
    {
        m_oldSpacing.resize(dims);
    }
    for (size_t i = 0; i < dims; ++i)
    {
        m_oldSpacing[i] = m_spacing[i];
    }
    return m_oldSpacing;
}

//------------------------------------------------------------------------------

void Image::setSpacing(const SpacingType& spacing)
{
    FW_RAISE_EXCEPTION_IF(::fwData::Exception("Spacing must have a maximum of 3 dimensions"),
                          spacing.size() > 3);

    for (size_t i = 0; i < 3; ++i)
    {
        if (i < spacing.size())
        {
            m_spacing[i] = spacing[i];
        }
        else
        {
            m_spacing[i] = 0.;
        }
    }
}

//------------------------------------------------------------------------------

const Image::OriginType& Image::getOrigin() const
{
    const size_t dims = this->getNumberOfDimensions();
    if (m_oldOrigin.size() != dims)
    {
        m_oldOrigin.resize(dims);
    }
    for (size_t i = 0; i < dims; ++i)
    {
        m_oldOrigin[i] = m_origin[i];
    }
    return m_oldOrigin;
}

//------------------------------------------------------------------------------

void Image::setOrigin(const OriginType& origin)
{
    FW_RAISE_EXCEPTION_IF(::fwData::Exception("Origin must have a maximum of 3 dimensions"),
                          origin.size() > 3);

    for (size_t i = 0; i < 3; ++i)
    {
        if (i < origin.size())
        {
            m_origin[i] = origin[i];
        }
        else
        {
            m_origin[i] = 0.;
        }
    }
}

//------------------------------------------------------------------------------

const Image::SizeType& Image::getSize() const
{
    const size_t dims = this->getNumberOfDimensions();
    if (m_oldSize.size() != dims)
    {
        m_oldSize.resize(dims);
    }
    for (size_t i = 0; i < dims; ++i)
    {
        m_oldSize[i] = m_size[i];
    }

    return m_oldSize;
}

//------------------------------------------------------------------------------

void Image::setSize(const SizeType& size)
{
    FW_RAISE_EXCEPTION_IF(::fwData::Exception("Origin must have a maximum of 3 dimensions"),
                          size.size() > 3);

    for (size_t i = 0; i < 3; ++i)
    {
        if (i < size.size())
        {
            m_size[i] = size[i];
        }
        else
        {
            m_size[i] = 0;
        }
    }
}

//------------------------------------------------------------------------------

::fwTools::DynamicType Image::getPixelType() const
{
    typedef std::map<std::string, ::fwTools::DynamicType> DynamicTypeMapType;

    static DynamicTypeMapType dynamicTypeMap = {
        { ::fwTools::Type().string(), ::fwTools::DynamicType() },
        { "uint8",  ::fwTools::makeDynamicType<std::string>("unsigned char")  },
        { "uint16", ::fwTools::makeDynamicType<std::string>("unsigned short") },
        { "uint32", ::fwTools::makeDynamicType<std::string>("unsigned int")   },
        { "int8",   ::fwTools::makeDynamicType<std::string>("signed char")    },
        { "int16",  ::fwTools::makeDynamicType<std::string>("signed short")   },
        { "int32",  ::fwTools::makeDynamicType<std::string>("signed int")     },
        { "float",  ::fwTools::makeDynamicType<std::string>("float")          },
        { "double", ::fwTools::makeDynamicType<std::string>("double")         },

//special case for dynamic type : 64bits integers was not managed by dynamic type.
#if ( INT_MAX < LONG_MAX )
        {"uint64",  ::fwTools::makeDynamicType<std::string>("unsigned long")  },
        {"int64",   ::fwTools::makeDynamicType<std::string>("signed long")    },
#else
        {"uint32",  ::fwTools::makeDynamicType<std::string>("unsigned long")  },
        {"int32",   ::fwTools::makeDynamicType<std::string>("signed long")    },
        {"uint64",  ::fwTools::DynamicType() },
        {"int64",   ::fwTools::DynamicType() },
#endif
    };

    ::fwTools::DynamicType dtype = dynamicTypeMap[getType().string()];
    return dtype;
}

//------------------------------------------------------------------------------

} // namespace fwData
