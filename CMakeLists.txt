cmake_minimum_required(VERSION 3.16)

project(RSPIPUtility VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -----------------------------------------------------------------------------
# 2. 依赖查找
# -----------------------------------------------------------------------------

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(OpenCV REQUIRED)
find_package(GDAL REQUIRED)
find_package(OpenMP)

# -----------------------------------------------------------------------------
# 3. 配置 RSPIP 库路径

# RSPIP 头文件所在的根目录 (包含 include/RSPIP.h 的那个目录)
set(RSPIP_INCLUDE_DIR "E:/RSPIP/GuoShuai/RSPIPUtilities")

# RSPIP 静态库文件的绝对路径
set(RSPIP_LIBRARY_PATH "E:/RSPIP/GuoShuai/RSPIPUtilities/build/Release/RSPIP_Core.lib")

# -----------------------------------------------------------------------------
# 源文件设置
# -----------------------------------------------------------------------------
set(PROJECT_SOURCES
    main.cpp
    MainWindow.cpp MainWindow.h
    ResultViewer.cpp ResultViewer.h
    
    # [新增] 通用控件
    Common/FileSelectWidget.cpp Common/FileSelectWidget.h 
        Common/FileListWidget.cpp Common/FileListWidget.h 
    
    MosaicPage.cpp MosaicPage.h
    ReconstructPage.cpp ReconstructPage.h
    ColorBalancePage.cpp ColorBalancePage.h
    IAlgorithmPage.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(RSPIPUtility
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        Test.hpp
        IAlgorithmPage.h
        MosaicPage.h MosaicPage.cpp
        ReconstructPage.h ReconstructPage.cpp
        ColorBalancePage.h ColorBalancePage.cpp
        ResultViewer.h ResultViewer.cpp
    )
else()
    if(ANDROID)
        add_library(RSPIPUtility SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(RSPIPUtility
            ${PROJECT_SOURCES}
        )
    endif()
endif()

# -----------------------------------------------------------------------------
# 4. 包含路径与链接库
# -----------------------------------------------------------------------------

# 添加头文件搜索路径
target_include_directories(RSPIPUtility PRIVATE
    ${RSPIP_INCLUDE_DIR}/include     # 为了能引用 "RSPIP.h"
    ${GDAL_INCLUDE_DIR}
    ${OpenCV_INCLUDE_DIRS}
)

# 链接库
target_link_libraries(RSPIPUtility PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    ${RSPIP_LIBRARY_PATH}   # 链接RSPIP静态库
    ${OpenCV_LIBS}
    ${GDAL_LIBRARIES}
)

if(OpenMP_CXX_FOUND)
    target_link_libraries(RSPIPUtility PRIVATE OpenMP::OpenMP_CXX)
endif()

# -----------------------------------------------------------------------------
# 平台特定设置 (保持默认即可)
# -----------------------------------------------------------------------------
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.RSPIPUtility)
endif()
set_target_properties(RSPIPUtility PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS RSPIPUtility
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(RSPIPUtility)
endif()
