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
#ifndef BOARD_HH
#define BOARD_HH

#include "sensors.hh"
#include <string>
#include <cstring>
#include <cstdint>

#define PCI_DEV_BASE_PATH "/sys/bus/pci/devices/" 
#define PCI_BAR_FILE "/resource"

//GLOBAL VARIABLE
#define BUFFER_SIZE 2048
extern char CSV_BUFFER[BUFFER_SIZE];
extern bool FLAG_INFO;
extern bool FLAG_CSV_EXT;
#define TARGET_FD_CSV 3 //THE 4th FileDescriptor is used to write CSV data.

#define CSV_WRITE(format, ...) \
    sprintf(CSV_BUFFER, format, ##__VA_ARGS__); \
    if(write(TARGET_FD_CSV, CSV_BUFFER, strlen(CSV_BUFFER))<0) {\
        perror("ERROR WRITING TO CSV FILE");}


typedef char BUFFER_t[BUFFER_SIZE];

class alveo_board {
    public:
    alveo_board(const std::string& BDF_);

    ~alveo_board();

    /* DESC: Print power from FPGA
    * RETURN: Returns pointer to buffer where the data is written
    */
    char* print_power();

    /* DESC: Prints basic info from the FPGA
    * RETURN: Returns pointer to buffer where the data is written
    */
    char* print_info();

    /* DESC: Prints sensor data info from the FPGA
    * PARAM: 
    *   terminal_row: row index where to print data in the terminal
    *   terminal_cow: column index where to print data in the terminal
    * RETURN: Returns pointer to buffer where the data is written
    */
    char* print_sensor_data(int terminal_row, int terminal_col);

    /* DESC: Prints basic info from the FPGA
    * POST: CSV file modified with new data
    */
    void CSV_sensor_data();

    /* DESC: Prints basic info from the FPGA
    * POST: CSV file modified with new data
    */
    void CSV_header();

    /* DESC: Prints SOC info from the FPGA
    */
    void print_soc_status();

    /* DESC: tries a soc reset to the FPGA
    * PRE: FPGA type AlveoU45N
    * POST: SOC reset
    */
    void cmc_soc_reset();

    private:
        char BDF[13];
        char name_type[18];
        char Platform_name[20];
        bool XilinxPlatform;                //ENDORSED PLATFORM FROM XILINX?

        int vendor;
        int device;

        int resource;

        uint32_t Monitor_offset;            //CMS OFFSET FROM *pciBar
        uint32_t Register_map_offset;       //CMS REGISTER MAP OFFSET FROM *pciBar+Monitor_offset
        uint32_t Reset_reg_offset;

        uint8_t Alveo_enabled_mask;         //MASK TO CHECK FOR SENSORS

        uint32_t pf;                        //PHYSICAL FUNCTION THE CMS BAR RESIDES ON
        uint32_t *pciBar;                  //BAR[pf] ADDRESS
        int pciBarFd;
        
        reg_file_t REG_FILE_LIST;

        BUFFER_t tmp_buff;
        char terminal_comm[8];
    /* DESC: READ A REGISTER
    * PARAM: Offset for given register
    * RETURN: uint32_t with the value from the PCI pointer to BAR region plus offset
    */
    uint32_t READ_32bREG(const uint32_t std_offset);

    /* DESC: WRITE A VALUE INTO A REGISTER
    * PARAM: Offset for given register, value and mask
    * POST: REGISTER WRITTEN WITH A NEW VALUE
    */
    void WRITE_32bREG(const uint32_t std_offset, const uint32_t value, const uint32_t mask);

    /* DESC: We need to identify the pci device
    * POST: Initializes vendor and device
    * RETURN: Bool indicating if there is any information for said board
    */
    bool Info_from_board();

    /* DESC: Initializes REG_FILE_LIST
    * POST: Initializes map values with supported sensor for current alveo
    */
    void Init_reg_file_list(); 

    /* DESC: We need to identify the platform, this is made with the device number one can get through PCI library
    * POST: Initializes Monitor_offset, Platform_name, pf, Register_map_offset
    * RETURN: Bool indicating if the platform is on the known list
    */
    bool Identify_platform();

    /* DESC: Close FD and free memory map
    * POST: Free map allocation
    */
    void cleanup();

    /* DESC: Initializes pciBar and pciBarFd
    * POST: Memory map allocated
    * RETURN: Bool indicating if the function was successful
    */
    bool Open_BAR();

    /* DESC: Identifies the FPGA
    * POST: name_type and Alveo_enabled_mask initializa
    * RETURN: Bool indicating if the function was successful on finding the Board type
    */
    bool Identify_fpga(); //private

    /* DESC: Writes into the command register of the PCI of the FPGA
    * PARAM: 16 bit valye to write
    * POST: command register gets modified
    */
    void modify_command_register_pci(uint16_t value); //deprecated & private

     /* DESC: Enables the HBM monitorization
    * POST: HBM monitorization bit set
    */
    void enable_hbm_monitorization();
};



#endif  // BITINFO_HH
