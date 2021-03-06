/************************************************************************
 *
 * Copyright (C) 2018-2020 IRCAD France
 * Copyright (C) 2018-2020 IHU Strasbourg
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

#include "Tuto02GenericSceneCpp/Plugin.hpp"

#include <fwData/Image.hpp>
#include <fwData/Mesh.hpp>

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

namespace Tuto02GenericSceneCpp
{

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::Tuto02GenericSceneCpp::Plugin");

//------------------------------------------------------------------------------

Plugin::~Plugin() noexcept
{
}

//------------------------------------------------------------------------------

void Plugin::start()
{
}

//------------------------------------------------------------------------------

void Plugin::initialize()
{
    m_appManager = std::unique_ptr< ::fwServices::AppManager >(new ::fwServices::AppManager );
    m_appManager->create();

    // Objects declaration.
    ::fwData::Image::sptr image    = ::fwData::Image::New();
    ::fwData::Image::sptr texture  = ::fwData::Image::New();
    ::fwData::Mesh::sptr mesh      = ::fwData::Mesh::New();
    ::fwData::Image::sptr snapshot = ::fwData::Image::New();

    m_appManager->addObject(image, image->getID());
    m_appManager->addObject(mesh, mesh->getID());
    m_appManager->addObject(texture, texture->getID());
    m_appManager->addObject(snapshot, snapshot->getID());

    // UI declaration.
    auto mainView = m_appManager->addService("::gui::frame::SDefaultFrame", true, false);
    {
        ::fwServices::IService::ConfigType config;
        config.put("gui.frame.name", "Tuto02GenericSceneCpp");
        config.put("gui.frame.icon", "Tuto02GenericSceneCpp-0.2/tuto.ico");
        config.put("gui.frame.minSize.<xmlattr>.width", "720");
        config.put("gui.frame.minSize.<xmlattr>.height", "480");
        config.put("gui.menuBar", "");
        config.put("registry.menuBar.<xmlattr>.sid", "menuBarView");
        config.put("registry.view.<xmlattr>.sid", "containerView");
        mainView->configure(config);
    }

    auto progressBarView = m_appManager->addService("::gui::editor::SJobBar", true, false);

    auto menuBarView = m_appManager->addService("::gui::aspect::SDefaultMenuBar", "menuBarView", true, false);
    {
        ::fwServices::IService::ConfigType config;
        config.put("gui.layout.menu.<xmlattr>.name", "File");
        config.put("registry.menu.<xmlattr>.sid", "menuFileView");
        menuBarView->configure(config);
    }

    auto menuFileView = m_appManager->addService("::gui::aspect::SDefaultMenu", "menuFileView", true, false);
    {
        ::fwServices::IService::ConfigType config;
        {
            ::fwServices::IService::ConfigType gui;
            gui.put("<xmlattr>.shortcut", "Ctrl+I");
            gui.put("<xmlattr>.name", "Open image");
            config.add_child("gui.layout.menuItem", gui);
        }
        {
            ::fwServices::IService::ConfigType gui;
            gui.put("<xmlattr>.name", "Open mesh");
            gui.put("<xmlattr>.shortcut", "Ctrl+M");
            config.add_child("gui.layout.menuItem", gui);
        }
        {
            ::fwServices::IService::ConfigType gui;
            gui.put("<xmlattr>.name", "Open texture");
            gui.put("<xmlattr>.shortcut", "Ctrl+T");
            config.add_child("gui.layout.menuItem", gui);
        }
        {
            config.add("gui.layout", "separator");
        }
        {
            ::fwServices::IService::ConfigType gui;
            gui.put("<xmlattr>.name", "Quit");
            gui.put("<xmlattr>.shortcut", "Ctrl+Q");
            gui.put("<xmlattr>.specialAction", "QUIT");
            config.add_child("gui.layout.menuItem", gui);
        }
        {
            ::fwServices::IService::ConfigType registry;
            registry.put("<xmlattr>.sid", "openImageAct");
            config.add_child("registry.menuItem", registry);
        }
        {
            ::fwServices::IService::ConfigType registry;
            registry.put("<xmlattr>.sid", "openMeshAct");
            config.add_child("registry.menuItem", registry);
        }
        {
            ::fwServices::IService::ConfigType registry;
            registry.put("<xmlattr>.sid", "openTextureAct");
            config.add_child("registry.menuItem", registry);
        }
        {
            ::fwServices::IService::ConfigType registry;
            registry.put("<xmlattr>.sid", "quitAct");
            config.add_child("registry.menuItem", registry);
        }
        menuFileView->configure(config);
    }

    auto containerView = m_appManager->addService("::gui::view::SDefaultView", "containerView", true, false);
    {
        ::fwServices::IService::ConfigType config;
        {
            ::fwServices::IService::ConfigType gui;
            gui.put("<xmlattr>.type", "::fwGui::LineLayoutManager");
            {
                ::fwServices::IService::ConfigType orientation;
                orientation.put("<xmlattr>.value", "vertical");
                gui.add_child("orientation", orientation);
            }
            {
                ::fwServices::IService::ConfigType view;
                view.put("<xmlattr>.proportion", "1");
                gui.add_child("view", view);
            }
            {
                ::fwServices::IService::ConfigType view;
                view.put("<xmlattr>.proportion", "0");
                view.put("<xmlattr>.minHeight", "30");
                view.put("<xmlattr>.resizable", "no");
                view.put("<xmlattr>.backgroundColor", "#3E4453");
                gui.add_child("view", view);
            }
            config.add_child("gui.layout", gui);
        }
        {
            ::fwServices::IService::ConfigType registry;
            registry.put("<xmlattr>.sid", "genericSceneSrv");
            config.add_child("registry.view", registry);
        }
        {
            ::fwServices::IService::ConfigType registry;
            registry.put("<xmlattr>.sid", "editorsView");
            config.add_child("registry.view", registry);
        }
        containerView->configure(config);
    }

    auto editorsView = m_appManager->addService("::gui::view::SDefaultView", "editorsView", true, false);
    {
        ::fwServices::IService::ConfigType config;
        {
            ::fwServices::IService::ConfigType gui;
            gui.put("<xmlattr>.type", "::fwGui::LineLayoutManager");
            {
                ::fwServices::IService::ConfigType orientation;
                orientation.put("<xmlattr>.value", "horizontal");
                gui.add_child("orientation", orientation);
            }
            {
                ::fwServices::IService::ConfigType view;
                view.put("<xmlattr>.proportion", "0");
                view.put("<xmlattr>.minWidth", "50");
                gui.add_child("view", view);
            }
            {
                ::fwServices::IService::ConfigType view;
                view.put("<xmlattr>.proportion", "1");
                gui.add_child("view", view);
            }
            {
                ::fwServices::IService::ConfigType view;
                view.put("<xmlattr>.proportion", "0");
                view.put("<xmlattr>.minWidth", "30");
                gui.add_child("view", view);
            }
            config.add_child("gui.layout", gui);
        }
        {
            ::fwServices::IService::ConfigType registry;
            registry.put("<xmlattr>.sid", "showNegatoSrv");
            config.add_child("registry.view", registry);
        }
        {
            ::fwServices::IService::ConfigType registry;
            registry.put("<xmlattr>.sid", "sliderIndexEditorSrv");
            config.add_child("registry.view", registry);
        }
        {
            ::fwServices::IService::ConfigType registry;
            registry.put("<xmlattr>.sid", "snapshotSrv");
            config.add_child("registry.view", registry);
        }
        editorsView->configure(config);
    }

    // Begin generic scene.
    auto genericSceneSrv = m_appManager->addService("::fwRenderOgre::SRender", "genericSceneSrv", true, false);
    {
        ::fwServices::IService::ConfigType config;
        config.add("scene.background.<xmlattr>.topColor", "#36393E");
        config.add("scene.background.<xmlattr>.bottomColor", "#36393E");
        config.add("scene.layer.<xmlattr>.id", "default");
        config.add("scene.layer.<xmlattr>.order", "1");
        {
            ::fwServices::IService::ConfigType adaptor;
            adaptor.put("<xmlattr>.uid", "trackballInteractorAdp");
            config.add_child("scene.adaptor", adaptor);
        }
        {
            ::fwServices::IService::ConfigType adaptor;
            adaptor.put("<xmlattr>.uid", "textureAdp");
            config.add_child("scene.adaptor", adaptor);
        }
        {
            ::fwServices::IService::ConfigType adaptor;
            adaptor.put("<xmlattr>.uid", "meshAdp");
            config.add_child("scene.adaptor", adaptor);
        }
        {
            ::fwServices::IService::ConfigType adaptor;
            adaptor.put("<xmlattr>.uid", "negatoAdp");
            config.add_child("scene.adaptor", adaptor);
        }
        {
            ::fwServices::IService::ConfigType adaptor;
            adaptor.put("<xmlattr>.uid", "snapshotAdp");
            config.add_child("scene.adaptor", adaptor);
        }
        genericSceneSrv->configure(config);
    }

    auto trackballInteractorAdp = m_appManager->addService("::visuOgreAdaptor::STrackballCamera",
                                                           "trackballInteractorAdp", true, false);
    {
        ::fwServices::IService::ConfigType config;
        config.add("config.<xmlattr>.layer", "default");
        trackballInteractorAdp->configure(config);
    }

    auto textureAdp = m_appManager->addService("::visuOgreAdaptor::STexture", "textureAdp", true, false);
    {
        textureAdp->registerInput(texture, "image", true);
        ::fwServices::IService::ConfigType config;
        config.add("config.<xmlattr>.layer", "default");
        config.add("config.<xmlattr>.textureName", "textureID");
        textureAdp->configure(config);
    }

    auto meshAdp = m_appManager->addService("::visuOgreAdaptor::SMesh", "meshAdp", true, false);
    {
        meshAdp->registerInOut(mesh, "mesh", true);
        ::fwServices::IService::ConfigType config;
        config.add("config.<xmlattr>.layer", "default");
        config.add("config.<xmlattr>.textureName", "textureID");
        meshAdp->configure(config);
    }

    auto negatoAdp = m_appManager->addService("::visuOgreAdaptor::SNegato3D", "negatoAdp", true, false);
    {
        negatoAdp->registerInOut(image, "image", true );
        ::fwServices::IService::ConfigType config;
        config.add("config.<xmlattr>.layer", "default");
        config.add("config.<xmlattr>.sliceIndex", "axial");
        config.add("config.<xmlattr>.interactive", "true");
        negatoAdp->configure(config);
    }

    auto snapshotAdp = m_appManager->addService("::visuOgreAdaptor::SFragmentsInfo", "snapshotAdp", true, false);
    {
        snapshotAdp->registerInOut(snapshot, "image", true);
        ::fwServices::IService::ConfigType config;
        config.add("config.<xmlattr>.layer", "default");
        config.add("config.<xmlattr>.flip", "true");
        snapshotAdp->configure(config);
    }

    // Actions.
    auto openImageAct = m_appManager->addService("::gui::action::SStarter", "openImageAct", true, false);
    {
        ::fwServices::IService::ConfigType config;
        config.add("start.<xmlattr>.uid", "imageReaderSrv");
        openImageAct->configure(config);
    }

    auto openMeshAct = m_appManager->addService("::gui::action::SStarter", "openMeshAct", true, false);
    {
        ::fwServices::IService::ConfigType config;
        config.add("start.<xmlattr>.uid", "meshReaderSrv");
        openMeshAct->configure(config);
    }

    auto openTextureAct = m_appManager->addService("::gui::action::SStarter", "openTextureAct", true, false);
    {
        ::fwServices::IService::ConfigType config;
        config.add("start.<xmlattr>.uid", "textureReaderSrv");
        openTextureAct->configure(config);
    }

    auto quitAct = m_appManager->addService("::gui::action::SQuit", "quitAct", true, false);
    {
        quitAct->configure();
    }

    // Services.
    auto imageReaderSrv = m_appManager->addService("::uiIO::editor::SIOSelector", "imageReaderSrv", true, false);
    {
        imageReaderSrv->registerInOut(image, "data");
        ::fwServices::IService::ConfigType config;
        config.put("type.<xmlattr>.mode", "reader");
        imageReaderSrv->configure(config);
    }

    auto meshReaderSrv = m_appManager->addService("::uiIO::editor::SIOSelector", "meshReaderSrv", true, false);
    {
        meshReaderSrv->registerInOut(mesh, "data");
        ::fwServices::IService::ConfigType config;
        config.put("type.<xmlattr>.mode", "reader");
        meshReaderSrv->configure(config);
    }

    auto textureReaderSrv = m_appManager->addService("::uiIO::editor::SIOSelector", "textureReaderSrv", true, false);
    {
        textureReaderSrv->registerInOut(texture, "data");
        ::fwServices::IService::ConfigType config;
        config.put("type.<xmlattr>.mode", "reader");
        textureReaderSrv->configure(config);
    }

    auto showNegatoSrv =
        m_appManager->addService("::guiQt::editor::SSignalButton", "showNegatoSrv", true, false);
    {
        ::fwServices::IService::ConfigType config;
        config.add("config.checkable", true);
        config.add("config.icon", "flatIcon-0.1/icons/RedCross.svg");
        config.add("config.icon2", "flatIcon-0.1/icons/Layers.svg");
        config.add("config.iconWidth", "40");
        config.add("config.iconHeight", "16");
        config.add("config.checked", true);
        showNegatoSrv->configure(config);
    }

    auto sliderIndexEditorSrv = m_appManager->addService("::uiImageQt::SliceIndexPositionEditor",
                                                         "sliderIndexEditorSrv", true, false);
    {
        sliderIndexEditorSrv->registerInOut(image, "image", true);
        ::fwServices::IService::ConfigType config;
        config.put("sliceIndex", "axial");
        sliderIndexEditorSrv->configure(config);
    }

    auto snapshotSrv = m_appManager->addService("::guiQt::editor::SSignalButton", "snapshotSrv", true, false);
    {
        ::fwServices::IService::ConfigType config;
        config.add("config.checkable", false);
        config.add("config.icon", "flatIcon-0.1/icons/YellowPhoto.svg");
        snapshotSrv->configure(config);
    }

    auto imageWriterSrv = m_appManager->addService("::uiIO::editor::SIOSelector", "imageWriterSrv", true, false);
    {
        imageWriterSrv->registerInOut(snapshot, "data");
        ::fwServices::IService::ConfigType config;
        config.put("type.<xmlattr>.mode", "writer");
        config.put("selection.<xmlattr>.mode", "exclude");
        config.put("addSelection.<xmlattr>.service", "::ioAtoms::SWriter");
        imageWriterSrv->configure(config);
    }

    // Connections.
    {
        ::fwServices::helper::ProxyConnections connection;
        connection.addSignalConnection(meshReaderSrv->getID(), "jobCreated");
        connection.addSignalConnection(imageReaderSrv->getID(), "jobCreated");
        connection.addSignalConnection(textureReaderSrv->getID(), "jobCreated");
        connection.addSlotConnection(progressBarView->getID(), "showJob");
        m_appManager->addProxyConnection(connection);
    }
    {
        ::fwServices::helper::ProxyConnections connection;
        connection.addSignalConnection(snapshotSrv->getID(), "clicked");
        connection.addSlotConnection(imageWriterSrv->getID(), "update");
        m_appManager->addProxyConnection(connection);
    }
    {
        ::fwServices::helper::ProxyConnections connection;
        connection.addSignalConnection(showNegatoSrv->getID(), "toggled");
        connection.addSlotConnection(negatoAdp->getID(), "updateVisibility");
        m_appManager->addProxyConnection(connection);
    }

    // Start services.
    m_appManager->startServices();
}

//------------------------------------------------------------------------------

void Plugin::stop() noexcept
{
}

//------------------------------------------------------------------------------

void Plugin::uninitialize()
{
    m_appManager->destroy();
    m_appManager.reset();
}

//------------------------------------------------------------------------------

} // namespace Tuto02GenericSceneCpp.
