################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
hardware/TFT.obj: ../hardware/TFT.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"E:/ti/ccsv6/tools/compiler/ti-cgt-c2000_15.12.1.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 --include_path="E:/ti/ccsv6/tools/compiler/ti-cgt-c2000_15.12.1.LTS/include" --include_path="D:/DSPw/DATA_AQ0/hardware" --include_path="D:/DSPw/ADCRPMCANRUNACCURATE_DATA_AQ0/include" --advice:performance=all -g --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="hardware/TFT.d" --obj_directory="hardware" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

hardware/touch.obj: ../hardware/touch.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"E:/ti/ccsv6/tools/compiler/ti-cgt-c2000_15.12.1.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 --include_path="E:/ti/ccsv6/tools/compiler/ti-cgt-c2000_15.12.1.LTS/include" --include_path="D:/DSPw/DATA_AQ0/hardware" --include_path="D:/DSPw/ADCRPMCANRUNACCURATE_DATA_AQ0/include" --advice:performance=all -g --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="hardware/touch.d" --obj_directory="hardware" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

hardware/tupian.obj: ../hardware/tupian.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"E:/ti/ccsv6/tools/compiler/ti-cgt-c2000_15.12.1.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 --include_path="E:/ti/ccsv6/tools/compiler/ti-cgt-c2000_15.12.1.LTS/include" --include_path="D:/DSPw/DATA_AQ0/hardware" --include_path="D:/DSPw/ADCRPMCANRUNACCURATE_DATA_AQ0/include" --advice:performance=all -g --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="hardware/tupian.d" --obj_directory="hardware" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


