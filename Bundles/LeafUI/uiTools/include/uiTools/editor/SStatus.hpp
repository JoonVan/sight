/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UITOOLS_EDITOR_SSTATUS_HPP__
#define __UITOOLS_EDITOR_SSTATUS_HPP__

#include <QPointer>
#include <QLabel>

#include <fwTools/Failed.hpp>

#include <gui/editor/IEditor.hpp>

#include "uiTools/config.hpp"

class QPushButton;

namespace uiTools
{

namespace editor
{

/**
 * @brief   SStatus service shows a colored square (red, orange, green) representing a status.
 * @class   SStatus
 *
 * To change the status color, you should call the slots 'changeToGreen', 'changeToOrange', 'changeToRed' or 'toggleGreenRed'.
 */
class UITOOLS_CLASS_API SStatus : public QObject,
                                  public ::gui::editor::IEditor
{

public:

    fwCoreServiceClassDefinitionsMacro ( (SStatus)(::gui::editor::IEditor) );

    /// Constructor. Do nothing.
    UITOOLS_API SStatus() throw();

    /// Destructor. Do nothing.
    UITOOLS_API virtual ~SStatus() throw();

    /**
     * @name Slots API
     *@{
     */
    UITOOLS_API static const ::fwCom::Slots::SlotKeyType s_CHANGE_TO_GREEN_SLOT;
    typedef ::fwCom::Slot<void ()> ChangeToGreenSlotType;

    UITOOLS_API static const ::fwCom::Slots::SlotKeyType s_CHANGE_TO_RED_SLOT;
    typedef ::fwCom::Slot<void ()> ChangeToRedSlotType;

    UITOOLS_API static const ::fwCom::Slots::SlotKeyType s_CHANGE_TO_ORANGE_SLOT;
    typedef ::fwCom::Slot<void ()> ChangeToOrangeSlotType;

    UITOOLS_API static const ::fwCom::Slots::SlotKeyType s_TOGGLE_GREEN_RED_SLOT;
    typedef ::fwCom::Slot< void ( bool ) > ToggleGreenRedSlotType;
    /** @} */

protected:

    typedef ::fwRuntime::ConfigurationElement::sptr Configuration;

    /**
     * @brief Install the layout.
     */
    virtual void starting() throw(::fwTools::Failed);

    /**
     * @brief Destroy the layout.
     */
    virtual void stopping() throw(::fwTools::Failed);

    /// Does nothing
    virtual void updating() throw(::fwTools::Failed);

    /// Does nothing
    virtual void swapping() throw(::fwTools::Failed);

    /**
     * @brief Configures the status tooltip
     *
     * @code{.xml}
       <service uid="..." impl="::uiTools::editor::SStatus">
           <form>circle</form>
           <size>
               <width>20</width>
               <height>20</height>
           </size>
           <labelStatus>SCP Server</labelStatus>
           <red>Stopped</red>
           <green>Tracking</green>
           <orange>Started</orange>
       </service>
       @endcode
     */
    virtual void configuring() throw(fwTools::Failed);

    /// Overrides
    virtual void info( std::ostream &_sstream );

    /// SLOT : change label color
    void changeToGreen();

    /// SLOT : change label color
    void changeToRed();

    /// SLOT : change label color
    void changeToOrange();

    /// SLOT : change label color (true = green, false = red)
    void toggleGreenRed(bool green);

private:

    QPointer< QLabel > m_indicator;
    QPointer< QLabel > m_labelStatus;

    /// Slot to call changeToGreen()
    ChangeToGreenSlotType::sptr m_slotChangeToGreen;

    /// Slot to call changeToRed()
    ChangeToRedSlotType::sptr m_slotChangeToRed;

    /// Slot to call changeToOrange()
    ChangeToOrangeSlotType::sptr m_slotChangeToOrange;

    /// Slot to call changeToGreenRed()
    ToggleGreenRedSlotType::sptr m_slotToggleGreenRed;

    std::string m_greenTooltip; ///< Tooltip for green status
    std::string m_redTooltip; ///< Tooltip for red status
    std::string m_orangeTooltip; ///< Tooltip for orange status

    bool m_isCircular; ///< label is a circle if true (else it's a square)

    size_t m_width; ///< width of idicator
    size_t m_height; ///< height of idicator

};

} // namespace editor
} // namespace uiTools

#endif /*__UITOOLS_EDITOR_SSTATUS_HPP__*/


