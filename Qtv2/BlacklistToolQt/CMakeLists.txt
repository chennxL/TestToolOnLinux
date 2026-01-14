cmake_minimum_required(VERSION 3.16)
project(BlacklistTool VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 查找Qt6组件
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network)

# ========== PSI封装库配置 ==========
set(PSIWRAPPER_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/third_party/psiwrapper)
set(PSIWRAPPER_INCLUDE_DIR ${PSIWRAPPER_ROOT}/include)
set(PSIWRAPPER_LIB_DIR ${PSIWRAPPER_ROOT}/lib)

# 打印调试信息
message(STATUS "PSI Wrapper Include Dir: ${PSIWRAPPER_INCLUDE_DIR}")
message(STATUS "PSI Wrapper Lib Dir: ${PSIWRAPPER_LIB_DIR}")

# 包含目录
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${PSIWRAPPER_INCLUDE_DIR}
)

# 源文件
set(PROJECT_SOURCES
    # Main
    src/main.cpp
    src/mainwindow.cpp

    # Network
    network/networkrequest.cpp
    network/apiservice.cpp

    # Stores
    stores/blackliststore.cpp
    stores/testsetstore.cpp

    # Crypto
    crypto/cryptowrapper.cpp

    # Widgets
    widgets/createblacklistwidget.cpp
    widgets/createtestsetwidget.cpp
    widgets/encryptiontestwidget.cpp

    # Utils
    utils/messagehelper.cpp
)

# 头文件
set(PROJECT_HEADERS
    include/mainwindow.h
    include/networkrequest.h
    include/apiservice.h
    include/blackliststore.h
    include/testsetstore.h
    include/createblacklistwidget.h
    include/createtestsetwidget.h
    include/encryptiontestwidget.h
    include/messagehelper.h
    include/cryptowrapper.h
)

# 创建可执行文件
add_executable(${PROJECT_NAME}
    ${PROJECT_SOURCES}
    ${PROJECT_HEADERS}
    include/blacklistinfo.h
    include/blacklistbitdecoder.h
)

add_subdirectory(third_party/QXlsx/QXlsx)

# 链接Qt库和PSI封装库
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
    ${PSIWRAPPER_LIB_DIR}/libpsiwrapper.so
    ${PSIWRAPPER_LIB_DIR}/libpsi.so
    pthread
    QXlsx::QXlsx  # 添加这一行
)

# 设置运行时库路径
set_target_properties(${PROJECT_NAME} PROPERTIES
    INSTALL_RPATH "${PSIWRAPPER_LIB_DIR}"
    BUILD_WITH_INSTALL_RPATH TRUE
)

# 安装规则
install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    RUNTIME DESTINATION bin
)
