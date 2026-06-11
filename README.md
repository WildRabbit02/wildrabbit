# ec_visualizer

Standalone CyberRT data visualizer and monitor for robotics applications. Supports real-time camera image and LiDAR point cloud visualization with cross-host RTPS communication and Foxglove Studio integration.

## Features

- 🖼️ **Real-time Image Display**: Subscribe to and visualize camera streams
- 🌩️ **Point Cloud Rendering**: 3D OpenGL visualization of LiDAR data  
- 📡 **Cross-Host Communication**: RTPS multicast support for distributed systems
- 🔌 **Foxglove Bridge**: WebSocket server for Foxglove Studio integration
- 📊 **Channel Monitor**: Auto-discovery of available CyberRT channels
- 🧪 **Unit Tests**: Comprehensive test suite with GoogleTest

## Architecture

```
┌─────────────────┐     ┌─────────────────┐     ┌─────────────────────┐
│ 192.168.1.108   │     │ 192.168.1.102   │     │ Monitoring Host     │
│ Camera Container│     │ LiDAR Container │     │                     │
│                 │     │                 │     │  ┌───────────────┐  │
│ /camera/left ───┼─────┤                 │     │  │ ec_visualizer │  │
│ /camera/right ──┼─────┤                 │     │  │               │  │
│        RTPS     │     │        RTPS     │     │  │ - NodeWrapper │  │
│   239.255.0.1   │     │   239.255.0.1   │     │  │ - ChannelMon  │  │
│    :9090 UDP    │     │    :9090 UDP    │     │  │ - WebSocket   │  │
└─────────────────┘     └─────────────────┘     │  │ - Renderer    │  │
                                                │  └───────┬───────┘  │
                                                └──────────┼──────────┘
                                                           │
                                          ┌────────────────┼────────────────┐
                                          │                │                │
                                   Local Qt Window   WebSocket:8765   Foxglove Studio
                                   (Direct Render)   (Foxglove Proto)  (Remote PC)
```

## Quick Start

### Prerequisites

- CyberRT runtime libraries (`libcyber.so`, `libprotobuf-lite.so`)
- Qt5 (Core, Gui, Widgets, OpenGL)
- Eigen3
- CMake 3.14+
- Optional: GoogleTest for unit tests

### Build

```bash
cd ec_visualizer
mkdir build && cd build

# Configure with CyberRT path
cmake .. \
  -DCYBER_ROOT=/usr/local/cyber \
  -DBUILD_TESTING=ON \
  -DBUILD_FOXGLOVE_BRIDGE=ON

make -j$(nproc)
```

### Run

```bash
# Load environment
source ec_visualizer_env.sh

# Local mode (same host)
./ec_visualizer

# Cross-host mode with RTPS
./ec_visualizer --rtps-enabled --multicast-ip 239.255.0.1

# Foxglove bridge mode
./ec_visualizer --rtps-enabled --mode foxglove --ws-port 8765
```

## Configuration

### Cross-Host Setup

For your scenario with cameras on `192.168.1.108` and LiDAR on `192.168.1.102`:

1. **Deploy configuration** to ALL hosts:
   ```bash
   sudo cp config/ec_cyber_conf.xml /etc/cyber/conf/
   ```

2. **Verify RTPS settings** are identical:
   - Multicast IP: `239.255.0.1`
   - Port: `9090`

3. **Open firewall** on all hosts:
   ```bash
   sudo ufw allow 9090/udp
   ```

4. **Run visualizer** on monitoring host:
   ```bash
   ./ec_visualizer --rtps-enabled --multicast-ip 239.255.0.1
   ```

### Environment Variables

| Variable | Description | Default |
|----------|-------------|---------|
| `CYBER_ROOT` | CyberRT installation path | `/usr/local/cyber` |
| `LD_LIBRARY_PATH` | Automatically set by env script | - |

## Command Line Options

| Option | Description |
|--------|-------------|
| `--rtps-enabled` | Enable RTPS for cross-host communication |
| `--multicast-ip <ip>` | Set RTPS multicast IP (default: 239.255.0.1) |
| `--port <port>` | Set RTPS port (default: 9090) |
| `--mode <mode>` | Run mode: `local` or `foxglove` |
| `--ws-port <port>` | WebSocket port for Foxglove (default: 8765) |
| `--help` | Show help message |

## Testing

```bash
cd build
ctest --verbose
# or
./test_runner.sh
```

## Project Structure

```
ec_visualizer/
├── CMakeLists.txt              # Build configuration
├── cmake/
│   └── ec_visualizer_env.sh.in # Environment script template
├── config/
│   └── ec_cyber_conf.xml       # RTPS configuration
├── include/
│   ├── ec_node_wrapper.h       # CyberRT node wrapper
│   ├── visualizer_window.h     # Qt main window
│   ├── image_display.h         # Image rendering widget
│   ├── pointcloud_display.h    # OpenGL point cloud widget
│   ├── channel_monitor.h       # Channel discovery
│   └── foxglove_bridge.h       # WebSocket bridge
├── src/
│   ├── main.cpp                # Entry point
│   ├── ec_node_wrapper.cpp
│   ├── visualizer_window.cpp
│   ├── image_display.cpp
│   ├── pointcloud_display.cpp
│   ├── channel_monitor.cpp
│   └── foxglove_bridge.cpp
├── tests/
│   └── test_*.cpp              # Unit tests
└── docs/
    └── deployment_architecture.md
```

## License

MIT License

## Version History

- **v2.2.0** - Renamed to ec_visualizer, removed Apollo branding
- **v2.1.0** - Added Foxglove WebSocket bridge
- **v2.0.1** - Added configurable CyberRT path support
- **v2.0.0** - Initial release with tests and documentation
