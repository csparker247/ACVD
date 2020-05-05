# --------------------------------------------------------------------------
# Library compilation

# Public headers
file(GLOB _vtkSurface_hdrs include/*.h)

add_library(vtkSurface
  src/vtkDelaunay.cxx
  src/vtkQuadricTools.cxx
  src/vtkRandomTriangulation.cxx
  src/vtkSurface.cxx
  src/vtkSurfaceBase.cxx
  src/vtkVolumeProperties.cxx
  src/vtkNeighbourhoodComputation.cxx
  src/vtkCurvatureMeasure.cxx
  src/vtkMyMinimalStandardRandomSequence.cxx
)
add_library("ACVD::vtkSurface" ALIAS vtkSurface)
target_include_directories(vtkSurface
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<INSTALL_INTERFACE:${INSTALL_INCLUDE_DIR}/ACVD/Common>
)
target_link_libraries(vtkSurface
    PUBLIC
        vtkCommonSystem
    PRIVATE
        vtkCommonCore
        vtkCommonDataModel
        vtkFiltersCore
)

set_target_properties(vtkSurface
  PROPERTIES
    DEBUG_POSTFIX "-d"
    VERSION "${ACVD_VERSION}"
    SOVERSION "${ACVD_LIB_SOVERSION}"
    PUBLIC_HEADER "${_vtkSurface_hdrs}"
)

install(
  TARGETS vtkSurface
  EXPORT ACVDTargets
  RUNTIME DESTINATION ${INSTALL_BIN_DIR}
  LIBRARY DESTINATION ${INSTALL_LIB_DIR}
  ARCHIVE DESTINATION ${INSTALL_LIB_DIR}
  PUBLIC_HEADER DESTINATION ${INSTALL_INCLUDE_DIR}/ACVD/Common
)
