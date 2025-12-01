# 💻 Scheduler Simulator with GUI

## 🎯 Objetivo del Proyecto

Este proyecto implementa un simulador de algoritmos de planificación de CPU (**CPU Scheduling Simulator**) con una interfaz gráfica (GUI) interactiva utilizando **GTK3**. El simulador permite:

1.  Simular 5 algoritmos de planificación principales (FIFO, SJF, STCF, RR, MLFQ).
2.  Visualizar la ejecución de los procesos mediante **diagramas de Gantt**.
3.  Calcular métricas clave de rendimiento (Turnaround Time, Response Time, Fairness).
4.  Comparar el rendimiento de los algoritmos lado a lado.

## ⚙️ Componentes Implementados

| Archivo | Descripción | Parte |
| :--- | :--- | :--- |
| `src/algorithms.c` | Implementación de FIFO, SJF, STCF, Round Robin y MLFQ. | Parte 2 |
| `src/metrics.c` | Funciones para calcular las métricas de rendimiento y el índice de Fairness. | Parte 3 |
| `src/gui_gtk.c` | Implementación de la Interfaz Gráfica (GUI) usando GTK3. Maneja la entrada de procesos y la visualización de la gráfica de Gantt. | Parte 4 |
| `src/report.c` | Generador de reportes de comparación en formato Markdown/HTML. | Parte 5 |
| `include/*.h` | Archivos de cabecera con las estructuras de datos y firmas de funciones (Parte 1). | Parte 1 |

## 🛠️ Requisitos e Instalación

### Requisitos de Dependencias

Este proyecto requiere las siguientes dependencias en un entorno Linux:

1.  **GCC** (GNU Compiler Collection)
2.  **Make**
3.  **GTK3 Development Libraries**

### Instalación de Dependencias (Ubuntu/Debian)

```bash
sudo apt update
sudo apt install build-essential libgtk-3-dev
