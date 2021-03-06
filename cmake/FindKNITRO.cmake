find_path(KNITRO_INCLUDE_DIR
knitro.h
HINTS $ENV{KNITRO}/include
)

if(KNITRO_INCLUDE_DIR)
   set(KNITRO_FOUND_INCLUDE TRUE)
   message(STATUS "Found KNITRO include dir: ${KNITRO_INCLUDE_DIR}")
else(KNITRO_INCLUDE_DIR)
   message(STATUS "Could not find KNITRO include dir")
endif(KNITRO_INCLUDE_DIR)

find_library(KNITRO_LIBRARY NAMES knitro HINTS $ENV{KNITRO}/lib)

if(KNITRO_LIBRARY)
  set(KNITRO_LIBRARIES ${KNITRO_LIBRARY} ${CMAKE_DL_LIBS} $ENV{KNITRO_EXTRA_LIBS})
   message(STATUS "Found KNITRO libraries: ${KNITRO_LIBRARIES}")
else(KNITRO_LIBRARY)
   message(STATUS "Could not find KNITRO library")
endif(KNITRO_LIBRARY)

if(KNITRO_FOUND_INCLUDE AND KNITRO_LIBRARIES)
  set(KNITRO_FOUND TRUE)
endif(KNITRO_FOUND_INCLUDE AND KNITRO_LIBRARIES)
