/************************************************************************
 *
 * Copyright (C) 2018 IRCAD France
 * Copyright (C) 2018 IHU Strasbourg
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

#include "opImageFilter/SFlip.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwServices/macros.hpp>

#include <imageFilterOp/Flipper.hpp>

namespace opImageFilter
{

fwServicesRegisterMacro(::fwServices::IOperator, ::opImageFilter::SFlip, ::fwData::Image);

static const ::fwServices::IService::KeyType s_IMAGE_IN    = "source";
static const ::fwServices::IService::KeyType s_IMAGE_INOUT = "target";

const ::fwCom::Slots::SlotKeyType SFlip::s_FLIP_AXIS0_SLOT = "flipAxis0";
const ::fwCom::Slots::SlotKeyType SFlip::s_FLIP_AXIS1_SLOT = "flipAxis1";
const ::fwCom::Slots::SlotKeyType SFlip::s_FLIP_AXIS2_SLOT = "flipAxis2";

//------------------------------------------------------------------------------

SFlip::SFlip()
{
    // Do not flip any axis by default
    m_flipAxes.push_back(false);
    m_flipAxes.push_back(false);
    m_flipAxes.push_back(false);

    // Initialize the slots
    newSlot(s_FLIP_AXIS0_SLOT, &SFlip::flipAxis0, this);
    newSlot(s_FLIP_AXIS1_SLOT, &SFlip::flipAxis1, this);
    newSlot(s_FLIP_AXIS2_SLOT, &SFlip::flipAxis2, this);
}

//------------------------------------------------------------------------------

SFlip::~SFlip()
{

}

//------------------------------------------------------------------------------

void SFlip::configuring()
{

}

//------------------------------------------------------------------------------

void SFlip::starting()
{

}

//------------------------------------------------------------------------------

void SFlip::updating()
{
    ::fwData::Image::csptr inImg = this->getInput< ::fwData::Image >(s_IMAGE_IN);

    ::fwData::mt::ObjectReadLock inImLock(inImg);

    ::fwData::Image::sptr outImg = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);

    ::fwData::mt::ObjectWriteLock outImLock(outImg);

    SLM_ASSERT("No 'imageIn' found !", inImg);
    SLM_ASSERT("No 'imageOut' found !", outImg);

    ::imageFilterOp::Flipper::flip(inImg, outImg, m_flipAxes);

    auto imgBufModifSig = outImg->signal< ::fwData::Image::BufferModifiedSignalType >
                              (::fwData::Image::s_BUFFER_MODIFIED_SIG);

    imgBufModifSig->asyncEmit();

    auto imgModifSig = outImg->signal< ::fwData::Image::ModifiedSignalType >
                           (::fwData::Image::s_MODIFIED_SIG);

    imgModifSig->asyncEmit();
}

//------------------------------------------------------------------------------

void SFlip::stopping()
{

}

//------------------------------------------------------------------------------

void SFlip::flipAxis0()
{
    m_flipAxes[0] = !(m_flipAxes[0]);
    this->updating();
}

//------------------------------------------------------------------------------

void SFlip::flipAxis1()
{
    m_flipAxes[1] = !(m_flipAxes[1]);
    this->updating();
}

//------------------------------------------------------------------------------

void SFlip::flipAxis2()
{
    m_flipAxes[2] = !(m_flipAxes[2]);
    this->updating();
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SFlip::getAutoConnections() const
{
    ::fwServices::IService::KeyConnectionsMap connections;
    connections.push(s_IMAGE_IN, ::fwData::Image::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_IMAGE_IN, ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

} // namespace opImageFilter
