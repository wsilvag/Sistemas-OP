

Walter Silva Gaxiola 

# Lab 1.4: GRUB Configuration and Boot Process Analysis

**Student:** Walter Silva Gaxiola
**Date:** 24/11/25
**Course:** Sistemas Operativos

---

## 📝 Executive Summary

En este laboratorio, se analizó la configuración del gestor de arranque **GRUB (Grand Unified Bootloader)** del sistema operativo invitado (Ubuntu/Debian) en una máquina virtual.
Se identificaron y documentaron los valores por defecto del *timeout* y las opciones de línea de comando.
Posteriormente, se modificó el tiempo de espera a **30 segundos** y se implementó la visualización completa del menú para facilitar la interacción.
Finalmente, se investigaron los métodos clave para la recuperación del sistema en caso de una falla crítica de GRUB.

---

## ✅ Objectives Completed

* [**Checklist Completo**]: GRUB timeout successfully modified to 30 seconds.
* [**Checklist Completo**]: System boots correctly after changes.
* [**Checklist Completo**]: Complete documentation of all procedures.
* [**Checklist Completo**]: Research of at least 3 recovery methods.
* [**Checklist Completo**]: Clear and legible screenshot evidence.
* [**Checklist Completo**]: Organized and complete GitHub repository.

---

## ⚙️ Main Changes Implemented

1.  **Tiempo de Espera (Timeout) Modificado:** Cambiado de **`GRUB_TIMEOUT=0 `** a **`GRUB_TIMEOUT=30`** segundos.
2.  **Estilo de Menú Activado:** Se agregó la línea **`GRUB_TIMEOUT_STYLE=menu`** para garantizar la visibilidad del menú y el contador de tiempo al iniciar la VM.
3.  **Configuración de Arranque:** Se aseguró que la entrada por defecto sea el primer elemento del menú (**`GRUB_DEFAULT=0`**).
4.  **Generación de Configuración:** Los cambios se aplicaron usando el comando **`sudo update-grub`**.

---

## 🧠 Key Learnings

* La configuración principal de GRUB reside en **`/etc/default/grub`**, pero el sistema de arranque lee la configuración generada automáticamente en **`/boot/grub/grub.cfg`**.
* El comando **`sudo update-grub`** es esencial, ya que traduce las opciones sencillas del archivo `/etc/default/grub` al complejo archivo `.cfg`.
* Tener una copia de seguridad (`grub.backup`) es crítico, ya que los errores de sintaxis en el archivo principal pueden impedir el arranque del sistema.
* El método más robusto para recuperar un sistema con GRUB dañado es mediante el uso de un **Live USB** y el comando **`chroot`**.

---

## 📂 Evidence Files

| Carpeta | Archivo | Descripción |
| :--- | :--- | :--- |
| `screenshots/` | `01-original-boot-menu.png` | Captura del menú GRUB antes de cualquier modificación. |
| `screenshots/` | `03-grub-config-after.png` | Captura del archivo `/etc/default/grub` con `GRUB_TIMEOUT=30` y `GRUB_TIMEOUT_STYLE=menu`. |
| `screenshots/` | `05-update-grub-output.png` | Salida del comando `sudo update-grub` que confirma la generación exitosa del nuevo `.cfg`. |
| `configs/` | `grub-default-original.txt` | Contenido del archivo `/etc/default/grub` antes de las modificaciones. |
| `scripts/` | `grub-backup.sh` | Script funcional para crear copias de seguridad de la configuración. |
| `docs/` | `grub-recovery-guide.md` | Documentación de los tres métodos de recuperación (Rescue Mode, Live USB/chroot, Restauración de Backup). |
