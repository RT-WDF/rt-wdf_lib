# Try to find SuperLU 4.x
# Once done, this will define
#
#  SuperLU_FOUND        - system has SuperLU
#  SuperLU_LIBRARY      - Link this to use SuperLU
#  SuperLU_INCLUDE_DIR  - directory of SuperLU headers

find_path(SuperLU_INCLUDE_DIR slu_cdefs.h
  /usr/include/superlu/
  /usr/include/SuperLU/
  /usr/include/
  /usr/local/include/superlu/
  /usr/local/include/SuperLU/
  /usr/local/include/
)

find_library(SuperLU_LIBRARY
  NAMES superlu
  PATHS /usr/lib64 /usr/lib /usr/local/lib64 /usr/local/lib
)

SET(SuperLU_FOUND NO)

IF (SuperLU_LIBRARY AND SuperLU_INCLUDE_DIR)
  # Now determine if the version is version 4.
  IF (EXISTS "${SuperLU_INCLUDE_DIR}/slu_cdefs.h")
    FILE(READ "${SuperLU_INCLUDE_DIR}/slu_cdefs.h" _slu_HEADER_CONTENTS)
    
    STRING(REGEX REPLACE ".*version ([0-9]).*" "\\1" SLU_VERSION_MAJOR "${_slu_HEADER_CONTENTS}")
    
    IF ("${SLU_VERSION_MAJOR}" EQUAL "4")
      SET(SuperLU_FOUND YES)
    ELSE ()
      IF (NOT SuperLU_FIND_QUIETLY)
        MESSAGE(STATUS "Found SuperLU version ${SLU_VERSION_MAJOR}, but version 4 is required")
      ENDIF ()
    ENDIF ()
  ELSE ()
    IF (NOT SuperLU_FIND_QUIETLY)
      MESSAGE(STATUS "Found SuperLU and headers, but could not verify version 4")
    ENDIF ()
  ENDIF ()
ENDIF()

IF (SuperLU_LIBRARY AND NOT SuperLU_INCLUDE_DIR)
  IF (NOT SuperLU_FIND_QUIETLY)
    MESSAGE(STATUS "Incomplete installation of SuperLU: no SuperLU headers found")
  ENDIF()
ENDIF()
  
IF(NOT SuperLU_FOUND) 
  IF (NOT SuperLU_FIND_QUIETLY)
    MESSAGE(STATUS "Could not find SuperLU")
  ENDIF()
ELSE()
  MESSAGE(STATUS "Found SuperLU: ${SuperLU_LIBRARY}")
ENDIF()

