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

#pragma once

#include "arData/config.hpp"
#include "arData/TimeLine.hpp"
#include "arData/timeline/Buffer.hpp"

#include <boost/array.hpp>
#include <boost/pool/poolfwd.hpp>

fwCampAutoDeclareDataMacro((arData)(BufferTL));

namespace arData
{
/**
 * @brief   This class defines a timeline of buffers. It implements basic features of the Timeline interface such as
 *          pushing or retrieving objects. Allocation must be done by inherited classes.
 */
class ARDATA_CLASS_API BufferTL : public TimeLine
{

public:
    fwCoreClassMacro(BufferTL, ::fwData::Object)

    typedef ::fwCore::HiResClock::HiResClockType TimestampType;
    typedef std::map< TimestampType, SPTR(::arData::timeline::Buffer) > TimelineType;
    typedef std::pair< TimestampType, SPTR(::arData::timeline::Buffer) > BufferPairType;
    typedef ::boost::pool<> PoolType;

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    ARDATA_API BufferTL( ::fwData::Object::Key key );

    /// Destructor
    ARDATA_API virtual ~BufferTL();

    /// Check if the type of an object is compatible with this timeline
    ARDATA_API virtual bool isObjectValid(const CSPTR(::arData::timeline::Object)& obj) const = 0;

    /**
     * @brief Return the closest object to the given timestamp
     * @param timestamp timestamp used to find the closest object
     * @param direction direction to find the closest object (PAST, FUTURE, BOTH)
     */
    ARDATA_API virtual CSPTR(::arData::timeline::Object) getClosestObject(
        ::fwCore::HiResClock::HiResClockType timestamp, DirectionType direction = BOTH) const override;

    /// Return the object matching the specified timestamp, returns NULL if object is not found
    ARDATA_API virtual CSPTR(::arData::timeline::Object) getObject(::fwCore::HiResClock::HiResClockType timestamp)
    const override;

    /// Clear the timeline
    ARDATA_API virtual void clearTimeline();

    /// Push a buffer to the timeline
    ARDATA_API virtual void pushObject(const SPTR(::arData::timeline::Object)& obj) override;

    /// Remove a buffer to the timeline
    ARDATA_API virtual SPTR(::arData::timeline::Object) popObject(TimestampType timestamp) override;

    /// Change a buffer timestamp to the timeline
    ARDATA_API virtual void modifyTime(TimestampType timestamp, TimestampType newTimestamp) override;

    /// Change a buffer object to the specified timestamp
    ARDATA_API virtual void setObject(TimestampType timestamp, const SPTR(::arData::timeline::Object)& obj) override;

    /// Return the last object in the timeline
    ARDATA_API CSPTR(::arData::timeline::Object) getNewerObject() const;

    /// Return the last timestamp in the timeline
    ARDATA_API ::fwCore::HiResClock::HiResClockType getNewerTimestamp() const;

    /// Change the maximum size of the timeline
    void setMaximumSize(size_t maximumSize)
    {
        m_maximumSize = maximumSize;
    }

    /// Default Timeline Size
    ARDATA_API static const size_t s_DEFAULT_TIMELINE_MAX_SIZE;

    /// Return true if the pool is allocated
    bool isAllocated() const
    {
        return m_pool != nullptr;
    }

protected:

    /// Allocate the pool buffer.
    ARDATA_API void allocPoolSize(std::size_t size);

    /// Mutex to protect m_timeline and m_pool access
    mutable ::fwCore::mt::ReadWriteMutex m_tlMutex;

    ///Timeline
    TimelineType m_timeline;

    /// Pool of buffer
    SPTR(PoolType) m_pool;

    /// maximum size
    size_t m_maximumSize;

}; // class BufferTL

} // namespace arData
