---
title: Deployment Architecture
---

# ec_visualizer Deployment Architecture

## System Overview

```mermaid
graph TD
    subgraph "LAN Environment (192.168.1.x)"
        direction TB
        
        subgraph "Host A: Camera Source (192.168.1.108)"
            A_Container["🐳 Container: ec_opencv_a9"]
            A_Cyber["⚙️ CyberRT Node (Camera)"]
            A_Container --> A_Cyber
            A_Cyber -- "Publish /camera/left, /camera/right" --> A_RTPS
        end

        subgraph "Host B: LiDAR Source (192.168.1.102)"
            B_Container["🐳 Container: ec_ds"]
            B_Cyber["⚙️ CyberRT Node (LiDAR)"]
            B_Container --> B_Cyber
            B_Cyber -- "Publish /lidar/points" --> B_RTPS
        end

        subgraph "Host C: Monitoring & Visualization (Local)"
            C_App["🖥️ ec_visualizer (v2.2.0)"]
            
            subgraph "Internal Modules"
                Mod_Wrapper["EcNodeWrapper<br/>(RTPS Subscriber)"]
                Mod_Monitor["Channel Monitor<br/>(Discovery)"]
                Mod_WS["WebSocket Server<br/>(Foxglove Bridge)"]
                Mod_Render["Render Engine<br/>(Qt/OpenGL)"]
            end
            
            C_App --> Mod_Wrapper
            C_App --> Mod_Monitor
            C_App --> Mod_WS
            C_App --> Mod_Render
            
            Local_UI["🖼️ Local Qt Window<br/>(Direct Display)"]
            Mod_Render --> Local_UI
        end

        subgraph "External Clients"
            Foxglove["🦊 Foxglove Studio<br/>(Remote/Local PC)"]
        end
    end

    A_RTPS(("📡 RTPS Multicast<br/>239.255.0.1:9090"))
    B_RTPS(("📡 RTPS Multicast<br/>239.255.0.1:9090"))
    
    A_Cyber -.-> A_RTPS
    B_Cyber -.-> B_RTPS
    
    A_RTPS == "UDP Multicast Stream" ==> Mod_Wrapper
    B_RTPS == "UDP Multicast Stream" ==> Mod_Wrapper
    
    Mod_WS == "WebSocket (ws://ip:8765)<br/>Foxglove Protocol" ==> Foxglove

    style C_App fill:#e8f5e9,stroke:#2e7d32,stroke-width:2px
    style Foxglove fill:#fff3e0,stroke:#ef6c00
```

## Key Components

1. **Data Plane**: RTPS multicast for real-time sensor data
2. **Control Plane**: WebSocket server for Foxglove integration
3. **Configuration**: Consistent cyber_conf.xml across all hosts