/************************************************************************
 *
 * Copyright (C) 2017-2020 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
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

#include "cvSegmentation/config.hpp"

#include <colourImageMasking/Masker.hpp>

#include <fwData/Image.hpp>

#include <fwServices/IOperator.hpp>

namespace cvSegmentation
{

/**
 * @brief   Service that learns a foreground and background color model and allow to segment the foreground on a new
 * image using an Expectation Maximization algorithm.
 *
 * @see ::fwServices::IOperator
 *
 * @section Slots Slots
 * - \b setBackground() : Slot to set the background image to learn the background model color
 * - \b setForeground() : Slot to set the foreground image to learn the foreground model color
 * - \b setThreshold(int threshold) : Slot to set the threshold value to get the final binary image
 * - \b setNoiseLevel(double noiseLevel) : Slot to set the noise level added in the learning steps
 * - \b setBackgroundComponents(int bgComponents) : Slot to set the number of background components learned
 * - \b setForegroundComponents(int fgComponents) : Slot to set the number of foreground components learned
 * - \b clearMaskTL() : Slot to clear the output foreground mask timeline and reset the last timestamp.
 *
 * @section XML XML Configuration
 * @code{.xml}
        <service uid="..." type="::cvSegmentation::SColourImageMasking" >
            <in key="videoTL" uid="..." autoConnect="yes" />
            <inout key="mask" uid="..." />
            <inout key="videoMaskTL" uid="..." />
            <config scaleFactor="0.5" noise="30" foregroundComponents="5" backgroundComponents="5"/>
            <HSV>
                <lower>35,0,0</lower>
                <upper>360.5,255,255</upper>
            </HSV>
        </service>
   @endcode
 *
 * @subsection Input Input
 * - \b mask [::fwData::Image] : mask image to perform image segmentation in
 * - \b videoTL [::arData::FrameTL] : Timeline to extract image from a video to perform the learning steps
 *
 * @subsection In-Out In-Out
 * - \b videoMaskTL [::arData::FrameTL] : Timeline to put masks inside where the foreground is segmented (outside of the
 * mask = 0)
 *
 * @subsection Configuration Configuration
 * - \b scaleFactor (optional)(default: 1.0) : factor to scale the image to perform image masking on
 * - \b noise (optional)(default: 0.0) : standard deviation value for the additive noise used during the foreground
 * learning step. It allows to perform a more robust learning step by providing different values close to the initial
 * pixel. It is sometimes needed when real values are a little bit different than the given input image used during the
 * learning step. For example, to avoid missing pixels when brightness or shadows is changing.
 * - \b foregroundComponents (optional)(default: 5) : number of components learned in the foreground color model
 * - \b backgroundComponents (optional)(default: 5) : number of components learned in the foreground color model
 * - \b HSV (optional) : values in HSV defined by <lower>(default: 0,0,0) and <upper> (default: 255,255,255) tags
 * allowing to compute automatically the mask during the foreground color model learning step
 */
class CVSEGMENTATION_CLASS_API SColourImageMasking final : public ::fwServices::IOperator
{
public:

    /// Generates default methods as New, dynamicCast, ...
    fwCoreServiceMacro(SColourImageMasking, ::fwServices::IOperator)

    /// Initializes the slots and member variables.
    CVSEGMENTATION_API SColourImageMasking() noexcept;

    /// Destroys the service.
    CVSEGMENTATION_API ~SColourImageMasking() noexcept override;

    /// Defines auto connection for this service (update slot) to the frame timeline (objectPushed)
    ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

protected:

    /// Initializes segmentation method parameters.
    CVSEGMENTATION_API void configuring() override;

    /// Initializes the colour image masker.
    CVSEGMENTATION_API void starting() override;

    /// Does nothing.
    CVSEGMENTATION_API void stopping() override;

    /// Computes the image mask on a frame.
    CVSEGMENTATION_API void updating() override;

private:

    /// SLOT: Sets background image and learn background model.
    void setBackground();

    /// SLOT: Sets foreground image and learn foreground model.
    void setForeground();

    /// SLOT: Sets the threshold value to compute final binary image.
    void setThreshold(int threshold);

    /// SLOT: Sets the noise level added in the learning steps.
    void setNoiseLevel(double noiseLevel);

    /// SLOT: Sets the number of background components learned.
    void setBackgroundComponents(int bgComponents);

    /// SLOT: Sets the number of foreground components learned.
    void setForegroundComponents(int fgComponents);

    /// SLOT: Clears the output mask timeline and reset the last timestamp.
    void clearMaskTL();

    /// Object performing the Expectation Maximization segmentation.
    std::unique_ptr< ::colourImageMasking::Masker > m_masker;

    /// Current timestamp.
    ::fwCore::HiResClock::HiResClockType m_lastVideoTimestamp;

    /// Reduction factor.
    float m_scaleFactor;

    /// Opencv scale factor.
    ::cv::Size m_maskDownsize;

    /// Opencv HSV lower value to threshold the image used during foreground color model learning step.
    ::cv::Scalar m_lowerColor;

    /// Opencv HSV upper value to threshold the image used during foreground color model learning step.
    ::cv::Scalar m_upperColor;

    /// Noise level to add during the foreground learning step.
    double m_noise;

    /// Number of background components.
    int m_backgroundComponents;

    /// Number of foreground components.
    int m_foregroundComponents;
};

} // namespace cvSegmentation.
