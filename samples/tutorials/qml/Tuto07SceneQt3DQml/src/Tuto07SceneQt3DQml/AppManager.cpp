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

#include "Tuto07SceneQt3DQml/AppManager.hpp"

#include <fwData/Mesh.hpp>

#include <fwServices/op/Add.hpp>
#include <fwServices/registry/ObjectService.hpp>

namespace Tuto07SceneQt3DQml
{

static const std::string s_MESH_ID = "mesh";

//------------------------------------------------------------------------------

AppManager::AppManager()
{
}

//------------------------------------------------------------------------------

AppManager::~AppManager()
{
}

//------------------------------------------------------------------------------

void AppManager::initialize()
{
    this->create();
}

//------------------------------------------------------------------------------

void AppManager::uninitialize()
{
    this->stopAndUnregisterServices();
    this->destroy();
}

//------------------------------------------------------------------------------

::fwRenderQt3D::data::Mesh* AppManager::getMesh()
{
    return m_mesh;
}

//------------------------------------------------------------------------------

::fwRenderQt3D::core::GenericScene* AppManager::getScene()
{
    return m_scene;
}

//------------------------------------------------------------------------------

void AppManager::setMesh(::fwRenderQt3D::data::Mesh* _mesh)
{
    m_mesh = _mesh;
}

//------------------------------------------------------------------------------

void AppManager::setScene(::fwRenderQt3D::core::GenericScene* _scene)
{
    m_scene = _scene;
}

//------------------------------------------------------------------------------

void AppManager::onOpenModel()
{
    // Reads a mesh.
    const auto meshReader = ::fwServices::add("::uiIO::editor::SIOSelector");
    ::fwServices::IService::ConfigType meshReaderConfig;
    meshReaderConfig.put("type.<xmlattr>.mode", "reader");
    meshReaderConfig.put("type.<xmlattr>.class", "::fwData::Mesh");
    meshReader->setConfiguration(meshReaderConfig);
    meshReader->configure();
    meshReader->start();
    meshReader->update();
    auto mesh = meshReader->getLockedOutput< ::fwData::Mesh >("data").get_shared();
    this->addObject(mesh, s_MESH_ID);

    // Associates the mesh with the one declared in 'ui.qml'.
    if(mesh)
    {
        m_mesh->setScene(m_scene);
        m_mesh->setMesh(mesh);
        m_mesh->centerCameraOnMesh();
    }

    meshReader->stop();
    ::fwServices::OSR::unregisterService(meshReader);
}

//------------------------------------------------------------------------------

} // namespace Tuto07SceneQt3DQml.
