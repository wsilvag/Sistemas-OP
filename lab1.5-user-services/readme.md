# Lab 1.5: Users and Services Management

**Student:** Walter
**Date:** 25 de noviembre de 2025
**Course:** Operating Systems

---

## Executive Summary

En este laboratorio, se realizaron tareas fundamentales de administración de sistemas Linux, incluyendo la creación y gestión de usuarios y grupos, configuración de permisos, administración de servicios con `systemd`, análisis de logs del sistema, y monitoreo de recursos. Se implementaron buenas prácticas de seguridad y se documentaron todos los pasos y comandos utilizados.

---

## Tasks Completed

- Creación y gestión de usuarios y grupos con permisos adecuados.
- Configuración y control de servicios del sistema usando `systemd`.
- Análisis de logs del sistema para monitoreo y solución de problemas.
- Creación de un servicio personalizado y automatización de tareas con scripts.
- Documentación profesional de todos los procedimientos y resultados.

---

## Users and Groups Created

| User          | Groups                     | Home Directory      | Shell         | Purpose               |
|---------------|----------------------------|---------------------|---------------|-----------------------|
| alice         | developers, administrators | /home/alice         | /bin/bash     | Lead Developer        |
| bob           | developers                 | /home/bob           | /bin/bash     | Junior Developer      |
| charlie       | testers                    | /home/charlie       | /bin/bash     | QA Tester             |
| temp_user     | -                          | /home/temp_user     | /bin/bash     | Temporary User        |
| service_account | -                        | -                   | /usr/sbin/nologin | System Account    |

---

## Services Configured

- Apache2 web server
- Custom simple-service
- SSH service analysis
- System service dependencies

---

## Key Learnings

- Uso avanzado de comandos para gestión de usuarios y grupos.
- Configuración y control de servicios con `systemd`.
- Interpretación de logs del sistema para diagnóstico y monitoreo.
- Implementación de scripts para automatización de tareas administrativas.
- Aplicación de buenas prácticas de seguridad en la administración de sistemas.

---

## Security Measures Implemented

- Contraseñas seguras para todos los usuarios.
- Expiración de cuentas temporales.
- Uso adecuado de `sudo` y permisos de archivo.
- Creación de usuarios del sistema sin shell de login.
- Configuración de reglas personalizadas en `sudoers`.
- Monitoreo y análisis de logs para detección de problemas.
