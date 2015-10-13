/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VIDEOQT_SFRAMEGRABBER_HPP__
#define __VIDEOQT_SFRAMEGRABBER_HPP__

#include "videoQt/config.hpp"
#include <fwTools/Failed.hpp>

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwServices/IController.hpp>
#include <videoQt/player/QVideoPlayer.hpp>

#include <QObject>
#include <QPointer>
#include <QImage>

fwCorePredeclare( (arData)(Camera) )

namespace videoQt
{

/**
 * @brief   Defines the service which grab video frame.
 * @class   SFrameGrabber
 */
class VIDEOQT_CLASS_API SFrameGrabber : public QObject,
                                        public ::fwServices::IController
{
Q_OBJECT;
public:


    fwCoreServiceClassDefinitionsMacro ( (SFrameGrabber)(::fwServices::IController) );

    /// Constructor. Do nothing.
    VIDEOQT_API SFrameGrabber() throw();

    /// Destructor. Do nothing.
    VIDEOQT_API virtual ~SFrameGrabber() throw();

    /**
     * @name Signals API
     * @{
     */
    VIDEOQT_API static const ::fwCom::Signals::SignalKeyType s_POSITION_MODIFIED_SIG;
    typedef ::fwCom::Signal<void (std::int64_t)> PositionModifiedSignalType;

    VIDEOQT_API static const ::fwCom::Signals::SignalKeyType s_DURATION_MODIFIED_SIG;
    typedef ::fwCom::Signal<void (std::int64_t)> DurationModifiedSignalType;
    /** @} */

    /**
     * @name Slots API
     * @{
     */
    VIDEOQT_API static const ::fwCom::Slots::SlotKeyType s_START_CAMERA_SLOT;
    typedef ::fwCom::Slot<void ()> StartCameraSlotType;

    VIDEOQT_API static const ::fwCom::Slots::SlotKeyType s_STOP_CAMERA_SLOT;
    typedef ::fwCom::Slot<void ()> StopCameraSlotType;

    VIDEOQT_API static const ::fwCom::Slots::SlotKeyType s_PAUSE_CAMERA_SLOT;
    typedef ::fwCom::Slot<void ()> PauseCameraSlotType;

    VIDEOQT_API static const ::fwCom::Slots::SlotKeyType s_LOOP_VIDEO_SLOT;
    typedef ::fwCom::Slot<void ()> LoopVideoSlotType;

    VIDEOQT_API static const ::fwCom::Slots::SlotKeyType s_SET_POSITION_VIDEO_SLOT;
    typedef ::fwCom::Slot<void (int)> SetPositionVideoSlotType;
    ///@}

protected:

    /// Initialize the layout and the camera.
    VIDEOQT_API virtual void starting() throw( ::fwTools::Failed );

    /// Destroys the layout.
    VIDEOQT_API virtual void stopping() throw( ::fwTools::Failed );

    /// Does nothing.
    VIDEOQT_API virtual void updating() throw(::fwTools::Failed);

    /**
     * @verbatim
       <service uid="${GENERIC_UID}_VideoGrabber" impl="::videoQt::SFrameGrabber" autoConnect="no">
           <cameraFwId>cameraID</cameraFwId>
       </service>
       @endverbatim
     * - \b cameraFwId: fwID of the arData::Camera used to display video.
     **/
    VIDEOQT_API virtual void configuring() throw( ::fwTools::Failed );

    /// SLOT : Initialize and start camera (restart camera if is already started)
    void startCamera();

    /// SLOT : Stop camera
    void stopCamera();

    /// SLOT : Pause camera
    void pauseCamera();

    /// SLOT : enable/disable loop in video
    void toggleLoopMode();

    /// SLOT : set the new position in the video.
    void setPosition(int64_t position);

    /// Gets camera from m_cameraID
    SPTR(::arData::Camera) getCamera();

    /// Mirrored the frame in the desired direction
    void setMirror(bool horizontallyFlip = false, bool verticallyFlip = false)
    {
        m_horizontallyFlip = horizontallyFlip;
        m_verticallyFlip   = verticallyFlip;
    }

protected Q_SLOTS:

    /// Call when duration of the video changed.
    void onDurationChanged(qint64 duration);

    /// Call when reading position changed in the video.
    void onPositionChanged(qint64 position);

    /// SLOT: copy and push video frame in the timeline.
    void presentFrame(const QVideoFrame& frame);

private:

    /// FwID of arData::Camera
    std::string m_cameraID;

    /// state of the loop mode
    bool m_loopVideo;

    /// Camera
    player::QVideoPlayer* m_videoPlayer;

    /// Worker for the m_slotPresentFrame
    ::fwThread::Worker::sptr m_worker;

    /// Mutex to protect concurrent access for m_videoFrame
    mutable ::fwCore::mt::ReadWriteMutex m_videoFrameMutex;

    /// Mirror frame in horizontal direction
    bool m_horizontallyFlip;

    /// Mirror frame in vertical direction
    bool m_verticallyFlip;
};

} // namespace videoQt

#endif /*__VIDEOQT_SFRAMEGRABBER_HPP__*/


