# Service Configuration Report

## Services Analyzed and Configured

- **Apache2**: Instalado, iniciado, detenido y habilitado para inicio automático.
- **SSH**: Verificado su estado y dependencias.
- **simple-service**: Servicio personalizado creado y gestionado con `systemd`.

## Custom Service Implementation

- **Script**: `/home/walter/simple-service.sh`
- **Service Unit**: `/etc/systemd/system/simple-service.service`
- **Log**: `/tmp/simple-service.log`

## Service Dependency Analysis

- **SSH**: Depende de `sysinit.target`, `basic.target`, y `network.target`.
- **Apache2**: Depende de `network.target`.

## Troubleshooting Procedures

1. **Apache2 Configuration Error**:
   - Se movió el archivo de configuración para simular un error.
   - Se revisaron logs con `journalctl -u apache2`.
   - Se restauró la configuración y se reinició el servicio.
