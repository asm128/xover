if (UNIX)
  add_definitions(-Wno-switch)
endif()

add_executable(xoverclient
  xover_client.cpp
  )

target_link_libraries(xoverclient cpprest)
