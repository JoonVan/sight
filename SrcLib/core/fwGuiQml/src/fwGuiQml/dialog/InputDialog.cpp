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

#include "fwGuiQml/dialog/InputDialog.hpp"

#include <fwGui/registry/macros.hpp>

#include <fwQml/QmlEngine.hpp>

#include <fwRuntime/operations.hpp>

#include <boost/assign/list_of.hpp>

#include <QGuiApplication>
#include <QObject>

fwGuiRegisterMacro( ::fwGuiQml::dialog::InputDialog, ::fwGui::dialog::IInputDialog::REGISTRY_KEY );

namespace fwGuiQml
{
namespace dialog
{

//------------------------------------------------------------------------------

InputDialog::InputDialog(::fwGui::GuiBaseObject::Key key) :
    m_input("")
{
}

//------------------------------------------------------------------------------

InputDialog::~InputDialog()
{
}

//------------------------------------------------------------------------------

void InputDialog::setTitle( const std::string& title )
{
    m_title = title;
}

//------------------------------------------------------------------------------

void InputDialog::setMessage( const std::string& msg )
{
    m_message = msg;
}

//------------------------------------------------------------------------------

/// Set the input text in the input field
void InputDialog::setInput(const std::string& text)
{
    m_input = text;
}

//------------------------------------------------------------------------------

/// Get the input text in the input field
std::string InputDialog::getInput()
{
    QString title = QString::fromStdString(m_title);
    QString text  = QString::fromStdString(m_message);
    // get the qml engine QmlApplicationEngine
    SPTR(::fwQml::QmlEngine) engine = ::fwQml::QmlEngine::getDefault();
    m_isClicked                     = false;
    // get the path of the qml ui file in the 'rc' directory
    auto dialogPath = ::fwRuntime::getLibraryResourceFilePath("fwGuiQml-0.1/dialog/LocationDialog.qml");

    // load the qml ui component
    m_dialog = engine->createComponent(dialogPath);
    m_dialog->setProperty("title", title);
    m_dialog->findChild< QObject* >("message")->setProperty("text", text);
    m_dialog->findChild< QObject* >("answer")->setProperty("placeholderText", QString::fromStdString(m_input));
    //slot to retrieve the result and open the dialog with invoke
    QObject::connect(m_dialog, SIGNAL(filesNameChange(QVariant,bool)),
                     this, SLOT(resultDialog(QVariant,bool)));
    QMetaObject::invokeMethod(m_dialog, "open");
    // boolean to check first if it has called the slot or secondly if the FileDialog isn't visible
    while (!m_isClicked && m_dialog->property("visible").toBool())
    {
        qGuiApp->processEvents();
    }
    delete m_dialog;
    return m_input;
}

//------------------------------------------------------------------------------

void InputDialog::resultDialog(const QVariant& msg, bool isOk)
{
    if (isOk)
    {
        m_input = msg.toString().toStdString();
    }
    else
    {
        m_input = "";
    }
    m_isClicked = true;
}

//------------------------------------------------------------------------------

} // namespace dialog
} // namespace fwGuiQml
