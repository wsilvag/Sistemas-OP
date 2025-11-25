

Parte 3: Investigación de Recuperación

1. Método 1: GRUB Rescue Mode (Modo Rescate)

El Modo Rescate (grub rescue>) aparece cuando GRUB
no puede encontrar su etapa final de configuración
(usualmente porque se movió el archivo /boot/grub/grub.cfg
o la partición de /boot).

---Concepto------------------Comandos clave y explicacion-------------------------------
Cómo Acceder                 El sistema entra automáticamente en este modo si falla
                             al cargar la configuración.
----------------------------------------------------------------------------------------
Comandos BásiCos             1. ls: Lista las particiones disponibles (ej: (hd0,msdos1),
                             (hd0,gpt2)). 
                             2. set prefix=(hdX,gpty)/boot/grub: Indica a GRUB
                             dónde están sus archivos (/boot/grub).
                             3. set root=(hdX,gpty): Indica la partición raíz.
----------------------------------------------------------------------------------------
Proceso de Arranque Manual   4. insmod normal: Carga el módulo de arranque normal.
                             5. normal: Intenta cargar el menú de GRUB.
----------------------------------------------------------------------------------------




2. Método 2: Recovery from Live USB/DVD (Recuperación con USB Live)

Este es el método más seguro y poderoso, ya que usa un sistema operativo temporal
(el Live USB) para reparar el GRUB del disco duro de la VM.

---Pasos----------------Comandos Clave y Explicación--------------------------------------
1. Arrancar y Montar    Inicia la VM desde el USB/ISO Live. Luego, monta la partición
                        raíz de tu sistema (sdaX es tu disco duro, Y es la partición raíz).
 
                        sudo mount /dev/sd**aY** /mnt
------------------------------------------------------------------------------------------
2. Usar chroot          chroot te permite actuar como si ya estuvieras dentro
                        del sistema operativo roto.

                        sudo chroot /mnt
------------------------------------------------------------------------------------------
3. Reinstalación       Reinstala GRUB en el Master Boot Record (MBR) del disco
   de GRUB             (/dev/sda, sin número de partición).
 
                       grub-install /dev/sda
-----------------------------------------------------------------------------------------
4. Actualizar/Salir   Una vez instalado, genera la configuración final y sale.
                      update-grub
                      exit
----------------------------------------------------------------------------------------





3. Método 3: Backup Restoration (Restauración de Copia de Seguridad)
---Pasos----------------Comandos Clave y Explicación-------------------------------------- 
1. Restaurar el
 Archivo Principal      Copia la versión de respaldo sobre la versión dañada
                        (esto deshace tu edición).
                        sudo cp /etc/default/grub.backup /etc/default/grub
----------------------------------------------------------------------------------------
2. Regenerar la
 Configuración          El sistema debe volver a generar el archivo grub.cfg
                        a partir del archivo de configuración restaurado.

                        sudo update-grub
----------------------------------------------------------------------------------------
3. Verificación	       Reinicia el sistema para confirmar que el menú GRUB aparece
                       y el sistema arranca correctamente.
 
                       sudo reboot
---------------------------------------------------------------------------------------






