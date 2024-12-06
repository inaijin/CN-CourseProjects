# **Network Simulator to Analyze Routing Algorithms**

## **Introduction**

Welcome to our **Network Simulator to Analyze Routing Algorithms** project! This project aims to build a network simulation environment that mimics real-world network behavior. The simulator is designed to study and analyze various routing algorithms, including RIP and OSPF, under different network conditions and topologies.

The core of our simulation is built using **C++ with the Qt Framework**, utilizing modular, object-oriented programming principles. This ensures scalability, maintainability, and efficient handling of components such as routers, PCs, and Autonomous Systems (AS). The simulator supports dynamic configurations via JSON files, enabling the creation of custom topologies and simulation parameters.

---

## **Phases of the Project**

Our project is divided into multiple phases, each focusing on a specific aspect of the simulation:

1. **Phase 1 (Point Zero Phase)**: 
   - Initialize the network by constructing the topology based on the configuration file.
   - Set up components such as PCs, routers, ports, and Autonomous Systems (AS).
   - Ensure all nodes are connected but remain inactive.

2. **Phase 2 (Identification Phase)**:
   - Activate the network components and assign IP addresses dynamically using DHCP.
   - Routers exchange control packets to identify their neighbors and build initial routing tables.

3. **Phase 3 (Execution Phase)**:
   - Simulate network traffic by generating and transmitting data packets.
   - Observe the behavior of routing algorithms under normal and stress conditions.

4. **Phase 4 (Analysis and Reporting)**:
   - Analyze the simulation results, including metrics like packet loss, hop count, and waiting cycles.
   - Compare the performance of routing algorithms and generate comprehensive reports.

---

## **Phase 1: Point Zero Phase**

### **Objective**
In this phase, we aim to initialize the network environment by:
- Setting up the topology based on the configuration file.
- Creating and connecting essential components such as PCs, routers, and ASes.
- Leaving the nodes in an inactive state for further setup in Phase 2.

### **Implementation Details**

#### **Key Components**

1. **MACAddress & MACAddressGenerator**
   - **Purpose**: Provides a unique MAC address for each network node.
   - **Challenge**: Ensuring uniqueness and validating address formats.
   - **Solution**: `MACAddressGenerator` uses a `QSet` to track generated addresses, preventing duplicates. Address validation leverages `QRegularExpression`.

   **Code Details**:
   - `MACAddress::isValid`: Validates the format using a regex pattern.
   - `MACAddressGenerator::generate`: Generates a random, unique MAC address.

2. **IP Class**
   - **Purpose**: Abstracts IPv4 and IPv6 functionality into a single template-based class.
   - **Challenges**:
     1. Ensuring template reusability for both IP versions.
     2. Handling `QString`-based input and converting it to numeric values for simulation.
   - **Solutions**:
     - Used templates to specialize IP handling for IPv4 and IPv6.
     - Integrated `QHostAddress` for seamless conversion between string representations and raw address formats.

   **Key Features**:
   - `toString` converts numeric IP to its human-readable form.
   - `fromString` validates and parses IP strings using `QHostAddress`.
   - **Fix**: Addressed template specialization bugs by carefully separating shared and specific functionality for IPv4 and IPv6.

3. **EventsCoordinator**
   - **Purpose**: Centralized event management for time-based simulations.
   - **Challenges**:
     - Integrating with `DataGenerator` to synchronize packet generation and emission.
     - Managing event-driven triggers for tick-based simulation.
   - **Solution**:
     - Used `QTimer` for periodic event emission.
     - Connected `EventsCoordinator` with `DataGenerator` to preload and emit packets dynamically.

   **Key Features**:
   - `startClock`: Starts the simulation timer.
   - `setDataGenerator`: Links the packet generator for dynamic event handling.
   - **Fix**: Resolved integration issues by explicitly managing `QSharedPointer` lifetimes for packets.

4. **DataGenerator**
   - **Purpose**: Generates network packets using a Poisson distribution.
   - **Challenges**:
     - Ensuring the distribution was correctly spread over time.
     - Generating packets with valid destinations and payloads.
   - **Solution**:
     - Used a custom Poisson distribution generator to model traffic.
     - Validated and randomized destinations to avoid empty queues.

   **Key Features**:
   - `generatePackets`: Generates packets dynamically based on configured destinations.
   - **Fix**: Adjusted time-scaling logic to ensure proper temporal distribution of packets.

5. **Headers (DataLinkHeader & TCPHeader)**
   - **Purpose**: Encapsulate packet data with network and transport layer headers.
   - **Challenges**:
     - Ensuring compatibility with the `Packet` class.
     - Managing header updates dynamically during routing.
   - **Solutions**:
     - Designed flexible setters and getters for seamless integration.
     - Added validation for source and destination addresses in headers.

6. **Packet**
   - **Purpose**: Represents a data unit in the network.
   - **Challenges**:
     - Integrating with headers while maintaining encapsulation.
   - **Solution**:
     - Used composition to include `DataLinkHeader` and `TCPHeader` in `Packet`.

   **Key Features**:
   - `addToPath`: Tracks routing path for analysis.
   - `markDropped`: Flags packets dropped due to network conditions.

#### **Topology Construction**
- **Tool**: `Simulator` reads and parses the JSON configuration file then gives it to `AutonomousSystem` to initialize nodes and links.
- Nodes (PCs, routers) are assigned unique MAC addresses and prepared for activation in later phases.

---

### **Challenges and Fixes**

1. **Template Specialization in IP Class**:
   - Template-based design was initially buggy due to shared constructors and methods.
   - **Fix**: Explicitly specialized methods for IPv4 and IPv6 while keeping a common base.

2. **Packet Integration with Headers**:
   - Initial design overlooked seamless header inclusion.
   - **Fix**: Refactored `Packet` class to include setters/getters for `DataLinkHeader` and `TCPHeader`.

3. **DataGenerator Timing Distribution**:
   - Packet generation wasn't evenly distributed across the timeline.
   - **Fix**: Adjusted Poisson logic to factor in time scaling and rebalanced packet intervals.

---

### **Testing**
- **Tools**: Qt’s QTest framework.
- **Tests**:
  - **MACAddressGenerator**: Validated uniqueness and format compliance.
  - **IP**: Checked string-to-value conversions and template specialization.
  - **EventsCoordinator**: Verified timer triggers and packet queue management.
  - **DataGenerator**: Confirmed Poisson distribution accuracy.

---

### **State at the End of Phase 1**
- Topology is fully constructed and connected.
- Nodes remain inactive for Phase 2 activation and IP assignment.

---

## **Future Phases**

- **Phase 2**: Activate the network and assign IP addresses using DHCP. Begin routing table setup.
- **Phase 3**: Simulate real-world traffic, generate and transmit data packets, and observe routing algorithm behavior.
- **Phase 4**: Analyze simulation results, compare routing algorithms, and generate detailed reports.
