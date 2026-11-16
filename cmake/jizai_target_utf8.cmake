## # jizai_target_utf8
##
## On Windows, embeds an application manifest that sets the process code page to UTF-8, so that
## `argv` and narrow (char) file APIs use UTF-8 regardless of the system locale.
##
## https://learn.microsoft.com/windows/apps/design/globalizing/use-utf8-code-page
function(jizai_target_utf8 TARGET)
    if(MSVC)
        target_sources(${TARGET} PRIVATE "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/utf8.manifest")
    endif()
endfunction()
