# Computer Networks Course Projects - Fall 2024

This repository contains the projects completed as part of the **Computer Networks** course at our university. Each project focuses on a different aspect of networking, ranging from **distributed voice call systems** using WebRTC to **network simulation** using GNS3 and **routing algorithm analysis**. Below, you'll find detailed descriptions of each project, along with their objectives, tools used, and results.

---

## Project Descriptions

1. [CA1: Distributed Voice Call Using WebRTC](#ca1-distributed-voice-call-using-webrtc)
2. [CA2: Introduction to GNS3](#ca2-introduction-to-gns3)
3. [CA3: Network Simulator for Routing Algorithms](#ca3-network-simulator-for-routing-algorithms)

---

### [CA1: Distributed Voice Call Using WebRTC](https://github.com/inaijin/ComputerNetworkProjects/tree/main/CA1_DistributedVoiceCallUsingWebRTC)

**Description**:  
This project involves the implementation of a **distributed live voice call system** using **WebRTC** and **Socket Programming**. The goal is to create a peer-to-peer communication system that allows two devices to exchange audio data in real-time without relying on a central server. The project focuses on understanding the fundamentals of **real-time communication protocols**, **data encoding/decoding**, and **distributed systems**.

**Key Features**:
- Implementation of a **peer-to-peer voice call system** using WebRTC.
- Use of **Opus codec** for audio compression and transmission.
- Integration of **libdatachannel** for WebRTC communication.
- Optional use of **Coturn** (TURN server) for NAT traversal.
- Development of a simple **Qt-based GUI** for user interaction.

**Tools Used**:
- **C++** for the core implementation.
- **Qt Framework** for the graphical user interface.
- **WebRTC** (via libdatachannel) for peer-to-peer communication.
- **Opus codec** for audio encoding/decoding.
- **Coturn** (optional) for handling NAT traversal.

**Results**:  
The project successfully demonstrates real-time voice communication between two devices. The system is tested for latency, audio quality, and reliability under different network conditions. Screenshots of the GUI and network traces are provided to illustrate the system's functionality.

---

### [CA2: Introduction to GNS3](https://github.com/inaijin/ComputerNetworkProjects/tree/main/CA2_IntroductionToGNS3)

**Description**:  
This project introduces **GNS3**, a network simulation tool used to emulate real-world network devices such as routers and switches. The goal is to gain hands-on experience with **network topology design**, **device configuration**, and **basic networking concepts** such as **VLANs**, **static routing**, and **dynamic routing** using **OSPF**.

**Key Features**:
- Setup and configuration of **GNS3** environment.
- Creation of network topologies with multiple routers and switches.
- Implementation of **VLANs** to segment network traffic.
- Configuration of **static routing** and **dynamic routing** (OSPF).
- Use of **Wireshark** for packet capture and analysis.

**Tools Used**:
- **GNS3** for network simulation.
- **Wireshark** for packet analysis.
- **Cisco IOS images** for router emulation.

**Results**:  
The project includes detailed documentation of the network topologies created in GNS3, along with screenshots of device configurations and routing tables. Packet captures from Wireshark are analyzed to demonstrate the flow of traffic between devices.

---

### [CA3: Network Simulator for Routing Algorithms](https://github.com/inaijin/ComputerNetworkProjects/tree/main/CA3_NetworkSimulatorForRoutingAlgorithms)

**Description**:  
This project involves the development of a **network simulator** to analyze and compare different **routing algorithms**, such as **RIP** and **OSPF**. The simulator is designed to emulate a network environment where routers dynamically exchange routing information and adapt to changes in network topology. The project also explores the impact of **network load**, **link failures**, and **traffic distribution** on routing performance.

**Key Features**:
- Implementation of **RIP** and **OSPF** routing protocols.
- Simulation of **network topologies** (e.g., mesh, ring-star).
- Use of **statistical distributions** (Poisson, Pareto) to model network traffic.
- Analysis of routing performance metrics such as **packet loss**, **hop count**, and **latency**.
- Optional implementation of **BGP** for inter-domain routing.

**Tools Used**:
- **C++** for the core implementation.
- **Qt Framework** for event-driven simulation.
- **Statistical distributions** (Poisson, Pareto) for traffic modeling.

**Results**:  
The simulator provides detailed logs and visualizations of routing paths, packet flows, and performance metrics. Comparative analysis of RIP and OSPF is conducted, highlighting the strengths and weaknesses of each protocol under different network conditions.

---

## How to Use This Repository

Each project is contained in its own directory, with the following structure:
- **CA1_DistributedVoiceCallUsingWebRTC**: Contains the source code, documentation, and results for the WebRTC-based voice call system.
- **CA2_IntroductionToGNS3**: Includes GNS3 configuration files, topology diagrams, and Wireshark captures.
- **CA3_NetworkSimulatorForRoutingAlgorithms**: Contains the network simulator source code, routing algorithm implementations, and performance analysis.

To explore a specific project, navigate to its directory and follow the instructions in the respective `README.md` file.

---

## Contributing

Contributions to this repository are welcome! If you have suggestions, improvements, or additional features to add, feel free to open an issue or submit a pull request. Please ensure that your contributions align with the project's goals and include appropriate documentation.

---

## Contact

For any questions or feedback, feel free to reach out to the contributors via GitHub or email.
