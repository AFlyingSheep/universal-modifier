#ifndef FORMAT_HPP
#define FORMAT_HPP

#include <windows.h>

#include <chrono>
#include <cstdio>
#include <iomanip>
#include <iostream>

#define UM_INFO(format, ...)                                                   \
  do {                                                                         \
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);                         \
                                                                               \
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;                                    \
    WORD saved_attributes;                                                     \
                                                                               \
    /* Save current color attributes */                                        \
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);                        \
    saved_attributes = consoleInfo.wAttributes;                                \
                                                                               \
    /* Set new color attributes */                                             \
    SetConsoleTextAttribute(hConsole,                                          \
                            FOREGROUND_GREEN | FOREGROUND_INTENSITY);          \
    auto now = std::chrono::system_clock::now();                               \
    std::time_t time_now = std::chrono::system_clock::to_time_t(now);          \
    std::tm local_time;                                                        \
    localtime_s(&local_time, &time_now);                                       \
    std::cout << "[INFO]";                                                     \
    std::cout << "[" << std::setfill('0') << std::setw(2)                      \
              << local_time.tm_hour << ":" << std::setfill('0')                \
              << std::setw(2) << local_time.tm_min << ":" << std::setfill('0') \
              << std::setw(2) << local_time.tm_sec << "] ";                    \
                                                                               \
    /* Restore original color attributes */                                    \
    SetConsoleTextAttribute(hConsole, saved_attributes);                       \
    printf(format, ##__VA_ARGS__);                                             \
  } while (0)

#define UM_WARN(format, ...)                                                   \
  do {                                                                         \
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);                         \
                                                                               \
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;                                    \
    WORD saved_attributes;                                                     \
                                                                               \
    /* Save current color attributes */                                        \
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);                        \
    saved_attributes = consoleInfo.wAttributes;                                \
                                                                               \
    /* Set new color attributes */                                             \
    SetConsoleTextAttribute(                                                   \
        hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);   \
    auto now = std::chrono::system_clock::now();                               \
    std::time_t time_now = std::chrono::system_clock::to_time_t(now);          \
    std::tm local_time;                                                        \
    localtime_s(&local_time, &time_now);                                       \
    std::cout << "[WARN]";                                                     \
    std::cout << "[" << std::setfill('0') << std::setw(2)                      \
              << local_time.tm_hour << ":" << std::setfill('0')                \
              << std::setw(2) << local_time.tm_min << ":" << std::setfill('0') \
              << std::setw(2) << local_time.tm_sec << "] ";                    \
                                                                               \
    /* Restore original color attributes */                                    \
    SetConsoleTextAttribute(hConsole, saved_attributes);                       \
    printf(format, ##__VA_ARGS__);                                             \
  } while (0)

#define UM_ERROR(format, ...)                                                  \
  do {                                                                         \
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);                         \
                                                                               \
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;                                    \
    WORD saved_attributes;                                                     \
                                                                               \
    /* Save current color attributes */                                        \
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);                        \
    saved_attributes = consoleInfo.wAttributes;                                \
                                                                               \
    /* Set new color attributes */                                             \
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);  \
    auto now = std::chrono::system_clock::now();                               \
    std::time_t time_now = std::chrono::system_clock::to_time_t(now);          \
    std::tm local_time;                                                        \
    localtime_s(&local_time, &time_now);                                       \
    std::cout << "[ERROR]";                                                    \
    std::cout << "[" << std::setfill('0') << std::setw(2)                      \
              << local_time.tm_hour << ":" << std::setfill('0')                \
              << std::setw(2) << local_time.tm_min << ":" << std::setfill('0') \
              << std::setw(2) << local_time.tm_sec << "] ";                    \
                                                                               \
    /* Restore original color attributes */                                    \
    SetConsoleTextAttribute(hConsole, saved_attributes);                       \
    printf(format, ##__VA_ARGS__);                                             \
  } while (0)

#endif