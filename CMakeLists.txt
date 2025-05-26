cmake_minimum_required(VERSION 3.16)

project(VisorNiftiQt VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Buscar Qt (Qt6 o Qt5) y los módulos Widgets y PrintSupport
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets PrintSupport)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets PrintSupport)

# ITK
set(ITK_DIR "usr/local/lib/cmake/ITK-6.0")
find_package(ITK REQUIRED)
include_directories(${ITK_INCLUDE_DIRS})
link_directories(${ITK_LIBRARY_DIRS})
add_definitions(${ITK_DEFINITIONS})

# OpenCV
set(OpenCV_DIR "/home/bryam/Música/opencv/build/lib/cmake/opencv4")
find_package(OpenCV REQUIRED COMPONENTS core imgproc highgui videoio)
include_directories(${OpenCV_INCLUDE_DIRS})

# Fuentes
set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui

    nifti_loader/nifti_loader.h
    nifti_loader/nifti_loader.cpp
    slice_extractor/slice_extractor.h
    slice_extractor/slice_extractor.cpp

    frontend/welcomepage.cpp
    frontend/welcomepage.h
    frontend/slicepage.cpp
    frontend/slicepage.h

    filters/mainfilter.cpp
    filters/mainfilter.h
    filters/filter_thresholding/threshold.cpp
    filters/filter_thresholding/threshold.h
    filters/filter_suavizado/suavizado.cpp
    filters/filter_suavizado/suavizado.h
    utils/nifti_utils/nifti_utils.cpp
    utils/nifti_utils/nifti_utils.h
    utils/slice_renderer/slice_renderer.cpp
    utils/slice_renderer/slice_renderer.h
    utils/statics_generator/qcustomplot.cpp
    utils/statics_generator/qcustomplot.h
    utils/utils_main.cpp
    utils/utils_main.h
    utils/statics_generator/statistics.cpp
    utils/statics_generator/statistics.h
    utils/statics_generator/statswindow.cpp
    utils/statics_generator/statswindow.h
    filters/filter_contrast/contrast.cpp
    filters/filter_contrast/contrast.h
    filters/filter_bordes/bordes.cpp
    filters/filter_bordes/bordes.h
    filters/filter_color_thresh/color_thresh.cpp
    filters/filter_color_thresh/color_thresh.h
    filters/filter_pixel_ops/pixel_ops.cpp
    filters/filter_pixel_ops/pixel_ops.h
    filters/filter_suavizado/suavizado.cpp
    filters/filter_suavizado/suavizado.h


)

# Crear ejecutable
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(VisorNiftiQt
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        filters/filter_color_thresh/color_thresh.h filters/filter_color_thresh/color_thresh.cpp
        filters/filter_pixel_ops/pixel_ops.h filters/filter_pixel_ops/pixel_ops.cpp

    )
else()
    if(ANDROID)
        add_library(VisorNiftiQt SHARED ${PROJECT_SOURCES})
    else()
        add_executable(VisorNiftiQt ${PROJECT_SOURCES})
    endif()
endif()

# Incluir directorios
target_include_directories(VisorNiftiQt PRIVATE
    ${CMAKE_SOURCE_DIR}/filters
    ${CMAKE_SOURCE_DIR}/nifti_loader
    ${CMAKE_SOURCE_DIR}/slice_extractor
    ${CMAKE_SOURCE_DIR}/frontend
    ${CMAKE_SOURCE_DIR}/utils
)

# Enlazar librerías
target_link_libraries(VisorNiftiQt PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::PrintSupport
    ${ITK_LIBRARIES}
    ${OpenCV_LIBS}
)

# macOS/iOS propiedades
if(${QT_VERSION} VERSION_LESS 6.1.0)
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.VisorNiftiQt)
endif()

set_target_properties(VisorNiftiQt PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# Instalación
include(GNUInstallDirs)
install(TARGETS VisorNiftiQt
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Finalizar ejecutable en Qt6
if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(VisorNiftiQt)
endif()
