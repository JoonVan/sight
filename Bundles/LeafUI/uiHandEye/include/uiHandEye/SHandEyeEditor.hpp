/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIHANDEYE_SHANDEYEEDITOR_HPP__
#define __UIHANDEYE_SHANDEYEEDITOR_HPP__

#include "uiHandEye/config.hpp"

#include <arData/MatrixTL.hpp>

#include <fwData/Composite.hpp>
#include <fwData/PointList.hpp>
#include <fwData/Vector.hpp>

#include <gui/editor/IEditor.hpp>

#include <QLabel>
#include <QListWidget>
#include <QPointer>
#include <QPushButton>

namespace uiHandEye
{
/**
 * @brief   SHandEyeEditor service is used to handle the calibration points acquisition.
 * This service takes two matrixTL as inputs, and fills the two vector of matrix when the 'addSlot' is called.
 *
 * @see ::trackingCalibration::SHandEyeCalibration
 *
 * @section slots slots
 * - \b add(): Add new calibration points
 * - \b remove(): Remove calibration points
 * - \b reset(): Reset currently acquiered calibration points
 * - \b getSelection(): Display a dialog with the current matrices
 *
 * @code{.xml}
    <service uid="..." type="::uiHandEye::SHandEyeEditor">
        <in key="matrixTL1" uid="..." />
        <in key="matrixTL2" uid="..." />
        <inout key="matrixVector1" uid="..." />
        <inout key="matrixVector2" uid="..." />
    </service>
   @endcode

 * @subsection InOut InOut
 * - \b matrixVector1 [::fwData::Vector]: first vector filled with {Ai, Aj, ...} matrix
 *[::fwData::TransformationMatrix3D] when 'addSlot' is called
 * - \b matrixVector2 [::fwData::Vector]: second vector filled with {Bi, Bj, ...} matrix
 *[::fwData::TransformationMatrix3D] when 'addSlot' is called
 *
 * @subsection Input Input
 * - \b matrixTL1 [::arData::FrameTL]: the first matrixTL filled with {Ai, Aj, ...} matrices
 *[::fwData::TransformationMatrix3D]
 * - \b matrixTL2 [::arData::FrameTL]: the second matrixTL filled with {Bi, Bj, ...} matrices
 *[::fwData::TransformationMatrix3D]
 */
class UIHANDEYE_CLASS_API SHandEyeEditor : public QObject,
                                           public ::gui::editor::IEditor
{
Q_OBJECT;

public:
    fwCoreServiceClassDefinitionsMacro((SHandEyeEditor)(::gui::editor::IEditor));

    /**
     * @name Slots API
     * @{
     */

    UIHANDEYE_API static const ::fwCom::Slots::SlotKeyType s_ADD_SLOT;
    typedef ::fwCom::Slot<void ()> AddSlotType;

    UIHANDEYE_API static const ::fwCom::Slots::SlotKeyType s_REMOVE_SLOT;
    typedef ::fwCom::Slot<void ()> RemoveSlotType;

    UIHANDEYE_API static const ::fwCom::Slots::SlotKeyType s_RESET_SLOT;
    typedef ::fwCom::Slot<void ()> ResetSlotType;

    UIHANDEYE_API static const ::fwCom::Slots::SlotKeyType s_GET_SELECTION_SLOT;
    typedef ::fwCom::Slot<void ()> GetSelectionSlotType;
    ///@}

    /**
     * @brief Constructor.
     */
    UIHANDEYE_API SHandEyeEditor() noexcept;

    /**
     * @brief Destructor.
     */
    ~SHandEyeEditor() noexcept
    {
    }

protected:

    UIHANDEYE_API void configuring() override;

    /**
     * @brief Starting method : This method is used to initialize the service.
     */
    UIHANDEYE_API void starting() override;

    /**
     * @brief Stopping method : This method is used to stop the service.
     */
    UIHANDEYE_API void stopping() override;

    /**
     * @brief Updating method : This method is used to update the service.
     */
    UIHANDEYE_API void updating() override;

    /**
     * @brief Slot called when the user presses the add acquisition button.
     */
    UIHANDEYE_API void add();

    /**
     * @brief Slot called when the user presses the remove acquisition button.
     */
    UIHANDEYE_API void remove();

    /**
     * @brief Slot called when the user presses the reset button.
     */
    UIHANDEYE_API void reset();

    /**
     * @brief Slot called when the user presses the display button.
     */
    UIHANDEYE_API void getSelection();

private Q_SLOTS:
    /**
     * @brief Slot called when an element is double-clicked in the vector widget.
     */
    void onItemDoubleClicked(QListWidgetItem*);

private:

    /**
     * @brief Index of the last acquisition.
     */
    int m_captureIdx;

    /**
     * @brief Label displaying the number of point acquisitions.
     */
    QPointer< QLabel > m_nbCapturesLabel;

    /**
     * @brief Calibration point vector.
     */
    QPointer< QListWidget > m_capturesListWidget;

    /// Last timestamp
    ::fwCore::HiResClock::HiResClockType m_lastTimestamp;

};
} // namespace uiHandEye

#endif // __UIHANDEYE_SHANDEYEEDITOR_HPP__
