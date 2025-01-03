# Qt Network Simulator

**A modular, extensible network simulation framework built with Qt 6 (tested on Windows).**  
This project provides an environment to model **Autonomous Systems (AS)**, **routers**, **PCs**, and **routing protocols** (RIP, OSPF, BGP), along with **DHCP** for IP assignment. It also demonstrates **packet queuing**, allowing you to observe basic congestion behavior.

## Table of Contents
1. [Overview](#overview)  
2. [Features](#features)  
3. [Getting Started](#getting-started)  
   - [Prerequisites](#prerequisites)  
   - [Building the Project](#building-the-project)  
   - [Running the Simulator](#running-the-simulator)  
4. [Project Structure](#project-structure)  
5. [How It Works (High-Level)](#how-it-works-high-level)  
6. [Router Internals & Protocols](#router-internals--protocols)  
   - [Queue & Congestion Control](#queue--congestion-control)  
   - [Routing Table & Route Entries](#routing-table--route-entries)  
   - [RIP Implementation](#rip-implementation)  
   - [OSPF Implementation](#ospf-implementation)  
   - [BGP Implementation](#bgp-implementation)  
   - [DHCP Server/Client Logic](#dhcp-serverclient-logic)  
7. [Supporting Classes](#supporting-classes)  
   - [Packet (`Packet.h/.cpp`)](#packet-packethcpp)  
   - [Port (`Port.h/.cpp`)](#port-porthcpp)  
   - [PortBindingManager (`PortBindingManager.h/.cpp`)](#portbindingmanager-portbindingmanagerhcpp)  
   - [TopologyBuilder (`TopologyBuilder.h/.cpp`)](#topologybuilder-topologybuilderhcpp)  
   - [TopologyController (If Present)](#topologycontroller-if-present) 

---

## Overview

This simulator models:
- **Autonomous Systems (AS)**, each containing multiple **routers** (and optionally PCs).
- **Routing protocols** within and between ASes:
  - **RIP** or **OSPF** for intra-AS routing.
  - **BGP** (EBGP/IBGP) for inter-AS routing.
- **DHCP** for dynamic IP assignment (to routers and PCs).
- **Queue-based congestion control**: each router has a buffer where packets can be dropped if the buffer is full.
- **Metrics** to track packet drops, route convergence, packet hops, waiting cycles, etc.

**Goal**: Provide a flexible environment for learning, testing, and prototyping network protocols and topologies.

---

## Features

1. **DHCP Phase** for assigning IP addresses to routers and PCs.  
2. **Routing Protocols**:  
   - **RIP** (distance-vector, hop count metric)  
   - **OSPF** (link-state, LSAs, Dijkstra)  
   - **BGP** (optional for inter-AS routing)  
3. **Congestion Control** via **fixed-size router queues** (packets dropped on overflow).  
4. **Topology**: mesh, ring-star, or **torus** (wraparound).  
5. **Command-Line Integration** to specify protocol choices (enable BGP, choose RIP or OSPF, enable torus, etc.).  
6. **Visualization**: ASCII diagrams of the topology, logs of routing tables, and DHCP assignments.  
7. **Metrics Collection**: Tracks packet drops, wait cycles, route usage, etc.

---

## Getting Started

### Prerequisites

- **Qt 6** (or higher)  
- A **C++17**-capable compiler (MSVC, Clang, or GCC)  
- **CMake** or **qmake** (depending on your Qt project setup)  

*(Tested primarily on Windows with Qt 6.)*

### Building the Project

1. **Clone** the repository:
   ```bash
   git clone https://github.com/YourUsername/QtNetworkSimulator.git
   cd QtNetworkSimulator
   ```

2. **Configure & Build** (example with CMake):
   ```bash
   mkdir build && cd build
   cmake ..
   cmake --build .
   ```
   Or open in **Qt Creator** (using the `.pro` or CMake files) and build directly.

### Running the Simulator

After a successful build, an executable (e.g., `QtNetworkSimulator.exe`) is created. Run it via:

```bash
./QtNetworkSimulator.exe --bgp yes --first-as-algo 1 --second-as-algo 2 --torus yes
```
- This example **enables BGP**, sets **AS1** to use **RIP** (`1`), **AS2** to use **OSPF** (`2`), and **enables a torus** topology.  
- If no arguments are provided, the simulator **prompts** interactively for protocol settings.

*(Or if in QT environment it's already been adjusted in the run configs)*

---

## Project Structure

```
QtNetworkSimulator/
├── src/
│   ├── NetworkSimulator/
│   │   ├── Simulator.h/.cpp
│   │   ├── ApplicationContext.h/.cpp
│   │   ├── Network.h/.cpp
│   │   ├── Router.h/.cpp        <-- Router logic, routing protocols, queue
│   │   ├── ...
│   ├── MetricsCollector/
│   ├── Globals/
│   ├── Packet/
│   │   ├── Packet.h/.cpp        <-- Packet structure & headers
│   ├── Port/
│   │   ├── Port.h/.cpp          <-- Port connections for router/PC
│   ├── PortBindingManager/
│   │   ├── PortBindingManager.h/.cpp
│   ├── Network/
│   │   ├── PC.h/.cpp            <-- PC logic (sending/receiving data)
│   ├── Topology/
│   │   ├── TopologyBuilder.h/.cpp  <-- Building network topologies
│   │   ├── TopologyController.h/.cpp (if present)
│   ├── main.cpp                 <-- Entry point
├── configs/
│   └── mainConfig/
│       └── config.json          <-- Default config
└── README.md
```

**Key Components**:
- **`main.cpp`**: Starts `QCoreApplication`, loads `Simulator`, and runs the event loop.  
- **`Simulator.*`**: Central logic to build the network, enable protocols, and coordinate the run.  
- **`Network.*`**: Manages creation of Autonomous Systems, their routers, and optional torus.  
- **`Router.*`**: The heart of routing, including **RIP**, **OSPF**, **BGP** handlers, **DHCP** phases, and **queue-based congestion**.  
- **`MetricsCollector.*`**: Records simulation data (packet drops, wait cycles, route changes, etc.).

---

## How It Works (High-Level)

1. **Initialization**  
   - **Config**: The simulator reads `config.json` or uses command-line overrides.  
   - **Network**: The simulator spawns each AS with the specified number of routers and PCs. Optional **torus** links are created if requested.

2. **DHCP Assignment**  
   - First, **routers** request IPs from a DHCP server router.  
   - Then, **PCs** also send DHCP requests.  
   - Once assigned, each node logs its IP and configures local routes.

3. **Routing Protocols**  
   - Within each AS, you can choose **RIP** or **OSPF**.  
   - If BGP is **enabled**, border routers in different ASes exchange **EBGP** messages, and inside each AS, internal routers use **IBGP** updates to learn external routes.

4. **Simulation Loop**  
   - **EventsCoordinator** or a QTimer triggers “ticks” for RIP updates, OSPF Hello/LSA, BGP route advertisements.  
   - Routers update their routing tables until the network converges.

5. **Data Generation**  
   - PCs then generate data packets. Routers forward them according to the best route.  
   - If a router’s queue is full, the packet is dropped (congestion). If TTL is 0, the packet is also dropped.

6. **Visualization & Metrics**  
   - The simulator can print an ASCII diagram of the network.  
   - Each router logs its **routing table**.  
   - **Metrics** record how many packets were sent, dropped, or successfully received, along with waiting cycles, total hops, etc.

---

## Router Internals & Protocols

The **`Router`** class is where most network behaviors are defined:
- **`processPacket()`**: Receives an incoming packet and decides to enqueue it, handle it (RIP, OSPF, DHCP, etc.), or forward it.
- **Timers & Handlers** for **RIP** (distance vector), **OSPF** (link state), and **BGP** messages.

### Queue & Congestion Control

Each router has a **buffer** (`m_buffer`) for incoming packets:
```cpp
QQueue<BufferedPacket> m_buffer;
int m_bufferSize;  // Maximum size
QTimer *m_bufferTimer;
```
- **`enqueuePacketToBuffer(...)`** checks if the buffer is full:
  ```cpp
  bool Router::enqueuePacketToBuffer(const PacketPtr_t &packet) {
      QMutexLocker locker(&m_bufferMutex);
      if (m_buffer.size() >= m_bufferSize) {
          // Congestion => drop packet
          if (m_metricsCollector) m_metricsCollector->recordPacketDropped();
          return false;
      }
      // Otherwise enqueue
      BufferedPacket bp;
      bp.packet = packet;
      bp.enqueueTime = QDateTime::currentMSecsSinceEpoch();
      m_buffer.enqueue(bp);
      return true;
  }
  ```
- **`processBuffer()`** (called periodically by `m_bufferTimer`):
  - Removes packets that have spent too long in the queue (`m_bufferRetentionTime`).  
  - If a packet is too old, it’s dropped (timed out in queue).

### Routing Table & Route Entries

Each router stores routes in a **routing table** (`m_routingTable`), a list of `RouteEntry` structs:
```cpp
struct RouteEntry {
    QString destination;
    QString mask;
    QString nextHop;
    int metric;
    RoutingProtocol protocol;
    qint64 lastUpdateTime;
    PortPtr_t learnedFromPort;

    bool isDirect;
    bool vip;
    int invalidTimer;
    int holdDownTimer;
    int flushTimer;
    // ...
};
```
- **`destination`** & **`mask`**: The IP address/network the route covers.  
- **`nextHop`**: Where to forward packets for this destination. If `nextHop == destination`, it may be a local PC or direct interface.  
- **`metric`**: RIP or OSPF cost, or BGP metric.  
- **`isDirect`**: Indicates a directly connected interface (e.g., router’s own IP or a PC behind it).  
- **Timers** (`invalidTimer`, `holdDownTimer`, etc.) used by RIP to age out stale routes.

**Finding the Best Route** (`findBestRoutePath(destinationIP)`):
1. **Iterate** over the table, matching the `destinationIP` (or relevant network prefix).  
2. **Select** the route with the **lowest metric**.  
3. If multiple matches exist, the route with the best (lowest) metric is chosen.  
4. If **no** route is found, the packet is dropped.  

When forwarding data packets:
- If `nextHop` matches the router’s own IP or a directly connected IP, the router delivers the packet to the local PC (or to itself).  
- Otherwise, it decrements **TTL** and sends the packet out through the port that leads to `nextHop`.

### RIP Implementation

- **Distance-vector** approach:
  - **`sendRIPUpdate()`** periodically broadcasts the router’s routing table.  
  - **`processRIPUpdate()`** handles incoming RIP packets, updating metrics if the new route is better.  
  - **Timers**:  
    - `RIP_UPDATE_INTERVAL`: Send updates every few seconds.  
    - `RIP_INVALID_TIMER`, `RIP_HOLDOWN_TIMER`, `RIP_FLUSH_TIMER` handle stale routes.  
  - **RIP_INFINITY = 16**: Metric 16 means “unreachable.”

### OSPF Implementation

- **Link-state** approach:
  - **Hello Packets** (`sendOSPFHello()`) to discover neighbors.  
  - **LSAs** (`sendLSA()`) flooded to exchange topology info.  
  - **Dijkstra** (`runDijkstra()`) run locally to compute shortest paths from the Link-State Database (`m_lsdb`).  

### BGP Implementation

- **Border Gateway Protocol** for inter-AS routing:
  - **EBGP**: Exchanged between routers in **different** ASes.  
  - **IBGP**: Shared within a single AS so internal routers learn routes from the border router.  
- **Border Routers**: Identified if a router has a port connected to an ID outside its AS range.  
- **`processEBGPUpdate()`** and **`processIBGPUpdate()`** parse BGP route advertisements and update the routing table accordingly.

### DHCP Server/Client Logic

- **DHCP Requests** (`DHCP_REQUEST`) and **Offers** (`DHCP_OFFER`):
  - Some routers are configured as **DHCP servers**.  
  - Routers or PCs that need an IP send a DHCP request.  
  - The DHCP server replies with an offer containing an available IP.  
- **`processDHCPResponse()`** assigns the IP to the router, which sets a direct route for that IP in its routing table.

---

## Supporting Classes

### Packet (`Packet.h/.cpp`)

Represents **packets** traveling through the network:
- **`PacketType`** enum: Data, Control, RIPUpdate, OSPFHello, OSPFLSA, DHCPRequest, DHCPOffer, etc.  
- **`m_payload`**: The main string-based content (e.g., “RIP_UPDATE:192.168.1.0,255.255.255.0,1#...”).  
- **`m_ttl`**: Decremented each hop. If it hits 0, the router drops the packet.  
- **`m_path`**: A record of which routers the packet visited.  
- **`m_waitingCycle`** & **`m_totalCycle`** track waiting times and total journey length for metrics.

Example snippet:
```cpp
void Packet::decrementTTL() {
    m_ttl--;
    if (m_ttl < 0) {
        // Drop condition handled by router
    }
}
```

### Port (`Port.h/.cpp`)

- Each **router** or **PC** has one or more **ports** to connect them to other nodes.  
- A `Port` can be **bound** to another `Port` so that a call to `sendPacket()` on one triggers `receivePacket()` on the other (via Qt signals/slots).  
- Tracks sent/received packet counts, plus connected device info.

```cpp
void Port::sendPacket(const PacketPtr_t &data) {
    QMutexLocker locker(&m_mutex);
    ++m_numberOfPacketsSent;
    emit packetSent(data);
}
```

### PortBindingManager (`PortBindingManager.h/.cpp`)

- Handles the **binding** of two ports:
  ```cpp
  void PortBindingManager::bind(
      const QSharedPointer<Port> &port1,
      const QSharedPointer<Port> &port2,
      int router1Id, int router2Id)
  {
      // Connect port1->packetSent to port2->receivePacket, and vice versa
      // Mark them as connected, store them in m_bindings, etc.
  }
  ```
- If a port is connected to a PC, it sets up direct PC-to-router links.  

### TopologyBuilder (`TopologyBuilder.h/.cpp`)

- Creates **routers** and **PCs** based on the config.  
- Connects them in a specified **topology** (mesh, ring-star, or **torus**).  
- **`makeMeshTorus()`** specifically handles wraparound connections in a 4×4 grid.  
- Assigns DHCP servers if specified.

```cpp
void TopologyBuilder::buildTopology(bool torus) {
    createRouters();
    setupTopology();
    if (torus) makeMeshTorus();
    createPCs();
    configureDHCPServers();
}
```

### TopologyController (If Present)

- May handle advanced tasks like coordinating **DHCP** phases or bridging multiple ASes.  
- Not always required—some logic might be integrated directly into `TopologyBuilder` or `Network`.

---

## Final Notes

- **Router** is the project’s core, managing **routing protocols**, **DHCP**, and **queue-based congestion**.  
- **Routing Table** entries are updated via RIP, OSPF, or BGP, then used by `findBestRoutePath()` to forward data packets.  
- **Packet** objects carry data or protocol messages. **Ports** connect routers/PCs, while **PortBindingManager** links them.  
- **TopologyBuilder** orchestrates the creation of routers/PCs and their interconnections.  
- **Metrics** track packet drops, wait cycles, route usage, etc., providing insights into simulation performance.
