/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#include "fwStructuralPatch/fwData/Material/V2ToV3.hpp"

#include <fwAtoms/String.hpp>
#include <fwAtomsPatch/StructuralCreatorDB.hpp>


namespace fwStructuralPatch
{

namespace fwData
{

namespace Material
{

V2ToV3::V2ToV3() : ::fwAtomsPatch::IStructuralPatch()
{
    m_originClassname = "::fwData::Material";
    m_targetClassname = "::fwData::Material";
    m_originVersion   = "2";
    m_targetVersion   = "3";
}

// ----------------------------------------------------------------------------

V2ToV3::~V2ToV3()
{
}

// ----------------------------------------------------------------------------

V2ToV3::V2ToV3( const V2ToV3 &cpy ) : ::fwAtomsPatch::IStructuralPatch(cpy)
{
}

// ----------------------------------------------------------------------------

void V2ToV3::apply(
    const ::fwAtoms::Object::sptr& previous,
    const ::fwAtoms::Object::sptr& current,
    ::fwAtomsPatch::IPatch::NewVersionsType& newVersions)
{
    IStructuralPatch::apply(previous, current, newVersions);

    // Update object version
    this->updateVersion(current);

    // Create helper
    ::fwAtomsPatch::helper::Object helper(current);
    helper.addAttribute("lighting", ::fwAtoms::Boolean::New(true) );
}

} // namespace Material

} // namespace fwData

} // namespace fwStructuralPatch

