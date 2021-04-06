#include <gtk/gtk.h>
#include <cairo.h>
#include <string.h>
#include "../include/MapColorisation.h"
#include "../include/MapFilterColor.h"
#include "../include/MapRebuiltHoles.h"
#include "../include/tools.h"
#include "../include/vector.h"

typedef struct {
    GtkDrawingArea *area;
    cairo_surface_t *image_surface;
    GdkPixbuf *pixbuf;
    cairo_format_t format;
    int width;
    int height;
    char* filename;
} Image;

typedef struct {
    GtkWindow *window;
    GtkButton *open;
    GtkButton *launch;
    GtkButton *step;
    GtkButton *color_button;
    GtkWidget *dfc;
    GtkWidget *color_dialog;
    GtkScale *rotate_scale;
    GtkScale *zoom_scale;
    double rotation;
    double zoom;
    int state;
    GdkRGBA rgba;
    Image image_input;
    Image image_output;
    GtkButton *save;
} Ui;

// File --> Open
gboolean on_image_load(GtkButton *button, gpointer user_data)
{
    (void)button;
    Ui *ui = user_data;
    
    // Name of file to open from dialog box
    gchar *file_name = calloc(500, sizeof(char));
    
    GdkPixbuf *pixbuf_input_load = NULL;
    GdkPixbuf *pixbuf_output_load = NULL;
    GError *error = NULL;
    GError *error2 = NULL;

    // Show the "Open Image" dialog box
    gtk_widget_show(ui->dfc);

    // Check return value from Open Image dialog box to see if user clicked
    // the Open button
    if (gtk_dialog_run(GTK_DIALOG (ui->dfc)) == GTK_RESPONSE_OK) {
        // Get the file name from the dialog box
        file_name = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(ui->dfc));
        if (file_name != NULL) {
            //gtk_image_set_from_file(GTK_IMAGE(ui->img_main), file_name);
            pixbuf_input_load = gdk_pixbuf_new_from_file(file_name, &error);
            pixbuf_output_load = gdk_pixbuf_new_from_file(file_name, &error2);
            if (!error && !error2)
            {
                //Save the pixbuf in user data for input
                ui->image_input.pixbuf = pixbuf_input_load;
                ui->image_input.format = (gdk_pixbuf_get_has_alpha (ui->image_input.pixbuf)
                        ) ? CAIRO_FORMAT_ARGB32 : CAIRO_FORMAT_RGB24;
                ui->image_input.width = gdk_pixbuf_get_width(ui->image_input.pixbuf);
                ui->image_input.height = gdk_pixbuf_get_height(ui->image_input.pixbuf);

                //Same for output
                ui->image_output.pixbuf = pixbuf_output_load;
                ui->image_output.format = (gdk_pixbuf_get_has_alpha (
                            ui->image_output.pixbuf)) ? CAIRO_FORMAT_ARGB32 :
                    CAIRO_FORMAT_RGB24;
                ui->image_output.width = gdk_pixbuf_get_width(ui->image_output.pixbuf);
                ui->image_output.height = gdk_pixbuf_get_height(ui->image_output.pixbuf);

                // Resize the drawing area to trigger scrollbars
                gtk_widget_set_size_request(GTK_WIDGET(ui->image_input.area),
                        ui->image_input.width, ui->image_input.height);
                gtk_widget_set_size_request(GTK_WIDGET(ui->image_output.area),
                        ui->image_output.width, ui->image_output.height);
                
                //Create cairo surfaces for zoom and rotation
                ui->image_input.image_surface = cairo_image_surface_create(
                        ui->image_input.format, ui->image_input.width,
                        ui->image_input.height);
                ui->image_output.image_surface = cairo_image_surface_create(
                        ui->image_output.format, ui->image_output.width,
                        ui->image_output.height);

                cairo_t *cr = cairo_create(ui->image_input.image_surface);
                cairo_t *cr2 = cairo_create(ui->image_output.image_surface);
                
                //Links the surfaces to the pixbuf
                gdk_cairo_set_source_pixbuf(cr, ui->image_input.pixbuf, 0, 0);
                gdk_cairo_set_source_pixbuf(cr2, ui->image_output.pixbuf, 0, 0);

                //Draws the surfaces
                cairo_paint(cr);
                cairo_paint(cr2);

                gtk_widget_queue_draw_area(GTK_WIDGET(ui->image_input.area), 0,
                        0, ui->image_input.width, ui->image_input.height);
                gtk_widget_queue_draw_area(GTK_WIDGET(ui->image_output.area), 0,
                        0, ui->image_output.width, ui->image_output.height);

                strcpy(ui->image_input.filename, file_name);
                
                gtk_widget_set_sensitive(GTK_WIDGET(ui->launch), TRUE);
                gtk_widget_set_sensitive(GTK_WIDGET(ui->step), TRUE);
            }
            else
            {
                if (error)
                    g_critical(error->message);
                if (error2)
                    g_critical(error2->message);
            }
        }
    }

    free(file_name);
    free(error);
    free(error2);
    
    // Finished with the "Open Image" dialog box, so hide it
    gtk_widget_hide(ui->dfc);
    
    return TRUE;
}

gboolean on_draw_input(GtkWidget *area, cairo_t *cr, gpointer user_data)
{
    Ui *ui = user_data;

    if(ui->image_input.pixbuf != NULL)
    {
        gtk_widget_set_size_request(GTK_WIDGET(ui->image_input.area),
                ui->image_input.width * ui->zoom,
                ui->image_input.height * ui->zoom);
        int width = gtk_widget_get_allocated_width(GTK_WIDGET(area));
        int height = gtk_widget_get_allocated_height(GTK_WIDGET(area));

        cairo_translate(cr, width/2.0, height/2.0);
        cairo_rotate(cr, ui->rotation);
        cairo_scale(cr, ui->zoom, ui->zoom);
        cairo_translate(cr, -width/2.0, -height/2.0);

        cairo_set_source_surface(cr, ui->image_input.image_surface,
                width/2.0 - ui->image_input.width/2.0,
                height/2.0 - ui->image_input.height/2.0);
        cairo_paint(cr);
    }

    return FALSE;
}

gboolean on_draw_output(GtkWidget *area, cairo_t *cr, gpointer user_data)
{
    Ui *ui = user_data;
    if (ui->image_output.pixbuf != NULL)
    {
        gtk_widget_set_size_request(GTK_WIDGET(ui->image_output.area),
                ui->image_output.width * ui->zoom,
                ui->image_output.height * ui->zoom);
        int width = gtk_widget_get_allocated_width(GTK_WIDGET(area));
        int height = gtk_widget_get_allocated_height(GTK_WIDGET(area));

        cairo_translate(cr, width/2.0, height/2.0);
        cairo_rotate(cr, ui->rotation);
        cairo_scale(cr, ui->zoom, ui->zoom);
        cairo_translate(cr, -width/2.0, -height/2.0);

        cairo_set_source_surface(cr, ui->image_output.image_surface,
                width/2.0 - ui->image_output.width/2.0,
                height/2.0 - ui->image_output.height/2.0);
        cairo_paint(cr);
    }

    return FALSE;
}

// Handler for the rotate_scale
gboolean on_rotate_scale_change_value(GtkScale* scale, gpointer user_data)
{
    Ui *ui = user_data;
    
    // Saves the value of angle in radiants
    ui->rotation = gtk_range_get_value(GTK_RANGE(scale)) * G_PI / 180;

    int width_in = gtk_widget_get_allocated_width(GTK_WIDGET(ui->image_input.area));
    int height_in = gtk_widget_get_allocated_height(GTK_WIDGET(ui->image_input.area));
    int width_out = gtk_widget_get_allocated_width(GTK_WIDGET(ui->image_output.area));
    int height_out = gtk_widget_get_allocated_height(GTK_WIDGET(ui->image_output.area));

    // Redraws the rotated image
    gtk_widget_queue_draw_area(GTK_WIDGET(ui->image_input.area), 0, 0,
            width_in, height_in);
    gtk_widget_queue_draw_area(GTK_WIDGET(ui->image_output.area), 0, 0,
            width_out, height_out);

    return TRUE;
}

// Handler for the zoom_scale
gboolean on_zoom_scale_change_value(GtkScale* scale, gpointer user_data)
{
    Ui *ui = user_data;

    // Saves the value of zoom
    ui->zoom = gtk_range_get_value(GTK_RANGE(scale));
    
    int width_in = gtk_widget_get_allocated_width(GTK_WIDGET(ui->image_input.area));
    int height_in = gtk_widget_get_allocated_height(GTK_WIDGET(ui->image_input.area));
    int width_out = gtk_widget_get_allocated_width(GTK_WIDGET(ui->image_output.area));
    int height_out = gtk_widget_get_allocated_height(GTK_WIDGET(ui->image_output.area));

    // Redraws the zoomed image
    gtk_widget_queue_draw_area(GTK_WIDGET(ui->image_input.area), 0, 0,
            width_in, height_in);
    gtk_widget_queue_draw_area(GTK_WIDGET(ui->image_output.area), 0, 0,
            width_out, height_out);

    return TRUE;
}

/*
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
*/


gboolean on_color(GtkButton *button __attribute((unused)), gpointer user_data)
{
    Ui *ui = user_data;
    gtk_widget_show(ui->color_dialog);
    GdkRGBA rgba = {0, 0, 0, 0};

    if (gtk_dialog_run(GTK_DIALOG(ui->color_dialog)) == GTK_RESPONSE_OK)
    {
        GtkWidget *colorSelection =
            gtk_color_selection_dialog_get_color_selection(
                    GTK_COLOR_SELECTION_DIALOG(ui->color_dialog));
        
        gtk_color_selection_get_current_rgba(
                GTK_COLOR_SELECTION(colorSelection), &rgba);

        ui->rgba = rgba;
        
        printf("color:\nr: %f\ng: %f\nb: %f\na: %f\n", rgba.red,
                rgba.green, rgba.blue, rgba.alpha);
    }

    gtk_widget_hide(ui->color_dialog);

    return TRUE;
}

// Handler for the launch button
gboolean on_launch(GtkButton* button __attribute__((unused)), gpointer user_data)
{
    Ui *ui = user_data;

    SDL_Surface *image;
    image = SDL_LoadBMP(ui->image_input.filename);

    printf("filename: %s\n", ui->image_input.filename);

    SDL_Surface *test;
    test = SDL_LoadBMP("Pictures/map_ign_2.bmp");

    if(image == NULL)
    {
        printf("SDL_LoadBMP image failed: %s\n", SDL_GetError());
        return TRUE;
    }

    SDL_Surface *image2 = BMP_To_BW(image);
    if (image2 == NULL)
    {
        printf("SDL_LoadBMP image2 failed: %s\n", SDL_GetError());
        return TRUE;
    }

    double r = ui->rgba.red * 255,
           g = ui->rgba.green * 255,
           b = ui->rgba.blue * 255;

    //bmp_filter(image);
    
    GError *error = NULL;

    ui->image_output.pixbuf = gdk_pixbuf_new_from_file("Pictures/Results/image.bmp", &error);
    ui->image_output.format = (gdk_pixbuf_get_has_alpha (
                ui->image_output.pixbuf)) ? CAIRO_FORMAT_ARGB32 : 
        CAIRO_FORMAT_RGB24;
    ui->image_output.width = gdk_pixbuf_get_width(ui->image_output.pixbuf);
    ui->image_output.height = gdk_pixbuf_get_height(ui->image_output.pixbuf);
    
    cairo_t *cr2 = cairo_create(ui->image_output.image_surface);
                
    //Links the surfaces to the pixbuf
    gdk_cairo_set_source_pixbuf(cr2, ui->image_output.pixbuf, 0, 0);

    //Draws the surfaces
    cairo_paint(cr2);
    
    gtk_widget_queue_draw_area(GTK_WIDGET(ui->image_output.area), 0,
                        0, ui->image_output.width, ui->image_output.height);

    /*
    Uint32 src_format = image->format->format;
    Uint32 dst_format;

    gboolean has_alpha = SDL_ISPIXELFORMAT_ALPHA(src_format);
    if (has_alpha)
        dst_format = SDL_PIXELFORMAT_RGBA32;
    else
        dst_format = SDL_PIXELFORMAT_RGB24;
    
    int rowstride = gdk_pixbuf_get_rowstride(ui->image_output.pixbuf);
    guchar *pixels;

    pixels = gdk_pixbuf_get_pixels(ui->image_output.pixbuf);

    SDL_LockSurface(image);

    SDL_ConvertPixels(image->w, image->h, src_format,
            image->pixels, image->pitch,
            dst_format, pixels, rowstride);
    SDL_UnlockSurface(image);

    int width_out = gtk_widget_get_allocated_width(GTK_WIDGET(ui->image_output.area));
    int height_out = gtk_widget_get_allocated_height(GTK_WIDGET(ui->image_output.area));

    gtk_widget_queue_draw_area(GTK_WIDGET(ui->image_output.area), 0, 0,
            width_out, height_out);
    */

    SDL_FreeSurface(image);
    SDL_FreeSurface(test);

    return TRUE;
}

gboolean on_step(GtkButton* button __attribute__((unused)), gpointer user_data)
{
    Ui *ui = user_data;

    switch(ui->state)
    {
        case 0:
            break;
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        default:
            break;
    }

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
    if (gtk_builder_add_from_file(builder, "Application/app.glade", &error) == 0)
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
    GtkButton* step = GTK_BUTTON(gtk_builder_get_object(builder, "step"));
    GtkButton* save = GTK_BUTTON(gtk_builder_get_object(builder, "save_text"));
    GtkScale* rotate_scale = GTK_SCALE(gtk_builder_get_object(builder,
                "rotate_scale"));
    GtkScale* zoom_scale = GTK_SCALE(gtk_builder_get_object(builder,
                "zoom_scale"));
    GtkDrawingArea *area_input = GTK_DRAWING_AREA(gtk_builder_get_object(builder,
                "image_input"));
    GtkDrawingArea *area_output = GTK_DRAWING_AREA(gtk_builder_get_object(
                builder, "image_output"));
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
        .zoom = 1.00,//gtk_range_get_value(GTK_RANGE(zoom_scale)),
        .rotation = 0,//gtk_range_get_value(GTK_RANGE(zoom_scale)),
        .launch = launch,
        .step = step,
        .save = save,
        .state = 0,
        .rgba =
        {
            .red = -1,
            .green = -1,
            .blue = -1
        },
        .image_input =
        {
            .area = area_input,
            .filename = calloc(500, sizeof(char))
        },
        .image_output = 
        {
            .area = area_output,
            .filename = NULL
        }
    };

    // Connects signal handlers.
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(open, "clicked", G_CALLBACK(on_image_load), &ui);
    g_signal_connect(rotate_scale, "value-changed", G_CALLBACK(
                on_rotate_scale_change_value), &ui);
    g_signal_connect(zoom_scale, "value-changed", G_CALLBACK(
                on_zoom_scale_change_value), &ui);
    g_signal_connect(area_input, "draw", G_CALLBACK(on_draw_input), &ui);
    g_signal_connect(area_output, "draw", G_CALLBACK(on_draw_output), &ui);
    //g_signal_connect(save, "clicked", G_CALLBACK(on_save), &ui);
    g_signal_connect(launch, "clicked", G_CALLBACK(on_launch), &ui);
    g_signal_connect(step, "clicked", G_CALLBACK(on_step), &ui);
    g_signal_connect(color_button, "clicked", G_CALLBACK(on_color), &ui);

    // Frees builder
    g_object_unref(builder);

    // Shows the window
    gtk_widget_show(GTK_WIDGET(window));

    // Runs the main loop.
    gtk_main();

    free(ui.image_input.filename);

    // Exits.
    return 0;
}

