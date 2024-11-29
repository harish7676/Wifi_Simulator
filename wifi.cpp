#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <string>
#include <queue>
#include <memory>

using namespace std;

// Base exception class for WiFi errors
class wifi_exception : public exception {
protected:
    string message;
public:
    explicit wifi_exception(const string& msg) : message(msg) {}
    const char* what() const noexcept override { return message.c_str(); }
};

// Network packet class to handle data transmission (WiFi 4)
class NetworkPacket {
private:
    int size;
    double transmissionTime;
    string type;

public:
    NetworkPacket(int sizeBytes, double bandwidth, double modulation, const string& packetType = "DATA")
        : size(sizeBytes), type(packetType) {
        if (sizeBytes <= 0 || bandwidth <= 0 || modulation <= 0) {
            throw wifi_exception("Invalid packet parameters");
        }
        transmissionTime = (size * 8.0) / (bandwidth * modulation);
    }

    int getSize() const { return size; }
    double getTransmissionTime() const { return transmissionTime; }
    string getType() const { return type; }
};

// Frequency channel class to manage channel state
class FreqChannel {
public:
    enum State { FREE, OCCUPIED };

private:
    State state;
    string identifier;

public:
    FreqChannel(const string& id = "Default") : state(FREE), identifier(id) {}
    
    void setState(State newState) { state = newState; }
    bool isAvailable() const { return state == FREE; }
    string getIdentifier() const { return identifier; }
};

// Base network user class for WiFi 4
class NetworkUser {
protected:
    int userID;
    double backoffTime;
    bool active;

public:
    NetworkUser(int id) : userID(id), backoffTime(0), active(true) {}
    virtual ~NetworkUser() = default;

    bool checkActivityStatus() const { return active; }
    void setActivityStatus(bool status) { active = status; }
    int getUserID() const { return userID; }
};

// WiFi 6 User implementation with OFDMA support
class WiFi6User : public NetworkUser {
private:
    int allocatedSubChannel;
public:
    WiFi6User(int id) : NetworkUser(id), allocatedSubChannel(-1) {}

    void allocateSubChannel(int subChannel) {
        allocatedSubChannel = subChannel;
    }

    int getAllocatedSubChannel() const {
        return allocatedSubChannel;
    }

    bool attemptTransmission(FreqChannel& channel, int subChannel) {
        if (allocatedSubChannel == subChannel && channel.isAvailable()) {
            channel.setState(FreqChannel::OCCUPIED);
            return true;
        }
        return false;
    }
};

// WiFi 4 User class simulating behavior
class WiFiUser {
private:
    int id;
    double backoffInterval;
    int collisionCount;
    bool waitingForAccess;

public:
    WiFiUser(int id) : id(id), collisionCount(0), waitingForAccess(false) {
        resetBackoffInterval();
    }

    void resetBackoffInterval() {
        backoffInterval = std::min((rand() % 21 + 1) * pow(2, collisionCount), 450.0);
    }

    bool attemptToTransmit(FreqChannel &channel, double &latency, double congestionFactor) {
        if (waitingForAccess) {
            waitingForAccess = false;
            return false;
        }

        bool collisionOccurred = (rand() % 150 < congestionFactor * 100);

        if (!collisionOccurred) {
            channel.setState(FreqChannel::OCCUPIED);
            latency += backoffInterval;
            collisionCount = 0;
            channel.setState(FreqChannel::FREE);
            return true;
        } else {
            collisionCount++;
            resetBackoffInterval();
            waitingForAccess = true;
            return false;
        }
    }

    double getBackoffInterval() const { return backoffInterval; }
};

// WiFi 4 Access Point class to manage network activity
class WiFi4AccessPoint {
private:
    FreqChannel channel;
    std::vector<WiFiUser> clients;
    std::vector<double> latencyRecords;
    int successfulTransfers;
    double totalDuration;
    const double transferRate;
    const double packetSizeInBits;

public:
    WiFi4AccessPoint() : 
        successfulTransfers(0), 
        totalDuration(0),
        transferRate(20e6 * 8 * (5.0 / 6.0)),
        packetSizeInBits(1024 * 8) {}

    void addClient(const WiFiUser& client) {
        clients.push_back(client);
    }

    void simulateNetwork(int numPackets) {
        latencyRecords.clear();
        successfulTransfers = 0;
        totalDuration = 0;

        const double idealDuration = packetSizeInBits / transferRate;
        const double congestion = std::min(0.05 * clients.size(), 0.5);

        if (clients.size() == 1) {
            // Single user scenario - ideal situation
            for (int i = 0; i < numPackets; ++i) {
                latencyRecords.push_back(idealDuration * 1000);
                successfulTransfers++;
                totalDuration += idealDuration;
            }
        } else {
            // Multiple users scenario
            for (int packetIndex = 0; packetIndex < numPackets; ++packetIndex) {
                for (auto &client : clients) {
                    double contentionDelay = (rand() % 50) * 0.001;
                    double backoffDelay = client.getBackoffInterval();
                    double totalLatency = contentionDelay + backoffDelay;

                    if (client.attemptToTransmit(channel, totalLatency, congestion)) {
                        totalLatency += idealDuration * 1000;
                        latencyRecords.push_back(totalLatency);
                        successfulTransfers++;
                    }
                    totalDuration += (totalLatency / 1000);
                }
            }
        }
    }

    void displayStatistics() const {
        std::cout << "--------------------------------------------------------\n";
        std::cout << "Simulation Results for " << clients.size() << " Clients:\n";

        double maxPossibleThroughput = transferRate / 1e6;
        double actualThroughput = (successfulTransfers * packetSizeInBits) / (totalDuration);
        double achievableThroughput = std::min(actualThroughput / 1e6, maxPossibleThroughput);

        double avgLatency = latencyRecords.empty() ? 0 : 
            std::accumulate(latencyRecords.begin(), latencyRecords.end(), 0.0) / latencyRecords.size();
        double peakLatency = latencyRecords.empty() ? 0 : 
            *std::max_element(latencyRecords.begin(), latencyRecords.end());

        std::cout << "Throughput: " << maxPossibleThroughput << " Mbps\n"
                  << "Achievable Throughput: " << achievableThroughput << " Mbps\n"
                  << "Average Latency: " << avgLatency << " ms\n"
                  << "Peak Latency: " << peakLatency << " ms\n";
    }
};

// WiFi 5 Access Point implementation with MU-MIMO support
class WiFi5AccessPoint {
private:
    FreqChannel channel;
    vector<WiFiUser*> users;

public:
    WiFi5AccessPoint() : channel("WiFi5_Channel") {}

    void registerUser(WiFiUser* user) {
        users.push_back(user);
    }

    void simulateMU_MIMO(int numPackets) {
        vector<double> latencies;
        double totalThroughput = 0;
        cout << "--- WiFi 5 MU-MIMO Simulation ---\n";
        cout << "Number of Users: " << users.size() << "\n";

        for (int t = 0; t < numPackets; ++t) {
            for (auto& user : users) {
                double latency = 0.0;
                if (user->attemptToTransmit(channel, latency, 0.1)) {
                    totalThroughput += 20e6 * 8 * (5.0 / 6.0); // Transfer rate for WiFi 5
                    latencies.push_back(latency);
                }
            }
        }

        // Calculate statistics
        double avgLatency = accumulate(latencies.begin(), latencies.end(), 0.0) / latencies.size();
        double maxLatency = *max_element(latencies.begin(), latencies.end());

        cout << "Total Throughput: " << totalThroughput / 1e6 << " Mbps\n";
        cout << "Average Latency: " << avgLatency << " ms\n";
        cout << "Max Latency: " << maxLatency << " ms\n";
    }
};


// WiFi 6 Access Point with OFDMA support
class WiFi6AccessPoint {
private:
    double bandwidth;
    double bitsPerSymbol;
    double codingRate;
    vector<WiFi6User*> users;
    FreqChannel channel;  // Add channel as a member variable

public:
    WiFi6AccessPoint(double bandwidth, double bitsPerSymbol, double codingRate)
        : bandwidth(bandwidth), bitsPerSymbol(bitsPerSymbol), codingRate(codingRate), channel("WiFi6_Channel") {}

    void registerUser(WiFi6User* user) {
        users.push_back(user);
    }

    void simulateOFDMA(int numPackets) {
        double totalThroughput = 0;
        vector<double> userLatencies;
        int subChannelIndex = 0;

        for (int t = 0; t < numPackets; ++t) {  // Simulating 100 time slots
            for (size_t i = 0; i < users.size(); ++i) {
                WiFi6User* user = users[i];
                user->allocateSubChannel(subChannelIndex);

                if (user->attemptTransmission(channel, subChannelIndex)) {
                    double throughput = (bandwidth / users.size()) * 8.0 * codingRate;
                    totalThroughput += throughput;

                    // Add latency (simulated)
                    userLatencies.push_back((rand() % 100) * 0.1);
                }
                subChannelIndex = (subChannelIndex + 1) % 10;
            }
        }

        // Calculate statistics
        double avgLatency = accumulate(userLatencies.begin(), userLatencies.end(), 0.0) / userLatencies.size();
        double maxLatency = *max_element(userLatencies.begin(), userLatencies.end());

        cout << "Total Throughput: " << totalThroughput / 1e6 << " Mbps\n";
        cout << "Average Latency: " << avgLatency << " ms\n";
        cout << "Max Latency: " << maxLatency << " ms\n";
    }
};


// Run WiFi 4 simulation
void runWiFi4Simulation(int numClients, int numPackets) {
    WiFi4AccessPoint ap;
    for (int i = 0; i < numClients; ++i) {
        ap.addClient(WiFiUser(i));
    }
    ap.simulateNetwork(numPackets);
    ap.displayStatistics();
}

// Run WiFi 5 simulation
void runWiFi5Simulation(int numClients, int numPackets) {
    WiFi5AccessPoint ap;
    for (int i = 0; i < numClients; ++i) {
        ap.registerUser(new WiFiUser(i));
    }
    ap.simulateMU_MIMO(numPackets);
}

// Run WiFi 6 simulation
void runWiFi6Simulation(int numClients, int numPackets) {
    double bandwidth = 20e6;       // 20 MHz channel
    double bitsPerSymbol = 8.0;    // 256-QAM
    double codingRate = 5.0 / 6.0; // Coding rate 5/6

    WiFi6AccessPoint ap(bandwidth, bitsPerSymbol, codingRate);

    for (int i = 0; i < numClients; ++i) {
        ap.registerUser(new WiFi6User(i));
    }

    ap.simulateOFDMA(numPackets);
}

// Main function with user choice
int main() {
    srand(time(0));

    int choice;
    while (true) {
        cout << "Choose WiFi Simulation Type:\n";
        cout << "1. WiFi 4\n";
        cout << "2. WiFi 5\n";
        cout << "3. WiFi 6\n";
        cout << "4. Exit\n";  // Add an exit option
        cout << "Enter your choice: ";
        cin >> choice;

        if (choice == 4) {
            cout << "Exiting the program. Goodbye!\n";
            break;  // Exit the loop and the program
        }

        int numPackets;
        cout << "Enter number of packets: ";
        cin >> numPackets;

        // Running different user scenarios
        if (choice == 1) {
            cout << "\nWiFi 4 Simulation\n";
            for (int numClients : {1, 10, 100}) {
                cout << "\nSimulating with " << numClients << " clients:\n";
                runWiFi4Simulation(numClients, numPackets);
            }
        } else if (choice == 2) {
            cout << "\nWiFi 5 Simulation\n";
            for (int numClients : {1, 10, 100}) {
                cout << "\nSimulating with " << numClients << " clients:\n";
                runWiFi5Simulation(numClients, numPackets);
            }
        } else if (choice == 3) {
            cout << "\nWiFi 6 Simulation\n";
            for (int numClients : {1, 10, 100}) {
                cout << "\nSimulating with " << numClients << " clients:\n";
                runWiFi6Simulation(numClients, numPackets);
            }
        } else {
            cout << "Invalid choice. Please try again.\n";
        }

        // After each simulation, ask if the user wants to exit or continue
        char continueChoice;
        cout << "\nDo you want to run another simulation? (y/n): ";
        cin >> continueChoice;

        if (continueChoice != 'y' && continueChoice != 'Y') {
            cout << "Exiting the program. Goodbye!\n";
            break;  // Exit the loop and the program
        }
    }

    return 0;
}

