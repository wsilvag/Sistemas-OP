#!/bin/bash
# Script para hacer backup de GRUB
echo "Creando backup de configuración de GRUB..."
timestamp=$(date +%Y%m%d_%H%M%S)

# Backup de /etc/default/grub
if sudo cp /etc/default/grub /etc/default/grub.backup.$timestamp; then
    echo "Backup de /etc/default/grub creado exitosamente."
else
    echo "Error al crear backup de /etc/default/grub"
    exit 1
fi

# Backup de /boot/grub/grub.cfg (Opcional pero recomendado)
if sudo cp /boot/grub/grub.cfg /boot/grub/grub.cfg.backup.$timestamp; then
    echo "Backup de grub.cfg creado exitosamente."
else
    echo "Error al crear backup de grub.cfg"
fi
