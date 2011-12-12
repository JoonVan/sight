/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iostream>

#include <fwCore/base.hpp>

#include <boost/version.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>

#include "fwXML/validator/DataFolderValidator.hpp"

namespace fwXML
{

//-----------------------------------------------------------------------------

DataFolderValidator::DataFolderValidator()
{
}

//-----------------------------------------------------------------------------

DataFolderValidator::~DataFolderValidator()
{}

//-----------------------------------------------------------------------------

/// erase schema and search them on given folder
void DataFolderValidator::collecteSchema(const ::boost::filesystem::path &folder)
{
    namespace fs = ::boost::filesystem;

    OSLM_ASSERT(folder.string() << " is not a directory", fs::is_directory(folder) );

    fs::directory_iterator end_iter;
    for ( fs::directory_iterator dir_itr( folder );   dir_itr != end_iter;   ++dir_itr )
    {
#if BOOST_FILESYSTEM_VERSION > 2
        if (fs::extension( dir_itr->path().filename()  ) == ".xsd" )
#else
        if (fs::extension( dir_itr->filename()  ) == ".xsd" )
#endif
        {
            registerSchema( *dir_itr );
        }
    }
}

//-----------------------------------------------------------------------------

/// append schema
void DataFolderValidator::registerSchema(const ::boost::filesystem::path &xsdfilename)
{
    std::string className = ::boost::filesystem::basename( xsdfilename.leaf() );
    OSLM_INFO("Registering schema from " << xsdfilename.string() << " for class " << className );
    m_schemaPath[className] = xsdfilename;
}

//-----------------------------------------------------------------------------

void DataFolderValidator::clearErrorLog()
{
    m_errorLog.clear();
}

//-----------------------------------------------------------------------------

const std::string DataFolderValidator::getErrorLog() const
{
    return m_errorLog;
}

//-----------------------------------------------------------------------------

const bool DataFolderValidator::validate( xmlNodePtr node )
{
    std::vector< xmlNodePtr > nodes;
    bool result = validateSingle(node);
    node = node->children;
    while ( result && node)
    {
        if ( node->type == XML_ELEMENT_NODE )
        {
            nodes.push_back(node);
        }
        node = node->next;
    }

    std::vector< xmlNodePtr >::iterator iter;
    xmlDocPtr  doc = xmlNewDoc(BAD_CAST "1.0");
    xmlNodePtr subNode;
    for(iter = nodes.begin(); iter != nodes.end(); ++iter)
    {
        // create a sub doc to validate
        // see http://mail.gnome.org/archives/xml/2006-May/msg00094.html for pb detail
        subNode = *iter;
        xmlDocSetRootElement(doc, subNode);

        result &= validate(subNode);

        if (result == false)
        {
            return false; // with correct errlog setted
        }
    }
    xmlFreeDoc(doc);
    return result;
}

//-----------------------------------------------------------------------------

const bool DataFolderValidator::validateSingle( xmlNodePtr node )
{
    SLM_ASSERT("node not instanced", node);
    std::string key( (const char*)(node->name) );
    ::boost::filesystem::path xsdPath = m_schemaPath[key];
    bool result = true;

    OSLM_DEBUG("key " << key << " - Path: " << xsdPath.string());


    if ( !xsdPath.empty() )
    {
        std::string path;
#if BOOST_FILESYSTEM_VERSION > 2
        path = xsdPath.string();
#else
        path = xsdPath.native_file_string();
#endif
        if (m_validators.find(path) == m_validators.end())
        {
            ::fwRuntime::io::Validator validator(xsdPath);
            m_validators.insert( ValidatorMapType::value_type(path, validator));
        }
        //xmlNodePtr newNode = xmlCopyNode(node,1);
        ::fwRuntime::io::Validator &validator = (*m_validators.find(path)).second;

        OSLM_INFO(" DataFolderValidator::validateSingle " << key );

        result = validator.validate(node);
        m_errorLog = validator.getErrorLog();
        OSLM_INFO(" DataFolderValidator " << key << " validation result(OK=1)=" << result);

    }
    OSLM_DEBUG_IF("No schema validation for XML node " << key << " : assuming content correct", xsdPath.empty() );

    return result;
}

//-----------------------------------------------------------------------------

void DataFolderValidator::copySchema(const ::boost::filesystem::path &dstFolder, const ::boost::filesystem::path &srcFolder  )
{
    namespace fs = ::boost::filesystem;

    // set the correct source
    // * srcFolder if specified
    // share/fwXML-XXX/ else

    ::boost::filesystem::path sourcePath;
    if ( srcFolder.empty() )
    {
        std::ostringstream pathLocation;
        pathLocation << "share/fwXML_" <<  FWXML_VER;
        sourcePath = fs::current_path() / pathLocation.str();
    }
    else
    {
        sourcePath = srcFolder;
    }

    // copy all XSD file from srcFolder to dstFolder
    SLM_ASSERT("Source path: "<<sourcePath<<", is not a directory.", fs::is_directory(sourcePath));
    SLM_ASSERT("Dest folder: "<<dstFolder<<", is not a directory.", fs::is_directory(dstFolder));

    fs::directory_iterator end_iter;
    for ( fs::directory_iterator dir_itr( sourcePath );   dir_itr != end_iter;   ++dir_itr )
    {

        if ( !fs::is_directory( *dir_itr ) )
        {
#if BOOST_FILESYSTEM_VERSION > 2
            if (fs::extension( dir_itr->path().filename()  ) == ".xsd" )
            {
                fs::path dstFile = dstFolder / dir_itr->path().filename();
#else
                if (fs::extension( dir_itr->filename()  ) == ".xsd" )
                {
                    fs::path dstFile = dstFolder / dir_itr->filename();
#endif
                if (  fs::exists(dstFile ) )
                {
                    fs::remove( dstFile ); // allow overwriting schema
                }
                fs::copy_file( *dir_itr,  dstFile );
            }
        }
    }
}

//-----------------------------------------------------------------------------

}
