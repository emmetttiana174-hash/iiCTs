#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 3.16...3.21)
project(GammaTherapy)

#----------------------------------------------------------------------------
# Find Geant4 package
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

#----------------------------------------------------------------------------
# Find Qt5 for embedding support
find_package(Qt5 COMPONENTS Widgets OpenGL X11Extras REQUIRED)
set(CMAKE_AUTOMOC ON)

#----------------------------------------------------------------------------
# Include the Geant4 USE file. Even if it fails to set the macro,
# it's still needed for other settings like include paths.
include(${Geant4_USE_FILE})

#----------------------------------------------------------------------------
# Locate sources and headers for this project
include_directories(${PROJECT_SOURCE_DIR}/include ${Geant4_INCLUDE_DIR})

file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

list(APPEND sources 
  ${PROJECT_SOURCE_DIR}/src/DetectorMaterials.cc
  ${PROJECT_SOURCE_DIR}/src/EmbeddingHelper.cc
)

#----------------------------------------------------------------------------
# Add the executable
add_executable(GammaTherapy GammaTherapy.cc ${sources})

#----------------------------------------------------------------------------
# [FINAL FIX] Manually add the G4VIS_USE_QT preprocessor definition.
# We are forcing this because the automatic detection via Geant4_USE_FILE
# is failing silently in your environment. Since we have confirmed that
# Geant4 supports Qt and the project links Qt, this is a safe and
# definitive solution.
#----------------------------------------------------------------------------
target_compile_definitions(GammaTherapy PUBLIC G4VIS_USE_QT)

#----------------------------------------------------------------------------
# Link all necessary libraries
target_link_libraries(GammaTherapy 
  ${Geant4_LIBRARIES}
  Qt5::Widgets
  Qt5::OpenGL
  Qt5::X11Extras
)

#----------------------------------------------------------------------------
# Copy scripts and install executable (unchanged)
set(GammaTherapy_SCRIPTS
    be.in be_w.in cu_w.in GammaTherapy.in GammaTherapy.out vis.mac
  )
foreach(_script ${GammaTherapy_SCRIPTS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
    )
endforeach()
install(TARGETS GammaTherapy DESTINATION bin)