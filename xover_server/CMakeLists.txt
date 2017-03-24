if (UNIX)
  add_definitions(-Wno-sign-compare -Wno-enum-compare)
endif()

add_executable(xoverserver
  xover_server.cpp
  dealer.cpp
  table.cpp
  )

target_link_libraries(xoverserver cpprest)
