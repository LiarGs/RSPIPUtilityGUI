cmake_minimum_required(VERSION 3.16)

project(RSPIPUtility VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(OpenCV REQUIRED)
find_package(GDAL REQUIRED)
find_package(OpenMP)

set(RSPIP_ROOT "$ENV{RSPIP_ROOT}" CACHE PATH "Path to the RSPIPUtilities repository root")
set(RSPIP_LIBRARY_PATH "$ENV{RSPIP_LIBRARY_PATH}" CACHE FILEPATH "Path to the RSPIP core static library")

set(_RSPIP_ROOT_CANDIDATES
    "${CMAKE_CURRENT_SOURCE_DIR}/../RSPIPUtilities"
    "E:/RSPIP/GuoShuai/RSPIPUtilities"
)

if(NOT RSPIP_ROOT OR NOT EXISTS "${RSPIP_ROOT}/include")
    foreach(candidate IN LISTS _RSPIP_ROOT_CANDIDATES)
        if(EXISTS "${candidate}/include")
            set(RSPIP_ROOT
                "${candidate}"
                CACHE PATH
                "Path to the RSPIPUtilities repository root"
                FORCE)
            message(STATUS "Auto-detected RSPIP_ROOT: ${RSPIP_ROOT}")
            break()
        endif()
    endforeach()
endif()

if(NOT RSPIP_ROOT OR NOT EXISTS "${RSPIP_ROOT}/include")
    message(FATAL_ERROR
        "RSPIP_ROOT is not set or invalid. Please point it to the RSPIPUtilities repository root.")
endif()

if(NOT RSPIP_LIBRARY_PATH OR NOT EXISTS "${RSPIP_LIBRARY_PATH}")
    set(_RSPIP_LIBRARY_CANDIDATES
        "${RSPIP_ROOT}/build/Release/RSPIP_Core.lib"
        "${RSPIP_ROOT}/build/RelWithDebInfo/RSPIP_Core.lib"
        "${RSPIP_ROOT}/build/Debug/RSPIP_Core.lib"
    )

    foreach(candidate IN LISTS _RSPIP_LIBRARY_CANDIDATES)
        if(EXISTS "${candidate}")
            set(RSPIP_LIBRARY_PATH
                "${candidate}"
                CACHE FILEPATH
                "Path to the RSPIP core static library"
                FORCE)
            message(STATUS "Auto-detected RSPIP library: ${RSPIP_LIBRARY_PATH}")
            break()
        endif()
    endforeach()
endif()

if(NOT RSPIP_LIBRARY_PATH OR NOT EXISTS "${RSPIP_LIBRARY_PATH}")
    message(FATAL_ERROR
        "RSPIP_LIBRARY_PATH is not set or invalid. Please point it to RSPIP_Core.lib.")
endif()

set(UI_SOURCES
    UI/MainWindow.h
    UI/MainWindow.cpp
    UI/Common/FileListWidget.h
    UI/Common/FileListWidget.cpp
    UI/Common/FileSelectWidget.h
    UI/Common/FileSelectWidget.cpp
    UI/Pages/ModulePageBase.h
    UI/Pages/ModulePageBase.cpp
)

set(PANEL_SOURCES
    Panels/AlgorithmPanelBase.h
    Panels/Common/ImageTaskHelper.h
    Panels/Common/ImageTaskHelper.cpp
    Panels/CloudDetection/PixelThresholdPanel.h
    Panels/CloudDetection/PixelThresholdPanel.cpp
    Panels/ColorBalance/MatchStatisticsPanel.h
    Panels/ColorBalance/MatchStatisticsPanel.cpp
    Panels/Evaluation/EvaluatorPanelBase.h
    Panels/Evaluation/EvaluatorPanelBase.cpp
    Panels/Evaluation/PSNREvaluatorPanel.h
    Panels/Evaluation/PSNREvaluatorPanel.cpp
    Panels/Evaluation/RMSEEvaluatorPanel.h
    Panels/Evaluation/RMSEEvaluatorPanel.cpp
    Panels/Evaluation/SSIMEvaluatorPanel.h
    Panels/Evaluation/SSIMEvaluatorPanel.cpp
    Panels/Evaluation/BoundaryGradientEvaluatorPanel.h
    Panels/Evaluation/BoundaryGradientEvaluatorPanel.cpp
    Panels/Mosaic/MosaicPanelBase.h
    Panels/Mosaic/MosaicPanelBase.cpp
    Panels/Mosaic/SimplePanel.h
    Panels/Mosaic/SimplePanel.cpp
    Panels/Mosaic/ShowOverlapPanel.h
    Panels/Mosaic/ShowOverlapPanel.cpp
    Panels/Mosaic/AdaptivePatchPanel.h
    Panels/Mosaic/AdaptivePatchPanel.cpp
    Panels/Mosaic/AdaptiveColorBalancePatchPanel.h
    Panels/Mosaic/AdaptiveColorBalancePatchPanel.cpp
    Panels/Mosaic/AdaptiveIsophotePatchPanel.h
    Panels/Mosaic/AdaptiveIsophotePatchPanel.cpp
    Panels/Preprocess/GeoCoordinateAlignPanel.h
    Panels/Preprocess/GeoCoordinateAlignPanel.cpp
    Panels/Reconstruct/ReconstructPanelBase.h
    Panels/Reconstruct/ReconstructPanelBase.cpp
    Panels/Reconstruct/SimplePanel.h
    Panels/Reconstruct/SimplePanel.cpp
    Panels/Reconstruct/ColorBalancePanel.h
    Panels/Reconstruct/ColorBalancePanel.cpp
    Panels/Reconstruct/IsophotePanel.h
    Panels/Reconstruct/IsophotePanel.cpp
)

set(APPLICATION_SOURCES
    Application/Execution/AlgorithmRequestBase.h
    Application/Execution/AlgorithmRequest.h
    Application/Execution/AlgorithmRequestCommon.h
    Application/Execution/AlgorithmRequestCommon.cpp
    Application/Execution/CloudDetectionRequest.h
    Application/Execution/CloudDetectionRequest.cpp
    Application/Execution/ColorBalanceRequest.h
    Application/Execution/ColorBalanceRequest.cpp
    Application/Execution/PreprocessRequest.h
    Application/Execution/PreprocessRequest.cpp
    Application/Execution/ReconstructRequest.h
    Application/Execution/ReconstructRequest.cpp
    Application/Execution/MosaicRequest.h
    Application/Execution/MosaicRequest.cpp
    Application/Execution/EvaluationRequest.h
    Application/Execution/EvaluationRequest.cpp
    Application/Execution/ExecutionContext.h
    Application/Registry/ModuleRegistry.h
    Application/Registry/ModuleRegistry.cpp
)

set(INFRASTRUCTURE_SOURCES
    Infrastructure/Execution/ExecutionTypes.h
    Infrastructure/Execution/OutputPlanner.h
    Infrastructure/Execution/OutputPlanner.cpp
    Infrastructure/Execution/TaskExecutor.h
    Infrastructure/Execution/TaskExecutor.cpp
    Infrastructure/IO/ImageIOService.h
    Infrastructure/IO/ImageIOService.cpp
)

set(PROJECT_SOURCES
    main.cpp
    ${UI_SOURCES}
    ${PANEL_SOURCES}
    ${APPLICATION_SOURCES}
    ${INFRASTRUCTURE_SOURCES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(RSPIPUtility
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
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

target_include_directories(RSPIPUtility PRIVATE
    ${RSPIP_ROOT}/include
    ${GDAL_INCLUDE_DIR}
    ${OpenCV_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/UI
)

target_link_libraries(RSPIPUtility PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    ${RSPIP_LIBRARY_PATH}
    ${OpenCV_LIBS}
    ${GDAL_LIBRARIES}
)

if(OpenMP_CXX_FOUND)
    target_link_libraries(RSPIPUtility PRIVATE OpenMP::OpenMP_CXX)
endif()

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
