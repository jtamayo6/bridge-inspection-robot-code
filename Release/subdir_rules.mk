################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
Adafruit_GPS.obj: ../Adafruit_GPS.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP432 Compiler'
	"/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me -O2 --include_path="/Users/justin/Documents/TI_CCS_Workspace/Bridge_Inspection_Robot" --include_path="/Users/justin/ti/tirex-content/simplelink_msp432_sdk_1_30_00_40/source/third_party/CMSIS/Include" --include_path="/Users/justin/ti/tirex-content/simplelink_msp432_sdk_1_30_00_40/kernel/tirtos/packages/ti/sysbios/posix" --include_path="/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.1.LTS/include" --advice:power=all --advice:power_severity=suppress --define=__MSP432P401R__ -g --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="Adafruit_GPS.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

MSP_EXP432P401R.obj: ../MSP_EXP432P401R.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP432 Compiler'
	"/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me -O2 --include_path="/Users/justin/Documents/TI_CCS_Workspace/Bridge_Inspection_Robot" --include_path="/Users/justin/ti/tirex-content/simplelink_msp432_sdk_1_30_00_40/source/third_party/CMSIS/Include" --include_path="/Users/justin/ti/tirex-content/simplelink_msp432_sdk_1_30_00_40/kernel/tirtos/packages/ti/sysbios/posix" --include_path="/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.1.LTS/include" --advice:power=all --advice:power_severity=suppress --define=__MSP432P401R__ -g --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="MSP_EXP432P401R.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

PololuQik.obj: ../PololuQik.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP432 Compiler'
	"/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me -O2 --include_path="/Users/justin/Documents/TI_CCS_Workspace/Bridge_Inspection_Robot" --include_path="/Users/justin/ti/tirex-content/simplelink_msp432_sdk_1_30_00_40/source/third_party/CMSIS/Include" --include_path="/Users/justin/ti/tirex-content/simplelink_msp432_sdk_1_30_00_40/kernel/tirtos/packages/ti/sysbios/posix" --include_path="/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.1.LTS/include" --advice:power=all --advice:power_severity=suppress --define=__MSP432P401R__ -g --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="PololuQik.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

accelThread.obj: ../accelThread.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP432 Compiler'
	"/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me -O2 --include_path="/Users/justin/Documents/TI_CCS_Workspace/Bridge_Inspection_Robot" --include_path="/Users/justin/ti/tirex-content/simplelink_msp432_sdk_1_30_00_40/source/third_party/CMSIS/Include" --include_path="/Users/justin/ti/tirex-content/simplelink_msp432_sdk_1_30_00_40/kernel/tirtos/packages/ti/sysbios/posix" --include_path="/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.1.LTS/include" --advice:power=all --advice:power_severity=suppress --define=__MSP432P401R__ -g --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="accelThread.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

gpsThread.obj: ../gpsThread.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP432 Compiler'
	"/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me -O2 --include_path="/Users/justin/Documents/TI_CCS_Workspace/Bridge_Inspection_Robot" --include_path="/Users/justin/ti/tirex-content/simplelink_msp432_sdk_1_30_00_40/source/third_party/CMSIS/Include" --include_path="/Users/justin/ti/tirex-content/simplelink_msp432_sdk_1_30_00_40/kernel/tirtos/packages/ti/sysbios/posix" --include_path="/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.1.LTS/include" --advice:power=all --advice:power_severity=suppress --define=__MSP432P401R__ -g --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="gpsThread.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

main_tirtos.obj: ../main_tirtos.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP432 Compiler'
	"/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me -O2 --include_path="/Users/justin/Documents/TI_CCS_Workspace/Bridge_Inspection_Robot" --include_path="/Users/justin/ti/tirex-content/simplelink_msp432_sdk_1_30_00_40/source/third_party/CMSIS/Include" --include_path="/Users/justin/ti/tirex-content/simplelink_msp432_sdk_1_30_00_40/kernel/tirtos/packages/ti/sysbios/posix" --include_path="/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.1.LTS/include" --advice:power=all --advice:power_severity=suppress --define=__MSP432P401R__ -g --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="main_tirtos.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

motorThread.obj: ../motorThread.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP432 Compiler'
	"/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me -O2 --include_path="/Users/justin/Documents/TI_CCS_Workspace/Bridge_Inspection_Robot" --include_path="/Users/justin/ti/tirex-content/simplelink_msp432_sdk_1_30_00_40/source/third_party/CMSIS/Include" --include_path="/Users/justin/ti/tirex-content/simplelink_msp432_sdk_1_30_00_40/kernel/tirtos/packages/ti/sysbios/posix" --include_path="/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.1.LTS/include" --advice:power=all --advice:power_severity=suppress --define=__MSP432P401R__ -g --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="motorThread.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

sdCardThread.obj: ../sdCardThread.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP432 Compiler'
	"/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me -O2 --include_path="/Users/justin/Documents/TI_CCS_Workspace/Bridge_Inspection_Robot" --include_path="/Users/justin/ti/tirex-content/simplelink_msp432_sdk_1_30_00_40/source/third_party/CMSIS/Include" --include_path="/Users/justin/ti/tirex-content/simplelink_msp432_sdk_1_30_00_40/kernel/tirtos/packages/ti/sysbios/posix" --include_path="/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.1.LTS/include" --advice:power=all --advice:power_severity=suppress --define=__MSP432P401R__ -g --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="sdCardThread.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

xbeeThread.obj: ../xbeeThread.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP432 Compiler'
	"/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me -O2 --include_path="/Users/justin/Documents/TI_CCS_Workspace/Bridge_Inspection_Robot" --include_path="/Users/justin/ti/tirex-content/simplelink_msp432_sdk_1_30_00_40/source/third_party/CMSIS/Include" --include_path="/Users/justin/ti/tirex-content/simplelink_msp432_sdk_1_30_00_40/kernel/tirtos/packages/ti/sysbios/posix" --include_path="/Applications/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.1.LTS/include" --advice:power=all --advice:power_severity=suppress --define=__MSP432P401R__ -g --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="xbeeThread.d" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


