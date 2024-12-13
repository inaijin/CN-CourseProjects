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

2. **Phase 2 (Component Implementation)**:
   - Implement foundational classes necessary for the network to function, including:
     - **IP**
     - **IPHeader**
     - **Port**
     - **PortBindingManager**
     - **Node**
     - **PC**
     - **Router** (without routing algorithms)
     - **TopologyBuilder**
     - **TopologyController**
     - **AutonomousSystem**
   - By the end of this phase, the topology should be fully constructed and operational, capable of simulating packet exchanges.
   - Some DHCP functionality may also be included to enable dynamic IP assignment.

3. **Phase 3 (Execution Phase)**:
   - Simulate network traffic by generating and transmitting data packets.
   - Implement and test routing algorithms such as RIP and OSPF.
   - Observe the behavior of these algorithms under various conditions, including link failures and congestion.

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

## **Phase 2: Component Implementation**

### **Objective**
The second phase focuses on implementing the core components required to establish a functional network topology. The primary goal is to ensure that nodes such as PCs and routers can communicate over a constructed network. This phase includes integrating DHCP functionality to assign dynamic IP addresses and ensuring seamless communication between all network entities.

### **Implementation Details**

#### **Key Components**

1. **IP & IPHeader**
   - **Purpose**: Provides abstraction for handling IPv4 and IPv6 headers, encapsulating details like source and destination addresses.
   - **Challenges**:
     - Refactoring the existing `IP` implementation into `IP` and `IPHeader` components.
     - Managing template specialization for IPv4 and IPv6 headers.
   - **Solutions**:
     - Separated common functionality into an abstract base class `AbstractIPHeader`.
     - Implemented specialized `IPv4Header` and `IPv6Header` classes for handling respective protocols.
     - Integrated `QHostAddress` to validate and parse IP strings seamlessly.
   - **Code Highlights**:
     - `toBytes` and `fromBytes` methods serialize and deserialize headers for packet exchange.
     - Template-based IP handling ensures reusability for both IPv4 and IPv6.
   - **Fixes**:
     - Addressed template specialization bugs by carefully segregating shared functionality for IPv4 and IPv6.

2. **Port & PortBindingManager**
   - **Purpose**: Facilitates packet transmission and manages dynamic connections between network nodes.
   - **Challenges**:
     - Handling thread safety during packet exchange without introducing deadlocks.
     - Ensuring the `PortBindingManager` handles concurrent bindings efficiently.
   - **Solutions**:
     - Implemented `QMutex` in `Port` to ensure thread-safe operations for packet counters and connection states.
     - Used `Qt::QueuedConnection` in `PortBindingManager` to avoid direct cross-thread calls.
   - **Code Highlights**:
     - `Port::sendPacket` emits packets while incrementing counters atomically.
     - `PortBindingManager::bind` establishes bi-directional connections dynamically.
   - **Fixes**:
     - Resolved deadlock issues in DHCP by restructuring mutex usage and avoiding unnecessary locking during packet forwarding.

3. **Node, PC & Router**
   - **Purpose**: Represent core network entities with shared attributes and unique functionalities.
   - **Challenges**:
     - Implementing multi-threading for routers and PCs using `QThread` while maintaining consistent state.
     - Forwarding packets in routers with proper TTL management to avoid infinite loops.
   - **Solutions**:
     - Designed `Node` as a base class with shared properties like `id` and `ipAddress`.
     - `PC` and `Router` inherit from `Node`, implementing distinct features like packet generation and forwarding.
     - Ensured that routers track seen packets to prevent loops and validate TTL before forwarding.
   - **Code Highlights**:
     - `Router::forwardPacket` decrements TTL and retransmits packets on all connected ports.
     - `PC::generatePacket` simulates data generation and transmission through its port.
   - **Fixes**:
     - Used `moveToThread` during initialization to ensure proper thread handling for nodes.
     - Prevented packet duplication and looping by maintaining a set of processed packets.

4. **TopologyBuilder & TopologyController**
   - **Purpose**: Construct and manage the network topology dynamically based on JSON configurations.
   - **Challenges**:
     - Parsing complex JSON configurations to build custom topologies like Ring-Star.
     - Dynamically binding ports between routers and PCs while maintaining thread safety.
   - **Solutions**:
     - `TopologyBuilder` handles topology creation, node instantiation, and DHCP server configuration.
     - `TopologyController` validates the constructed topology and initiates phases like DHCP assignment.
   - **Code Highlights**:
     - `TopologyBuilder::setupTopology` creates Ring-Star and Mesh topologies dynamically, validating connections.
     - `TopologyController::initiateDHCPPhase` initiates IP assignment by coordinating DHCP servers and clients.
   - **Fixes**:
     - Resolved port availability issues during Ring-Star configuration by revalidating port connections after each bind.

5. **Autonomous System (AS)**
   - **Purpose**: Encapsulates routers and PCs within a single logical domain, supporting modular network construction.
   - **Challenges**:
     - Splitting logic between `TopologyBuilder` and `AutonomousSystem` for better modularity.
     - Establishing inter-AS connections while avoiding configuration conflicts.
   - **Solutions**:
     - `AutonomousSystem` integrates `TopologyBuilder` and `TopologyController` for topology construction and runtime management.
     - Connections between ASes are managed dynamically by `TopologyController::connectToOtherAS`.
   - **Code Highlights**:
     - Modular design allows independent initialization and runtime control of each AS.

---

### **Challenges and Fixes**

1. **Thread Management in Nodes**:
   - **Issue**: Incorrect thread assignment led to inconsistent behavior in `PC` and `Router` instances.
   - **Fix**: Used `moveToThread` during node initialization to ensure proper thread ownership.

2. **Port Binding Deadlocks**:
   - **Issue**: Mutex deadlocks occurred during packet forwarding with DHCP integration.
   - **Fix**: Reorganized locking mechanisms and minimized critical sections during packet operations.

3. **Custom Topology Parsing**:
   - **Issue**: Errors in Ring-Star configuration due to JSON parsing and incorrect port bindings.
   - **Fix**: Enhanced JSON validation and added debug logging to trace configuration issues.

---

### **Testing**
- **Tools**: Qt’s QTest framework and custom simulation scripts.
- **Tests**:
  - **IP & IPHeader**: Validated serialization/deserialization and IPv4/IPv6 compatibility.
  - **Port & PortBindingManager**: Verified dynamic port bindings and packet exchange using multi-threaded scenarios.
  - **Node, PC & Router**: Tested node initialization, packet generation, and forwarding with various TTL values.
  - **TopologyBuilder**: Checked topology creation against JSON configurations, focusing on custom topologies like Ring-Star.
  - **AutonomousSystem**: Simulated multi-AS connections and validated inter-AS communication.

---

### **State at the End of Phase 2**
- The network topology is fully constructed and validated.
- All nodes (PCs and routers) are initialized and connected.
- Preliminary DHCP functionality is implemented, enabling IP assignment.
- The system is ready to simulate basic packet exchanges and prepare for Phase 3.

## **Future Phases**

- **Phase 3**: Introduce routing algorithms (e.g., RIP, OSPF), simulate network traffic, and observe behavior under various scenarios.
- **Phase 4**: Perform a detailed analysis of the simulation results and generate comprehensive reports comparing algorithm performance.
