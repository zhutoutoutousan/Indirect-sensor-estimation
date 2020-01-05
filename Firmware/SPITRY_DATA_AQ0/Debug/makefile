################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := E:/ti/ccsv6/tools/compiler/ti-cgt-c2000_15.12.1.LTS

GEN_OPTS__FLAG := 
GEN_CMDS__FLAG := 

ORDERED_OBJS += \
"./hardware/TFT.obj" \
"./hardware/touch.obj" \
"./hardware/tupian.obj" \
"./source/ADC_INIT.obj" \
"./source/DSP2833x_ADC_cal.obj" \
"./source/DSP2833x_Adc.obj" \
"./source/DSP2833x_CSMPasswords.obj" \
"./source/DSP2833x_CodeStartBranch.obj" \
"./source/DSP2833x_CpuTimers.obj" \
"./source/DSP2833x_DefaultIsr.obj" \
"./source/DSP2833x_ECap.obj" \
"./source/DSP2833x_EPwm.obj" \
"./source/DSP2833x_EQep.obj" \
"./source/DSP2833x_GlobalVariableDefs.obj" \
"./source/DSP2833x_Gpio.obj" \
"./source/DSP2833x_MemCopy.obj" \
"./source/DSP2833x_PieCtrl.obj" \
"./source/DSP2833x_PieVect.obj" \
"./source/DSP2833x_SysCtrl.obj" \
"./source/DSP2833x_Xintf.obj" \
"./source/DSP2833x_usDelay.obj" \
"./source/Example_2833xECap_Capture_Pwm.obj" \
"./source/Sampletable_right_move.obj" \
"../cmd/DSP2833x_Headers_nonBIOS.cmd" \
"../cmd/F28335.cmd" \
"../lib/SFO_TI_Build.lib" \
"../lib/SFO_TI_Build_V5.lib" \
"../lib/SFO_TI_Build_V5B.lib" \
"../lib/SFO_TI_Build_V5B_fpu.lib" \
"../lib/SFO_TI_Build_V5_fpu.lib" \
"../lib/SFO_TI_Build_fpu.lib" \
"../lib/rts2800_fpu32.lib" \
"../lib/rts2800_ml.lib" \
$(GEN_CMDS__FLAG) \
-llibc.a \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include cmd/subdir_vars.mk
-include hardware/subdir_vars.mk
-include lib/subdir_vars.mk
-include source/subdir_vars.mk
-include cmd/subdir_rules.mk
-include hardware/subdir_rules.mk
-include lib/subdir_rules.mk
-include source/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
SPITRY_DATA_AQ0.out \

EXE_OUTPUTS__QUOTED += \
"SPITRY_DATA_AQ0.out" \

BIN_OUTPUTS += \
SPITRY_DATA_AQ0.hex \

BIN_OUTPUTS__QUOTED += \
"SPITRY_DATA_AQ0.hex" \


# All Target
all: SPITRY_DATA_AQ0.out

# Tool invocations
SPITRY_DATA_AQ0.out: $(OBJS) $(CMD_SRCS) $(LIB_SRCS) $(GEN_CMDS)
	@echo 'Building target: $@'
	@echo 'Invoking: C2000 Linker'
	"E:/ti/ccsv6/tools/compiler/ti-cgt-c2000_15.12.1.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 -g --diag_warning=225 --display_error_number --diag_wrap=off -z -m"SPITRY_DATA_AQ0.map" --stack_size=0x300 --warn_sections -i"E:/ti/ccsv6/tools/compiler/ti-cgt-c2000_15.12.1.LTS/lib" -i"E:/ti/ccsv6/tools/compiler/ti-cgt-c2000_15.12.1.LTS/include" --reread_libs --display_error_number --diag_wrap=off --xml_link_info="SPITRY_DATA_AQ0_linkInfo.xml" --rom_model -o "SPITRY_DATA_AQ0.out" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

SPITRY_DATA_AQ0.hex: $(EXE_OUTPUTS)
	@echo 'Invoking: C2000 Hex Utility'
	"E:/ti/ccsv6/tools/compiler/ti-cgt-c2000_15.12.1.LTS/bin/hex2000"  -o "SPITRY_DATA_AQ0.hex" $(EXE_OUTPUTS__QUOTED)
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXE_OUTPUTS__QUOTED)$(BIN_OUTPUTS__QUOTED)
	-$(RM) "hardware\TFT.d" "hardware\touch.d" "hardware\tupian.d" "source\DSP2833x_Adc.d" "source\DSP2833x_CpuTimers.d" "source\DSP2833x_DefaultIsr.d" "source\DSP2833x_ECap.d" "source\DSP2833x_EPwm.d" "source\DSP2833x_EQep.d" "source\DSP2833x_GlobalVariableDefs.d" "source\DSP2833x_Gpio.d" "source\DSP2833x_MemCopy.d" "source\DSP2833x_PieCtrl.d" "source\DSP2833x_PieVect.d" "source\DSP2833x_SysCtrl.d" "source\DSP2833x_Xintf.d" "source\Example_2833xECap_Capture_Pwm.d" "source\Sampletable_right_move.d" 
	-$(RM) "source\ADC_INIT.d" 
	-$(RM) "hardware\TFT.obj" "hardware\touch.obj" "hardware\tupian.obj" "source\ADC_INIT.obj" "source\DSP2833x_ADC_cal.obj" "source\DSP2833x_Adc.obj" "source\DSP2833x_CSMPasswords.obj" "source\DSP2833x_CodeStartBranch.obj" "source\DSP2833x_CpuTimers.obj" "source\DSP2833x_DefaultIsr.obj" "source\DSP2833x_ECap.obj" "source\DSP2833x_EPwm.obj" "source\DSP2833x_EQep.obj" "source\DSP2833x_GlobalVariableDefs.obj" "source\DSP2833x_Gpio.obj" "source\DSP2833x_MemCopy.obj" "source\DSP2833x_PieCtrl.obj" "source\DSP2833x_PieVect.obj" "source\DSP2833x_SysCtrl.obj" "source\DSP2833x_Xintf.obj" "source\DSP2833x_usDelay.obj" "source\Example_2833xECap_Capture_Pwm.obj" "source\Sampletable_right_move.obj" 
	-$(RM) "source\DSP2833x_ADC_cal.d" "source\DSP2833x_CSMPasswords.d" "source\DSP2833x_CodeStartBranch.d" "source\DSP2833x_usDelay.d" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

