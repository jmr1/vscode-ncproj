# DetectDistro.cmake
# Usage:
#   include(path/to/DetectDistro.cmake)
#   detect_distro()
#   message(STATUS "Detected distro: ${DETECTED_DISTRO}")
#   message(STATUS "Detected distro ID: ${DETECTED_DISTRO_ID}")

function(detect_distro)
    set(DETECTED_DISTRO "")
    set(DETECTED_DISTRO_ID "")

    if(CMAKE_SYSTEM_NAME MATCHES "Linux")

        # Try lsb_release
        execute_process(COMMAND /usr/bin/lsb_release -ds
                        OUTPUT_VARIABLE DETECTED_DISTRO
                        RESULT_VARIABLE _lsb_result
                        OUTPUT_STRIP_TRAILING_WHITESPACE
                        ERROR_QUIET)
        execute_process(COMMAND /usr/bin/lsb_release -is
                        OUTPUT_VARIABLE DETECTED_DISTRO_ID
                        RESULT_VARIABLE _lsb_id_result
                        OUTPUT_STRIP_TRAILING_WHITESPACE
                        ERROR_QUIET)

        # Fallback to /etc/os-release
        if(NOT DETECTED_DISTRO OR _lsb_result)
            if(EXISTS "/etc/os-release")
                file(READ "/etc/os-release" OS_RELEASE_CONTENT)
                string(REGEX MATCH "PRETTY_NAME=\"([^\"]+)\"" _ ${OS_RELEASE_CONTENT})
                set(DETECTED_DISTRO "${CMAKE_MATCH_1}")
                string(REGEX MATCH "ID=([a-zA-Z0-9]+)" _ ${OS_RELEASE_CONTENT})
                set(DETECTED_DISTRO_ID "${CMAKE_MATCH_1}")
            endif()
        endif()

        # Normalize some known distros
        string(TOLOWER "${DETECTED_DISTRO_ID}" DETECTED_DISTRO_ID)
        if(DETECTED_DISTRO_ID STREQUAL "debian")
            set(DETECTED_DISTRO_ID "debian")
        elseif(DETECTED_DISTRO_ID STREQUAL "ubuntu")
            set(DETECTED_DISTRO_ID "ubuntu")
        elseif(DETECTED_DISTRO_ID STREQUAL "alpine")
            set(DETECTED_DISTRO_ID "alpine")
        elseif(DETECTED_DISTRO_ID STREQUAL "arch")
            set(DETECTED_DISTRO_ID "arch")
        endif()

    elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
        execute_process(COMMAND sw_vers -productName OUTPUT_VARIABLE _mac_name OUTPUT_STRIP_TRAILING_WHITESPACE)
        execute_process(COMMAND sw_vers -productVersion OUTPUT_VARIABLE _mac_version OUTPUT_STRIP_TRAILING_WHITESPACE)
        set(DETECTED_DISTRO "${_mac_name} ${_mac_version}")
        set(DETECTED_DISTRO_ID "macos")

    elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
        # Very basic detection for Windows (you can improve using PowerShell)
        set(DETECTED_DISTRO "Windows")
        set(DETECTED_DISTRO_ID "windows")
    endif()

    # Export to parent scope
    set(DETECTED_DISTRO "${DETECTED_DISTRO}" PARENT_SCOPE)
    set(DETECTED_DISTRO_ID "${DETECTED_DISTRO_ID}" PARENT_SCOPE)

    # Report
    message(STATUS "Detected system: ${CMAKE_SYSTEM_NAME}")
    if(DETECTED_DISTRO)
        message(STATUS "Detected distro: ${DETECTED_DISTRO}")
    else()
        message(WARNING "Could not detect Linux distro")
    endif()
endfunction()
