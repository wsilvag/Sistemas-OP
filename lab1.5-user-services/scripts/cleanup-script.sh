#!/bin/bash
# Script para limpiar el laboratorio

# Eliminar usuarios
sudo userdel -r alice
sudo userdel -r bob
sudo userdel -r charlie
sudo userdel -r temp_user
sudo userdel service_account

# Eliminar grupos
sudo groupdel developers
sudo groupdel testers
sudo groupdel administrators

# Detener y eliminar servicio personalizado
sudo systemctl stop simple-service
sudo systemctl disable simple-service
sudo rm /etc/systemd/system/simple-service.service
sudo systemctl daemon-reload
sudo rm /tmp/simple-service.log

# Desinstalar Apache2
sudo systemctl stop apache2
sudo systemctl disable apache2
sudo apt remove -y apache2

# Eliminar directorios compartidos
sudo rm -rf /shared/
