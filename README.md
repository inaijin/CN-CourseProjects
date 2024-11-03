# CN-CourseProjects

**Group 3:** Kourosh Sajjadi / Ali Samanian

This repository contains projects developed for our Computer Networks course. Each project, referred to as a "Course Assignment" (CA), explores a different aspect of network programming and distributed systems. Below is a list of projects and their descriptions.

## Projects

### CA1: Socket Programming DistributedVoiceCall

The CA1 project focuses on implementing a distributed voice call system using WebRTC. This application facilitates real-time audio communication across devices on a local networks by leveraging WebRTC for peer-to-peer connectivity.

**Core Technologies:**
- **WebRTC:** Enables peer-to-peer communication with audio streaming.
- **Signaling Server:** Establishes initial peer connection parameters (SDP) and candidate exchange.
- **Opus Codec:** Encoding/ our audio packets using the opec codec.

**Features:**
- **Role-based Setup:** Users can select "caller" or "callee" roles.
- **Audio Streaming:** Real-time audio exchange between peers.
- **Candidate Management:** Handles network candidates for reliable connectivity.
- **Status Updates:** Displays call status updates to the user.

**Challenges and Solutions:**
We addressed various challenges in SDP management, candidate handling, and audio setup, implementing robust solutions that ensure smooth audio streaming and NAT traversal for seamless communication.

For more detailed information, please refer to the [CA1 project directory](https://github.com/inaijin/CN-CourseProjects/tree/main/CA1-SocketProgramming/DistributedVoiceCall) and its README.

---

Further projects and assignments will be added to this repository as we progress through the course.
