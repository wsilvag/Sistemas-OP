#!/bin/bash
# Script para crear usuarios y grupos automáticamente

# Crear usuarios
sudo useradd -m -s /bin/bash -c "Alice Johnson - Lead Developer" alice
sudo useradd -m -s /bin/bash -c "Bob Smith - Junior Developer" bob
sudo useradd -m -s /bin/bash -c "Charlie Brown - QA Tester" charlie
sudo useradd -m -s /bin/bash -e 2025-12-31 temp_user
sudo useradd -r -s /usr/sbin/nologin service_account

# Crear grupos
sudo groupadd developers
sudo groupadd testers
sudo groupadd administrators

# Agregar usuarios a grupos
sudo usermod -aG developers alice
sudo usermod -aG developers bob
sudo usermod -aG testers charlie
sudo usermod -aG administrators alice

# Asignar contraseñas (ejecutar manualmente después)
echo "Remember to set passwords for users: alice, bob, charlie, temp_user"
