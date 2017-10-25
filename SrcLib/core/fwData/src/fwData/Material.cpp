/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwData/Material.hpp"

#include "fwData/Exception.hpp"
#include "fwData/registry/macros.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

fwDataRegisterMacro( ::fwData::Material );

namespace fwData
{

//------------------------------------------------------------------------------

const ::fwCom::Signals::SignalKeyType Material::s_ADDED_TEXTURE_SIG   = "addedTexture";
const ::fwCom::Signals::SignalKeyType Material::s_REMOVED_TEXTURE_SIG = "removedTexture";

//------------------------------------------------------------------------------

Material::Material(::fwData::Object::Key key) :
    m_shadingMode(PHONG),
    m_representationMode(SURFACE),
    m_optionsMode(STANDARD),
    m_ambient( Color::New(0.05f, 0.05f, 0.05f, 1.f) ),
    m_diffuse( Color::New() )
{
    newSignal< AddedTextureSignalType >(s_ADDED_TEXTURE_SIG);
    newSignal< RemovedTextureSignalType >(s_REMOVED_TEXTURE_SIG);

    // Fill the texture vector with nullptr
    for(int i = 0; i < m_numberOfTextureUnits; i++)
    {
        m_texture.push_back(nullptr);
    }
}

//------------------------------------------------------------------------------

Material::~Material()
{
}

//------------------------------------------------------------------------------

void Material::shallowCopy(const Object::csptr& _source )
{
    Material::csptr other = Material::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );
    this->fieldShallowCopy( _source );

    m_ambient = other->m_ambient;
    m_diffuse = other->m_diffuse;
    m_texture = other->m_texture;

    m_shadingMode        = other->m_shadingMode;
    m_representationMode = other->m_representationMode;
    m_optionsMode        = other->m_optionsMode;
}

//------------------------------------------------------------------------------

void Material::cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache)
{
    Material::csptr other = Material::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );
    this->fieldDeepCopy( _source, cache );

    m_ambient = ::fwData::Object::copy( other->m_ambient, cache );
    m_diffuse = ::fwData::Object::copy( other->m_diffuse, cache );

    for(size_t i = 0; i < other->getTextureContainerSize(); i++)
    {
        ::fwData::Texture::sptr ot = other->getTexture(i);
        if(ot != nullptr)
        {
            ::fwData::Texture::sptr dt = ::fwData::Texture::New();
            dt->deepCopy(ot);
            m_texture[i] = dt;
        }
    }

    m_shadingMode        = other->m_shadingMode;
    m_representationMode = other->m_representationMode;
    m_optionsMode        = other->m_optionsMode;
}

//------------------------------------------------------------------------------

Color::sptr Material::ambient() const
{
    return m_ambient;
}

//------------------------------------------------------------------------------

Color::sptr Material::diffuse() const
{
    return m_diffuse;
}

//------------------------------------------------------------------------------

::fwData::Texture::sptr Material::initTexture(const size_t id) const
{
    OSLM_ASSERT(
        "The number of texture units (" << m_texture.size() << ") is insufficient to map a texture to " << id << " " << m_texture.size(),
        id < m_texture.size());

    if(m_texture.at(id) == nullptr)
    {
        ::fwData::Texture::sptr t = ::fwData::Texture::New();
        m_texture[id]             = t;
    }

    return m_texture.at(id);
}

//------------------------------------------------------------------------------

::fwData::Texture::sptr Material::getTexture(const size_t id) const
{
    OSLM_ASSERT(
        "The number of texture units (" << m_texture.size() << ") is insufficient to map a texture to " << id << " " << m_texture.size(),
        id < m_texture.size());

    return m_texture.at(id);
}

//------------------------------------------------------------------------------

void Material::setAmbient(const Color::sptr& ambient)
{
    m_ambient = ambient;
}

//------------------------------------------------------------------------------

void Material::setDiffuse(const Color::sptr& diffuse)
{
    m_diffuse = diffuse;
}

//------------------------------------------------------------------------------

size_t Material::getTextureContainerSize() const
{
    return m_texture.size();
}

//------------------------------------------------------------------------------

} //namespace fwData
