#!/bin/bash
# Script para restaurar GRUB
echo "Restaurando configuración de GRUB..."

# Pide al usuario qué archivo restaurar ( toma el más reciente o uno fijo)
# Aquí asumimos que restauramos el archivo .backup genérico si existe
if [ -f /etc/default/grub.backup ]; then
    sudo cp /etc/default/grub.backup /etc/default/grub
    echo "Archivo restaurado. Ejecutando update-grub..."
    sudo update-grub
    echo "Restauración completada."
else
    echo "No se encontró el archivo /etc/default/grub.backup"
    exit 1
fi
