if (NOT WINDOWS_STORE AND NOT WINDOWS_PHONE)
  add_subdirectory(xover_server)
  add_subdirectory(xover_client)
else()
  # TODO: implement GUI version
endif()
