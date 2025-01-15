/*                                                                                                                                                                                           #
# This file contains confidential and proprietary information of Xilinx, Inc.                        
#
# This file was generated for a Bachelor's thesis at BSC-CNS, under no circumstance            
# it tries to behave as it was made by Xilinx, or to gain any profit from it.                  
#
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

#include <iostream>
#include <string>
#include "board.hh"
#include <vector>
#include <filesystem>
#include <iomanip>
#include <fstream>
#include <unistd.h>
#include <chrono>
#include <ctime>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


using namespace std;
namespace fs = std::filesystem;

char CSV_BUFFER[BUFFER_SIZE];

bool FLAG_ALL=false;
bool FLAG_INFO=false;
bool FLAG_CSV=false;
bool FLAG_CSV_EXT=false;

int MS_LOOP_TIME = 2500;
string CSV_FILENAME = "cms_data.csv";

#define MS_MINIMUM_TIME 10
#define XILINX_VENDOR "0x10ee"

bool string_in_vector(const string& A, const vector<string>& V) {
    
    for (int i = 0; i < V.size(); i++) {
        cout << A << " " << V[i] << endl;
        if (A == V[i]) return true;
    }
    return false;
}

int setup_boards(vector<alveo_board *>& my_boards, const vector<string> BDF) {
    const std::string pci_path = "/sys/bus/pci/devices";
    const std::string Xvendor = XILINX_VENDOR;
    int Xilinx_devices_found = 0;

    for (const auto& entry : fs::directory_iterator(pci_path)) {
        const std::string device_path = entry.path();
        const std::string dev_ID = entry.path().filename();

        if (((BDF.size() != 0) && (string_in_vector(dev_ID,BDF))) || BDF.size() == 0){
            const std::string vendor_file = device_path + "/vendor";
            const std::string device_file = device_path + "/device";

            std::ifstream vendor_stream(vendor_file);
            std::ifstream device_stream(device_file);            
            if (vendor_stream.is_open() && device_stream.is_open())  {
                std::string vendor_id, device_id;
                vendor_stream >> vendor_id;
                device_stream >> device_id;
                if (Xvendor == vendor_id) {
                    std::cout << "PCI Device: " << dev_ID
                            << "\n  Vendor: " << vendor_id
                            << "\n  Device: " << device_id << "\n\n";
                    
                    alveo_board *tmp = nullptr;
                    try {
                        tmp = new alveo_board(dev_ID);
                    } catch (const std::runtime_error& e) {
                        std::cerr << e.what() << std::endl;
                        usleep(2000000);
                    }
                    if (tmp != nullptr) {
                        my_boards.push_back(tmp);
                        Xilinx_devices_found++;
                    } else printf("Board not added due to errors\n");
                }
            }
        }
    }
    return Xilinx_devices_found;
}

void USAGE() {
    cout << "USAGE: ./telemetry.exe [additional options]"<< endl;
	cout << "Additional options: "<< endl;
	cout << "\t -csv [optional]; Activate CSV support, to be written on the same folder as 'cms_data.csv'" << endl;
    cout << "\t\t -o <csv_filename.csv> ; With relative path (implicit './'): i.e. '/my_previous_folder/desktop/my_csv.csv'" << endl;
    cout << "\t\t -e ; write 'extended' data from all FPGAs sensors" << endl;
    cout << "\t -d <BDF> ; Specify to only get data from one device" << endl;
    cout << "\t -n <num devices> {<list of devices>}; Specify the number of devices: i.e. -n 2 0000:08:00.0 0000:09:00.0" << endl;
	cout << "\t -a ; Show data from all devices" << endl;
	cout << "\t -v ; Extra verbosity, Not made to be used with GUI with more than 4 FPGAs" << endl;
    cout << "\t -t <time milliseconds>; Specify refresh time in milliseconds (Minimum 500ms)" << endl;
    cout << "\t -r <BDF>; Specify Device to reset" << endl;
    cout << "\t -i <BDF>; Print information from device" << endl;
    exit(1);
}

vector<string> SetupARGS(const int argc, char *argv[]) {
    bool specified_boards = 0;
    string argument;
    vector<string> devices = {};
    for (int i = 1; i < argc; i++) {
        argument = argv[i];
        if (argument == "-h") USAGE();
        else if (argument == "-a" && !specified_boards) {FLAG_ALL=true; specified_boards=1; }
        else if (argument == "-v") FLAG_INFO=true;
        else if (argument == "-e" && FLAG_CSV) FLAG_CSV_EXT=true;
        else if (argument == "-d" && !specified_boards) {FLAG_ALL = false; devices.push_back(argv[i+1]); specified_boards=1;}
        else if (argument == "-csv"){FLAG_CSV=true; }
        else if (argument == "-o" && FLAG_CSV) {
            CSV_FILENAME=argv[i+1];
            i += 1; 
        }
        else if (argument == "-t") {
            MS_LOOP_TIME = atoi(argv[i+1]);
            if(MS_LOOP_TIME<MS_MINIMUM_TIME) MS_LOOP_TIME=MS_MINIMUM_TIME;
            i += 1;    
        }
        else if (argument == "-r") {
            vector<alveo_board *> my_boards;
            if (setup_boards(my_boards, {argv[i+1]})!= 1) {std::cerr << "Xilinx specified device to reset not found!\n"; exit(1);}
            my_boards[0]->cmc_soc_reset();
            exit(0);
        }
        else if (argument == "-i") {
            vector<alveo_board *> my_boards;
            if (setup_boards(my_boards, {argv[i+1]})!= 1) {std::cerr << "Xilinx specified device to print information not found!\n"; exit(1);}
            printf("%s",my_boards[0]->print_info());
            printf("SOC STATUS: ");
            my_boards[0]->print_soc_status();
            exit(0);
        }
        else if (argument == "-n" && !specified_boards) {
            int number_of_devices = atoi(argv[i+1]);
            for (int j = 1; j< number_of_devices; j++) {
                devices.push_back(argv[i+1+j]);
            }
            i += 1+number_of_devices;
            specified_boards=1;
        }
    }
    return devices;
}
#define MAX_FILE_SIZE 100 * 1024 * 1024  // 100 MB limit

void init_csv(vector<alveo_board *>& my_boards) {
    char csv_path[64];
    snprintf(csv_path, 64, "./%s",CSV_FILENAME.c_str());

    int fd_csv = open(csv_path, O_CREAT | O_WRONLY | O_TRUNC, 0666);
    if (fd_csv < 0) {perror("Error; CSV file opening\n"); exit(1);}

    //I will make the CSV file as the 4th channel descriptor
    if (dup2(fd_csv,TARGET_FD_CSV) < 0){
        perror("Error; duplicating file descriptor\n");
        close(fd_csv);
        exit(1);
    }
    CSV_WRITE("Timestamp: ,");
    int num_devs = my_boards.size();
    for (int i = 0; i < num_devs; i++) {
        my_boards[i]->CSV_header();
    }
    CSV_WRITE("\n");
}

void main_loop(vector<alveo_board *>& my_boards) {
    int num_devs = my_boards.size();

    using clock = std::chrono::high_resolution_clock;
    auto time_start = clock::now();
    std::chrono::milliseconds period(MS_LOOP_TIME);
    auto timenow = chrono::system_clock::to_time_t(chrono::system_clock::now()); 
    
    while (true) {         
        std::chrono::milliseconds diff = std::chrono::duration_cast<std::chrono::milliseconds>(clock::now() - time_start);
        if (diff > period){
            time_start = clock::now();
            
            timenow = chrono::system_clock::to_time_t(chrono::system_clock::now()); 
            string timestring = strtok(ctime(&timenow), "\n");
            CSV_WRITE("%s ,", timestring.c_str());
            printf("\033[2J\033[1;1H");
            for (int i = 0; i < num_devs; i++) {
                if(FLAG_CSV)my_boards[i]->CSV_sensor_data();
                printf("%s", my_boards[i]->print_sensor_data(i/4, (i%4)*44)); //44 constant width characters

            }
            printf("\n");
            cout << ctime(&timenow) << endl; 

            if (FLAG_CSV) {
                struct stat file_stat;
                // Check file size to make sure it does not get too big
                if (fstat(TARGET_FD_CSV, &file_stat) == 0 && file_stat.st_size >= MAX_FILE_SIZE) {
                    cerr << "CSV File size surpassed maximum size " << MAX_FILE_SIZE <<endl;
                    close(TARGET_FD_CSV);
                    exit(0);
                }  
                CSV_WRITE("\n"); 
            }
        else {usleep(diff.count()*1000*0.8);} // Let the CPU rest some time with the difference 
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc == 1) USAGE();
    vector<string> BDFs = SetupARGS(argc, argv);

    int number_of_devices = BDFs.size();
    vector<alveo_board *> my_boards;
    
    if (number_of_devices > 0) {
       if (setup_boards(my_boards, BDFs)!= number_of_devices) {std::cerr << "Xilinx specified device not found!\n"; exit(1);}
    }
    else {
        if (!setup_boards(my_boards, BDFs)) {std::cerr << "No Xilinx devices were found!\n"; exit(1);}
    }

    int num_devs = my_boards.size();
    for (int i = 0; i < num_devs; i++) {
        my_boards[i]->print_info();
    }

    if(FLAG_CSV) init_csv(my_boards);

    main_loop(my_boards);

    return 0;
}
