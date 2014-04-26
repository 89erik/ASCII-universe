#include "gravity.h"
#include "physics.h"
#include "object.h"

#include <stdlib.h>
#include <math.h>
#include <gtk/gtk.h>

static GdkPixmap* pixmap = NULL;
static GtkWidget* drawing_area;

static int screen_width = 1200;
static int screen_height = 800;
static int print_delay = 5;

static gboolean running = false;

typedef struct coordinate {
    int x;
    int y;
} coordinate_t;

GtkWidget* window;

static gboolean configure_event(GtkWidget* widget, GdkEventConfigure* event) {
    if (pixmap) {
        g_object_unref (pixmap);
    }
    pixmap = gdk_pixmap_new(widget->window,
                            widget->allocation.width,
                            widget->allocation.height,
                            -1);
    gdk_draw_rectangle(pixmap,
                       widget->style->black_gc,
                       TRUE,
                       0, 0,
                       widget->allocation.width,
                       widget->allocation.height);

    return TRUE;
}

/* Redraw the screen from the backing pixmap */
static gboolean expose_event(GtkWidget* widget, GdkEventExpose* event) {
    gdk_draw_drawable (widget->window,
                       widget->style->fg_gc[gtk_widget_get_state (widget)],
                       pixmap,
                       event->area.x, event->area.y,
                       event->area.x, event->area.y,
                       event->area.width, event->area.height);

    return FALSE;
}

/* Draw a rectangle on the screen */
static void draw(int x, int y, int size) {
    GdkRectangle update_rect;

    update_rect.x = x - 5;
    update_rect.y = y - 5;
    update_rect.width = size;
    update_rect.height = size;
    gdk_draw_rectangle (pixmap,
              drawing_area->style->white_gc,
              TRUE,
              update_rect.x, update_rect.y,
              update_rect.width, update_rect.height);
    gtk_widget_queue_draw_area (drawing_area, 
                      update_rect.x, update_rect.y,
                      update_rect.width, update_rect.height);
}

static void clear() {
    int x = 0;
    int y = 0;
    int size = screen_width > screen_height? screen_width : screen_height;

    GdkRectangle update_rect;

    update_rect.x = x - 5;
    update_rect.y = y - 5;
    update_rect.width = size;
    update_rect.height = size;
    gdk_draw_rectangle (pixmap,
              drawing_area->style->black_gc,
              TRUE,
              update_rect.x, update_rect.y,
              update_rect.width, update_rect.height);
    gtk_widget_queue_draw_area (drawing_area, 
                      update_rect.x, update_rect.y,
                      update_rect.width, update_rect.height);
}

extern bool centering;
extern int center_object;
extern int offset_x;
extern int offset_y;
extern int n_objects;
extern object_t** objects;

int zoom = 1;
#define APPLY_ZOOM() {x *= zoom; y *= zoom; r *= zoom;}

void gui_print() {
    int i;
    int x,y,r;
    bool in_bounds;
    clear();

    /* Sets centered object */
    if (centering && n_objects > 0) {
        object_t* c = objects[center_object];
        x = nearbyint(c->x);
        y = nearbyint(c->y);

        APPLY_ZOOM();

        offset_x = (screen_width/2)  - x;
        offset_y = (screen_height/2) - y;
    }

    for (i=0; i<n_objects; i++) {
        x = nearbyint(objects[i]->x);
        y = nearbyint(objects[i]->y);
        r = nearbyint(objects[i]->r);

        APPLY_ZOOM();

        x += offset_x;
        y += offset_y;


        in_bounds = x < screen_width  && x >= 0 && y < screen_height && y >= 0;
        if (in_bounds) {
            draw(x,y,r);
        }
    }
}

void quit() {
    exit(0);
}

static gboolean time_handler(GtkWidget* widget) {
    tick();
    gui_print();
    return running;
}

void start_stop() {
    if (!running) {
        g_timeout_add(print_delay, (GSourceFunc) time_handler, (gpointer) window);
    }
    running = !running;
}


gboolean object_adding_in_progress = FALSE;
coordinate_t initial_click;

static gboolean button_press_event(GtkWidget* widget, GdkEventButton *event) {
    if (pixmap == NULL) return FALSE;
    int x,y;
    GdkModifierType state;
    gdk_window_get_pointer (event->window, &x, &y, &state);
    
    if (object_adding_in_progress) {
        double vx = (double) (x - initial_click.x) / 64;
        double vy = (double) (y - initial_click.y) / 64;

        insert_new_object(initial_click.x - offset_x, initial_click.y - offset_y, 5, 5, vx, vy);
        object_adding_in_progress = FALSE;
    
    } else {
        initial_click.x = x;
        initial_click.y = y;
        object_adding_in_progress = TRUE;
    }

    return TRUE;
}

int main(int argc, char *argv[]) {
    GtkWidget* vbox;
    GtkWidget* stop_button;

    gtk_init (&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_name(window, "Test Input");

    vbox = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER (window), vbox);
    gtk_widget_show(vbox);

    /* Close window action */
    g_signal_connect(window, "destroy", G_CALLBACK (quit), NULL); 

    /* Create the drawing area */
    drawing_area = gtk_drawing_area_new ();
    gtk_widget_set_size_request(GTK_WIDGET (drawing_area), screen_width, screen_height);
    gtk_box_pack_start(GTK_BOX (vbox), drawing_area, TRUE, TRUE, 0);
    gtk_widget_show(drawing_area);

    /* Signals used to handle backing pixmap */
    g_signal_connect(drawing_area, "expose_event", G_CALLBACK (expose_event), NULL);
    g_signal_connect(drawing_area, "configure_event", G_CALLBACK (configure_event), NULL);

    g_signal_connect (drawing_area, "button_press_event",
                    G_CALLBACK (button_press_event), NULL);

    gtk_widget_set_events (drawing_area, GDK_EXPOSURE_MASK
                         | GDK_LEAVE_NOTIFY_MASK
                         | GDK_BUTTON_PRESS_MASK
                         | GDK_POINTER_MOTION_MASK
                         | GDK_POINTER_MOTION_HINT_MASK);

    /* Stop button */
    stop_button = gtk_button_new_with_label("start/stop");
    gtk_box_pack_start(GTK_BOX (vbox), stop_button, FALSE, FALSE, 0);
    g_signal_connect_swapped(stop_button, "clicked", G_CALLBACK (start_stop), window);
    gtk_widget_show(stop_button);

    gtk_widget_show(window);

    init_simulation(argc, argv);

    start_stop();

    gtk_main();
    return 0;
}

