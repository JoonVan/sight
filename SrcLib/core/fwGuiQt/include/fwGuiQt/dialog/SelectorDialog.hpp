/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "fwGuiQt/config.hpp"

#include <fwGui/dialog/ISelectorDialog.hpp>

#include <QPushButton>
#include <QVector>

#include <vector>

namespace fwGuiQt
{
namespace dialog
{
//------------------------------------------------------------------------------

/**
 * @brief   SelectorDialog allowing the choice of an element among severals (_selections)
 */
class FWGUIQT_CLASS_API SelectorDialog : public ::fwGui::dialog::ISelectorDialog
{
public:

    fwCoreClassMacro(SelectorDialog, ::fwGui::dialog::ISelectorDialog, ::fwGui::factory::New< SelectorDialog >);

    FWGUIQT_API SelectorDialog(::fwGui::GuiBaseObject::Key key);

    FWGUIQT_API virtual ~SelectorDialog();

    /**
     * @brief The string list that can be chosen by the selector.
     */
    FWGUIQT_API virtual void setSelections(std::vector< std::string > _selections) override;

    /**
     * @brief Sets the selector title.
     */
    FWGUIQT_API virtual void setTitle(std::string _title) override;

    /**
     * @brief Show the selector and return the selection.
     */
    FWGUIQT_API virtual std::string show() override;

    /// Set the message
    FWGUIQT_API virtual void setMessage(const std::string& msg) override;

    /// Add a custom button to this dialog
    FWGUIQT_API virtual void addCustomButton(const std::string& label, std::function<void()> clickedFn) override;

private:

    std::vector< std::string > m_selections;

    /// Dialog box message
    std::string m_message;
    std::string m_title;

    /// Stores custom buttons
    QVector< QPushButton* > m_customButtons;
};

} // namespace dialog
} // namespace fwGuiQt
