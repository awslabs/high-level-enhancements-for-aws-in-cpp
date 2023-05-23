# See https://stackoverflow.com/questions/3065220/ctest-with-multiple-commands
macro(EXEC_CHECK CMD)
    execute_process(COMMAND ${CMD} RESULT_VARIABLE CMD_RESULT)
    if(CMD_RESULT)
        message(FATAL_ERROR "Error running ${CMD}")
    endif()
endmacro()

execute_process(COMMAND aws lambda delete-function --function-name ${LAMBDA_NAME}
 COMMAND aws lambda create-function --function-name ${LAMBDA_NAME} --role ${LAMBDA_EXECUTION_ROLE} --runtime provided --timeout 15 --memory-size 128 --handler ${LAMBDA_NAME} --zip-file fileb://${LAMBDA_NAME}.zip
 COMMAND_ERROR_IS_FATAL LAST)