#include "physics.h"
#include "object.h"
#include "user_input.h"

#include <stdlib.h>
#include <math.h>
#include <gtk/gtk.h>

/* GTK widgets */
static GdkPixmap* pixmap = NULL;
static GtkWidget* drawing_area;
static GtkWidget* window;

static GtkWidget* mass_entry;
static GtkWidget* radius_entry;

static gboolean   entry_focus;

/* Screen dimensions */
static int screen_width = 1200;
static int screen_height = 800;

/* Screen printing */
static int      print_delay = 5;
       bool     centering = false;
       int      center_object = 0;
static i_vec_t  print_offset = {0,0};
static double   zoom = 1.0;

/* Screen trail */
static gboolean trail = false;
static i_vec_t* trail_dots;
static int      n_trail_dots = 0;
static int      trail_dots_size = 0;
#define TRAIL_DOTS_INITIAL_SIZE 512

/* Screen object insertion */
static gboolean object_adding_in_progress = false;
static i_vec_t initial_click;
static i_vec_t pointer_pos;

/* Simulation state */
static gboolean running = true;
extern int n_objects;
extern object_t** objects;

/* Function prototypes */
double f_from_entry(GtkWidget* entry);

static void insert_trail_dot(i_vec_t trail_dot) {
    if (!(n_trail_dots+1 < trail_dots_size)) {
        if (trail_dots_size == 0) trail_dots_size = TRAIL_DOTS_INITIAL_SIZE;
        else                      trail_dots_size *= 2;

        trail_dots = (i_vec_t*) realloc(trail_dots, sizeof(i_vec_t) * trail_dots_size);
    }
    trail_dots[n_trail_dots++] = trail_dot;
}

static void clear_trail_dots() {
    n_trail_dots = 0;
    trail_dots_size = 0;
    free(trail_dots);
    trail_dots = NULL;
}

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

void start_stop() {
    running = !running;
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

#define KEY_UP    65362
#define KEY_LEFT  65361
#define KEY_DOWN  65364
#define KEY_RIGHT 65363
#define KEY_SHIFT 65505
#define KEY_PLUS  65451
#define KEY_MINUS 65453
#define KEY_ESCAPE 65307
#define KEY_BACKSPACE 65288
#define SCROLL    10
#define SCROLL_EXTRA 10
static gboolean shift = FALSE;

static gboolean key_press(GtkWidget* widget, GdkEventKey* event) {
    if (entry_focus) return false;

    uint key = event->keyval;
    switch (key) {
        case KEY_UP:
            print_offset.y += SCROLL + (shift * SCROLL_EXTRA);
            centering = false;
            break;
        case KEY_DOWN:
            print_offset.y -= SCROLL + (shift * SCROLL_EXTRA);
            centering = false;
            break;
        case KEY_LEFT:
            print_offset.x += SCROLL + (shift * SCROLL_EXTRA);
            centering = false;
            break;
        case KEY_RIGHT:
            print_offset.x -= SCROLL + (shift * SCROLL_EXTRA);
            centering = false;
            break;
        case KEY_SHIFT:
            shift = TRUE;
            break;
        case 'c':
            centering = !centering;
            break;
        case 't':
        {
            if (trail) clear_trail_dots();
            trail = !trail;
            break;
        }
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        {
            int c = key - '0';
            if (c >= 0 && c <= n_objects) {
                center_object = c;
            }
            break;
        }
        case '+':
        case KEY_PLUS:
            if (zoom >= 1) {
                zoom++;
                print_offset.x -= screen_width/2;
                print_offset.y -= screen_height/2;
            } else {
                zoom *= 2;
            }

            break;
        case '-':
        case KEY_MINUS:
            if (zoom > 1) {
                zoom--;
                print_offset.x += screen_width/2;
                print_offset.y += screen_height/2;
            } else {
                zoom /= 2;
            }
            break;
        case KEY_ESCAPE:
            object_adding_in_progress = FALSE;
            break;
        case KEY_BACKSPACE:
            if (n_objects > 0) n_objects--;
            break;

        case ' ':
            start_stop();
            break;

        default:
            printf("key pressed:    %d (%c)\n", key, key);
            break;
    }

    return true;
}

static gboolean key_release(GtkWidget* widget, GdkEventKey* event) {
    uint key = event->keyval;
    switch (key) {
        case KEY_SHIFT:
            shift = FALSE;
            break;
    }
    return !entry_focus;
}

static void draw_circle(i_vec_t pos, int radius) {
    int diameter = radius*2;
    if (diameter > 1) {
        int x_corner = pos.x - radius;
        int y_corner = pos.y - radius;
        gdk_draw_arc(pixmap, drawing_area->style->white_gc, TRUE, 
               x_corner, y_corner, diameter, diameter,0, 360*64 );
    } else {
        gdk_draw_rectangle(pixmap, drawing_area->style->white_gc, TRUE,
                pos.x, pos.y, 1, 1);
    }
}

void apply_coordinate_zoom(i_vec_t* c) {
    c->x = (c->x) * zoom;
    c->y = (c->y) * zoom;
}

void apply_radius_zoom(int* r) {
    *r = (*r) * zoom;
    if (*r < 1) *r = 1;
}

gboolean in_bounds(i_vec_t pos) {
    return pos.x < screen_width  && pos.x >= 0 && pos.y < screen_height && pos.y >= 0;
}

void gui_print() {
    int i;
    int r;
    i_vec_t pos;

    gdk_draw_rectangle(pixmap, drawing_area->style->black_gc, TRUE, 0, 0, screen_width, screen_height);
    

    /* Sets centered object */
    if (centering) {
        object_t* c = objects[center_object];
        pos = vec_ftoi(c->p);

        apply_coordinate_zoom(&pos);

        print_offset.x = (screen_width/2)  - pos.x;
        print_offset.y = (screen_height/2) - pos.y;
    }
    
    if (trail) {
        for (i=0; i<n_trail_dots; i++) {
            pos = trail_dots[i];
            apply_coordinate_zoom(&pos);
            i_vec_accumulate(&pos, &print_offset);
            if (in_bounds(pos)) {
                gdk_draw_rectangle(pixmap, drawing_area->style->white_gc, TRUE, pos.x, pos.y, 1, 1);
            }
        }
    }

    for (i=0; i<n_objects; i++) {
        pos = vec_ftoi(objects[i]->p);
        if (trail) {
            insert_trail_dot(pos);
        }

        r = nearbyint(objects[i]->r);

        apply_coordinate_zoom(&pos);
        apply_radius_zoom(&r);

        i_vec_accumulate(&pos, &print_offset);

        if (in_bounds(pos)) {
            draw_circle(pos, r);
        }
    }
    if (object_adding_in_progress) {
        double size = f_from_entry(radius_entry);
        draw_circle(initial_click, size*zoom);
        gdk_draw_line(pixmap,
                drawing_area->style->white_gc,
                initial_click.x, initial_click.y,
                pointer_pos.x, pointer_pos.y);
    }

    /* Updates screen */
    gtk_widget_queue_draw_area(drawing_area, 0, 0, screen_width, screen_height);
}

void quit() {
    exit(0);
}

static gboolean time_handler(GtkWidget* widget) {
    if (running) tick();
    gui_print();
    return TRUE;
}

static gboolean motion_notify_event(GtkWidget* widget, GdkEventButton *event) {
    if (!object_adding_in_progress) return;
    GdkModifierType state;
    gdk_window_get_pointer (event->window, &pointer_pos.x, &pointer_pos.y, &state);
}

static gboolean text_entry_focus_in_event(GtkWidget* widget, GdkEventFocus* event) {
    //printf("focus in\n");
    entry_focus = true;

    return FALSE;
}

static gboolean text_entry_focus_out_event(GtkWidget* widget, GdkEventFocus* event) {
    g_signal_connect(window, "key_press_event", G_CALLBACK(key_press), NULL);
    g_signal_connect(window, "key_release_event", G_CALLBACK(key_release), NULL);
    //printf("focus out\n");

    entry_focus = false;

    return FALSE;
}

double f_from_entry(GtkWidget* entry) {
    return atof(gtk_entry_get_text(GTK_ENTRY(entry)));
}


#define MOUSE_LEFT_BUTTON  1
#define MOUSE_RIGHT_BUTTON 3
static gboolean button_press_event(GtkWidget* widget, GdkEventButton *event) {
    if (pixmap == NULL) return FALSE;
    i_vec_t pointer;
    GdkModifierType state;
    gdk_window_get_pointer(event->window, &pointer.x, &pointer.y, &state); 

    if (event->button == MOUSE_RIGHT_BUTTON) {
        f_vec_t pos, v;
        if (centering) {
            v = objects[center_object]->v;
        }
        pos.x = (pointer.x - print_offset.x) / zoom;
        pos.y = (pointer.y - print_offset.y) / zoom;

        insert_new_object(pos, f_from_entry(mass_entry), f_from_entry(radius_entry), v);
        object_adding_in_progress = FALSE;

    } else if (object_adding_in_progress) {
        f_vec_t pos, v;
        v.x = ((double) (pointer.x - initial_click.x) / 64) / zoom;
        v.y = ((double) (pointer.y - initial_click.y) / 64) / zoom;

        pos.x = (initial_click.x - print_offset.x) / zoom;
        pos.y = (initial_click.y - print_offset.y) / zoom;

        if (centering) {
            f_vec_accumulate(&v, &objects[center_object]->v);
        }

        insert_new_object(pos, f_from_entry(mass_entry), f_from_entry(radius_entry), v);
        object_adding_in_progress = FALSE;
    
    } else {
        pointer_pos = initial_click = pointer;
        object_adding_in_progress = TRUE;
    }

    return TRUE;
}

GtkWidget* init_insertion_box() {
    GtkWidget* insertion_box;
    GtkWidget* mass_label;
    GtkWidget* radius_label; 

    /* Init box */
    insertion_box = gtk_vbox_new(FALSE, 0);

    /* Mass entry */
    mass_label = gtk_label_new("Mass: ");
    mass_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(mass_entry), "1.0");
    gtk_entry_set_visibility(GTK_ENTRY(mass_entry), true);
    gtk_editable_select_region(GTK_EDITABLE(mass_entry), 0, GTK_ENTRY(mass_entry)->text_length);
    gtk_editable_set_editable(GTK_EDITABLE(mass_entry), true);
    gtk_box_pack_start(GTK_BOX(insertion_box), mass_label, true, true, 0);
    gtk_box_pack_start(GTK_BOX(insertion_box), mass_entry, true, true, 0);
    gtk_widget_show(mass_label);
    gtk_widget_show(mass_entry);

    /* Radius entry */
    radius_label = gtk_label_new("Radius: ");
    radius_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(radius_entry), "1.0");
    gtk_entry_set_visibility(GTK_ENTRY(radius_entry), true);
    gtk_editable_select_region(GTK_EDITABLE(radius_entry), 0, GTK_ENTRY(radius_entry)->text_length);
    gtk_editable_set_editable(GTK_EDITABLE(radius_entry), true);
    gtk_box_pack_start(GTK_BOX(insertion_box), radius_label, true, true, 0);
    gtk_box_pack_start(GTK_BOX(insertion_box), radius_entry, true, true, 0);
    gtk_widget_show(radius_label);
    gtk_widget_show(radius_entry);
    
    
    /* Widget focus */
    g_signal_connect_swapped(radius_entry, "focus_in_event", G_CALLBACK(text_entry_focus_in_event), window);
    g_signal_connect_swapped(mass_entry, "focus_in_event", G_CALLBACK(text_entry_focus_in_event), window);
    g_signal_connect_swapped(radius_entry, "focus_out_event", G_CALLBACK(text_entry_focus_out_event), window);
    g_signal_connect_swapped(mass_entry, "focus_out_event", G_CALLBACK(text_entry_focus_out_event), window);

    
    /* Foucs out button */
    GtkWidget* focus_button;
    focus_button = gtk_button_new_with_label("activate hotkeys");
    gtk_box_pack_start(GTK_BOX (insertion_box), focus_button, FALSE, FALSE, 0);
    g_signal_connect_swapped(focus_button, "clicked", G_CALLBACK (text_entry_focus_out_event), window);
    gtk_widget_show(focus_button);

    return insertion_box;
}

int main(int argc, char *argv[]) {
    GtkWidget* toplevel_box;

    gtk_init (&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_name(window, "Test Input");

    toplevel_box = gtk_hbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER (window), toplevel_box);
    gtk_widget_show(toplevel_box);

    /* Close window action */
    g_signal_connect(window, "destroy", G_CALLBACK (quit), NULL); 

    /* Create the drawing area */
    drawing_area = gtk_drawing_area_new ();
    gtk_widget_set_size_request(GTK_WIDGET (drawing_area), screen_width, screen_height);
    gtk_box_pack_start(GTK_BOX (toplevel_box), drawing_area, TRUE, TRUE, 0);
    gtk_widget_show(drawing_area);

    /* Signals used to handle backing pixmap */
    g_signal_connect(drawing_area, "expose_event", G_CALLBACK (expose_event), NULL);
    g_signal_connect(drawing_area, "configure_event", G_CALLBACK (configure_event), NULL);
    g_signal_connect(drawing_area, "button_press_event", G_CALLBACK (button_press_event), NULL);

    g_signal_connect(window, "motion_notify_event", G_CALLBACK(motion_notify_event), NULL);

    gtk_widget_set_events (drawing_area, GDK_EXPOSURE_MASK
                         | GDK_LEAVE_NOTIFY_MASK
                         | GDK_BUTTON_PRESS_MASK
                         | GDK_POINTER_MOTION_MASK
                         | GDK_POINTER_MOTION_HINT_MASK);
    
    /* Key events */
    g_signal_connect(window, "key_press_event", G_CALLBACK(key_press), NULL);
    g_signal_connect(window, "key_release_event", G_CALLBACK(key_release), NULL);



    GtkWidget* insertion_box = init_insertion_box();
    gtk_container_add(GTK_CONTAINER(toplevel_box), insertion_box);
    gtk_widget_show(insertion_box);

    gtk_widget_show(window);

    add_default_objects();

    g_timeout_add(print_delay, (GSourceFunc) time_handler, (gpointer) window);

    gtk_main();
    return 0;
}

