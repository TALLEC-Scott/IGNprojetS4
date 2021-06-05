#ifndef APP_H
#define APP_H

#include <gtk/gtk.h>
#include <cairo.h>

typedef struct {
    GtkDrawingArea  *area;
    cairo_surface_t *image_surface;
    GdkPixbuf       *pixbuf;
    cairo_format_t  format;
    int             width;
    int             height;
    char            *filename;
    char            *shortname;
} Image;

typedef struct {
    GtkWindow   *wcb;
    GtkButton   *button_topo;
    GtkButton   *button_road;
    GtkButton   *button_river;
    GtkButton   *color_ok;
    GtkButton   *color_cancel;
    GdkRGBA     topo;
    GdkRGBA	road;
    GdkRGBA	river;
    GdkRGBA     default_color;
    gboolean    topo_was_set;
    gboolean    road_was_set;
    gboolean    river_was_set;
} Colors;

typedef struct {
    GtkWindow       *window;
    GtkWidget       *output_event_box;
    GtkSwitch       *switch_auto;
    GtkLabel        *x_label;
    GtkLabel        *y_label;
    GtkButton       *rectif_ok;
    GtkButton       *rectif_cancel;
    GtkButton       *rectif_done;
    GtkEntryBuffer  *coord_text_buff;
    gulong          handler_id;
    gdouble         x_pos;
    gdouble         y_pos;
} Rectif;

typedef struct {
    GtkWidget           *dialog;
    GtkComboBoxText     *screen_size;
    GtkComboBoxText     *dropdown_type;
    int     type;
    long    width;
    long    height;
} Model;

typedef struct {
    GtkWindow           *window;
    GtkScrolledWindow   *scrl_out;
    GtkScrolledWindow   *scrl_in;
    GtkButton           *open;
    GtkButton           *launch;
    GtkButton           *step_f;
    GtkButton           *step_b;
    GtkButton           *color_button;
    GtkButton           *modelise;
    GtkWidget           *dfc;
    GtkWidget           *color_dialog;
    GtkSwitch           *switch_auto_analysis;
    GtkToggleButton     *rectif_button;
    GtkScale            *rotate_scale;
    GtkScale            *zoom_scale;
    GtkImage            *res_scale;
    GtkLabel            *output_label;
    double      rotation;
    double      zoom;
    int         state;
    gboolean    is_analysis_auto;
    gboolean    analysis_done;
    gboolean    is_step;
    Image       image_input;
    Image       image_output;
    Colors      colors;
    Rectif      rectif;
    Model       model;
    int         **bp;
    int         **tab;
    int         **h;
    int         **road_major;
    int         **road_minor;
    int         **river;
    int         **trail;
    int         **name;
    int         argc;
    char        **argv;
} Ui;


gboolean on_image_load(GtkButton *button __attribute((unused)), gpointer user_data);
gboolean on_switch_auto_analysis(GtkWidget *switch_auto __attribute__((unused)),
        gboolean state, gpointer user_data);
gboolean on_area_press(GtkWidget *area __attribute__((unused)),
        GdkEventButton *event, gpointer user_data);
gboolean on_switch_auto_rectif(GtkWidget *switch_auto __attribute__((unused)),
        gboolean state, gpointer user_data);
gboolean on_draw_input(GtkWidget *area, cairo_t *cr, gpointer user_data);
gboolean on_draw_output(GtkWidget *area, cairo_t *cr, gpointer user_data);
gboolean on_rotate_scale_change_value(GtkScale* scale, gpointer user_data);
gboolean on_zoom_scale_change_value(GtkScale* scale, gpointer user_data);
gboolean on_color(GtkButton *button, gpointer user_data);
gboolean on_color_ok(GtkButton *b __attribute__((unused)), gpointer user_data);
gboolean on_color_cancel(GtkButton *b __attribute__((unused)),
        gpointer user_data);
gboolean on_rectif_ok(GtkButton *b __attribute__((unused)), gpointer user_data);
gboolean on_rectif_cancel(GtkButton *b __attribute__((unused)),
        gpointer user_data);
gboolean on_rectif_done(GtkButton *b __attribute__((unused)),
        gpointer user_data);
gboolean on_launch(GtkButton *bt __attribute__((unused)), gpointer user_data);
gboolean on_step_forward(GtkButton *button, gpointer user_data);
gboolean on_step_backward(GtkButton *button, gpointer user_data);
gboolean on_modelise(GtkButton *button __attribute__((unused)), gpointer user_data);

#endif
