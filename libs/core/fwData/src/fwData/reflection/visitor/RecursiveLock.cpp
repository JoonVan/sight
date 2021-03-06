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

#define CAMP_COMPILATION

#include "fwData/reflection/visitor/RecursiveLock.hpp"

#include "fwData/Object.hpp"
#include "fwData/reflection/exception/NullPointer.hpp"
#include "fwData/reflection/exception/ObjectNotFound.hpp"
#include "fwData/reflection/mapper.hpp"

#include <fwCore/mt/types.hpp>

#include <fwMemory/BufferObject.hpp>

namespace fwData
{
namespace reflection
{
namespace visitor
{

struct LockVisitor : public camp::ValueVisitor< void >
{
    SPTR(RecursiveLock::LockVectType) m_locks;

    LockVisitor(SPTR(RecursiveLock::LockVectType)locks) :
        m_locks(locks)
    {
    }

    //------------------------------------------------------------------------------

    void operator()(camp::NoType )
    {
    }

    //------------------------------------------------------------------------------

    void operator()(bool )
    {
    }

    //------------------------------------------------------------------------------

    void operator()(long )
    {
    }

    //------------------------------------------------------------------------------

    void operator()(double )
    {
    }

    //------------------------------------------------------------------------------

    void operator()(const std::string& )
    {
    }

    //------------------------------------------------------------------------------

    void operator()(const camp::EnumObject& )
    {
    }

    //------------------------------------------------------------------------------

    void operator()(const camp::UserObject& value)
    {
        const camp::Class& metaclass = value.getClass();
        if ( value.pointer() )
        {
            SLM_DEBUG( "visit class= '" << metaclass.name() << "' ( classname = '"<< value.call("classname") <<"' )" );
            if( metaclass.hasFunction("is_a") )
            {
                if( value.call("is_a", ::camp::Args("::fwData::Object")).to<bool>() )
                {
                    ::fwData::Object* ptr      = value.get< ::fwData::Object* >();
                    ::fwData::Object::sptr obj = ptr->getSptr();
                    ::fwData::reflection::visitor::RecursiveLock visitor( obj, m_locks );
                }
                else if( value.call("is_a", ::camp::Args("::fwMemory::BufferObject")).to<bool>() )
                {
                    ::fwMemory::BufferObject* ptr     = value.get< ::fwMemory::BufferObject* >();
                    ::fwMemory::BufferObject::sptr bo = ptr->getSptr();
                    SPTR(::fwCore::mt::ReadLock) lock
                        = SPTR(::fwCore::mt::ReadLock)(new ::fwCore::mt::ReadLock(bo->getMutex()));
                    m_locks->push_back(lock);
                }
            }
        }
    }
};

//-----------------------------------------------------------------------------

RecursiveLock::RecursiveLock( ::fwData::Object::csptr object, SPTR(LockVectType)locks ) :
    m_object(object),
    m_locks(locks)
{
    SPTR(::fwCore::mt::ReadLock) lock
        = SPTR(::fwCore::mt::ReadLock)(new ::fwCore::mt::ReadLock(m_object->getMutex()));
    m_locks->push_back(lock);
    m_campObj = camp::UserObject( *object );
    this->lock();
}

//-----------------------------------------------------------------------------

RecursiveLock::~RecursiveLock()
{
}

//-----------------------------------------------------------------------------

void RecursiveLock::visit(const camp::SimpleProperty& )
{
}

//-----------------------------------------------------------------------------

void RecursiveLock::visit(const camp::EnumProperty& )
{
}

//-----------------------------------------------------------------------------

void RecursiveLock::visit(const camp::MapProperty& property)
{
    const std::string name( property.name() );
    const size_t size = property.getSize( m_campObj );

    std::pair< ::camp::Value, ::camp::Value > value;
    ::camp::Value first;
    ::camp::Value second;

    for (size_t var = 0; var < size; ++var)
    {
        value = property.getElement(m_campObj, var);

        first  = value.first;
        second = value.second;

        SLM_ASSERT("Not managed type for map key.",
                   first.type() == ::camp::stringType ||
                   first.type() == ::camp::intType ||
                   first.type() == ::camp::realType );

        if ( second.type() == ::camp::userType )
        {
            LockVisitor visitor(m_locks);
            second.visit( visitor );
        }
    }
}

//-----------------------------------------------------------------------------

void RecursiveLock::visit(const camp::ArrayProperty& property)
{
    const std::string name( property.name() );
    const size_t size = property.size( m_campObj );

    ::camp::Value value;

    for (size_t var = 0; var < size; ++var)
    {
        value = property.get(m_campObj, var);

        if ( value.type() == ::camp::userType )
        {
            LockVisitor visitor(m_locks);
            value.visit( visitor );
        }
    }
}

//-----------------------------------------------------------------------------

void RecursiveLock::visit(const camp::UserProperty& property)
{
    const std::string name( property.name() );

    ::camp::Value elemValue = property.get( m_campObj );
    LockVisitor visitor(m_locks);
    elemValue.visit( visitor );
}

//-----------------------------------------------------------------------------

void RecursiveLock::visit(const camp::Function& )
{
}

//-----------------------------------------------------------------------------

void RecursiveLock::lock()
{
    const camp::Class& camClass = ::camp::classByName( m_object->getClassname() );
    camClass.visit( *this );
}

//-----------------------------------------------------------------------------

} // namespace visitor
} // namespace reflection
} // namespace fwData
