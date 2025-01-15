# Alveo_Telemetry
This tool enables the user to establish a succesfull connection with the Alveo CMS IP. Which is an IP dedicated to extract data from the embedded processor which monitors the FPGA. This tool is independent from the type of driver and DMA controller used (QDMA or XDMA). The user can create their own platform without much modifications needed.

## Installation
To install, download this repository. Execute the makefile and ready to go.
It may require to additionally install some other libraries as 'pci/pci.h' or update C++ compiler.

## Usage
When there is a FPGA with a working system which contains either some of the Xilinxs platforms or a custom made design with a CMS. Make sure this program has the correct memory offsets, if it is needed modify it, load the relevant bitstream and execute the following program:

  USAGE: ./telemetry.exe [additional options] <br/>
  Additional options: <br/>
* '-csv [optional]' Activate CSV support, to be written on the same folder as 'cms_data.csv' <br/>
  * '-o <csv_filename.csv>' With relative path (implicit './'): i.e. '/my_previous_folder/desktop/my_csv.csv' <br/>
  * '-e' write 'extended' data from all FPGAs sensors <br/>
* '-d <BDF>' Specify to only get data from one device <br/>
* '-n <num devices> {<list of devices>}' Specify the number of devices: i.e. -n 2 0000:08:00.0 0000:09:00.0 <br/>
* '-a' Show data from all devices <br/>
* '-v' Extra verbosity, Not made to be used with GUI with more than 4 FPGAs <br/>
* '-t <time milliseconds>' Specify refresh time in milliseconds (Minimum 500ms) <br/>
* '-r <BDF>' Specify Device to reset <br/>
* '-i <BDF>' Print information from device <br/>
    
## Modifying the content
A user can create their own platform modying a couple of lines in file 'board.cpp'.
Under function 'Identify_platform' a user can add their own platform using the 'device' ID which can be configures on any QDMA-XDMA IPs.
In the switch case that only gets executed once, create a new case with an unique device ID. <br/>
**Important parameters:** <br/>
* Monitor_offset: The address at which the CMS is connected. <br/>
* Platform_name: The name your platform has (optional). <br/>
* pf: the physical function in which the CMS resides from the PCI reserved space. <br/>
* Reset_reg_offset: Indicate the CMS/CMC/XMC reset register offset. In the case of CMC and CMS the offsets differ. Normally for CMS resides on 0x20000. <br/>
* Register_map_offset: From the monitor offset, the offset in which the first registers (starting by Register MAP ID) are located. Normally for CMS  resides on 0x28000.

This program does not ensure the functionality with all of Xilinxs platforms because we don't have full access to the offsets of said registers.<br/>
### Disclaimer
Under no circumstance this file tries to behave as it was made by Xilinx, or to gain any profit from it.        <br/>         
 
Copyright 2022 Barcelona Supercomputing Center-Centro Nacional de Supercomputación  <br/>
Licensed under the Solderpad Hardware License v 2.1 (the "License"); <br/>
you may not use this file except in compliance with the License, or, at your option, the Apache License version 2.0. <br/>
You may obtain a copy of the License at <br/>
  
http://www.solderpad.org/licenses/SHL-2.1 <br/>

Unless required by applicable law or agreed to in writing, software <br/>
distributed under the License is distributed on an "AS IS" BASIS, <br/>
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. <br/>
See the License for the specific language governing permissions and <br/>
limitations under the License.  <br/>

### Author:
For any issues, advices, bugs, legal requirements contact: <br/>
Joan Teruel Jurado <br/>
Barcelona Supercomputer Center - Centro Nacional de Supercomputación <br/>
email: joan.teruel@bsc.es <br/>
