if (CONFIG_ARM)
    add_subdirectory(arm)
else()
    message(FATAL_ERROR "Unknown FreeRTOS architecture!")
endif()
