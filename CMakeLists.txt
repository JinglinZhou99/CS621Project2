build_lib(
  LIBNAME CS621Project2
  SOURCE_FILES
    model/diffserv.cc
    model/traffic-class.cc
    model/filter.cc
    model/filter-element.cc
  HEADER_FILES
    model/diffserv.h
    model/traffic-class.h
    model/filter.h
    model/filter-element.h
  LIBRARIES_TO_LINK
    ${libcore}
    ${libnetwork}
)

# Uncomment if you have examples
# if(${ENABLE_EXAMPLES})
#   add_subdirectory(examples)
# endif()

# Uncomment if you have tests
# if(${ENABLE_TESTS})
#   add_subdirectory(test)
# endif()
