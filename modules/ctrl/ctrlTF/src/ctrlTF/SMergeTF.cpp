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

#include "ctrlTF/SMergeTF.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

namespace ctrlTF
{

static const ::fwServices::IService::KeyType s_TF_POOL_INPUT = "tfPool";
static const ::fwServices::IService::KeyType s_TF_INOUT      = "tf";

static const ::fwCom::Slots::SlotKeyType s_MERGE_SLOT = "merge";

//------------------------------------------------------------------------------

SMergeTF::SMergeTF() noexcept
{
    newSlot(s_MERGE_SLOT, &SMergeTF::merge, this);
}

//------------------------------------------------------------------------------

SMergeTF::~SMergeTF() noexcept
{
}

//------------------------------------------------------------------------------

void SMergeTF::configuring()
{
}

//------------------------------------------------------------------------------

void SMergeTF::starting()
{
    this->updating();
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SMergeTF::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_TF_POOL_INPUT, ::fwData::Object::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_TF_POOL_INPUT, ::fwData::Composite::s_ADDED_OBJECTS_SIG, s_UPDATE_SLOT);
    connections.push(s_TF_POOL_INPUT, ::fwData::Composite::s_REMOVED_OBJECTS_SIG, s_UPDATE_SLOT);
    return connections;
}

//------------------------------------------------------------------------------

void SMergeTF::updating()
{
    m_connections.disconnect();

    // Iterates over each TF to create connections.
    {
        const auto tfPool = this->getLockedInput< ::fwData::Composite >(s_TF_POOL_INPUT);
        SLM_ASSERT("input '" + s_TF_POOL_INPUT + "' must have at least on TF inside.", tfPool->size() > 0);

        for(::fwData::Composite::value_type poolElt : *tfPool)
        {
            // Checks if it's a TF.
            const ::fwData::TransferFunction::sptr tf = ::fwData::TransferFunction::dynamicCast(poolElt.second);
            SLM_ASSERT("inout '" + s_TF_POOL_INPUT + "' must contain only TF.", tf);

            m_connections.connect(tf, ::fwData::TransferFunction::s_MODIFIED_SIG, this->getSptr(), s_MERGE_SLOT);
            m_connections.connect(tf, ::fwData::TransferFunction::s_POINTS_MODIFIED_SIG, this->getSptr(), s_MERGE_SLOT);
            m_connections.connect(tf, ::fwData::TransferFunction::s_WINDOWING_MODIFIED_SIG,
                                  this->getSptr(), s_MERGE_SLOT);
        }
    }

    this->merge();
}

//------------------------------------------------------------------------------

void SMergeTF::stopping()
{
}

//------------------------------------------------------------------------------

void SMergeTF::merge() const
{
    // Get the TF pool.
    const auto tfPool = this->getLockedInput< ::fwData::Composite >(s_TF_POOL_INPUT);
    SLM_ASSERT("input '" + s_TF_POOL_INPUT + "' must have at least on TF inside.", tfPool->size() > 0);

    // Clear the output TF.
    const auto outTF = this->getLockedInOut< ::fwData::TransferFunction >(s_TF_INOUT);
    outTF->clear();

    // Iterates over each TF to merge them in the output one.
    typedef ::fwData::TransferFunction::TFValueType TFValue;
    TFValue min = std::numeric_limits< TFValue >::max();
    TFValue max = std::numeric_limits< TFValue >::lowest();
    for(::fwData::Composite::value_type poolElt : *tfPool)
    {
        // Checks if the composite element is a TF.
        const ::fwData::TransferFunction::csptr tf = ::fwData::TransferFunction::dynamicCast(poolElt.second);
        SLM_ASSERT("inout '" + s_TF_POOL_INPUT + "' must contain only TF.", tf);
        const ::fwData::mt::locked_ptr tfLock(tf);

        const ::fwData::TransferFunction::TFValuePairType minMaxValues = tf->getMinMaxTFValues();
        const ::fwData::TransferFunction::TFValueType minWL            = tf->getWLMinMax().first;
        const ::fwData::TransferFunction::TFValueType window           = tf->getWindow();
        const ::fwData::TransferFunction::TFValueType width            = minMaxValues.second - minMaxValues.first;

        const auto addTFPoint = [&](::fwData::TransferFunction::TFValueType _value, double _delta)
                                {
                                    ::fwData::TransferFunction::TFValueType value;
                                    value  = (_value - minMaxValues.first) / width;
                                    value  = value * window + minWL;
                                    value += _delta;

                                    outTF->addTFColor(value, this->mergeColors(tfPool.get_shared(), value));
                                    if(value < min)
                                    {
                                        min = value;
                                    }
                                    if(value > max)
                                    {
                                        max = value;
                                    }
                                };

        // Add new TF value to the output.
        bool first = true;
        ::fwData::TransferFunction::TFValueType previousValue = 0;
        for(const ::fwData::TransferFunction::TFDataType::value_type& elt : tf->getTFData())
        {
            // If the TF interpolation mode is not linear, we create new point in the merged TF.
            if(!first && tf->getInterpolationMode() == ::fwData::TransferFunction::NEAREST)
            {
                ::fwData::TransferFunction::TFValueType middleValue = previousValue + (elt.first - previousValue) / 2.;
                addTFPoint(middleValue, -1.);
                addTFPoint(middleValue, 1.);
            }

            // If the TF is clamped, we create new point in the merged TF.
            if(first && tf->getIsClamped())
            {
                addTFPoint(elt.first, -1.);
            }

            // Computes TF value from TF space to window/level space.
            if(tf->getIsClamped())
            {
                // This avoid precision error due to the TF clamp.
                if(first)
                {
                    addTFPoint(elt.first, 1.);
                }
                else
                {
                    addTFPoint(elt.first, -1.);
                }
            }
            else
            {
                addTFPoint(elt.first, 0.);
            }

            first         = false;
            previousValue = elt.first;
        }

        if(tf->getIsClamped())
        {
            addTFPoint(previousValue, 1.);
        }
    }

    // Updates the window/level.
    if(outTF->getWindow() > 0)
    {
        outTF->setWLMinMax(::fwData::TransferFunction::TFValuePairType(min, max));
    }
    else
    {
        outTF->setWLMinMax(::fwData::TransferFunction::TFValuePairType(max, min));
    }

    // Sends the modification signal.
    const auto sig = outTF->signal< ::fwData::TransferFunction::PointsModifiedSignalType >(
        ::fwData::TransferFunction::s_POINTS_MODIFIED_SIG);
    sig->asyncEmit();
}

//------------------------------------------------------------------------------

::fwData::TransferFunction::TFColor SMergeTF::mergeColors(const ::fwData::Composite::csptr _tfPool,
                                                          ::fwData::TransferFunction::TFValueType _value) const
{
    ::fwData::TransferFunction::TFColor result;
    for(::fwData::Composite::value_type poolElt : *_tfPool)
    {
        // Checks if the composite element is a TF.
        const ::fwData::TransferFunction::csptr tf = ::fwData::TransferFunction::dynamicCast(poolElt.second);
        SLM_ASSERT("inout '" + s_TF_POOL_INPUT + "' must contain only TF.", tf);
        const ::fwData::mt::locked_ptr tfLock(tf);

        // Gets window/level informations to change TF value from window/level space to TF space .
        const ::fwData::TransferFunction::TFValuePairType minMaxValues = tf->getMinMaxTFValues();
        const ::fwData::TransferFunction::TFValueType minWL            = tf->getWLMinMax().first;
        const ::fwData::TransferFunction::TFValueType window           = tf->getWindow();
        const ::fwData::TransferFunction::TFValueType width            = minMaxValues.second - minMaxValues.first;

        ::fwData::TransferFunction::TFValueType value = (_value - minWL) / window;
        value                                         = value * width + minMaxValues.first;

        // The value need to be in the TF space, that why it's converted before.
        ::fwData::TransferFunction::TFColor currentColor = tf->getInterpolatedColor(value);

        result.r += currentColor.r;
        result.g += currentColor.g;
        result.b += currentColor.b;
        result.a += currentColor.a * (1.0 - result.a);
    }

    result.r = result.r > 1.f ? 1.f : result.r;
    result.g = result.g > 1.f ? 1.f : result.g;
    result.b = result.b > 1.f ? 1.f : result.b;
    result.a = result.a > 1.f ? 1.f : result.a;

    return result;
}

} // namespace ctrlTF.
