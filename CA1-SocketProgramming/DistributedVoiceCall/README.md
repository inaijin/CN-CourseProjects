# WebRTC-based DistributedVoiceCall (Qt)

## Overview
This project establishes distribute audio communication using WebRTC within a Qt framework. Key tasks include setting up and managing peer connections, configuring audio tracks, handling signaling (SDP), and ICE candidates, encoding using opus codec and etc ...

### Features
- **Peer Connection Management**: Tracks peer IDs and manages connection states.
- **Signaling (SDP Generation and Management)**: Creates, sends, and manages SDP offers/answers.
- **ICE Candidate Handling**: Adds remote ICE candidates to ensure connection stability.
- **Audio Streaming**: Sets up an Opus-encoded audio track for real-time communication.
- **Audio Encoding**: Encoding and decoding using opus codec.

---

## Code Documentation

### `WebRTC::init`
**Purpose**: Initializes the WebRTC component, configuring ICE settings and preparing for a new session.

**Challenge**: **Clear old peer connections** to avoid interference with new ones. 
- **Solution**: `m_peerConnections.clear()` ensures any stale connections are removed before new ones are added.

### `addPeer`
**Purpose**: Adds a new peer connection and establishes callbacks for SDP and state changes.

**Key Challenges**:
1. **Pending Candidates**: When a peer connection isn't fully established, incoming ICE candidates cannot be immediately added.
   - **Solution**: Store candidates in `m_pendingCandidates` and add them once the peer connection is ready.
2. **Callback Management**: Setting up reliable callbacks for SDP descriptions and state changes.
   - **Solution**: `onLocalDescription`, `onLocalCandidate`, and `onStateChange` manage these callbacks. The `Q_EMIT` macro signals state changes to Qt, allowing the application to handle connection status dynamically.

### `setupAudioTrack`
**Purpose**: Configures the audio track with Opus codec, bitrate, and SSRC to facilitate real-time audio communication.

**Challenges and Solutions**:
1. **Opus Codec Compatibility**: The Opus codec is preferred for real-time communication due to low latency, but requires specific configuration.
   - **Solution**: Added `media.addOpusCodec(111);`, specifying payload type 111 for Opus, which is essential for cross-platform compatibility.
2. **Handling Incoming RTP Packets**: Incoming audio data must be processed as RTP packets to maintain audio integrity.
   - **Solution**: Set up `onMessage` callback to capture incoming data and verify packet size, type, and payload.

### `generateOfferSDP` and `generateAnswerSDP`
**Purpose**: Generates SDP offers and answers for session initiation.

**Challenges**:
- **Synchronization**: If peers generate SDP simultaneously, mismatches may occur.
   - **Solution**: Ensure each peer waits for the `setLocalDescription` call, which triggers SDP callbacks to handle responses in sequence.

### `sendAudioFrame`
**Purpose**: Sends audio frames over the RTP channel for active peer connections.

**Challenges**:
1. **RTP Header Management**: To ensure seamless streaming, the RTP header must be updated for each frame.
   - **Solution**: Added `rtc::RtpHeader` with parameters like sequence number and timestamp, managing them with `QMap` to track each peer individually.
2. **Frame Duration**: Audio frames must align with Opus encoding standards (20 ms frame duration).
   - **Solution**: Calculate `samplesPerFrame` based on 48 kHz sample rate, with frame duration updating the timestamp.

### `setRemoteDescription` and `setRemoteCandidate`
**Purpose**: Sets the remote SDP description and adds ICE candidates.

**Challenges**:
1. **SDP Format Validation**: Some invalid SDP formats can interrupt the connection.
   - **Solution**: Convert JSON-based SDP to a base64 format, verifying fields like `type` and `sdp` before setting them.
2. **Pending Candidates**: Candidates arriving before peer connections are ready are queued.
   - **Solution**: `m_pendingCandidates` stores these candidates, and they're processed when connections are available.

### Audio Input / Output Management

The `AudioInput` and `AudioOutput` classes manage audio input and output for real-time streaming. Below is a breakdown of each key method.

---

### **AudioInput**

#### Constructor (`AudioInput::AudioInput`)
**Purpose**: Initializes the audio input, sets the audio format (48 kHz, mono, 16-bit), and creates an Opus encoder for efficient audio compression.

- **Challenge**: Configuring the Opus encoder with the correct sample rate and channels.
  - **Solution**: `initializeOpusEncoder()` sets up the encoder, handling errors during initialization and calculating the frame size (20 ms duration).

#### Destructor (`AudioInput::~AudioInput`)
- **Purpose**: Releases resources, stops the audio source, and destroys the Opus encoder.

#### `initializeOpusEncoder`
- **Purpose**: Sets up the Opus encoder for voice-over-IP (VoIP) with a frame size of 20 ms.
- **Challenge**: **Error Handling** for encoder initialization.
  - **Solution**: If encoder creation fails, outputs a debug message with the error and skips further setup.

#### `start`
- **Purpose**: Starts audio capture by creating a `QAudioSource` and opening an input device.
- **Challenge**: **Audio Device Readiness** — Ensure the input device is ready before capturing audio.
  - **Solution**: Connects to `QIODevice::readyRead` for buffer processing and opens the device in write-only mode.

#### `stop`
- **Purpose**: Stops audio capture, resets the input device and audio source, and closes the device.

#### `processAudioBuffer`
- **Purpose**: Reads audio data from the input device and writes it to the buffer.
- **Challenge**: **Data Availability** — Verifies data isn't empty before writing.

#### `writeData`
- **Purpose**: Encodes audio data using the Opus encoder and emits the encoded data for streaming.
- **Challenge**: **Frame Encoding** — Properly segment the audio buffer to align with Opus frame requirements.
  - **Solution**: Manages an internal buffer, splits data into frames, and encodes them with `opus_encode`. Outputs any errors encountered and logs encoding success.

---

### **AudioOutput**

#### Constructor (`AudioOutput::AudioOutput`)
- **Purpose**: Configures the audio output, sets up the audio format (48 kHz, mono, 16-bit), and initializes an Opus decoder for incoming audio.
- **Challenge**: **Decoder Initialization** — Ensure the Opus decoder is ready before playback.
  - **Solution**: Initializes with `opus_decoder_create` and checks for errors. Configures the `QAudioSink` for output and starts the output device.

#### Destructor (`AudioOutput::~AudioOutput`)
- **Purpose**: Releases resources by stopping the audio sink and destroying the Opus decoder.

#### `addData`
- **Purpose**: Adds incoming encoded audio data to a queue and immediately calls `play` to process it.
- **Challenge**: **Thread Safety** — Ensures safe access to `dataQueue`.
  - **Solution**: Uses `QMutexLocker` to synchronize data access.

#### `play`
- **Purpose**: Dequeues encoded audio data, decodes it using Opus, and writes the PCM data to the output device.
- **Challenge**: **Opus Decoding** — Decodes audio accurately and handles potential errors.
  - **Solution**: Converts the data into PCM format, adjusts for byte count, and verifies that all data is successfully written to the audio device. Logs issues if some bytes aren’t written to the output.

--- 

### Signaling Server / Client

The `SignalingServer` and `SignalingClient` classes manage signaling communication between peers, handling connection setup, message parsing, and the exchange of Session Description Protocol (SDP) and ICE candidates necessary for WebRTC. Below is a breakdown of each method.

---

### **SignalingServer**

#### Constructor (`SignalingServer::SignalingServer`)
- **Purpose**: Initializes the TCP server for handling client connections.
- **Challenge**: Ensure `tcpServer` properly connects to `newConnection`.
  - **Solution**: Connects `newConnection` to `onNewConnection` to handle new clients.

#### `startListening`
- **Purpose**: Starts the server on a specified port.
- **Challenge**: **Error Handling** for potential failure to bind.
  - **Solution**: Logs an error if the server fails to start.

#### `onNewConnection`
- **Purpose**: Handles new client connections and prepares the client socket.
- **Challenge**: Multiple connections handling.
  - **Solution**: Sets up signal connections (`readyRead`, `disconnected`, `errorOccurred`) for the new client.

#### `onReadyRead`
- **Purpose**: Processes incoming data from the client, parsing messages as JSON.
- **Challenge**: **Data Parsing and Validation** — Ensures valid JSON and known message types.
  - **Solution**: Parses messages line by line, checks for JSON validity, and handles "offer" and "candidate" types with corresponding signals.

#### `onErrorOccurred`
- **Purpose**: Logs errors occurring on the client socket connection.

#### `onClientDisconnected`
- **Purpose**: Cleans up the socket when a client disconnects, releasing memory.

#### `sendAnswer`
- **Purpose**: Sends an SDP answer message to the client.
- **Challenge**: **Connection Availability** — Ensures a connected client exists.
  - **Solution**: Checks for `clientSocket` and logs if no client is connected.

#### `sendCandidate`
- **Purpose**: Sends an ICE candidate message to the client.
- **Challenge**: **Connection Availability** — Similar to `sendAnswer`.
  - **Solution**: Checks for `clientSocket` and logs if no client is connected.

---

### **SignalingClient**

#### Constructor (`SignalingClient::SignalingClient`)
- **Purpose**: Initializes the client socket for server communication.

#### `connectToHost`
- **Purpose**: Connects to the signaling server on a specified host and port.
- **Challenge**: **Connection Status Check** — Verifies connection success.
  - **Solution**: Logs a warning if the socket fails to initiate the connection.

#### `onConnected`
- **Purpose**: Confirms successful connection to the server and emits a `connectedToServer` signal.

#### `onReadyRead`
- **Purpose**: Processes incoming data from the server, similar to the server’s `onReadyRead`.
- **Challenge**: **Data Parsing and Validation** — Ensures valid JSON and known message types.
  - **Solution**: Parses and handles "answer" and "candidate" messages with corresponding signals.

#### `onErrorOccurred`
- **Purpose**: Logs errors occurring on the socket connection.

#### `sendOffer`
- **Purpose**: Sends an SDP offer message to the server.
- **Challenge**: **Message Format** — Ensures message is properly formatted as JSON.

#### `sendCandidate`
- **Purpose**: Sends an ICE candidate message to the server.
- **Challenge**: **Message Format** — Ensures message is properly formatted as JSON.

### `CallManager` Overview
The `CallManager` class in this project is responsible for managing the WebRTC-based voice call setup between two peers, designated as a caller and a callee. The class handles signaling, manages audio input and output, and monitors connection states. It also allows switching between caller and callee roles, setting up each role's specific signaling, WebRTC initialization, and audio handling.

### Class Members

- **Constructor and Initial Properties**: 
  - `m_host`: Default host address for signaling, set to `127.0.0.1`.
  - `m_port`: Default signaling port, set to `12345`.
  - `peerID`: Identifier for the connected peer, set to `"peer"`.
  - `audioTrackOpen`: Flag to indicate if the audio track is open, initialized as `false`.

#### Properties
- `role`: Stores and manages the current role of the instance (either `"caller"` or `"callee"`).
- `host`: Manages the server address for signaling.
- `port`: Port for signaling communication.
- `status`: Holds the call status, such as "Caller initialized" or "Call ended."

### Public Methods

#### `void startCall()`
Initializes the call depending on the role (`caller` or `callee`). 
- Calls `setupCaller()` if the role is `caller`.
- Calls `setupCallee()` if the role is `callee`.

#### `void endCall()`
Ends the current call by stopping signaling, audio input, and WebRTC sessions. This also updates `m_status` to "Call ended" and emits the `statusChanged` signal.

### Private Methods

#### `void setupCaller()`
Sets up the `CallManager` instance as a caller:
- Initializes `webRTC` as `"caller"`.
- Connects to the signaling server through `SignalingClient`.
- Establishes connections to handle server responses:
  - Sends SDP offers to the server.
  - Listens for remote SDP answers and ICE candidates.
  - Initializes `AudioInput` and `AudioOutput` for the call.
  - Handles connection states and stops `AudioInput` on disconnection.
- Updates `m_status` to "Caller initialized."

#### `void setupCallee()`
Sets up the `CallManager` instance as a callee:
- Initializes `webRTC` as `"callee"`.
- Starts a local signaling server using `SignalingServer`.
- Establishes connections to manage caller offers:
  - Responds to SDP offers with answers.
  - Sends and receives ICE candidates.
  - Initializes `AudioInput` and `AudioOutput`.
  - Monitors connection state changes and stops `AudioInput` on disconnection.
- Updates `m_status` to "Callee initialized."

### Signals
- `roleChanged`: Emitted when the role property changes.
- `hostChanged`: Emitted when the host property changes.
- `portChanged`: Emitted when the port property changes.
- `statusChanged`: Emitted when the call status updates.

### Additional Details
The `CallManager` relies on several components for WebRTC management and signaling:
- **WebRTC**: Handles peer connections, SDP generation, and candidate management.
- **SignalingClient / SignalingServer**: Manages the signaling process, with `SignalingClient` for caller communication and `SignalingServer` for callee communication.
- **AudioInput / AudioOutput**: Manages the audio streams, encoding and decoding audio frames between peers. The `CallManager` starts the audio input when the track is opened and stops it upon disconnection.

This class integrates the signaling and WebRTC functionalities to enable a seamless voice call setup and management between peers.

Here’s the documentation for the `main.cpp` and the QML UI (`Main.qml`), which form the entry point and user interface for the WebRTC-based voice calling application:

### `main.cpp` Overview
The `main.cpp` file initializes the Qt application and sets up the `CallManager` instance, which manages call functionality in the backend.

#### Key Components

1. **Application Setup**:
   - Initializes `QGuiApplication`, setting the application name and version.
   - Sets the style for the Qt Quick controls to "Fusion" for a cohesive UI appearance.

2. **QQmlApplicationEngine**:
   - Initializes `QQmlApplicationEngine` to load the QML UI.
   - Creates an instance of `CallManager` to handle call setup, signaling, and teardown.
   - Exposes the `callManager` instance to QML, making it accessible for binding and UI control.

3. **Loading the QML UI**:
   - The main QML file (`Main.qml`) is loaded from the Qt Resource System (`qrc:/UI/Main.qml`).
   - A lambda function connects the object creation signal to detect errors during QML loading, exiting if there’s a failure to load the UI.

### `Main.qml` Overview
The `Main.qml` file defines the user interface for the voice call application. The UI allows the user to set the role (caller or callee), configure host and port details, monitor call status, and initiate or end calls.

#### Components in the UI

1. **ApplicationWindow**:
   - Main window with fixed dimensions (280x520) and title "WebRTC Voice Call".

2. **Properties**:
   - `isCalling`: Boolean property to track if a call is in progress, which determines button text and color.

3. **UI Layout**:
   - The interface is organized in a `ColumnLayout` with margins and spacing for a compact and intuitive UI.

#### Layout Elements

- **Role Selection**:
  - `ComboBox` for selecting the role (`caller` or `callee`). When the role changes, it updates `callManager.role` and shows or hides the host field based on the selected role.

- **Host Input**:
  - A `TextField` for entering the host address, only visible if the role is `caller`. The entered text updates `callManager.host`.

- **Port Input**:
  - A `TextField` for entering the port, with input restricted to digits only. The value updates `callManager.port`.

- **Status Display**:
  - `Label` displays the current call status using the `callManager.status` property, with blue text to highlight status updates.

- **Call Button**:
  - A `Button` toggles between "Call" and "End Call" based on `isCalling`.
    - **Click Handling**: When clicked:
      - If `isCalling` is true, calls `callManager.endCall()`, updates button text and color, and sets `isCalling` to false.
      - If `isCalling` is false, checks if a host is entered (when role is `caller`), then calls `callManager.startCall()`, updates button text and color, and sets `isCalling` to true.

#### Signal Connections and Bindings
- The UI directly binds to `callManager` properties like `role`, `host`, `port`, and `status`, ensuring the backend state is in sync with user actions.
- The `callBtn` dynamically updates text and color based on the call status, providing a visual cue.

---

## WebRTC and Coturn Overview for Voice Calling Applications

WebRTC (Web Real-Time Communication) is a powerful protocol for creating peer-to-peer connections, primarily used for real-time voice and video communication. In this project, we leveraged WebRTC for building a voice call application using a custom signaling mechanism and included Coturn to handle NAT traversal challenges. The following sections provide an in-depth look at the various aspects of WebRTC technology, including its benefits, limitations, and the roles of STUN, TURN, and signaling servers.

### Core Components of WebRTC

1. **Signaling Server**
    - The signaling server is responsible for establishing initial communication between peers. It handles the exchange of session information, including Session Description Protocol (SDP) messages and ICE (Interactive Connectivity Establishment) candidates. This is crucial because WebRTC itself does not define how signaling should be implemented, which allows us to choose a custom approach tailored to our application.
    - In our project, we created a custom signaling server component that exchanges offers, answers, and ICE candidates between the caller and callee. This part of WebRTC doesn’t handle media or data, only control messages that enable a peer-to-peer connection.

2. **STUN and TURN Servers**
    - **STUN (Session Traversal Utilities for NAT):** A STUN server allows clients to discover their public IP address when they are behind a NAT (Network Address Translator). This step is essential for most WebRTC connections since many users are behind NATs, which obscure their direct IP addresses.
    - **TURN (Traversal Using Relays around NAT):** When a direct connection cannot be established, even with a STUN server, a TURN server is used to relay the media traffic between peers. TURN servers handle media data and can bypass restrictive firewalls but are more resource-intensive.
    - In our application, Coturn is configured to act as a STUN and TURN server. This setup ensures that our voice calls remain stable even when direct connections are blocked by strict NATs or firewalls.

3. **ICE (Interactive Connectivity Establishment)**
    - ICE is the protocol used by WebRTC to find the best path to establish a peer-to-peer connection between clients. It works by trying various connection candidates—such as direct connections, STUN-assisted connections, and TURN-relayed connections—and selecting the most efficient one.
    - ICE candidate handling is integrated into our application, where we manage and send ICE candidates through the signaling channel to ensure connectivity, regardless of network complexity.

### Advantages of WebRTC

- **Low Latency and Real-Time Communication:** WebRTC’s peer-to-peer nature enables near-instantaneous communication, which is ideal for voice and video calls. By reducing the need for a central server to relay media, we achieve lower latency, enhancing user experience during calls.
- **Cross-Platform Support:** WebRTC is supported across multiple browsers and platforms, which broadens the accessibility of applications. It also integrates well with native mobile and desktop applications.
- **Encryption by Default:** Security is a key feature in WebRTC, with all media and data channels encrypted using SRTP (Secure Real-Time Protocol). This built-in encryption ensures that communication remains private and secure.
- **Scalability for Small Groups:** For applications targeting one-to-one or small-group calls, WebRTC can be more efficient and cost-effective compared to centralized server-based solutions, thanks to its direct peer-to-peer architecture.

### Challenges and Limitations of WebRTC

- **NAT Traversal Issues:** Establishing connections when users are behind NATs or restrictive firewalls can be challenging. The STUN and TURN mechanisms help, but when a TURN relay is necessary, it can add latency and increase server costs.
- **Bandwidth and Network Reliability:** WebRTC performs best on stable networks with sufficient bandwidth. In cases of unstable or limited networks, voice quality can degrade, which we handle in our app by closely monitoring peer connection states and dynamically managing audio input and output.
- **Complexity in Managing Signaling:** Unlike other real-time protocols, WebRTC leaves the signaling protocol implementation to developers, which adds complexity. Our implementation involved designing custom signaling components to exchange SDPs and ICE candidates, which required handling nuances in state management and network transitions.
- **Resource Intensity for TURN:** TURN servers relay all media data when direct paths are unavailable, resulting in higher server loads. This resource demand makes TURN less suitable for large-scale deployments unless sufficient infrastructure is allocated.

### Implementation Insights from Our Project

The integration of WebRTC and Coturn required overcoming several challenges, such as handling peer connection states and managing signaling events for SDPs and candidates. Specific techniques we implemented include:

- **Dynamic Role Switching:** Our application distinguishes between 'caller' and 'callee' roles, with different initialization paths for each. This approach simplifies the connection flow and ensures that each peer is correctly configured for its role.
- **State-Based Audio Control:** We implemented audio controls that depend on the peer connection state, automatically starting or stopping audio input based on connection stability. This helps optimize resource usage and enhances the user experience.
- **Error Handling and Status Reporting:** By emitting signals to update the application status, users receive real-time feedback on the connection process, aiding in diagnosing network or configuration issues.

In conclusion, WebRTC, combined with Coturn, offers a robust solution for real-time voice and video calls, though it presents challenges in complex network scenarios. Through hands-on experience with the protocol, we’ve gained a deeper understanding of its components, signaling flows, and how to manage peer-to-peer connectivity effectively. Our project demonstrates that while WebRTC has limitations, careful implementation can yield high-quality, scalable communication applications suitable for diverse platforms.
