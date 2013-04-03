/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef  __FWATOMCONVERSION_DATAVISITOR_HPP__
#define  __FWATOMCONVERSION_DATAVISITOR_HPP__

#include <map>

#include <fwCamp/camp/ExtendedClassVisitor.hpp>

#include <fwTools/UUID.hpp>

#include <fwData/Object.hpp>

#include <fwAtoms/Base.hpp>

#include "fwAtomConversion/config.hpp"

namespace fwAtomConversion
{

class FWATOMCONVERSION_CLASS_API DataVisitor : public ::camp::ExtendedClassVisitor
{

public:

    typedef std::map< ::fwTools::UUID::UUIDType, ::fwAtoms::Object::sptr > AtomCacheType;

    typedef std::string ClassnameType;

    /// ???
    FWATOMCONVERSION_API static const std::string CLASSNAME_METAINFO;

    /// ???
    FWATOMCONVERSION_API static ::fwAtoms::Object::sptr convert( ::fwData::Object::sptr dataObj, AtomCacheType & cache );

    /// ???
    FWATOMCONVERSION_API DataVisitor( ::fwData::Object::sptr dataObj, AtomCacheType & cache );

    /// ???
    FWATOMCONVERSION_API virtual ~DataVisitor();

    /// ???
    FWATOMCONVERSION_API void visit(const camp::SimpleProperty& property);

    /// ???
    FWATOMCONVERSION_API void visit(const camp::EnumProperty& property);

    /// ???
    FWATOMCONVERSION_API void visit(const camp::UserProperty& property);

    /// ???
    FWATOMCONVERSION_API void visit(const camp::ArrayProperty& property);

    /// ???
    FWATOMCONVERSION_API void visit(const camp::Function& function);

    /// ???
    FWATOMCONVERSION_API void visit(const camp::MapProperty& property);

    /// ???
    FWATOMCONVERSION_API ::fwAtoms::Object::sptr getAtomObject() const;

private:

    /// Reflection in camp world of m_dataObj
    ::camp::UserObject m_campDataObj;

    /// ???
    ::fwAtoms::Object::sptr m_atomObj;

    /// ???
    AtomCacheType & m_cache;
};

} // end namespace fwAtomConversion

#endif // __FWATOMCONVERSION_DATAVISITOR_HPP__
