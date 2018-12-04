/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#ifdef _WIN32
#include <windows.h>
#endif

#include <fwRuntime/io/ProfileReader.hpp>
#include <fwRuntime/operations.hpp>
#include <fwRuntime/profile/Profile.hpp>

#include <boost/filesystem.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/positional_options.hpp>
#include <boost/program_options/variables_map.hpp>

#include <csignal>
#include <ostream>
#include <stdio.h>
#include <string>
#include <vector>

#ifdef _WIN32
# define CONSOLE_LOG false;
# define FILE_LOG true;
#else
# define CONSOLE_LOG true;
# define FILE_LOG false;
#endif

#ifndef DEFAULT_PROFILE
#define DEFAULT_PROFILE profile.xml
#endif

#define GET_DEFAULT_PROFILE(x) #x
#define GET_DEFAULT_PROFILE2(x) GET_DEFAULT_PROFILE(x)
#define DEFAULT_PROFILE_STRING  GET_DEFAULT_PROFILE2(DEFAULT_PROFILE)

//------------------------------------------------------------------------------
#if defined(_WIN32) && _MSC_VER > 1499 &&  _MSC_VER < 1600 // Visual C++ 2008 only

 #pragma message ( "Setting up manifest..." )

 #if defined(_DEBUG)
// add a dependency on the retail crt even in debug
     #pragma comment(linker,"/manifestdependency:\"type='win32' " \
    "name='" __LIBRARIES_ASSEMBLY_NAME_PREFIX ".CRT' " \
    "version='" _CRT_ASSEMBLY_VERSION "' " \
    "processorArchitecture='*' " \
    "publicKeyToken='" _VC_ASSEMBLY_PUBLICKEYTOKEN "' " \
    "language='*'\"")
 #endif /* _DEBUG */

 #pragma comment(linker,"/manifestdependency:\"type='win32' " \
    "name='Microsoft.Windows.Common-Controls' " \
    "version='6.0.0.0' " \
    "processorArchitecture='*' " \
    "publicKeyToken='6595b64144ccf1df' " \
    "language='*'\"")

#endif /* _WIN32 && _MSC_VER > 1499 &&  _MSC_VER < 1600 */

//------------------------------------------------------------------------------
namespace po = boost::program_options;
namespace fs = boost::filesystem;

typedef fs::path PathType;
typedef std::vector< PathType > PathListType;
typedef std::vector< std::string > StringListType;

namespace std
{
//------------------------------------------------------------------------------

template<class A1, class A2>
inline ostream& operator<<(ostream& s, vector<A1, A2> const& vec)
{
    copy(vec.begin(), vec.end(), ostream_iterator<A1>(s, " "));
    return s;
}
}

//-----------------------------------------------------------------------------

#ifdef __APPLE__
std::pair<std::string, std::string> parsePns(const std::string& s)
{
    if (s.substr(0, 5) == "-psn_")
    {
        return std::make_pair(std::string("psn"), s.substr(5));
    }
    return std::make_pair(std::string(), std::string());
}
#endif

/// Wrapper for boost::filesystem::absolute, needed by clang 3.0 in use with
/// std::transform
PathType absolute( const PathType& path )
{
    return fs::absolute(path).normalize();
}

//-----------------------------------------------------------------------------

volatile sig_atomic_t gSignalStatus = 0;
//------------------------------------------------------------------------------

void signal_handler(int signal)
{
    gSignalStatus = signal;
    ::fwRuntime::profile::getCurrentProfile()->cleanup();
    ::fwRuntime::profile::getCurrentProfile()->stop();
}

//-----------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    PathListType bundlePaths;
    PathType rwd;
    PathType profileFile;
    ::fwRuntime::profile::Profile::ParamsContainer profileArgs;

    // Launcher options
    po::options_description options("Launcher options");
    options.add_options()
        ("help,h", "Show help message")
        ("bundle-path,B", po::value(&bundlePaths)->default_value
            (PathListType(1, (::fwRuntime::Runtime::getDefault()->getWorkingPath() / BUNDLE_RC_PREFIX).string())),
        "Adds a bundle path")
        ("rwd", po::value(&rwd)->default_value("./"), "Sets runtime working directory")
    ;

    // Log options
    bool consoleLog = CONSOLE_LOG;
    bool fileLog    = FILE_LOG;
    std::string logFile;
    const std::string defaultLogFile = "SLM.log";

    typedef ::fwCore::log::SpyLogger SpyLogger;
    int logLevel = SpyLogger::SL_TRACE;

    po::options_description logOptions("Log options");
    logOptions.add_options()
        ("clog", po::value(&consoleLog)->implicit_value(true)->zero_tokens(), "Enable log output to console")
        ("no-clog", po::value(&consoleLog)->implicit_value(false)->zero_tokens(), "Disable log output to console")
        ("flog", po::value(&fileLog)->implicit_value(true)->zero_tokens(), "Enable log output to file")
        ("no-flog", po::value(&fileLog)->implicit_value(false)->zero_tokens(), "Disable log output to file")
        ("log-output", po::value(&logFile)->default_value(defaultLogFile), "Log output filename")

        ("log-trace", po::value(&logLevel)->implicit_value(SpyLogger::SL_TRACE)->zero_tokens(), "Set loglevel to trace")
        ("log-debug", po::value(&logLevel)->implicit_value(SpyLogger::SL_DEBUG)->zero_tokens(), "Set loglevel to debug")
        ("log-info",  po::value(&logLevel)->implicit_value(SpyLogger::SL_INFO )->zero_tokens(), "Set loglevel to info")
        ("log-warn",  po::value(&logLevel)->implicit_value(SpyLogger::SL_WARN )->zero_tokens(), "Set loglevel to warn")
        ("log-error", po::value(&logLevel)->implicit_value(SpyLogger::SL_ERROR)->zero_tokens(), "Set loglevel to error")
        ("log-fatal", po::value(&logLevel)->implicit_value(SpyLogger::SL_FATAL)->zero_tokens(), "Set loglevel to fatal")
    ;

    // Hidden options
    po::options_description hidden("Hidden options");
    hidden.add_options()
#ifdef __APPLE__
    ("psn", po::value<std::string>(), "Application PSN number")
        ("NSDocumentRevisionsDebugMode", po::value<std::string>()->zero_tokens(), "DocumentRevisionsDebugMode")
#endif
    ("profile", po::value(&profileFile)->default_value(DEFAULT_PROFILE_STRING), "Profile file")
        ("profile-args", po::value(&profileArgs)->multitoken(), "Profile args")
    ;

    // Set options
    po::options_description cmdline_options;
    cmdline_options.add(options).add(logOptions).add(hidden);

    po::positional_options_description p;
    p.add("profile", 1).add("profile-args", -1);

    // Get options
    po::variables_map vm;

    try
    {
        po::store(po::command_line_parser(argc, argv)
                  .options(cmdline_options)
#ifdef __APPLE__
                  .extra_parser(parsePns)
#endif
                  .positional(p)
                  .run(),
                  vm);
        po::notify(vm);
    }
    catch(const po::error& e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    // If help
    if (vm.count("help"))
    {
        std::cout << "usage: " << argv[0] << " [options] [profile(=profile.xml)] [profile-args ...]" << std::endl;
        std::cout << "  use '--' to stop processing args for fwlauncher" << std::endl  << std::endl;
        std::cout << options << std::endl << logOptions << std::endl;
        return 0;
    }

    // Log file
    SpyLogger& logger = fwCore::log::SpyLogger::getSpyLogger();

    if(consoleLog)
    {
        logger.addStreamAppender(std::clog, static_cast<SpyLogger::LevelType>(logLevel));
    }

    if(fileLog)
    {
        FILE* pFile = fopen(logFile.c_str(), "w");
        if (pFile == NULL)
        {
            ::boost::system::error_code err;
            PathType sysTmp = fs::temp_directory_path(err);
            if(err.value() != 0)
            {
                // replace log file appender by stream appender: default dir and temp dir unreachable
                logger.addStreamAppender(std::clog, static_cast<SpyLogger::LevelType>(logLevel));
            }
            else
            {
                // creates SLM.log in temp directory: default dir unreachable
                sysTmp  = sysTmp / "SLM.log";
                logFile = sysTmp.string();
                logger.addFileAppender(logFile, static_cast<SpyLogger::LevelType>(logLevel));
            }
        }
        else
        {
            // creates SLM.log in default logFile directory
            logger.addFileAppender(logFile, static_cast<SpyLogger::LevelType>(logLevel));
        }
        fclose(pFile);
    }

#ifdef __APPLE__
    fs::path execPath = argv[0];

    if ( execPath.string().find(".app/") != std::string::npos || vm.count("psn"))
    {
        bool isChdirOkOSX = false;

        fs::path execPath = argv[0];

        while ( fs::extension(execPath) != ".app"
                && execPath != execPath.parent_path()
                && !fs::is_directory( execPath / fs::path(BUNDLE_RC_PREFIX))
                )
        {
            execPath = execPath.parent_path();
        }

        if ( fs::is_directory( execPath / "Contents" / fs::path(BUNDLE_RC_PREFIX) ) )
        {
            execPath = execPath / "Contents";
        }
        else
        {
            OSLM_ERROR_IF("Bundle directory not found.", !fs::is_directory( execPath / fs::path(BUNDLE_RC_PREFIX) ));
        }

        isChdirOkOSX = (chdir(execPath.string().c_str()) == 0);

        SLM_ERROR_IF("Was not able to find a directory to change to.", !isChdirOkOSX);
    }
#endif

    OSLM_INFO_IF( "Runtime working directory: " << rwd << " => " << ::absolute(rwd), vm.count("rwd") );

    OSLM_INFO_IF( "Profile path: " << profileFile << " => " << ::absolute(profileFile), vm.count("profile"));
    OSLM_INFO_IF( "Profile-args: " << profileArgs, vm.count("profile-args") );

    // Check if path exist
    OSLM_FATAL_IF( "Runtime working directory doesn't exist: " << rwd.string() << " => " << ::absolute(
                       rwd), !::boost::filesystem::exists(rwd.string()) );

    OSLM_FATAL_IF( "Profile path doesn't exist: " << profileFile.string() << " => " << ::absolute(
                       profileFile), !::boost::filesystem::exists(profileFile.string()));

    std::transform( bundlePaths.begin(), bundlePaths.end(), bundlePaths.begin(), ::absolute );
    profileFile = ::absolute(profileFile);
    rwd         = ::absolute(rwd);

    // Automatically adds the bundle folders where the profile.xml is located if it was not already there
    const auto profileBundlePath = profileFile.parent_path().parent_path();
    bool findProfileBundlePath   = false;
    for(const fs::path& bundlePath :  bundlePaths )
    {
        if(profileBundlePath == bundlePath)
        {
            findProfileBundlePath = true;
        }
    }
    if(!findProfileBundlePath)
    {
        bundlePaths.push_back(profileBundlePath);
    }

    for(const fs::path& bundlePath :  bundlePaths )
    {
        OSLM_INFO_IF( "Bundle paths are: " << bundlePath.string() << " => " << ::absolute(bundlePath),
                      vm.count("bundle-path") );
    }
    for(const fs::path& bundlePath :  bundlePaths )
    {
        OSLM_FATAL_IF( "Bundle paths doesn't exist: " << bundlePath.string() << " => " << ::absolute(
                           bundlePath), !::boost::filesystem::exists(bundlePath.string()) );
    }

#ifdef _WIN32
    const bool isChdirOk = static_cast<bool>(SetCurrentDirectory(rwd.string().c_str()) != 0);
#else
    const bool isChdirOk = ( chdir(rwd.string().c_str()) == 0 );
#endif // _WIN32
    OSLM_FATAL_IF( "Was not able to change directory to : " << rwd, !isChdirOk);

    for(const fs::path& bundlePath :  bundlePaths )
    {
        if ( fs::is_directory(bundlePath))
        {
            ::fwRuntime::addBundles( bundlePath );
        }
        else
        {
            OSLM_ERROR( "Bundle path " << bundlePath << " do not exists or is not a directory.");
        }
    }

    int retValue = 0;

    if ( fs::is_regular_file(profileFile))
    {
        ::fwRuntime::profile::Profile::sptr profile;

        try
        {
            profile = ::fwRuntime::io::ProfileReader::createProfile(profileFile);
            ::fwRuntime::profile::setCurrentProfile(profile);

            // Install a signal handler
            std::signal(SIGINT, signal_handler);

            profile->setParams(profileArgs);

            profile->start();
            profile->run();
            if(gSignalStatus == 0)
            {
                profile->stop();
            }
        }
        catch(std::exception& e)
        {
            OSLM_FATAL( e.what() );
            retValue = 1;
        }
        catch(...)
        {
            SLM_FATAL( "An unrecoverable error has occurred." );
            retValue = 1;
        }
    }
    else
    {
        OSLM_ERROR( "Profile file " << profileFile << " do not exists or is not a regular file.");
        retValue = 1;
    }

    return retValue;
}

//-----------------------------------------------------------------------------

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR args, int)
{
    return main(__argc, __argv);
}
#endif // _WIN32
