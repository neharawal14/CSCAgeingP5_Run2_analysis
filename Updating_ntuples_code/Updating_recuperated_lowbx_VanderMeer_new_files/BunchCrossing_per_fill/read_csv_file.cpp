#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
void read_csv_file(){

   // Open CSV file
    std::ifstream file("/eos/home-n/nrawal/CSCAgeing/VanderMeer_removal/bunchcrossing_luminosity/filtered_runs_2016.csv");  // Change this to your actual CSV file
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open CSV file!" << std::endl;
        return;
    }

    // Store run ranges in a vector of pairs
    std::vector<std::pair<int, int>> run_ranges;
    std::string line;
    std::getline(file, line);
// Read the CSV file line by line
while (std::getline(file, line)) {
    std::stringstream ss(line);
    std::string run_start_str,run_end_str; 
    int run_start, run_end;
    // Read the first three fields (ignoring whitespace issues)

    std::getline(ss, run_start_str, ',');
    std::getline(ss, run_end_str, ',');

    try {
            int run_start = std::stoi(run_start_str);
            int run_end = std::stoi(run_end_str);
            run_ranges.push_back(std::make_pair(run_start, run_end));
        } catch (const std::invalid_argument &e) {
            std::cerr << "Error: Unable to convert values in line: " << line << std::endl;
        }
}
file.close();
 std::cout << "Parsed Run Ranges:\n";
    for (const auto &range : run_ranges) {
        std::cout << "Start: " << range.first << ", End: " << range.second << std::endl;
    }

}

