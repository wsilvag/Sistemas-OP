#include <gtk/gtk.h>
#include <cairo.h>
#include <stdlib.h>
#include <stdio.h>
#include "../include/scheduler.h"

// Variables globales (no recomendado en producción, pero simple para GTK demo)
process_t current_processes[MAX_PROCESSES];
timeline_event_t current_timeline[MAX_TIMELINE_EVENTS];
int current_n = 0;
int timeline_length = 0;
int total_execution_time = 0;
GtkWidget *gantt_area = NULL; // Área de dibujo

// Función para asignar colores a PIDs
void get_process_color(int pid, double *r, double *g, double *b) {
    if (pid == -1) { // IDLE
        *r = 0.8; *g = 0.8; *b = 0.8;
        return;
    }
    // Asignación de color simple basado en PID (ejemplo)
    *r = (double)(pid * 15 % 256) / 255.0;
    *g = (double)(pid * 30 % 256) / 255.0;
    *b = (double)(pid * 45 % 256) / 255.0;
}

// Handler para dibujar el diagrama de Gantt
static gboolean draw_gantt_cb(GtkWidget *widget, cairo_t *cr, gpointer data) {
    if (timeline_length == 0) return FALSE;

    // Dimensiones del área de dibujo
    GtkAllocation allocation;
    gtk_widget_get_allocation(widget, &allocation);
    int width = allocation.width;
    int height = allocation.height;
    
    // Escala (ejemplo: 5 píxeles por unidad de tiempo)
    double time_scale = (double)width / total_execution_time;
    double bar_height = height * 0.4; // 40% de la altura

    // Dibuja los eventos de la línea de tiempo
    for (int i = 0; i < timeline_length; i++) {
        timeline_event_t *event = &current_timeline[i];
        
        // Coordenadas
        double x = event->time * time_scale;
        double w = event->duration * time_scale;
        double y = height * 0.3; // Centrar la barra

        double r, g, b;
        get_process_color(event->pid, &r, &g, &b);
        
        // Dibujar el rectángulo de ejecución
        cairo_set_source_rgb(cr, r, g, b);
        cairo_rectangle(cr, x, y, w, bar_height);
        cairo_fill(cr);
        
        // Dibuja la línea divisoria
        cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
        cairo_set_line_width(cr, 1.0);
        cairo_move_to(cr, x + w, 0);
        cairo_line_to(cr, x + w, height);
        cairo_stroke(cr);

        // Opcional: Escribir el PID
        if (event->pid > 0) {
            char label[10];
            snprintf(label, 10, "P%d", event->pid);
            cairo_move_to(cr, x + w/2 - 10, y + bar_height/2 + 5);
            cairo_show_text(cr, label);
        }
    }

    // Dibujar la escala de tiempo (eje X)
    // ... Implementar aquí el dibujo de la escala 0, 5, 10, 15...

    return FALSE;
}

// Función principal para construir la UI
static void activate_app(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *vbox;
    GtkWidget *hbox_controls;
    GtkWidget *run_button;
    // ... otros widgets (table, dropdown, metrics display)

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "CPU Scheduler Simulator");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // 1. Controles (Botón Run, Selector de Algoritmo, Quantum Input)
    hbox_controls = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox_controls, FALSE, FALSE, 0);

    // Botón Run (debe conectarse a una función que llame a schedule_algo y recalcule)
    run_button = gtk_button_new_with_label("Run Simulation");
    // g_signal_connect(run_button, "clicked", G_CALLBACK(run_simulation_cb), NULL);
    gtk_box_pack_start(GTK_BOX(hbox_controls), run_button, FALSE, FALSE, 0);

    // 2. Área del Diagrama de Gantt
    gantt_area = gtk_drawing_area_new();
    gtk_widget_set_size_request(gantt_area, 780, 200);
    g_signal_connect(G_OBJECT(gantt_area), "draw", G_CALLBACK(draw_gantt_cb), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), gantt_area, TRUE, TRUE, 0);

    // 3. Dashboard de Métricas (Tabla)
    // ... Implementar aquí la tabla o display de metrics_t

    gtk_widget_show_all(window);
}

// Función que coordina la simulación (DEBE LLAMAR A algorithms.c y metrics.c)
void run_simulation_cb(GtkWidget *widget, gpointer data) {
    // 1. Obtener la configuración (algoritmo, quantum, procesos de la tabla)
    // ... (Aquí se cargan los procesos en current_processes y current_n)

    // 2. Llamar al algoritmo seleccionado (ejemplo FIFO)
    // schedule_fifo(current_processes, current_n, current_timeline); 
    
    // 3. Calcular el tiempo total (ejemplo: tiempo del último evento)
    // total_execution_time = current_timeline[timeline_length-1].time;

    // 4. Calcular métricas
    // metrics_t results;
    // calculate_metrics(current_processes, current_n, total_execution_time, &results);

    // 5. Actualizar la GUI
    // Actualizar la tabla de métricas
    // forzar el redibujo del Gantt Chart:
    gtk_widget_queue_draw(gantt_area); 
}

// -------------------------------------------------------------------
// Función Principal (Punto de entrada)
// -------------------------------------------------------------------
int main(int argc, char *argv[]) {
    GtkApplication *app;
    int status;

    app = gtk_application_new("org.alterswa.schedsim", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate_app), NULL);

    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app); 

    return status;
}
