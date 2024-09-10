cmake_minimum_required(VERSION 3.16)

project(CtpDemo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_executable(
    CtpDemo
    src/main.cpp
    src/MdHandler.cpp
    src/TraderHandler.cpp
    src/config.cpp
    src/utils.cpp
)

include(GNUInstallDirs)
install(TARGETS CtpDemo
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

add_definitions("-fsized-deallocation")
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin") # mac
    target_link_libraries(CtpDemo ${CMAKE_SOURCE_DIR}/vender/ctp/mac/thostmduserapi_se.framework)
    target_link_libraries(CtpDemo ${CMAKE_SOURCE_DIR}/vender/ctp/mac/thosttraderapi_se.framework)
    target_include_directories(CtpDemo PRIVATE ${CMAKE_CURRENT_LIST_DIR}/vender/ctp/mac/thostmduserapi_se.framework/Versions/A/Headers)
    target_include_directories(CtpDemo PRIVATE ${CMAKE_CURRENT_LIST_DIR}/vender/ctp/mac/thosttraderapi_se.framework/Versions/A/Headers)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux") # linux
    target_link_libraries(CtpDemo bsd) #https://linux.die.net/man/3/strlcpy
    target_link_libraries(CtpDemo ${CMAKE_SOURCE_DIR}/vender/ctp/linux/libthostmduserapi_se.so)
    target_link_libraries(CtpDemo ${CMAKE_SOURCE_DIR}/vender/ctp/linux/libthosttraderapi_se.so)
    target_include_directories(CtpDemo PRIVATE ${CMAKE_CURRENT_LIST_DIR}/vender/ctp/linux)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Windows") # windows

endif()

target_include_directories(CtpDemo PRIVATE ${CMAKE_CURRENT_LIST_DIR}/include)
target_include_directories(CtpDemo PRIVATE ${CMAKE_CURRENT_LIST_DIR}/vender/utils/json)
target_include_directories(CtpDemo PRIVATE ${CMAKE_CURRENT_LIST_DIR}/vender/utils/base64)
configure_file(${CMAKE_CURRENT_LIST_DIR}/config/account-config.json account-config.json COPYONLY)
