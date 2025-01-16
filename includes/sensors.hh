/*                                                                                                                                                                                           #
# Under no circumstance this file tries to behave as it was made by Xilinx, or to gain any profit from it.                
#  
# Copyright 2022 Barcelona Supercomputing Center-Centro Nacional de Supercomputación
# Licensed under the Solderpad Hardware License v 2.1 (the "License");
# you may not use this file except in compliance with the License, or, at your option, the Apache License version 2.0.
# You may obtain a copy of the License at
#
#     http://www.solderpad.org/licenses/SHL-2.1
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License. 
*/
#ifndef SENSORS_HH
#define SENSORS_HH

/*
THIS FILE CONTAINS THE DIRECTIONS OF THE SENSORS ON THE CMS/CMC/XMC (THEY ARE ALL EQUIVALENT)
YOU CAN READ THEM THROUGH BAR ACCESSES GIVEN AN OFFSET
*/
//RESET_REG (CMC/XMC/CMS) HAS A DIFFERENT OFFSET
#define RESET_REG       0x0
#define DISABLE_RESET   0x0
#define ENABLE_RESET    0x1

//Maximum CMS size
#define CMS_TOTAL_BAR_SIZE 0x40000

//THESE REGISTERS HAVE A DIFFERENT OFFSET
#define FIRMWARE_SIZE 0x20000
#define REG_MAP_1_SIZE  0x2000

#define REG_MAP_ID_REG_VALUE        0x74736574
#define REG_MAP_ID_REG              0x000
#define FW_VERSION_REG              0x004
#define STATUS_REG                  0x008
#define ERROR_REG                   0x00C
#define PROFILE_NAME_REG            0x014
#define CONTROL_REG                 0x018

//FPGA BOARD TYPE (read from PROFILE_NAME_REG)
#define U200_U250                   0x55325858
#define U280                        0x55323830
#define U50                         0x55353041
#define U55                         0x5535354E
#define U45N                        0x55323641
#define X3                          0x58334100
#define UL3422                      0x55333432
#define UL3524                      0x55333234

#define MASK_U200_U250              0b10000000
#define MASK_U280                   0b01000000
#define MASK_U50                    0b00100000
#define MASK_U55                    0b00010000
#define MASK_U45N                   0b00001000
#define MASK_X3                     0b00000100
#define MASK_UL3422                 0b00000010
#define MASK_UL3524                 0b00000001

//Offsets for MAX, AVG and INS regs
#define MAX 0x0
#define AVG 0x4
#define INS 0x8

//SENSORS NAME                      Sensor string        offset,  Mask  DIV_Factor   Known Location
#define	REG_12V_PEX			        "12V_PEX       (V)", {0x020, 0b11111111, 1000}
#define REG_3V3_PEX			        "3V3_PEX       (V)", {0x02C, 0b11111110, 1000}
#define	REG_3V3_AUX			        "3V3_AUX       (V)", {0x038, 0b11000101, 1000}
#define	REG_12V_AUX			        "12V_AUX       (V)", {0x044, 0b11011011, 1000}
#define	REG_DDR4_VPP_BTM	        "DDR4_VPP_BTM  (V)", {0x050, 0b11000000, 1000} //DDR voltage ¿BANK 1?
#define	REG_SYS_5V5			        "SYS_5V5       (V)", {0x05C, 0b11110000, 1000}
#define	REG_VCC1V2_TOP		        "VCC1V2_TOP    (V)", {0x068, 0b11000011, 1000}
#define	REG_VCC1V8			        "VCC1V8        (V)", {0x074, 0b11111111, 1000}
#define	REG_VCC0V85			        "VCC0V85       (V)", {0x080, 0b11000000, 1000}
#define	REG_DDR4_VPP_TOP	        "DDR4_VPP_TOP  (V)", {0x08C, 0b11000011, 1000} //DDR voltage ¿BANK 2?
#define	REG_MGT0V9AVCC		        "MGT0V9AVCC    (V)", {0x098, 0b11110011, 1000}
#define	REG_12V_SW			        "12V_SW        (V)", {0x0A4, 0b11000000, 1000}
#define	REG_MGTAVTT			        "MGTAVTT       (V)", {0x0B0, 0b11110011, 1000}
#define	REG_VCC1V2_BTM		        "VCC1V2_BTM    (V)", {0x0BC, 0b11000000, 1000}
#define	REG_12V_PEX_I_IN	        "12V_PEX_I_IN  (A)", {0x0C8, 0b11111111, 1000}
#define	REG_12V_AUX_I_IN	        "12V_AUX_I_IN  (A)", {0x0D4, 0b11011011, 1000}
#define	REG_VCCINT_V		        "VCCINT_V      (V)", {0x0E0, 0b11111111, 1000} // Voltage supply for the internal core logic: supplies power to the internal logic functions, such as CLBs, block RAM, and DSP blocks.
#define	REG_VCCINT_I		        "VCCINT_I      (A)", {0x0EC, 0b11111111, 1000} // Current supply for the internal core logic: supplies power to the internal logic functions, such as CLBs, block RAM, and DSP blocks.
#define	REG_FPGA_TEMP			    "FPGA_TEMP     (C)", {0x0F8, 0b11111111, 0x01}
#define	REG_FAN_TEMP		        "FAN_TEMP      (C)", {0x104, 0b11000000, 0x01} //FAN
#define	REG_DIMM_TEMP0		        "DIMM_TEMP0    (C)", {0x110, 0b11000000, 0x01}
#define	REG_DIMM_TEMP1		        "DIMM_TEMP1    (C)", {0x11C, 0b11000000, 0x01}
#define	REG_DIMM_TEMP2		        "DIMM_TEMP2    (C)", {0x128, 0b10000000, 0x01}
#define	REG_DIMM_TEMP3		        "DIMM_TEMP3    (C)", {0x134, 0b10000000, 0x01}
#define	REG_FAN_SPEED		        "FAN_SPEED   (RPM)", {0x164, 0b11000000, 0x01} //FAN
#define	REG_SE98_TEMP0		        "SE98_TEMP0    (C)", {0x140, 0b11111111, 0x01} //PCB TOP FRONT
#define	REG_SE98_TEMP1		        "SE98_TEMP1    (C)", {0x14C, 0b11111111, 0x01} //PCB TOP REAR
#define	REG_SE98_TEMP2		        "SE98_TEMP2    (C)", {0x158, 0b10000000, 0x01} //PCB BTM FRONT
#define	REG_CAGE_TEMP0		        "CAGE_TEMP0    (C)", {0x170, 0b11111100, 0x01} //QSFP 0
#define	REG_CAGE_TEMP1		        "CAGE_TEMP1    (C)", {0x17C, 0b11011100, 0x01} //QSFP 1
#define	REG_CAGE_TEMP2		        "CAGE_TEMP2    (C)", {0x188, 0b00000000, 0x01} //QSFP 2
#define	REG_CAGE_TEMP3		        "CAGE_TEMP3    (C)", {0x194, 0b00000000, 0x01} //QSFP 3
#define	REG_HBM_TEMP		        "HBM_TEMP      (C)", {0x260, 0b01110000, 0x01}
#define	REG_VCC3V3			        "VCC3V3        (V)", {0x26C, 0b00111111, 1000}
#define	REG_3V3_PEX_I		        "3V3_PEX_I     (A)", {0x278, 0b00111110, 1000}
#define	REG_VCC0V85_I		        "VCC0V85_I     (A)", {0x284, 0b00000000, 1000}
#define	REG_HBM_1V2			        "HBM_1V2       (V)", {0x290, 0b00110000, 1000} //HBM voltage
#define	REG_VPP2V5			        "VPP2V5        (V)", {0x29C, 0b00110000, 1000}
#define	REG_VCCINT_BRAM		        "VCCINT_IO     (V)", {0x2A8, 0b11111111, 1000}
#define	REG_HBM_TEMP2		        "HBM_TEMP2     (C)", {0x2B4, 0b01110000, 0x01} //HBM
#define	REG_12V_AUX1                "12V_AUX1      (V)", {0x2C0, 0b00000000, 1000}
#define	REG_VCCINT_TEMP             "VCCINT_TEMP   (C)", {0x2CC, 0b10111111, 0x01}
#define	REG_3V3_AUX_I               "3V3_AUX_I     (A)", {0x2F0, 0b00000101, 1000}

#define	HEARTBEAT                   0x2FC // System registers, present on all FPGAs
#define	HOST_MSG_OFFSET		        0x300 // System registers, present on all FPGAs
#define	HOST_MSG_ERROR		        0x304 // System registers, present on all FPGAs
#define	HOST_MSG_HEADER		        0x308 // System registers, present on all FPGAs
#define	STATUS2			            0x30C // System registers, present on all FPGAs
#define	HEARTBEAT_ERR_CODE	        0x310 // System registers, present on all FPGAs

#define	REG_VCC1V2_I                "VCC1V2_I      (A)", {0x314, 0b00000000, 1000} // HBM current
#define	REG_V12_IN_I                "V12_IN_I      (A)", {0x320, 0b00000000, 1000}
#define	REG_V12_IN_AUX0_I           "V12_IN_AUX0_I (A)", {0x32C, 0b00000000, 1000}
#define	REG_V12_IN_AUX1_I           "V12_IN_AUX1_I (A)", {0x338, 0b00000000, 1000}
#define	REG_VCCAUX                  "VCCAUX        (V)", {0x344, 0b00000011, 1000} //Voltage supply for auxiliary logic: supplies power to the various auxiliary circuits, such as clock managers and dedicated configuration pins.
#define	REG_VCCAUX_PMC              "VCCAUX_PMC    (V)", {0x350, 0b00000000, 1000} //Voltage supply for auxiliary logic: supplies power to the various auxiliary circuits, such as clock managers and dedicated configuration pins.
#define	REG_VCCRAM                  "VCCRAM        (V)", {0x35C, 0b00000000, 1000}
#define	REG_VCCINT_VCU_0V9          "VCCINT_VCU_0V9(V)", {0x380, 0b00000000, 1000}
#define REG_1V2_VCCIO_MAX           "1V2_VCCIO     (V)", {0x38C, 0b00000000, 1000}
#define REG_GTAVCC_MAX              "GTAVCC        (V)", {0x398, 0b00000000, 1000}
#define REG_VCCSOC_MAX              "VCCSOC        (V)", {0x3B0, 0b00000000, 1000}
#define REG_VCC_5V0_MAX             "VCC_5V0       (V)", {0x3BC, 0b00000000, 1000}
#define REG_2V5_VPP23_MAX           "2V5_VPP23     (V)", {0x3C8, 0b00000000, 1000}
#define REG_GTVCC_AUX_MAX           "GTVCC_AUX     (V)", {0x3D4, 0b00000000, 1000}
#define REG_HBM_1V2_I_MAX           "HBM_1V2_I     (A)", {0x410, 0b00010000, 1000} //HBM current
#define REG_VCC1V5_MAX              "VCC1V5        (V)", {0x41C, 0b00000001, 1000}
#define REG_MGTAVCC_MAX             "MGTAVCC       (V)", {0x428, 0b00000011, 1000} //Voltage supply for the GTX transceiver: powers the internal analog circuits of the GTX transceiver. This includes analog circuits for the PLLs, transmitters, and receivers.
#define REG_MGTAVTT_I_MAX           "MGTAVTT_I     (A)", {0x434, 0b00000010, 1000} //Current supply for GTX transceiver termination circuits
#define REG_MGTAVCC_I_MAX           "MGTAVCC_I     (A)", {0x440, 0b00000011, 1000} //Current supply for the GTX transceiver: powers the internal analog circuits of the GTX transceiver. This includes analog circuits for the PLLs, transmitters, and receivers.

#define	CMC_HOST_MSG_REG            0x1000 // System registers, present on all FPGAs
#define	CMC_SOC_OPCODE_REG		    0x1004 // System registers, present on all FPGAs
#define	CMC_SOC_LENGTH_REG		    0x1008 // System registers, present on all FPGAs
#define	CMC_SOC_PAYLOAD_REG		    0x100C // System registers, present on all FPGAs

#include <map>
#include <vector>
typedef std::map<std::string, std::vector<uint32_t>> reg_file_t;

static const reg_file_t LITE_REG_FILE = {
    {REG_FPGA_TEMP},
    {REG_FAN_TEMP},
    {REG_FAN_SPEED}
};

static const reg_file_t REG_FILE = {
    {REG_12V_PEX},
    {REG_3V3_PEX},
    {REG_3V3_AUX},
    {REG_12V_AUX},
    {REG_DDR4_VPP_BTM},
    {REG_SYS_5V5},
    {REG_VCC1V2_TOP},
    {REG_VCC1V8},
    {REG_VCC0V85},
    {REG_DDR4_VPP_TOP},
    {REG_MGT0V9AVCC},
    {REG_12V_SW},
    {REG_MGTAVTT},
    {REG_VCC1V2_BTM},
    {REG_12V_PEX_I_IN},
    {REG_12V_AUX_I_IN},
    {REG_VCCINT_V},
    {REG_VCCINT_I},
    {REG_FPGA_TEMP},
    {REG_FAN_TEMP},
    {REG_DIMM_TEMP0},
    {REG_DIMM_TEMP1},
    {REG_DIMM_TEMP2},
    {REG_DIMM_TEMP3},
    {REG_FAN_SPEED},
    {REG_SE98_TEMP0},
    {REG_SE98_TEMP1},
    {REG_SE98_TEMP2},
    {REG_CAGE_TEMP0},
    {REG_CAGE_TEMP1},
    {REG_CAGE_TEMP2},
    {REG_CAGE_TEMP3},
    {REG_HBM_TEMP},
    {REG_VCC3V3},
    {REG_3V3_PEX_I},
    {REG_VCC0V85_I},
    {REG_HBM_1V2},
    {REG_VPP2V5},
    {REG_VCCINT_BRAM},
    {REG_HBM_TEMP2},
    {REG_12V_AUX1},
    {REG_VCCINT_TEMP},
    {REG_3V3_AUX_I},
    {REG_VCC1V2_I},
    {REG_V12_IN_I},
    {REG_V12_IN_AUX0_I},
    {REG_V12_IN_AUX1_I},
    {REG_VCCAUX},
    {REG_VCCAUX_PMC},
    {REG_VCCRAM},
    {REG_VCCINT_VCU_0V9},
    {REG_1V2_VCCIO_MAX},
    {REG_GTAVCC_MAX},
    {REG_VCCSOC_MAX},
    {REG_VCC_5V0_MAX},
    {REG_2V5_VPP23_MAX},
    {REG_GTVCC_AUX_MAX},
    {REG_HBM_1V2_I_MAX},
    {REG_VCC1V5_MAX},
    {REG_MGTAVCC_MAX},
    {REG_MGTAVTT_I_MAX},
    {REG_MGTAVCC_I_MAX}
};
static const std::vector<std::string> HOST_ERROR_RETURN_MSG{
"CMS_HOST_MSG_NO_ERR"               //0x0
,"CMS_HOST_MSG_BAD_OPCODE_ERR"      //0x1
,"CMS_HOST_MSG_BRD_INFO_MISSING_ERR"//0x2
,"CMS_HOST_MSG_LENGTH_ERR"          //0x3
,"CMS_HOST_MSG_SAT_FW_WRITE_FAIL"   //0x4
,"CMS_HOST_MSG_SAT_FW_UPDATE_FAIL"  //0x5
,"CMS_HOST_MSG_SAT_FW_LOAD_FAIL"    //0x6
,"CMS_HOST_MSG_SAT_FW_ERASE_FAIL"   //0x7
,"CMS_HOST_MSG_CSDR_FAILED"         //0x9
,"CMS_HOST_MSG_QSFP_FAIL"};         //0xA
#endif // SENSORS_HH
