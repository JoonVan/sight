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

#pragma once

#include "fwGui/config.hpp"
#include "fwGui/container/fwContainer.hpp"
#include "fwGui/GuiBaseObject.hpp"

#include <array>
#include <string>

namespace fwGui
{
namespace dialog
{
/**
 * @brief Defines the generic notification popup GUI.
 */
class FWGUI_CLASS_API INotificationDialog : public ::fwGui::GuiBaseObject
{

public:

    fwCoreClassMacro(INotificationDialog, ::fwGui::GuiBaseObject)

    /// Notification Type (changes Qss style).
    enum class Type
    {
        INFO = 0,
        SUCCESS,
        FAILURE,
        NONE,
        DEFAULT = INFO
    };

    /// Where to display notifications.
    enum class Position
    {
        TOP_RIGHT = 0,
        TOP_LEFT,
        BOTTOM_RIGHT,
        BOTTOM_LEFT,
        CENTERED,
        CENTERED_TOP,
        CENTERED_BOTTOM,
        DEFAULT = TOP_RIGHT
    };

    /// Constructor. Does nothing.
    FWGUI_API INotificationDialog();

    /// Destructor. Does nothing.
    FWGUI_API virtual ~INotificationDialog() override;

    typedef std::string FactoryRegistryKeyType;
    FWGUI_API static const FactoryRegistryKeyType REGISTRY_KEY;

    /**
     * @brief Sets the message.
     * @param _msg message as a std::string, it can be empty.
     */
    FWGUI_API virtual void setMessage(const std::string& _msg);

    /**
     * @brief Sets the notification type.
     * @param _type notification type @see Type.
     */
    FWGUI_API virtual void setType( Type _type);

    /**
     * @brief Sets the position.
     * @param _position notification position @see Position.
     */
    FWGUI_API virtual void setPosition( Position _position);

    /**
     * @brief Sets the size
     * @param _width width of the notification in pixel.
     * @param _height height of the notification in pixel.
     */
    FWGUI_API virtual void setSize(unsigned int _width, unsigned int _height);

    /**
     * @brief Sets the queue index of the notification (when notifications are queued).
     * @param _index index in the queue (0 = first, 1 = second,...).
     */
    FWGUI_API virtual void setIndex(unsigned int _index);

    /**
     * @brief Sets the duration in ms.
     * @param _durationInMs duration of the notification before closing (+ 1 sec of fade in/out effects).
     */
    FWGUI_API virtual void setDuration(int _durationInMs);

    /// Shows the message box and return the clicked button.
    FWGUI_API virtual void show() = 0;

    /// Gets the visibility, needs to be reimplemented.
    FWGUI_API virtual bool isVisible() const = 0;

    /// Closes the notification, needs to be reimplemented.
    FWGUI_API virtual void close() const = 0;

    /**
     * @brief Attaches a parent container to the notification.
     * @param _container the parent container, nullptr is handled.
     */
    FWGUI_API virtual void setContainer(::fwGui::container::fwContainer::csptr _container)
    {
        m_parentContainer = _container;
    }

protected:

    /// Total duration of the popup (cannot be below 2000ms).
    int m_duration {3000};

    /// Message to display.
    std::string m_message;

    /// Type of notification (may change the background color).
    Type m_notificationType {Type::DEFAULT};

    /// Where the notification will be displayed (relative to the active windows).
    Position m_position {Position::DEFAULT};

    /// Size as [width, height].
    std::array<unsigned int, 2> m_size {200, 60};

    /// Position of the notification, used to avoid overlapping
    /// when several notifications are shown. (0 = first, 1 = second, ...)
    unsigned int m_index {0};

    /// Pointer to the parent container, default nullptr.
    ::fwGui::container::fwContainer::csptr m_parentContainer {nullptr};

};

} //namespace dialog
} // namespace fwGui
