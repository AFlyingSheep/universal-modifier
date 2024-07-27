#ifndef STATUS_OUTPUT_HPP
#define STATUS_OUTPUT_HPP

#include <cstdio>

#include "format.hpp"
#include "modifier_status.hpp"

#define PRINT_MODIFIER_STATUS(status)            \
  do {                                           \
    if (status != MODIFIER_STATUS::SUCCESS) {    \
      const char* error_message = "";            \
      switch (status) {                          \
        case MODIFIER_STATUS::WRITE_ERROR:       \
          error_message = "Write buffer error!"; \
          break;                                 \
        case MODIFIER_STATUS::READ_ERROR:        \
          error_message = "Read buffer error!";  \
          break;                                 \
        case MODIFIER_STATUS::CANCEL:            \
          error_message = "Operation cancelled"; \
          break;                                 \
        default:                                 \
          error_message = "Unknown error";       \
          break;                                 \
      }                                          \
      UM_ERROR("%s\n", error_message);           \
    }                                            \
  } while (0)

#define PRINT_PROCESS_STATUS(status)                    \
  do {                                                  \
    if (status != PROCESS_STATUS::SUCCESS) {            \
      const char* error_message = "";                   \
      switch (status) {                                 \
        case PROCESS_STATUS::PROCESS_NOT_FOUND:         \
          error_message = "Process not found!";         \
          break;                                        \
        case PROCESS_STATUS::PROCESS_ENUM_ERROR:        \
          error_message = "Process enumeration error!"; \
          break;                                        \
        case PROCESS_STATUS::PROCESS_NOT_OPEN:          \
          error_message = "Process not open!";          \
          break;                                        \
        case PROCESS_STATUS::DLL_NOT_FOUNT:             \
          error_message = "DLL not found!";             \
          break;                                        \
        case PROCESS_STATUS::BASE_ADDRESS_NOT_FOUND:    \
          error_message = "Base address not found!";    \
          break;                                        \
        case PROCESS_STATUS::UNKNOWN_ERROR:             \
          error_message = "Unknown error";              \
          break;                                        \
        default:                                        \
          error_message = "Unknown error";              \
          break;                                        \
      }                                                 \
      UM_ERROR("%s\n", error_message);                  \
    }                                                   \
  } while (0)

#endif
