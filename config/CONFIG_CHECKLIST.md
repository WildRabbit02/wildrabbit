# ec_visualizer Configuration Checklist

## Cross-Host RTPS Configuration (192.168.1.108/102)

### 1. Parameter Consistency
All hosts MUST have identical settings in `ec_cyber_conf.xml`:

| Parameter | Value | Description |
|-----------|-------|-------------|
| `<enabled>` | `true` | Enable RTPS transport |
| `<multicast_ip>` | `239.255.0.1` | Multicast group IP |
| `<port>` | `9090` | UDP port for RTPS |

### 2. Deployment Steps

1. **Copy Configuration**
   ```bash
   sudo cp config/ec_cyber_conf.xml /usr/local/cyber/conf/
   ```

2. **Open Firewall** (all hosts)
   ```bash
   sudo ufw allow 9090/udp
   sudo iptables -A INPUT -p udp --dport 9090 -j ACCEPT
   ```

3. **Verify Network**
   ```bash
   ping 192.168.1.108
   ping 192.168.1.102
   ```

4. **Start ec_visualizer**
   ```bash
   source ec_visualizer_env.sh
   ./ec_visualizer --rtps-enabled --multicast-ip 239.255.0.1
   ```

5. **Verify Channels**
   ```bash
   cyber_channel list  # Should show remote channels
   ```

### 3. Troubleshooting

- **No data received**: Check firewall rules and multicast routing
- **High latency**: Verify network switch supports multicast efficiently
- **Connection refused**: Ensure all hosts use the same `cyber_conf.xml`
