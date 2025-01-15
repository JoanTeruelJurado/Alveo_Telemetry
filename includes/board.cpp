/*                                                                                                                                                                                           #
# This file contains confidential and proprietary information of Xilinx, Inc.                                                                                              
#
# This file was generated for a Bachelor's thesis at BSC-CNS, under no circumstance            
# it tries to behave as it was made by Xilinx, or to gain any profit from it.                  

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
#include "board.hh"

#include <iostream>
#include <ostream>
#include <fstream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <pci/pci.h>

#include <fcntl.h>

#include <stdexcept>

#define READ_32b(REG_offset_) \
    READ_32bREG(REG_offset_+this->Register_map_offset+INS)

#define WRITE_32b(REG_offset_, value_, mask_) \
    WRITE_32bREG(REG_offset_+this->Register_map_offset, value_, mask_)

alveo_board::~alveo_board() {
    cleanup();
}

void alveo_board::cleanup() {
    if (pciBar) {
        munmap(pciBar, CMS_TOTAL_BAR_SIZE);
        pciBar=nullptr;
    }
    if (pciBarFd >= 0) {
        close(pciBarFd);
        pciBarFd = -1;
    } 
}

char* alveo_board::print_info() {
    sprintf(tmp_buff, "%s| %s   %s   %x:%x\n",terminal_comm, name_type, BDF, vendor, device);
    snprintf(tmp_buff + strlen(tmp_buff), BUFFER_SIZE-strlen(tmp_buff),"%s| Name: %s\n", terminal_comm, Platform_name);
    return tmp_buff;
}

char* alveo_board::print_sensor_data(int terminal_row, int terminal_col) {
    printf("\e[%lud", (terminal_row*(9+REG_FILE_LIST.size()))); //7 rows for headers and power + 1 for '-----' + one row for map element + one for target row
    sprintf(terminal_comm, "\e[%uG", terminal_col);

    print_info();
    print_power();

    std::vector<uint32_t> values; //first position: address offset; second position: mask; third position 
    //if (!FLAG_INFO) {}
    if (FLAG_INFO) {
        auto it = REG_FILE_LIST.begin();
        while (it != REG_FILE_LIST.end()) { 
            values = it->second;
            if (values[2]!= 1) snprintf(tmp_buff + strlen(tmp_buff), BUFFER_SIZE - strlen(tmp_buff),"%s| %s %f \n", terminal_comm, it->first.c_str(), (float)(READ_32b(values[0])/(float)values[2]));
            else snprintf(tmp_buff + strlen(tmp_buff), BUFFER_SIZE - strlen(tmp_buff),"%s| %s %u \n", terminal_comm, it->first.c_str(), READ_32b(values[0]));   
            ++it;
        }
    } else { 
        auto it = LITE_REG_FILE.begin();
        auto it_end = LITE_REG_FILE.end();

        std::string reg_name;
        while (it != it_end) { 
            values = it->second;
            if (values[1] & Alveo_enabled_mask) {
                if (values[2]!= 1) snprintf(tmp_buff + strlen(tmp_buff), BUFFER_SIZE - strlen(tmp_buff),"%s| %s %f \n", terminal_comm, it->first.c_str(), (float)(READ_32b(values[0])/(float)values[2]));
                else snprintf(tmp_buff + strlen(tmp_buff), BUFFER_SIZE - strlen(tmp_buff),"%s| %s %u \n", terminal_comm, it->first.c_str(), READ_32b(values[0]));   
            }
            ++it;
        }
    }
    snprintf(tmp_buff + strlen(tmp_buff), BUFFER_SIZE - strlen(tmp_buff), "%s--------------------------------------------",terminal_comm);
    return tmp_buff;
}

char* alveo_board::print_power() {
    uint32_t pex12v, aux12v, pex3v3, aux3v3;
    uint32_t pex12v_I, aux12v_I, pex3v3_I, aux3v3_I;
    
    pex12v = READ_32b(0x020);
    aux12v = READ_32b(0x044);
    pex3v3 = READ_32b(0x02C);
    aux3v3 = READ_32b(0x038);
    pex12v_I = READ_32b(0x0C8);
    aux12v_I = READ_32b(0x0D4);
    pex3v3_I = READ_32b(0x278);
    aux3v3_I = READ_32b(0x2F0);
    
    float power, p_pex12v, p_aux12v, p_pex3v3, p_aux3v3;
    p_pex12v = (pex12v*pex12v_I)/1000000.0; 
    p_aux12v = (aux12v*aux12v_I)/1000000.0;
    p_pex3v3 = (pex3v3*pex3v3_I)/1000000.0;
    p_aux3v3 = (aux3v3*aux3v3_I)/1000000.0;
    power = p_pex12v+p_aux12v+p_pex3v3+p_aux3v3;

    snprintf(tmp_buff + strlen(tmp_buff), BUFFER_SIZE-strlen(tmp_buff),"%s| Power 12V PEX (W) %f \n", terminal_comm, p_pex12v);
    snprintf(tmp_buff + strlen(tmp_buff), BUFFER_SIZE-strlen(tmp_buff),"%s| Power 12V AUX (W) %f \n", terminal_comm, p_aux12v);
    snprintf(tmp_buff + strlen(tmp_buff), BUFFER_SIZE-strlen(tmp_buff),"%s| Power 3V3 PEX (W) %f \n", terminal_comm, p_pex3v3);
    snprintf(tmp_buff + strlen(tmp_buff), BUFFER_SIZE-strlen(tmp_buff),"%s| Power 3V3 AUX (W) %f \n", terminal_comm, p_aux3v3);
    snprintf(tmp_buff + strlen(tmp_buff), BUFFER_SIZE-strlen(tmp_buff),"%s| Total consumed power (W) %f \n", terminal_comm, power);
    return tmp_buff;
}

void alveo_board::enable_hbm_monitorization() {
   //Bit 27 (1=enable/0=disable)
   WRITE_32b(CONTROL_REG, 1<<27, 1<<27);
   
}

void alveo_board::CSV_sensor_data(){
    uint32_t pex12v, aux12v, pex3v3, aux3v3;
    uint32_t pex12v_I, aux12v_I, pex3v3_I, aux3v3_I;
    
    pex12v = READ_32b(0x020);
    aux12v = READ_32b(0x044);
    pex3v3 = READ_32b(0x02C);
    aux3v3 = READ_32b(0x038);
    pex12v_I = READ_32b(0x0C8);
    aux12v_I = READ_32b(0x0D4);
    pex3v3_I = READ_32b(0x278);
    aux3v3_I = READ_32b(0x2F0);
    
    float power, p_pex12v, p_aux12v, p_pex3v3, p_aux3v3;
    p_pex12v = (pex12v*pex12v_I)/1000000.0; 
    p_aux12v = (aux12v*aux12v_I)/1000000.0;
    p_pex3v3 = (pex3v3*pex3v3_I)/1000000.0;
    p_aux3v3 = (aux3v3*aux3v3_I)/1000000.0;
    power = p_pex12v+p_aux12v+p_pex3v3+p_aux3v3;

    //TARGET_FD_CSV = 3
    //Primarily data
    CSV_WRITE("%s, %f, %f, %f, %f, %f,", this->BDF, p_pex12v,p_aux12v,p_pex3v3,p_aux3v3, power);
    //Secondary data
    auto it = REG_FILE_LIST.begin();
    std::vector<uint32_t> values; //first position: address offset; second position: mask; third position 
    while (it != REG_FILE_LIST.end()) { 
        values = it->second;
        if (values[2]!= 1) {
             CSV_WRITE("%f,", (float)(READ_32b(values[0])/(float)values[2]));
        }
        else {
            CSV_WRITE("%u,", READ_32b(values[0]));
        }
        ++it;
    }

    return;
}

void alveo_board::CSV_header(){
    //TARGET_FD_CSV = 3
    //Primarydata
    CSV_WRITE("Target, P_12V PEX, P_12V AUX, P_3V3 PEX, P_3V3 AUX, Total power,");
    //Secondary data
    auto it = REG_FILE_LIST.begin();
    while (it != REG_FILE_LIST.end()) { 
        CSV_WRITE("%s,",(it->first).c_str());
        ++it;
    }
    return;
}

void alveo_board::print_soc_status(){
    if(Alveo_enabled_mask != MASK_U45N) {printf("Only available for U45N boards\n"); return;}

    WRITE_32bREG(CMC_HOST_MSG_REG, 0x11<<24, 0xffffffff);
    WRITE_32bREG(CMC_SOC_OPCODE_REG, 0x00, 0xffffffff);
    WRITE_32bREG(CONTROL_REG, 0x20, 0x20);

    while(READ_32b(CONTROL_REG) == 0x20) usleep(100000); //0.1 seconds

    if (READ_32b(HOST_MSG_ERROR) == 0x0) {
        printf("SOC status: %x \n", READ_32b(CMC_SOC_LENGTH_REG));
        printf("SOC type: %x \n", READ_32b(CMC_SOC_PAYLOAD_REG));
    } else printf("SOC Status read failed %s \n", HOST_ERROR_RETURN_MSG[READ_32b(HOST_MSG_ERROR)].c_str());

}

void alveo_board::cmc_soc_reset(){
    if(Alveo_enabled_mask == MASK_U45N) {
        WRITE_32bREG(CMC_HOST_MSG_REG, 0x12<<24, 0xffffffff);
        WRITE_32bREG(CMC_SOC_OPCODE_REG, 0x00, 0xffffffff);
        WRITE_32bREG(CMC_SOC_LENGTH_REG, 0x00, 0xffffffff);
        WRITE_32bREG(CMC_SOC_PAYLOAD_REG, 0xb002b002, 0xffffffff);

        WRITE_32bREG(CONTROL_REG, 0x20, 0x20);

        while(READ_32b(CONTROL_REG) == 0x20) usleep(100000); //0.1 seconds

        if (READ_32b(HOST_MSG_ERROR) == 0x0) printf("SOC reset control write complete\n");
        else printf("SOC reset control write failed; Reason: %s \n", HOST_ERROR_RETURN_MSG[READ_32b(HOST_MSG_ERROR)].c_str());
    } else { 
        printf("SOC reset only available for U45N boards\n");
        return;
    }
}

bool alveo_board::Open_BAR(){ //private
    char pciBarPath[64];
    snprintf(pciBarPath, 64, "%s%s%s%d", PCI_DEV_BASE_PATH, BDF, PCI_BAR_FILE,resource);

    pciBarFd = open(pciBarPath, O_RDWR | O_SYNC);
    if (pciBarFd < 0) {
        printf("%s \n", pciBarPath);
        perror("Could not open PCIe resources");
        return false;
    }
    
    pciBar = (uint32_t*) mmap(NULL, CMS_TOTAL_BAR_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, this->pciBarFd, this->Monitor_offset);
    
    if (pciBar == MAP_FAILED) {
        perror("Could not map BAR into process memory space");
        close(pciBarFd);
        return false;
    }

    int baseAddr = Reset_reg_offset; //writing to MicroBlaze_RESETN_REG as is reset low, we must pull high.
    uint32_t val = pciBar[(baseAddr)/sizeof(*pciBar)];
    val = pciBar[(baseAddr)/sizeof(*pciBar)];
    val = (val & (~0x01)) | ENABLE_RESET;
    pciBar[(baseAddr)/sizeof(*pciBar)] = val;

    usleep(500000); // Need some time to awake from reset 

    baseAddr = Register_map_offset + REG_MAP_ID_REG;
    val = pciBar[(baseAddr)/sizeof(*pciBar)];

    #if DEBUG
    printf("Offset: 0x%x, value: 0x%x; from 0x%ld and monitor 0x%x\n", baseAddr, val, (long int)pciBar, this->Monitor_offset);
    #endif

    if (val != REG_MAP_ID_REG_VALUE) throw std::runtime_error("Open_BAR: Magic number not found; Failed to find correct device");

    printf("Device with BDF: %s successfully accessed\n", BDF);


    return true;
}

void alveo_board::Init_reg_file_list() { //private
    auto it = LITE_REG_FILE.begin();
    auto it_end = LITE_REG_FILE.end();

    if (FLAG_INFO || FLAG_CSV_EXT) {
        it = REG_FILE.begin();
        it_end = REG_FILE.end();
    }

    std::string reg_name;
    std::vector<uint32_t> address_and_mask; //first position: address offset; second position: mask; third position division factor
    while (it != it_end) { 
        reg_name= it->first;
        address_and_mask = it->second;
        if (address_and_mask[1] & Alveo_enabled_mask) {
            REG_FILE_LIST.insert({reg_name, address_and_mask});
            #if DEBUG
            std::cout << reg_name << std::endl; 
            #endif
        }
        ++it;
    }
    return;
}

bool alveo_board::Identify_fpga(){ //private
    int baseAddr = Register_map_offset + PROFILE_NAME_REG;
    uint32_t val = pciBar[(baseAddr)/sizeof(*pciBar)];

    switch(val) {
	    case U200_U250: 
            snprintf(name_type, 18, "Alveo_U200/U250");
            Alveo_enabled_mask = MASK_U200_U250;
            break;
	    case U280: 
            snprintf(name_type, 18, "Alveo_U280");
            Alveo_enabled_mask = MASK_U280;
            break;
	    case U50: 
            snprintf(name_type, 18, "Alveo_U50");
            Alveo_enabled_mask = MASK_U50;
            break;
	    case U55: 
            snprintf(name_type, 18, "Alveo_U55");
            Alveo_enabled_mask = MASK_U55;
            break;
	    case U45N: 
            snprintf(name_type, 18, "Alveo_U45N");
            Alveo_enabled_mask = MASK_U45N;
            break;
        case X3: 
            snprintf(name_type, 18, "X3");
            Alveo_enabled_mask = MASK_X3;
            break;
	    case UL3422: 
            snprintf(name_type, 18, "UL3422");
            Alveo_enabled_mask = MASK_UL3422;
            break;
        case UL3524:
            snprintf(name_type, 18, "UL3524");
            Alveo_enabled_mask = MASK_UL3524;
            break;
        default:
            return false;
	}
    return true;
}

bool alveo_board::Identify_platform(){ //private
    resource = 0;
    // Some platforms have allocated the CMS space into the '/sys/bus/pci/devices/.../resource0 rather than in resource2
    // Different platforms may have their 'MB_RESETN_REG' on a different offset, we cannot assure that from platforms
    // which CMS/CMC/XMC is not mapped as indicated here [https://docs.amd.com/r/en-US/pg348-cms-subsystem/Register-Space]
    // a successfull conection can be stablished.
    switch(device) {
        case 0x5000: 
            Monitor_offset= 0x01E00000;
            strcpy(Platform_name,"U200 XDMA");
            pf=0;
            Reset_reg_offset=0x0;
            Register_map_offset=0x8000;
            break;
        case 0x5004: 
            Monitor_offset= 0x01E00000;
            strcpy(Platform_name,"U250 XDMA");
            pf=0;
            Reset_reg_offset=0x0;
            Register_map_offset=0x8000;
            break;
        case 0x5008: 
            Monitor_offset= 0x01E00000;
            strcpy(Platform_name,"U280 U280-ES1 XDMA");
            pf=0;
            Reset_reg_offset=0x0;
            Register_map_offset=0x8000;
            break;
        case 0x500c: 
            Monitor_offset= 0x01E00000;
            strcpy(Platform_name,"U280 XDMA");
            pf=0;
            Reset_reg_offset=0x0;
            Register_map_offset=0x8000;
            break;         
        case 0x5010: 
            Monitor_offset= 0x01E00000;
            strcpy(Platform_name,"U200 QDMA");
            pf=0;
            Reset_reg_offset=0x0;
            Register_map_offset=0x8000;
            break;
        case 0x5014: 
            Monitor_offset= 0x01E00000;
            strcpy(Platform_name,"U250 QDMA");
            pf=0;
            Reset_reg_offset=0x0;
            Register_map_offset=0x8000;
            break;
        case 0x5020: 
            Monitor_offset= 0x01E00000;
            strcpy(Platform_name,"U50 XDMA");
            pf=0;
            Reset_reg_offset=0x0;
            Register_map_offset=0x8000;
            break;
        case 0x5028: 
            Monitor_offset= 0x3000000;
           strcpy(Platform_name,"V350 XDMA");
            pf=0;
            Reset_reg_offset=0x0; // ???
            Register_map_offset=0x0;
            break;
        case 0x5050: 
            Monitor_offset= 0x01E00000;
            strcpy(Platform_name,"U25");
            pf=0;
            Reset_reg_offset=0x0;
            Register_map_offset=0x8000;
            break;
        case 0x503d: 
            Monitor_offset= 0x120000;
            strcpy(Platform_name,"U30 XDMA");
            pf=1;
            Reset_reg_offset=0x0; // ???
            Register_map_offset=0x0;
            break;
        case 0x513c: 
            Monitor_offset= 0x120000;
            strcpy(Platform_name,"U30 XDMA");
            pf=0;
            Reset_reg_offset=0x0; // ???
            Register_map_offset=0x0;
            break;
        case 0x5044: 
            Monitor_offset= 0x6000000;
            strcpy(Platform_name,"VCK5000 XDMA");
            pf=0;
            Reset_reg_offset=0x0; // ???
            Register_map_offset=0x0;
            break;
        case 0x5048: 
            Monitor_offset= 0x6000000;
            strcpy(Platform_name,"VCK5000 XDMA");
            pf=0;
            Reset_reg_offset=0x0; // ???
            Register_map_offset=0x0;
            break;
        case 0x5058: 
            Monitor_offset= 0x01E00000;
            strcpy(Platform_name,"U55N");
            pf=0;
            Reset_reg_offset=0x0;
            Register_map_offset=0x8000;
            break;
        case 0x505c: 
            Monitor_offset= 0x01E00000;
            strcpy(Platform_name,"U55 XDMA");
            pf=0;
            Reset_reg_offset=0x0;
            Register_map_offset=0x8000;
            break;
        case 0x6987: 
            Monitor_offset= 0x0;
            strcpy(Platform_name,"U.2");
            pf=0;
            Reset_reg_offset=0x0; //?
            Register_map_offset=0x120000;
            break;
        case 0x5060: 
            Monitor_offset= 0x01E00000;
            strcpy(Platform_name,"U50lv XDMA");
            pf=0;
            Reset_reg_offset=0x0;
            Register_map_offset=0x8000;
            break;
        case 0x506c: 
            Monitor_offset= 0x01E00000;
            strcpy(Platform_name,"U50c");
            pf=0;
            Reset_reg_offset=0x0;
            Register_map_offset=0x8000;
            break;
        case 0x8888: 
            Monitor_offset= 0x0;
           strcpy(Platform_name,"CMS EXDES");
            pf=0;
            Reset_reg_offset=0x20000; // ??
            Register_map_offset=0x28000;
            break;
        case 0x504e: 
            Monitor_offset= 0x01E00000;
            strcpy(Platform_name,"U26");
            pf=0;
            Reset_reg_offset=0x0;
            Register_map_offset=0x8000;
            break;
        case 0x5074: 
            Monitor_offset= 0x01E00000;
            strcpy(Platform_name,"X3");
            pf=0;
            Reset_reg_offset=0x0;
            Register_map_offset=0x8000;
            break;
        case 0x5078: 
            Monitor_offset= 0x6000000;
            strcpy(Platform_name,"V65 XDMA");
            pf=0;
            Reset_reg_offset=0x0;
            Register_map_offset=0x0;
            break;
        case 0x8800: 
            Monitor_offset= 0x0;
            strcpy(Platform_name,"CMS U200");
            pf=0;
            Reset_reg_offset=0x20000;
            Register_map_offset=0x28000;
            break;
        case 0x8804: 
            Monitor_offset= 0x0;
            strcpy(Platform_name,"CMS U250");
            pf=0;
            Reset_reg_offset=0x20000;
            Register_map_offset=0x28000;
            break;
        case 0x880c: 
            Monitor_offset= 0x0;
            strcpy(Platform_name,"CMS U280");
            pf=0;
            Reset_reg_offset=0x20000;
            Register_map_offset=0x28000;
            break;
        case 0x8820: 
            Monitor_offset= 0x0;
            strcpy(Platform_name,"CMS U50");
            pf=0;
            Reset_reg_offset=0x20000;
            Register_map_offset=0x28000;
            break;
        case 0x8858: 
            Monitor_offset= 0x0;
            strcpy(Platform_name,"CMS U55N");
            pf=0;
            Reset_reg_offset=0x20000;
            Register_map_offset=0x28000;
            break;
        case 0x885c: 
            Monitor_offset= 0x0;
           strcpy(Platform_name,"CMS U55C");
            pf=0;
            Reset_reg_offset=0x20000;
            Register_map_offset=0x28000;
            break;
        case 0x8860: 
            Monitor_offset= 0x0;
           strcpy(Platform_name,"CMS U50lv");
            pf=0;
            Reset_reg_offset=0x20000;
            Register_map_offset=0x28000;
            break;
        case 0x886c: 
            Monitor_offset= 0x0;
            strcpy(Platform_name,"CMS U50c");
            pf=0;
            Reset_reg_offset=0x20000;
            Register_map_offset=0x28000;
            break;
        case 0x884e: 
            Monitor_offset= 0x0;
            strcpy(Platform_name,"CMS U26");
            pf=0;
            Reset_reg_offset=0x20000;
            Register_map_offset=0x28000;
            break;
        case 0x8874: 
            Monitor_offset= 0x0;
            strcpy(Platform_name,"CMS X3");
            pf=0;
            Reset_reg_offset=0x20000;
            Register_map_offset=0x28000;
            break;
        case 0x902f: 
            Monitor_offset= 0x40000;
            strcpy(Platform_name,"BSC custom QDMA");
            pf=0;
            Reset_reg_offset=0x20000;
            Register_map_offset=0x28000;
            XilinxPlatform = false;
            resource = 2;
            break;
        case 0x9034: 
            Monitor_offset= 0x40000;
            strcpy(Platform_name,"BSC Lite QDMA");
            pf=0;
            Reset_reg_offset=0x20000;
            Register_map_offset=0x28000;
            XilinxPlatform = false;
            resource = 2;
            break;
        case 0x903f: 
            Monitor_offset= 0x40000;
            strcpy(Platform_name,"BSC Lite QDMA");
            pf=0;
            Reset_reg_offset=0x20000;
            Register_map_offset=0x28000;
            XilinxPlatform = false;
            resource = 2;
            break;
        default: return false;
    }
    if (device < uint32_t(0x902f)) XilinxPlatform = true;
    printf(" Name: %s \n CMS offset: 0x%x \n Regmap offset: 0x%x \n ", Platform_name, Monitor_offset, Register_map_offset);
    return true;
}

std::string readSysfsFile(const std::string &path) { //private
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << path << std::endl;
        return "";
    }
    
    std::string line = "";
    std::string content = "";
    while (getline(file, line)) content += line;

    file.close();
    return content;
}

bool alveo_board::Info_from_board(){ //private
    std::string sysfile_path = std::string(PCI_DEV_BASE_PATH) + std::string(BDF);
    
    unsigned int tmp_value;
    // Get vendor ID
    tmp_value = std::stoi(readSysfsFile(sysfile_path + "/vendor"), nullptr, 0);
    if (tmp_value==0) {
        std::cerr << "Failed to read vendor ID" << std::endl;
        return false;
    }
    vendor = tmp_value;
    
    // Get device ID
    tmp_value = std::stoi(readSysfsFile(sysfile_path + "/device"), nullptr, 0);
    if (tmp_value==0) {
        std::cerr << "Failed to read device ID" << std::endl;
        return false;
    }
    device = tmp_value;
    
    return true;
}

uint32_t alveo_board::READ_32bREG(const uint32_t std_offset) { //private
    return pciBar[(std_offset)/sizeof(*pciBar)];
}

void alveo_board::WRITE_32bREG(const uint32_t std_offset, const uint32_t value, const uint32_t mask=0xffffffff) { //private
    pciBar[(std_offset)/sizeof(*pciBar)] = (value & mask);
}

alveo_board::alveo_board(const std::string& newBDF) { //constructor
    std::strcpy(BDF, newBDF.c_str()); //initializes BDF
    try{
        if(!Info_from_board()) throw std::runtime_error("Info_from_board: Failed Info_from_board"); //initializes vendor and device
        if(!Identify_platform()) throw std::runtime_error("Identify_platform: Failed Identify_platform"); //initializes Monitor_offset, Platform_name, pf, Register_map_offset
        if(!Open_BAR()) throw std::runtime_error("Open_BAR: Failed Open_BAR"); //initializes pciBar, pciBarFd
        if(!Identify_fpga()) throw std::runtime_error("Identify_fpga: Failed to identify FPGA"); //initializes name_type and Alveo_enabled_mask
        Init_reg_file_list();           //initializes REG_FILE_LIST map
        enable_hbm_monitorization();
        std::cout << "alveo initialized" << std::endl;
    } catch (...) {
        cleanup();
        throw;
    }
}

void alveo_board::modify_command_register_pci(uint16_t value) {//deprecated
    char pciBarPath[64];
    snprintf(pciBarPath, 64, "%s%s/resource0", PCI_DEV_BASE_PATH, BDF );
    
    int Fd = open(pciBarPath, O_RDWR | O_SYNC);
    if (Fd < 0) {
        perror("Could not open PCIe register window");
        return;
    }

    void* BARadd = mmap(NULL, sysconf(_SC_PAGE_SIZE), PROT_READ | PROT_WRITE, MAP_SHARED, Fd, 0);
    if (BARadd == MAP_FAILED) {
        perror("Could not map BAR into process memory space");
        close(Fd);
        return;
    }

    //Command register is position on offset 0x04 from BAR0 
    uint16_t command_register = *(volatile uint16_t *)((int *)BARadd+0x04);  
    printf("Status from command register: 0x%0x\n", command_register);
    
    *(volatile uint16_t *)((int *)BARadd+0x04) = value;
    
    close(Fd);
    munmap(BARadd,sysconf(_SC_PAGE_SIZE));
    return;
}