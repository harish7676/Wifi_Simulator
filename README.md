# Wifi_Simulator

# WiFi Simulator

This project is a simulation of WiFi technologies (WiFi 4, WiFi 5, and WiFi 6) to study network performance metrics such as throughput and latency under different scenarios.

The simulator supports:
- **WiFi 4**: Basic functionality with contention-based channel access.
- **WiFi 5**: Enhanced performance with MU-MIMO support.
- **WiFi 6**: Advanced capabilities with OFDMA support.

## Features
- **Dynamic Packet Transmission**: Simulate varying numbers of packets and clients.
- **Latency and Throughput Metrics**: Evaluate network performance for different WiFi technologies.
- **Channel State Management**: Simulate channel availability and contention.

## Requirements
- **C++11**: The simulator uses modern C++ features.
- **g++ Compiler**: For compiling the source code.
- **Make**: For building the project (optional).

## Installation
1. Clone this repository:
   ```bash
   git clone https://github.com/<your-username>/wifi-simulator.git
   cd wifi-simulator

Build the project:
If using make:
bash
Copy code
make
If using g++ directly:
bash
Copy code
g++ -std=c++17 -o wifi wifi.cpp

Run the program to choose a WiFi simulation type:
./wifi

Follow the on-screen prompts to:

Choose the WiFi technology to simulate:
1: WiFi 4
2: WiFi 5
3: WiFi 6
4: Exit
Enter the number of packets for the simulation.
Observe the simulation results for varying numbers of clients (1, 10, and 100)

Example Run: Choose WiFi Simulation Type:
1. WiFi 4
2. WiFi 5
3. WiFi 6
4. Exit
Enter your choice: 1

WiFi 4 Simulation

Simulating with 1 clients:
Throughput: 19.2 Mbps
Achievable Throughput: 18.5 Mbps
Average Latency: 5.3 ms
Peak Latency: 10.2 ms

Simulating with 10 clients:
...

Do you want to run another simulation? (y/n):

Cleaning Up
To remove the compiled binary:
make clean