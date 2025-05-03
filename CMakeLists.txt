build_lib(
  LIBNAME CS621Project2
  SOURCE_FILES
    model/diffserv.cc
    model/traffic-class.cc
    model/filter.cc
    model/filter-element.cc
    model/source-ip-address.cc
    model/source-mask.cc
    model/source-port-number.cc
    model/destination-ip-address.cc
    model/destination-mask.cc
    model/destination-port-number.cc
    model/protocol-number.cc
  HEADER_FILES
    model/diffserv.h
    model/traffic-class.h
    model/filter.h
    model/filter-element.h
    model/source-ip-address.h
    model/source-mask.h
    model/source-port-number.h
    model/destination-ip-address.h
    model/destination-mask.h
    model/destination-port-number.h
    model/protocol-number.h
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
