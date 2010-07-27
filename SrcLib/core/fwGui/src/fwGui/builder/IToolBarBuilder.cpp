/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include "fwGui/builder/IToolBarBuilder.hpp"

namespace fwGui
{
namespace builder
{

const IToolBarBuilder::RegistryKeyType IToolBarBuilder::REGISTRY_KEY = "::fwGui::ToolBarBuilder";

//-----------------------------------------------------------------------------

IToolBarBuilder::IToolBarBuilder()
{}

//-----------------------------------------------------------------------------

IToolBarBuilder::~IToolBarBuilder()
{}

//-----------------------------------------------------------------------------

void IToolBarBuilder::initialize( ::fwRuntime::ConfigurationElement::sptr configuration)
{

}

//-----------------------------------------------------------------------------

::fwGui::fwToolBar::sptr IToolBarBuilder::getToolBar()
{
    return this->m_toolBar;
}

//-----------------------------------------------------------------------------

} // namespace builder
} // namespace fwGui



