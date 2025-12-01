#!/bin/bash
# Script para monitorear servicios

echo "=== Service Status ==="
systemctl status apache2
systemctl status simple-service
systemctl status ssh

echo "=== Service Logs ==="
journalctl -u apache2 --since "1 hour ago" | tail -n 10
journalctl -u simple-service --since "1 hour ago" | tail -n 10
