# Troubleshooting Log

## Issues and Solutions

1. **Apache2 Failed to Start**:
   - **Cause**: Configuration file missing.
   - **Solution**: Restaurar el archivo de configuración desde backup.

2. **Permission Denied on Shared Directories**:
   - **Cause**: Permisos incorrectos en `/shared/projects`.
   - **Solution**: Ajustar permisos con `chmod 770 /shared/projects`.

3. **Custom Service Not Running**:
   - **Cause**: Script sin permisos de ejecución.
   - **Solution**: Ejecutar `chmod +x /home/walter/simple-service.sh`.
