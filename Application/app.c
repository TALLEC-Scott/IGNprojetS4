#include <gtk/gtk.h>
#include <cairo.h>
#include <SDL/SDL.h>
#include <string.h>

typedef struct {
    GtkDrawingArea *area;
    cairo_surface_t *image_surface;
    GdkPixbuf *pixbuf;
    cairo_format_t format;
    double rotation;
    double zoom;
    int width;
    int height;
    char* filename;
} Image;

typedef struct {
    GtkWindow *window;
    GtkButton *open;
    GtkButton *launch;
    GtkButton *color_button;
    GtkWidget *dfc;
    GtkWidget *color_dialog;
    GtkScale *rotate_scale;
    GtkScale *zoom_scale;
    Image image;
    GtkTextView *outputText;
    GtkButton *save;
} Ui;

// File --> Open
gboolean on_image_load(GtkButton *button, gpointer user_data)
{
    (void)button;
    Ui *ui = user_data;
    
    // Name of file to open from dialog box
    gchar *file_name = calloc(200, sizeof(char));
    
    GdkPixbuf *pixbuf_load = NULL;
    GError *error = NULL;

    // Show the "Open Image" dialog box
    gtk_widget_show(ui->dfc);

    // Check return value from Open Image dialog box to see if user clicked
    // the Open button
    if (gtk_dialog_run(GTK_DIALOG (ui->dfc)) == GTK_RESPONSE_OK) {
        // Get the file name from the dialog box
        file_name = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(ui->dfc));
        if (file_name != NULL) {
            //gtk_image_set_from_file(GTK_IMAGE(ui->img_main), file_name);
            pixbuf_load = gdk_pixbuf_new_from_file(file_name, &error);
            if (!error)
            {
                //Save the pixbuf in user data
                ui->image.pixbuf = pixbuf_load;
                ui->image.format = (gdk_pixbuf_get_has_alpha (ui->image.pixbuf)
                        ) ? CAIRO_FORMAT_ARGB32 : CAIRO_FORMAT_RGB24;
                ui->image.width = gdk_pixbuf_get_width(ui->image.pixbuf);
                ui->image.height = gdk_pixbuf_get_height(ui->image.pixbuf);

                // Resize the drawing area to trigger scrollbars
                gtk_widget_set_size_request(GTK_WIDGET(ui->image.area),
                        ui->image.width, ui->image.height);

                ui->image.image_surface = cairo_image_surface_create(
                        ui->image.format, ui->image.width, ui->image.height);

                cairo_t *cr = cairo_create(ui->image.image_surface);
                gdk_cairo_set_source_pixbuf(cr, ui->image.pixbuf, 0, 0);

                cairo_paint(cr);

                gtk_widget_queue_draw_area(GTK_WIDGET(ui->image.area), 0, 0,
                        ui->image.width, ui->image.height);
                
                strcpy(ui->image.filename, file_name);
                
                gtk_widget_set_sensitive(GTK_WIDGET(ui->launch), TRUE);
            }
            else
                g_critical(error->message);
        }
    }

    free(file_name);
    free(error);
    
    // Finished with the "Open Image" dialog box, so hide it
    gtk_widget_hide(ui->dfc);
    
    return TRUE;
}

gboolean on_draw(GtkWidget *area, cairo_t *cr, gpointer user_data)
{
    Ui *ui = user_data;

    if(ui->image.pixbuf != NULL)
    {
        int width = gtk_widget_get_allocated_width(GTK_WIDGET(area));
        int height = gtk_widget_get_allocated_height(GTK_WIDGET(area));

        cairo_translate(cr, width/2.0, height/2.0);
        cairo_rotate(cr, ui->image.rotation);
        cairo_scale(cr, ui->image.zoom, ui->image.zoom);
        cairo_translate(cr, -width/2.0, -height/2.0);

        cairo_set_source_surface(cr, ui->image.image_surface,
                width/2.0 - ui->image.width/2.0,
                height/2.0 - ui->image.height/2.0);
        cairo_paint(cr);
    }

    return FALSE;
}

// Handler for the rotate_scale
gboolean on_rotate_scale_change_value(GtkScale* scale, gpointer user_data)
{
    Ui *ui = user_data;
    
    // Saves the value of angle in radiants
    ui->image.rotation = gtk_range_get_value(GTK_RANGE(scale)) * G_PI / 180;

    int width = gtk_widget_get_allocated_width(GTK_WIDGET(ui->image.area));
    int height = gtk_widget_get_allocated_height(GTK_WIDGET(ui->image.area));

    // Redraws the rotated image
    gtk_widget_queue_draw_area(GTK_WIDGET(ui->image.area), 0, 0,
            width, height);

    return TRUE;
}

// Handler for the zoom_scale
gboolean on_zoom_scale_change_value(GtkScale* scale, gpointer user_data)
{
    Ui *ui = user_data;

    // Saves the value of zoom
    ui->image.zoom = gtk_range_get_value(GTK_RANGE(scale));
    
    int width = gtk_widget_get_allocated_width(GTK_WIDGET(ui->image.area));
    int height = gtk_widget_get_allocated_height(GTK_WIDGET(ui->image.area));

    // Redraws the zoomed image
    gtk_widget_queue_draw_area(GTK_WIDGET(ui->image.area), 0, 0,
            width, height);

    return TRUE;
}

// Handler for the save button
gboolean on_save(GtkButton *button, gpointer user_data)
{
    (void)button;
    Ui *ui = user_data;

    // Creates the dialog
    GtkWidget *dialog;
    dialog = gtk_file_chooser_dialog_new ("Save",
            NULL,
            GTK_FILE_CHOOSER_ACTION_SAVE,
            "_Cancel", GTK_RESPONSE_CANCEL,
            "_Save", GTK_RESPONSE_ACCEPT,
            NULL);
    
    // Runs the dialog
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
    {
        char* filename;
        char* text;
        GtkTextBuffer *buffer;
        GtkTextIter start;
        GtkTextIter end;
        GError *err = NULL;
        gboolean result;

        filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

        // Lock textView while editing
        gtk_widget_set_sensitive(GTK_WIDGET(ui->outputText), FALSE);

        buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(ui->outputText));
        gtk_text_buffer_get_bounds(buffer, &start, &end);
        text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
        gtk_text_buffer_set_modified(buffer, FALSE);
        
        // Unlock textView
        gtk_widget_set_sensitive(GTK_WIDGET(ui->outputText), TRUE);
        
        // Saves text to file
        result = g_file_set_contents(filename, text, -1, &err);

        if(result == FALSE)
        {
            g_printerr("Error saving file : %s\n", err->message);
            g_clear_error(&err);
        }

        free(filename);
        free(text);
        free(err);
    }

    // Destroys and free the dialog
    gtk_widget_destroy(dialog);
        
    return TRUE;
}

gboolean on_color(GtkButton *button __attribute((unused)), gpointer user_data)
{
    Ui *ui = user_data;
    gtk_widget_show(ui->color_dialog);

    if (gtk_dialog_run(GTK_DIALOG(ui->color_dialog)) == GTK_RESPONSE_OK)
    {
        printf("color selected\nnot used (TODO)\n");
        fflush(stdout);
    }


    gtk_widget_hide(ui->color_dialog);

    return TRUE;
}

// Handler for the launch button
gboolean on_launch(GtkButton* button __attribute__((unused)), gpointer user_data __attribute((unused)))
{

    return TRUE;
}

int main (int argc, char *argv[])
{
    // App only launches whitout parameters
    if(argc > 1)
    {
        size_t i = 1;
        printf("Incorrect parameters:");
        while(argv[i])
            printf(" %s", argv[i++]);
        printf("\nUsage: %s\n", argv[0]);
        return 1;
    }

    // Initializes GTK
    gtk_init(NULL, NULL);

    // Gets gtk builder from the glade file
    GtkBuilder* builder = gtk_builder_new();
    GError* error = NULL;
    if (gtk_builder_add_from_file(builder, "app.glade", &error) == 0)
    {
        g_printerr("Error loading file: %s\n", error->message);
        g_clear_error(&error);
        return 1;
    }

    // Gets the widgets.
    GtkWindow* window = GTK_WINDOW(gtk_builder_get_object(builder,
                "window_main"));
    GtkWidget* dfc = GTK_WIDGET(gtk_builder_get_object(builder,
                "dlg_file_choose"));
    GtkButton* open = GTK_BUTTON(gtk_builder_get_object(builder, "open"));
    GtkButton* launch = GTK_BUTTON(gtk_builder_get_object(builder,
                "launch_analysis"));
    GtkButton* save = GTK_BUTTON(gtk_builder_get_object(builder, "save_text"));
    GtkScale* rotate_scale = GTK_SCALE(gtk_builder_get_object(builder,
                "rotate_scale"));
    GtkScale* zoom_scale = GTK_SCALE(gtk_builder_get_object(builder,
                "zoom_scale"));
    GtkDrawingArea *area = GTK_DRAWING_AREA(gtk_builder_get_object(builder,
                "image"));
    GtkTextView *outputText = GTK_TEXT_VIEW(gtk_builder_get_object(builder,
                "output_text"));
    GtkButton *color_button = GTK_BUTTON(gtk_builder_get_object(builder,
                "color"));
    GtkWidget *color_dialog = GTK_WIDGET(gtk_builder_get_object(builder,
                "color_dialog"));

    // Initialise data structure
    Ui ui =
    {
        .window = window,
        .open = open,
        .dfc = dfc,
        .color_button = color_button,
        .color_dialog = color_dialog,
        .rotate_scale = rotate_scale,
        .zoom_scale = zoom_scale,
        .launch = launch,
        .save = save,
        .outputText = outputText,
        .image =
        {
            .area = area,
            .rotation = 0,
            .zoom = 1.00,
            .filename = calloc(200, sizeof(char))
        }
    }; 

    // Connects signal handlers.
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(open, "clicked", G_CALLBACK(on_image_load), &ui);
    g_signal_connect(rotate_scale, "value-changed", G_CALLBACK(
                on_rotate_scale_change_value), &ui);
    g_signal_connect(zoom_scale, "value-changed", G_CALLBACK(
                on_zoom_scale_change_value), &ui);
    g_signal_connect(area, "draw", G_CALLBACK(on_draw), &ui);
    g_signal_connect(save, "clicked", G_CALLBACK(on_save), &ui);
    g_signal_connect(launch, "clicked", G_CALLBACK(on_launch), &ui);
    g_signal_connect(color_button, "clicked", G_CALLBACK(on_color), &ui);

    // Frees builder
    g_object_unref(builder);

    // Shows the window
    gtk_widget_show(GTK_WIDGET(window));

    // Runs the main loop.
    gtk_main();

    free(ui.image.filename);

    // Exits.
    return 0;
}

