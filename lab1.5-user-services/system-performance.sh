#!/bin/bash
echo "=== System Performance Report ==="
echo "Generated on: $(date)"
echo ""
echo "=== System Information ==="
uname -a
echo ""
echo "=== CPU Information ==="
lscpu | head -10
echo ""
echo "=== Memory Usage ==="
free -h
echo ""
echo "=== Disk Usage ==="
df -h
echo ""
echo "=== Load Average ==="
uptime
echo ""
echo "=== Top 10 CPU Processes ==="
ps aux --sort=-%cpu | head -11
echo ""
echo "=== Top 10 Memory Processes ==="
ps aux --sort=-%mem | head -11
