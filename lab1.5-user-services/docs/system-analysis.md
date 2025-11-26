# System Analysis Report

## System Performance Baseline

- **CPU**: Intel i5, 2 cores, 4 threads.
- **Memory**: 4GB RAM, 1GB used, 3GB free.
- **Disk**: 25GB total, 10GB used.

## Log Analysis Findings

- Se revisaron logs de `sshd`, `apache2`, y errores del sistema.
- No se encontraron errores críticos en los logs analizados.

## Resource Utilization Patterns

- Uso normal de CPU y memoria durante las pruebas.
- Picos de uso al iniciar servicios como Apache2.

## Monitoring Recommendations

- Usar `htop` para monitoreo en tiempo real.
- Revisar logs diariamente con `journalctl --since today`.
- Configurar alertas para errores críticos.
