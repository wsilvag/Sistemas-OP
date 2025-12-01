# 📘 Explicación Detallada de Algoritmos de Scheduling

## 1. FIFO (First In First Out)
* **Tipo**: No-preemptivo.
* **Lógica**: Los procesos se ejecutan en el estricto orden en que llegan a la cola de listos.
* **Implementación**: Se realiza una **ordenación inicial por `arrival_time`** y luego se simula la ejecución secuencial.

## 2. SJF (Shortest Job First)
* **Tipo**: No-preemptivo.
* **Lógica**: Cuando la CPU está libre, selecciona el proceso que ha llegado y que tiene el menor `burst_time`.
* **Implementación**: En el momento de la decisión, se busca en la cola de listos el proceso con el **mínimo `burst_time`**.

## 3. STCF (Shortest Time to Completion First)
* **Tipo**: Preemptivo.
* **Lógica**: En cualquier momento $t$, si un proceso recién llegado tiene un `remaining_time` menor que el proceso actualmente en ejecución, la CPU se conmuta.
* **Implementación**: Se verifica el `remaining_time` en **cada unidad de tiempo**.

## 4. Round Robin (RR)
* **Tipo**: Preemptivo.
* **Lógica**: Los procesos se colocan en una cola circular. Cada proceso recibe un **quantum** de tiempo (`quantum`). Si el proceso no termina dentro del quantum, se interrumpe y se mueve al final de la cola.
* **Implementación**: Se utiliza una **cola de listos (Queue)** para mantener el orden.

## 5. MLFQ (Multi-Level Feedback Queue)
* **Tipo**: Preemptivo, Dinámico.
* **Lógica**: Trata de priorizar trabajos cortos/interactivos.
    * Procesos nuevos $\rightarrow$ Q0 (Máxima prioridad).
    * Si usa todo el quantum $\rightarrow$ Baja a Q1.
    * Si el proceso es interrumpido por I/O $\rightarrow$ Permanece en su cola (buena heurística para interactivo).
    * **Boost de Prioridad**: Cada *N* segundos, todos los procesos regresan a Q0 para evitar la inanición (*starvation*).
* **Implementación**: Se requiere una estructura de **múltiples colas** (implementadas como arreglos o listas enlazadas).
