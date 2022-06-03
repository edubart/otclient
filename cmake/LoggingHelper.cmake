function(log_info MSG)
    message(STATUS "${MsgOk}${MSG}${MsgClr}")
endfunction()

function(log_war MSG)
    message(STATUS "${MsgWar}${MSG}${MsgClr}")
endfunction()

function(log_err MSG)
    message(SEND_ERROR "${MsgErr}${MSG}${MsgClr}")
endfunction()

function(log_fatal MSG)
    message(FATAL_ERROR "${MsgErr}${MSG}${MsgClr}")
endfunction()

function(log_option_enabled OPTION)
    message(STATUS "${MsgBold}Enabled:${MsgClr} ${MsgOk}${OPTION}${MsgClr}")
endfunction()

function(log_option_disabled OPTION)
    message(STATUS "${MsgBold}Disabled:${MsgClr} ${MsgWar}${OPTION}${MsgClr}")
endfunction()

function(log_program_missing PROGRAM)
    message(SEND_ERROR "${MsgErr}Unable to find ${PROGRAM}${MsgClr}")
endfunction()
