
# set the log level of the application before the variable is cached
# this way we can have, by default, the log level required to show something in the log/terminal
set(SPYLOG_LEVEL_Tuto05MultithreadConsoleCpp info)

fwLoadProperties()

find_package(Boost QUIET COMPONENTS log REQUIRED)

fwLink(
    ${Boost_LOG_LIBRARY}
)
