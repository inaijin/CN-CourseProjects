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

## **Phase 3: Routing Algorithm Implementation and Simulation**

### **Objective**
In Phase 3, we focused on implementing and integrating dynamic routing algorithms within the network simulator. The primary goals of this phase were to:
- **Implement Routing Protocols**: Integrate both **Routing Information Protocol (RIP)** and **Open Shortest Path First (OSPF)** within the router components.
- **Simulate Network Traffic**: Enable routers to exchange routing information and handle data packet transmissions.
- **Analyze Routing Behavior**: Observe and evaluate the performance and behavior of RIP and OSPF under various network conditions, including link failures and congestion.
- **Enhance Metrics Collection**: Extend the metrics collection capabilities to capture routing-specific data such as hop counts and routing table updates.

### **Implementation Details**

#### **1. Routing Protocols Integration**

##### **a. Routing Information Protocol (RIP)**
- **Purpose**: Implemented RIP as a distance-vector routing protocol to enable routers to exchange routing information periodically.
- **Key Features**:
  - **Periodic Updates**: Routers send RIP updates at regular intervals (`RIP_UPDATE_INTERVAL`).
  - **Route Management**: Handles route additions, updates, and invalidations based on received RIP updates.
  - **Loop Prevention**: Utilized mechanisms like split horizon and hold-down timers to prevent routing loops.

##### **b. Open Shortest Path First (OSPF)**
- **Purpose**: Integrated OSPF as a link-state routing protocol for more efficient and scalable route management.
- **Key Features**:
  - **Neighbor Discovery**: Implemented OSPF Hello packets to discover and maintain neighbor relationships.
  - **Link-State Advertisements (LSAs)**: Routers generate and flood LSAs to share network topology information.
  - **Shortest Path Calculation**: Utilized Dijkstra’s algorithm to compute the shortest path tree based on the LSDB (Link-State Database).

##### **c. Border Gateway Protocol (BGP)**
- **Purpose**: Incorporated both **External BGP (EBGP)** and **Internal BGP (IBGP)** to facilitate inter-AS and intra-AS routing.
- **Key Features**:
  - **Route Advertisement**: Enabled border routers to advertise and exchange routes with other ASes.
  - **Metric Handling**: Managed route metrics to determine the best paths for data transmission.

#### **2. Packet Processing Enhancements**
- **Dynamic Packet Handling**: Enhanced the `Router` class to process different types of packets, including RIP updates, OSPF Hello and LSA packets, and BGP updates.
- **Buffer Management**: Improved packet buffering mechanisms to handle incoming packets efficiently and prevent buffer overflows.

#### **3. Metrics Collection Extension**
- **Routing Metrics**: Extended the `MetricsCollector` to capture routing-specific metrics such as hop counts, route convergence times, and packet drop rates due to routing issues.
- **Performance Tracking**: Implemented detailed logging within routing functions to facilitate comprehensive performance analysis.

### **Challenges and Fixes**

1. **Template Specialization in IP Class**:
   - **Issue**: Initial template-based design for handling IPv4 and IPv6 caused bugs due to shared constructors and methods.
   - **Fix**: Explicitly specialized methods for IPv4 and IPv6, maintaining a common base class to ensure proper functionality and separation of concerns.

2. **Routing Loop Prevention**:
   - **Issue**: Potential for routing loops in RIP updates and BGP advertisements, leading to inefficient routing and packet loss.
   - **Fix**: Implemented split horizon and hold-down timers in RIP, and tracked seen packets in BGP to prevent loops. Additionally, maintained a set of seen packets to avoid processing duplicate routing information.

3. **Thread Management and Concurrency**:
   - **Issue**: Managing multi-threaded operations in routers led to inconsistent states and potential deadlocks, especially during simultaneous packet processing and routing updates.
   - **Fix**: Utilized `QMutex` and `QMutexLocker` to ensure thread-safe operations during packet processing and routing table updates. Refactored locking mechanisms to minimize critical sections and prevent deadlocks, particularly during DHCP integration and routing table modifications.

4. **Dijkstra’s Algorithm Implementation**:
   - **Issue**: Efficiently implementing Dijkstra’s algorithm within the router’s routing logic posed challenges in managing dynamic network topologies.
   - **Fix**: Carefully managed data structures for distance and previous node mappings, ensuring accurate and efficient computation of the shortest path tree. Optimized the algorithm to handle dynamic updates from LSAs without significant performance overhead.

5. **Dynamic Topology Changes**:
   - **Issue**: Handling dynamic changes in network topology, such as link failures or router failures, required the routing protocols to adapt in real-time.
   - **Fix**: Enhanced event handling within `EventsCoordinator` to manage dynamic events, ensuring routers can respond to topology changes by updating routing tables accordingly. Implemented mechanisms to detect and recover from link failures, triggering appropriate routing updates.

6. **BGP Integration Complexity**:
   - **Issue**: Integrating both EBGP and IBGP introduced complexity in managing inter-AS and intra-AS routing policies.
   - **Fix**: Structured the BGP implementation to differentiate between external and internal routing updates, applying appropriate metrics and policies based on the type of BGP session. Ensured clear separation of BGP logic from RIP and OSPF to maintain modularity and ease of maintenance.

### **Testing**

- **Tools**: Utilized Qt’s QTest framework alongside custom simulation scripts to validate the functionality and performance of routing algorithms.
- **Tests Conducted**:
  - **RIP Functionality**:
    - Verified periodic RIP updates and routing table convergence across multiple routers.
    - Tested route invalidation and hold-down timers upon simulated link failures.
    - Ensured loop prevention mechanisms effectively avoided routing loops.
  - **OSPF Functionality**:
    - Ensured proper neighbor discovery and maintenance through OSPF Hello packets.
    - Validated LSA generation, dissemination, and LSDB updates across the network.
    - Confirmed accurate routing table updates via Dijkstra’s algorithm based on the LSDB.
  - **BGP Functionality**:
    - Tested EBGP and IBGP route exchanges between border and internal routers.
    - Verified correct route advertisement, metric handling, and route selection policies.
    - Simulated inter-AS and intra-AS route propagation to ensure proper BGP behavior.
  - **Metrics Collection**:
    - Validated accurate recording of routing-specific metrics such as hop counts, route convergence times, and packet drop rates.
    - Analyzed metrics under various network conditions to assess routing protocol performance.
  - **Concurrency and Thread Safety**:
    - Simulated multi-threaded packet processing scenarios to ensure thread-safe operations.
    - Conducted stress tests to verify the absence of deadlocks and race conditions during high traffic loads and dynamic topology changes.

### **State at the End of Phase 3**
- **Fully Implemented Routing Protocols**: Both RIP and OSPF are fully integrated and operational within the router components, enabling dynamic route discovery and management.
- **BGP Functionality**: External and Internal BGP (EBGP and IBGP) are implemented, facilitating inter-AS and intra-AS routing.
- **Dynamic Routing Tables**: Routers maintain dynamic routing tables that update in real-time based on RIP, OSPF, and BGP protocols, ensuring optimal path selection and network resilience.
- **Enhanced Metrics Collection**: The system effectively captures detailed routing metrics, providing valuable insights into protocol performance and network behavior.
- **Ready for Traffic Simulation and Analysis**: The network simulator is now capable of simulating complex routing scenarios, handling dynamic topology changes, and providing comprehensive analysis of routing algorithm performance.
- **Preparation for Phase 4**: With routing algorithms implemented and metrics collection enhanced, the system is well-prepared to perform detailed analysis and reporting in Phase 4, comparing the performance of RIP and OSPF under various network conditions.

---

## **Future Phases**

- **Phase 4 (Analysis and Reporting)**:
  - **Objective**: Conduct a comprehensive analysis of the simulation results, focusing on metrics such as packet loss, hop count, route convergence times, and waiting cycles.
  - **Tasks**:
    - Compare the performance of RIP and OSPF under different network topologies and conditions.
    - Generate detailed reports and visualizations to illustrate the strengths and weaknesses of each routing protocol.
    - Provide recommendations for optimizing routing strategies based on the observed data.

We have successfully completed Phase 3, laying a solid foundation for in-depth analysis and performance evaluation of routing algorithms in Phase 4.
