#include <string.h>
#include <stdlib.h>
#include "app.h"
#include "tools.h"
#include "MapColorisation.h"
#include "MapFilterColor.h"
#include "MapRebuiltHoles.h"
#include "vector.h"
#include "Open_GL_exec.h"



// File --> Open
gboolean on_image_load(GtkButton *button __attribute((unused)), gpointer user_data)
{
    Ui *ui = user_data;
    
    // Name of file to open from dialog box
    gchar *file_name = calloc(500, sizeof(char));
    
    // Show the "Open Image" dialog box
    gtk_widget_show(ui->dfc);

    // Check return value from Open Image dialog box to see if user clicked
    // the Open button
    if (gtk_dialog_run(GTK_DIALOG (ui->dfc)) == GTK_RESPONSE_OK) {
        // Get the file name from the dialog box
        file_name = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(ui->dfc));
        if (file_name != NULL) {
            load_image(&ui->image_input, file_name);
            load_image(&ui->image_output, file_name);

            strcpy(ui->image_input.filename, file_name);
           
            if (gtk_toggle_button_get_active(ui->rectif_button))
                gtk_toggle_button_set_active(ui->rectif_button, FALSE);

            ui->image_input.shortname = ui->image_input.filename +
               strlen(ui->image_input.filename) - 1;
            
            while (*ui->image_input.shortname != '/' && 
                    ui->image_input.shortname > ui->image_input.filename)
                ui->image_input.shortname--;
            ui->image_input.shortname++;

            gtk_label_set_text(ui->output_label, ui->image_input.shortname);

            gtk_widget_set_sensitive(GTK_WIDGET(ui->switch_auto_analysis),
                    TRUE);
            gtk_widget_set_sensitive(GTK_WIDGET(ui->launch), TRUE);
            gtk_widget_set_sensitive(GTK_WIDGET(ui->step_f), TRUE);
            gtk_widget_set_sensitive(GTK_WIDGET(ui->step_b), FALSE);
            gtk_widget_set_sensitive(GTK_WIDGET(ui->rectif_button), FALSE);
            gtk_widget_set_sensitive(GTK_WIDGET(ui->modelise), FALSE);

            gtk_switch_set_active(ui->switch_auto_analysis, TRUE);
        }
    }

    ui->analysis_done = FALSE;
    ui->state = 0;

    if (ui->bp != NULL)
        free(ui->bp);
    if (ui->tab != NULL)
        free(ui->tab);
    if (ui->h)
        free(ui->h);
    if (ui->road_major != NULL)
        free(ui->road_major);
    if (ui->road_minor != NULL)
        free(ui->road_minor);
    if (ui->river != NULL)
        free(ui->river);
    if (ui->trail != NULL)
        free(ui->trail);


    // Finished with the "Open Image" dialog box, so hide it
    gtk_widget_hide(ui->dfc);
    
    return TRUE;
}

// restore default colors for topo, road and river
void reset_colors_to_default(Ui *ui)
{
    ui->colors.topo = ui->colors.default_color;
    ui->colors.road = ui->colors.default_color;
    ui->colors.river = ui->colors.default_color;
}

// Display a secondary window with three color pickers for topo, road and river
gboolean on_switch_auto_analysis(GtkWidget *switch_auto __attribute__((unused)),
        gboolean state, gpointer user_data)
{
    Ui *ui = user_data;

    // updates the auto boolean and the linked window (display or not)
    if (state == TRUE)
    {
        gtk_widget_hide(GTK_WIDGET(ui->colors.wcb));
        ui->is_analysis_auto = 1;

        reset_colors_to_default(ui);

        gtk_widget_set_sensitive(GTK_WIDGET(ui->step_f), TRUE);
        gtk_widget_set_sensitive(GTK_WIDGET(ui->launch), TRUE);

        return TRUE;
    }
    else
    {
        ui->is_analysis_auto = 0;
        gtk_widget_show(GTK_WIDGET(ui->colors.wcb));

        gtk_widget_set_sensitive(GTK_WIDGET(ui->step_f), FALSE);
        gtk_widget_set_sensitive(GTK_WIDGET(ui->launch), FALSE);
    }

    return TRUE;
}

// Handler for switch controlling automatic rectification
gboolean on_switch_auto_rectif(GtkWidget *switch_auto __attribute__((unused)),
        gboolean state, gpointer user_data)
{
    Ui *ui = user_data;
   if (state == TRUE)
       printf("rectif %d\n", gtk_switch_get_active(ui->rectif.switch_auto));
   else
       printf("rectif %d\n", gtk_switch_get_active(ui->rectif.switch_auto));

    return TRUE;
}

// Handler for manual elevation rectification
gboolean on_area_press(GtkWidget *area,
        GdkEventButton *event, gpointer user_data)
{
    Ui *ui = user_data;

    // new labels
    char    x_lab[25];
    char    y_lab[25];

    // get scrollbars position and add cursor position
    gdouble x = gtk_adjustment_get_value(gtk_scrolled_window_get_hadjustment(
                ui->scrl_out)) + event->x;
    gdouble y = gtk_adjustment_get_value(gtk_scrolled_window_get_vadjustment(
                ui->scrl_out)) + event->y;

    int width = gtk_widget_get_allocated_width(GTK_WIDGET(area));
    int height = gtk_widget_get_allocated_height(GTK_WIDGET(area));

    if (height > ui->image_output.height)
        y -= (height - ui->image_output.height) / 2;
    if (width > ui->image_output.width)
        x -= (width - ui->image_output.width) / 2;

    if (y > ui->image_output.height || x > ui->image_output.width ||
            y < 0 || x < 0)
        return TRUE;

    // loads click coordinates
    sprintf(x_lab, "X: %f", x);
    sprintf(y_lab, "Y: %f", y);

    // sets the text on both labels
    gtk_label_set_text(ui->rectif.x_label, x_lab);
    gtk_label_set_text(ui->rectif.y_label, y_lab);

    // Save coordinates in ui
    ui->rectif.x_pos = x;
    ui->rectif.y_pos = y;

    printf("x = %f, y = %f\n", x, y);

    return TRUE;
}

// Handler for manual rectification, display secondary window
gboolean on_rectif_button(GtkToggleButton *tbutton, gpointer user_data)
{
    Ui *ui = user_data;

    // change altitude
    if (gtk_toggle_button_get_active(tbutton))
    {
        // connects the signal for click coordinates
        ui->rectif.handler_id = g_signal_connect(ui->rectif.output_event_box,
                "button-press-event", G_CALLBACK(on_area_press), ui);

        // shows the secondary window
        gtk_widget_show(GTK_WIDGET(ui->rectif.window));

        gtk_window_present(ui->window);

        // restore defaults value for zoom and rotation
        gtk_range_set_value(GTK_RANGE(ui->rotate_scale), 0.00);
        gtk_range_set_value(GTK_RANGE(ui->zoom_scale), 1.00);

        // deactivate the scales and modelisation button
        gtk_widget_set_sensitive(GTK_WIDGET(ui->modelise), FALSE);
        gtk_widget_set_sensitive(GTK_WIDGET(ui->rotate_scale), FALSE);
        gtk_widget_set_sensitive(GTK_WIDGET(ui->zoom_scale), FALSE);
    }
    // finished changing
    else
    {
        // Error a signal should have been connected
        if (ui->rectif.handler_id == 0)
        {
            printf("huh oh, can't get click handler id,\
something went wrong\n");
        }
        else
        {
            // disconnect the click handler for coordinates
            g_signal_handler_disconnect(ui->rectif.output_event_box,
                ui->rectif.handler_id);
        }
        
        // restore sensitivity on zoom, rotation and modelisation button
        gtk_widget_set_sensitive(GTK_WIDGET(ui->rotate_scale), TRUE);
        gtk_widget_set_sensitive(GTK_WIDGET(ui->zoom_scale), TRUE);
        gtk_widget_set_sensitive(GTK_WIDGET(ui->modelise), TRUE);
       
        // closes the secondary window
        gtk_widget_hide(GTK_WIDGET(ui->rectif.window));
    }

    return TRUE;
}

// updates the input image & calls for redraw 
gboolean on_draw_input(GtkWidget *area, cairo_t *cr, gpointer user_data)
{
    Ui *ui = user_data;

    // if image exists
    if(ui->image_input.pixbuf != NULL)
    {
        // resize to trigger handles
        gtk_widget_set_size_request(GTK_WIDGET(ui->image_input.area),
                ui->image_input.width * ui->zoom,
                ui->image_input.height * ui->zoom);
        int width = gtk_widget_get_allocated_width(GTK_WIDGET(area));
        int height = gtk_widget_get_allocated_height(GTK_WIDGET(area));

        // translate for scale and rotation
        cairo_translate(cr, width/2.0, height/2.0);
        cairo_rotate(cr, ui->rotation);
        cairo_scale(cr, ui->zoom, ui->zoom);
        cairo_translate(cr, -width/2.0, -height/2.0);

        // links the surface to the image and paints
        cairo_set_source_surface(cr, ui->image_input.image_surface,
                width/2.0 - ui->image_input.width/2.0,
                height/2.0 - ui->image_input.height/2.0);
        cairo_paint(cr);
    }

    return FALSE;
}

// updates the output image & calls for redraw
gboolean on_draw_output(GtkWidget *area, cairo_t *cr, gpointer user_data)
{
    Ui *ui = user_data;
    if (ui->image_output.pixbuf != NULL)
    {
        // resize to trigger handles 
        gtk_widget_set_size_request(GTK_WIDGET(ui->image_output.area),
                ui->image_output.width * ui->zoom,
                ui->image_output.height * ui->zoom);
        int width = gtk_widget_get_allocated_width(GTK_WIDGET(area));
        int height = gtk_widget_get_allocated_height(GTK_WIDGET(area));

        // translate for scale and rotation
        cairo_translate(cr, width/2.0, height/2.0);
        cairo_rotate(cr, ui->rotation);
        cairo_scale(cr, ui->zoom, ui->zoom);
        cairo_translate(cr, -width/2.0, -height/2.0);

        // links the surface to the image for redraw
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

// Handles the color picker for the three colors in HSV format
gboolean on_color(GtkButton *button, gpointer user_data)
{
    Ui *ui = user_data;
    GdkRGBA color = {0, 0, 0, 0};
    const gchar *label;
    
    gtk_widget_show(ui->color_dialog);

    if (gtk_dialog_run(GTK_DIALOG(ui->color_dialog)) == GTK_RESPONSE_OK)
    {
        GtkWidget *colorSelection =
            gtk_color_selection_dialog_get_color_selection(
                    GTK_COLOR_SELECTION_DIALOG(ui->color_dialog));
        
        gtk_color_selection_get_current_rgba(
                GTK_COLOR_SELECTION(colorSelection), &color);

        label = gtk_button_get_label(button);

        if (strcmp(label, "Topographic Line Color") == 0)
        {
            ui->colors.topo = color;
            ui->colors.topo_was_set = TRUE;
        }
        else if (strcmp(label, "Road Color") == 0)
        {
            ui->colors.road = color;
            ui->colors.road_was_set = TRUE;
        }
        else if (strcmp(label, "River Color") == 0)
        {
            ui->colors.river = color;
            ui->colors.river_was_set = TRUE;
        }
        else
            printf("How in Hell did you manage to click on a button that \
doesn't exists ?!?\n");

        
        printf("color:\nr: %f\ng: %f\nb: %f\na: %f\n", color.red,
                color.green, color.blue, color.alpha);
    }

    gtk_widget_hide(ui->color_dialog);

    return TRUE;
}

// Handler for OK button in color window
gboolean on_color_ok(GtkButton *b __attribute__((unused)), gpointer user_data)
{
    Ui *ui = user_data;

    if (!(ui->colors.topo_was_set && ui->colors.road_was_set &&
            ui->colors.river_was_set))
    {
        GtkWidget *message = gtk_message_dialog_new(ui->window,
                GTK_DIALOG_MODAL,
                GTK_MESSAGE_WARNING,
                GTK_BUTTONS_OK,
                "Dear user, all colors must be set for the analysis.\n"
                "Please check that you have set each"
                "colors before continuing.");

        gtk_dialog_run(GTK_DIALOG(message));

        gtk_widget_destroy(message);
    }
    else
    {
        gtk_widget_set_sensitive(GTK_WIDGET(ui->step_f), TRUE);
        gtk_widget_set_sensitive(GTK_WIDGET(ui->launch), TRUE);

        gtk_widget_hide(GTK_WIDGET(ui->colors.wcb));
    }

    return TRUE;
}

// Handler for cancel button in color window
gboolean on_color_cancel(GtkButton *b __attribute__((unused)),
        gpointer user_data)
{
    Ui *ui = user_data;

    gtk_widget_hide(GTK_WIDGET(ui->colors.wcb));

    gtk_switch_set_active(ui->switch_auto_analysis, TRUE);

    reset_colors_to_default(ui);

    gtk_widget_set_sensitive(GTK_WIDGET(ui->step_f), TRUE);
    gtk_widget_set_sensitive(GTK_WIDGET(ui->launch), TRUE);
    
    return TRUE;
}

// Handler for ok button in rectif window
gboolean on_rectif_ok(GtkButton *b __attribute__((unused)), gpointer user_data)
{
    Ui *ui = user_data;
    
    const gchar *text = gtk_entry_buffer_get_text(ui->rectif.coord_text_buff);
    
    if (ui->rectif.x_pos == -1 || strlen(text) == 0)
    {
        GtkWidget *message = gtk_message_dialog_new(ui->window,
                GTK_DIALOG_MODAL,
                GTK_MESSAGE_WARNING,
                GTK_BUTTONS_OK,
                "Dear user, please note that to change "
                "the altitude a topographic line must be selected by clicking "
                "on the right image and the new altitude must be provided");
        
        gtk_dialog_run(GTK_DIALOG(message));

        gtk_widget_destroy(message);
        
        return TRUE;
    }


    // Tests if the text is a number, exits if false
    for (size_t i = 0; i < strlen(text); i++)
    {
        if (!isdigit(text[i]))
        {
            printf("Altitude = Number, moron!\n");
            return TRUE;
        }
    }

    int altitude;

    // reads a number in the text buffer
    sscanf(text, "%d", &altitude);

    printf("New altitude = %d\n", altitude);
    int x_int = (int)ui->rectif.x_pos;
    int y_int = (int)ui->rectif.y_pos;

    SDL_Surface *image_elevation;
    image_elevation = SDL_LoadBMP("Pictures/Results/image.bmp");
    if(image_elevation == NULL)
    {
        printf("SDL_LoadBMP image failed: %s\n", SDL_GetError());
        return TRUE;
    }

    // switch get active is reversed, its normal
    map_set_altitude(image_elevation, ui->h, ui->tab, x_int, y_int, altitude,
            ui->image_output.width, ui->image_output.height, 
            !gtk_switch_get_active(ui->rectif.switch_auto));

    map_update_bp(ui->h, ui->bp, ui->image_output.width,
        ui->image_output.height);

    load_image(&ui->image_output, "Pictures/Results/elevation.bmp");

    return TRUE;
}

// Handler for cancel button in rectif window
gboolean on_rectif_cancel(GtkButton *b __attribute__((unused)),
        gpointer user_data)
{
    Ui *ui = user_data;

    //gtk_widget_hide(GTK_WIDGET(ui->rectif.window));

    gtk_toggle_button_set_active(ui->rectif_button, FALSE);

    return TRUE;
}

// Handler for done button in rectif window
gboolean on_rectif_done(GtkButton *b __attribute__((unused)),
        gpointer user_data)
{
    Ui *ui = user_data;

    //gtk_widget_hide(GTK_WIDGET(ui->rectif.window));

    gtk_toggle_button_set_active(ui->rectif_button, FALSE);

    //gtk_widget_set_sensitive(GTK_WIDGET(ui->modelise), TRUE);

    return TRUE;
}

// Handler for the launch button
gboolean on_launch(GtkButton *bt __attribute__((unused)), gpointer user_data)
{
    Ui *ui = user_data;

    if (!ui->analysis_done)
    {
        SDL_Surface *image;
        image = SDL_LoadBMP(ui->image_input.filename);

        printf("filename: %s\n", ui->image_input.filename);

        //SDL_Surface *test;
        //test = SDL_LoadBMP("/home/yann/Documents/EPITA/Cours/2020_spe/s4/IGNprojetS4/Pictures/map_ign_2.bmp");

        if(image == NULL)
        {
            printf("SDL_LoadBMP image failed: %s\n", SDL_GetError());
            return TRUE;
        }
        
        
        double r = ui->colors.topo.red * 255,
               g = ui->colors.topo.green * 255,
               b = ui->colors.topo.blue * 255,
               r1 = ui->colors.road.red * 255,
               g1 = ui->colors.road.green * 255,
               b1 = ui->colors.road.blue * 255,
               r2 = ui->colors.river.red * 255,
               g2 = ui->colors.river.green * 255,
               b2 = ui->colors.river.blue * 255;


        ui->bp = (int**)calloc(image->w, sizeof(int*));
        ui->tab = (int**)calloc(image->w, sizeof(int*));
        ui->h = (int**)calloc(image->w, sizeof(int*));
        ui->road_major = (int**)calloc(image->w, sizeof(int*));
        ui->road_minor = (int**)calloc(image->w, sizeof(int*));
        ui->river = (int**)calloc(image->w, sizeof(int*));
        ui->trail = (int**)calloc(image->w, sizeof(int*));
        ui->name = (int**)calloc(image->w, sizeof(int*));


        for(int k = 0; k < image->w; k++)
        {
          ui->bp[k] = (int*)calloc(image->h, sizeof(int));
          ui->tab[k] = (int*)calloc(image->h, sizeof(int));
          ui->h[k] = (int*)calloc(image->h, sizeof(int));
          ui->road_major[k] = (int*)calloc(image->h, sizeof(int*));
          ui->road_minor[k] = (int*)calloc(image->h, sizeof(int*));
          ui->river[k] = (int*)calloc(image->h, sizeof(int*));
          ui->trail[k] = (int*)calloc(image->h, sizeof(int*));
          ui->name[k] = (int*)calloc(image->h, sizeof(int*));
        }


        bmp_filter(image, r, g, b,
               r1, g1, b1,
              r2, g2, b2,
              ui->bp, ui->tab, ui->h,
              ui->road_major, ui->road_minor,
              ui->river, ui->trail, ui->name);
        
        SDL_FreeSurface(image);
    }

    load_image(&ui->image_output, "Pictures/Results/image.bmp");

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

    if (!ui->is_step || ui->analysis_done)
    {
        ui->state = 9;
        gtk_widget_set_sensitive(GTK_WIDGET(ui->step_f), FALSE);
        gtk_widget_set_sensitive(GTK_WIDGET(ui->step_b), TRUE);
    }

    ui->analysis_done = TRUE;

    gtk_widget_set_sensitive(GTK_WIDGET(ui->rectif_button), TRUE);
    gtk_widget_set_sensitive(GTK_WIDGET(ui->modelise), TRUE);


    //SDL_FreeSurface(test);

    return TRUE;
}

gboolean on_step_forward(GtkButton *button, gpointer user_data)
{
    Ui *ui = user_data;

    char file[500];
    char *dir = "Pictures/Results/";

    switch(ui->state)
    {
        case 0:
            ui->is_step = TRUE;
            if (!ui->analysis_done)
                on_launch(NULL, ui);
            sprintf(file, "%stopo.bmp", dir);
            gtk_label_set_text(ui->output_label, "Topographic lines");
            ui->state++;
            gtk_widget_set_sensitive(GTK_WIDGET(ui->step_b), TRUE);
            break;
        case 1:
            sprintf(file, "%sroad_major.bmp", dir);
            gtk_label_set_text(ui->output_label, "Major roads");
            ui->state++;
            break;
        case 2:
            sprintf(file, "%sroad_minor.bmp", dir);
            gtk_label_set_text(ui->output_label, "Minor roads");
            ui->state++;
            break;
        case 3:
            sprintf(file, "%strail.bmp", dir);
            gtk_label_set_text(ui->output_label, "Trails");
            ui->state++;
            break;
        case 4:
            sprintf(file, "%sriver.bmp", dir);
            gtk_label_set_text(ui->output_label, "Rivers");
            ui->state++;
            break;
        case 5:
            sprintf(file, "%sign.bmp", dir);
            gtk_label_set_text(ui->output_label, "ign");
            ui->state++;
            break;
        case 6:
            sprintf(file, "%sneigh.bmp", dir);
            gtk_label_set_text(ui->output_label, "Ends of lines");
            ui->state++;
            break;
        case 7:
            sprintf(file, "%sholes.bmp", dir);
            gtk_label_set_text(ui->output_label, "Holes");
            ui->state++;
            break;
        case 8:
            sprintf(file, "%simage.bmp", dir);
            gtk_label_set_text(ui->output_label, "Result");
            gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);
            ui->state++;
            break;
        default:
            printf("Step by step forward: state > 8 should not be possible\n");
            break;
    }

    load_image(&ui->image_output, file);

    return TRUE;
}

// Handler for step backward button
gboolean on_step_backward(GtkButton *button, gpointer user_data)
{
    Ui *ui = user_data;
    
    char file[500];
    char *dir = "Pictures/Results/";

    switch(ui->state)
    {
        case 1:
            gtk_label_set_text(ui->output_label, ui->image_input.shortname);
            sprintf(file, "%s", ui->image_input.filename);
            gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);
            ui->state--;
            break;
        case 2:
            sprintf(file, "%stopo.bmp", dir);
            gtk_label_set_text(ui->output_label, "Topographic lines");
            ui->state--;
            break;
        case 3:
            sprintf(file, "%sroad_major.bmp", dir);
            gtk_label_set_text(ui->output_label, "Major roads");
            ui->state--;
            break;
        case 4:
            sprintf(file, "%sroad_minor.bmp", dir);
            gtk_label_set_text(ui->output_label, "Minor roads");
            ui->state--;
            break;
        case 5:
            sprintf(file, "%strail.bmp", dir);
            gtk_label_set_text(ui->output_label, "Trails");
            ui->state--;
            break;
        case 6:
            sprintf(file, "%sriver.bmp", dir);
            gtk_label_set_text(ui->output_label, "Rivers");
            ui->state--;
            break;
        case 7:
            sprintf(file, "%sign.bmp", dir);
            gtk_label_set_text(ui->output_label, "ign");
            ui->state--;
            break;
        case 8:
            sprintf(file, "%sneigh.bmp", dir);
            gtk_label_set_text(ui->output_label, "Ends of lines");
            ui->state--;
            break;
        case 9:
            sprintf(file, "%sholes.bmp", dir);
            gtk_label_set_text(ui->output_label, "Holes");
            ui->state--;
            gtk_widget_set_sensitive(GTK_WIDGET(ui->step_f), TRUE);
            break;
        default:
            printf("Step by step backward: state < 1 should not be possible\n");
            break;
    }

    load_image(&ui->image_output, file);

    return TRUE;
}

// Handler for modelisation button
gboolean on_modelise(GtkButton *button __attribute__((unused)), gpointer user_data)
{
    Ui *ui = user_data;

    if (gtk_dialog_run(GTK_DIALOG(ui->model.dialog)) == GTK_RESPONSE_OK)
    {
        // point matrix: ui->bp (double pointer)
        SDL_Surface *image = SDL_LoadBMP(ui->image_input.filename);

        process_array(ui->river, ui->h, image->h, image->w);
        process_array(ui->trail, ui->h, image->h, image->w);
        process_array(ui->road_major, ui->h, image->h, image->w);
        process_array(ui->road_minor, ui->h, image->h, image->w);

        execute_function(ui->argc, ui->argv, image, ui->bp, ui->river, ui->trail,
        ui->road_major, ui->road_minor, ui->model.width, ui->model.height,
        ui->model.type);
        SDL_FreeSurface(image);
    }

    gtk_widget_hide(ui->model.dialog);

    return TRUE;
}

gboolean on_size_changed(GtkComboBox *box, gpointer user_data)
{
    Ui *ui = user_data;

    gchar *size = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(box));
    gchar *save = size;

    char *separator = NULL;

    long w = strtol(size, &separator, 10);

    size = separator + 1;

    long h = strtol(size, &separator, 10);

    ui->model.width = w;
    ui->model.height = h;

    free(save);

    return TRUE;
}

gboolean on_model_type_changed(GtkComboBox *box, gpointer user_data)
{
    Ui *ui = user_data;

    ui->model.type = gtk_combo_box_get_active(box);

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
    GtkWindow *window = GTK_WINDOW(gtk_builder_get_object(builder,
                "window_main"));
    GtkWindow *wcb = GTK_WINDOW(gtk_builder_get_object(builder,
                "window_color_buttons"));
    GtkWindow *wrectif = GTK_WINDOW(gtk_builder_get_object(builder,
                "window_rectif"));
    GtkWidget *dfc = GTK_WIDGET(gtk_builder_get_object(builder,
                "dlg_file_choose"));
    GtkWidget *color_dialog = GTK_WIDGET(gtk_builder_get_object(builder,
                "color_dialog"));
    GtkWidget *model_dialog = GTK_WIDGET(gtk_builder_get_object(builder,
                "model_dialog"));
    GtkSwitch *switch_auto_analysis = GTK_SWITCH(gtk_builder_get_object(builder,
                "switch_auto_analysis"));
    GtkSwitch *switch_auto_rectif = GTK_SWITCH(gtk_builder_get_object(builder,
                "rectif_switch_auto"));
    GtkToggleButton *rectif_button = GTK_TOGGLE_BUTTON(gtk_builder_get_object(
                builder, "rectification"));
    GtkButton *open = GTK_BUTTON(gtk_builder_get_object(builder, "open"));
    
    GtkButton *launch = GTK_BUTTON(gtk_builder_get_object(builder,
                "launch_analysis"));
    GtkButton *step_f = GTK_BUTTON(gtk_builder_get_object(builder, "step_f"));

    GtkButton *step_b = GTK_BUTTON(gtk_builder_get_object(builder, "step_b"));
    
    GtkButton *modelise = GTK_BUTTON(gtk_builder_get_object(builder,
                "modelise"));
    GtkButton *button_color_topo = GTK_BUTTON(gtk_builder_get_object(builder,
                "color_topo"));
    GtkButton *button_color_road = GTK_BUTTON(gtk_builder_get_object(builder,
                "color_road"));
    GtkButton *button_color_river = GTK_BUTTON(gtk_builder_get_object(builder,
                "color_river"));
    GtkButton *color_ok = GTK_BUTTON(gtk_builder_get_object(builder,
                "color_ok"));
    GtkButton *color_cancel = GTK_BUTTON(gtk_builder_get_object(builder,
                "color_cancel"));
    GtkButton *rectif_ok = GTK_BUTTON(gtk_builder_get_object(builder,
                "rectif_ok"));
    GtkButton *rectif_cancel = GTK_BUTTON(gtk_builder_get_object(builder,
                "rectif_cancel"));
    GtkButton *rectif_done = GTK_BUTTON(gtk_builder_get_object(builder,
                "rectif_done"));
    GtkScale *rotate_scale = GTK_SCALE(gtk_builder_get_object(builder,
                "rotate_scale"));
    GtkScale *zoom_scale = GTK_SCALE(gtk_builder_get_object(builder,
                "zoom_scale"));
    GtkDrawingArea *area_input = GTK_DRAWING_AREA(gtk_builder_get_object(
                builder, "image_input"));
    GtkDrawingArea *area_output = GTK_DRAWING_AREA(gtk_builder_get_object(
                builder, "image_output"));
    GtkWidget *output_event_box = GTK_WIDGET(gtk_builder_get_object(
                builder, "output_event"));
    GtkLabel *x_label = GTK_LABEL(gtk_builder_get_object(builder, "x_pos"));

    GtkLabel *y_label = GTK_LABEL(gtk_builder_get_object(builder, "y_pos"));

    GtkLabel *output_label = GTK_LABEL(gtk_builder_get_object(builder,
                "output_label"));
    GtkScrolledWindow *scrl_in = GTK_SCROLLED_WINDOW(gtk_builder_get_object(
                builder, "scrl_in"));
    GtkScrolledWindow *scrl_out = GTK_SCROLLED_WINDOW(gtk_builder_get_object(
                builder, "scrl_out"));
    GtkEntryBuffer *coord_text_buff = GTK_ENTRY_BUFFER(gtk_builder_get_object(
                builder, "coord_text_buff"));
    GtkImage *res_scale = GTK_IMAGE(gtk_builder_get_object(builder,
                "res_scale"));
    GtkComboBoxText *model_screen_size = GTK_COMBO_BOX_TEXT(
            gtk_builder_get_object(builder, "model_screen_size"));
    GtkComboBoxText *model_type = GTK_COMBO_BOX_TEXT(gtk_builder_get_object(
                builder, "model_type"));


    // Initialise data structure
    Ui ui =
    {
        .window = window,
        .open = open,
        .dfc = dfc,
        .color_dialog = color_dialog,
        .switch_auto_analysis = switch_auto_analysis,
        .rectif_button = rectif_button,
        .rotate_scale = rotate_scale,
        .zoom_scale = zoom_scale,
        .zoom = 1.00,//gtk_range_get_value(GTK_RANGE(zoom_scale)),
        .rotation = 0,//gtk_range_get_value(GTK_RANGE(zoom_scale)),
        .is_analysis_auto = TRUE,
        .analysis_done = FALSE,
        .is_step = FALSE,
        .launch = launch,
        .step_f = step_f,
        .step_b = step_b,
        .modelise = modelise,
        .state = 0,
        .scrl_out = scrl_out,
        .scrl_in = scrl_in,
        .res_scale = res_scale,
        .output_label = output_label,
        .colors =
        {
            .wcb = wcb,
            .button_topo = button_color_topo,
            .button_road = button_color_road,
            .button_river = button_color_river,
            .color_ok = color_ok,
            .color_cancel = color_cancel,
            .topo = {0, 0, 0, 0},
            .road = {0, 0, 0, 0},
            .river = {0, 0, 0, 0},
            .topo_was_set = FALSE,
            .road_was_set = FALSE,
            .river_was_set = FALSE,
            .default_color = {0, 0, 0, 0}
        },
        .rectif =
        {
            .window = wrectif,
            .output_event_box = output_event_box,
            .x_label = x_label,
            .y_label = y_label,
            .switch_auto = switch_auto_rectif,
            .rectif_ok = rectif_ok,
            .rectif_cancel = rectif_cancel,
            .rectif_done = rectif_done,
            .coord_text_buff = coord_text_buff,
            .handler_id = 0,
            .x_pos = -1,
            .y_pos = -1
        },
        .model =
        {
            .dialog = model_dialog,
            .screen_size = model_screen_size,
            .dropdown_type = model_type,
            .type = 0,
            .width = 1920,
            .height = 1080
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
        },
        .bp = NULL,
        .tab = NULL,
        .h = NULL,
        .road_major = NULL,
        .road_minor = NULL,
        .river = NULL,
        .trail = NULL,
        .name = NULL,
        .argc = argc,
        .argv = argv
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
    g_signal_connect(step_f, "clicked", G_CALLBACK(on_step_forward), &ui);
    g_signal_connect(step_b, "clicked", G_CALLBACK(on_step_backward), &ui);
    g_signal_connect(button_color_topo, "clicked", G_CALLBACK(on_color), &ui);
    g_signal_connect(button_color_road, "clicked", G_CALLBACK(on_color), &ui);
    g_signal_connect(button_color_river, "clicked", G_CALLBACK(on_color), &ui);
    g_signal_connect(modelise, "clicked", G_CALLBACK(on_modelise), &ui);
    g_signal_connect(switch_auto_analysis, "state-set",
            G_CALLBACK(on_switch_auto_analysis), &ui);
    g_signal_connect(switch_auto_rectif, "state-set",
            G_CALLBACK(on_switch_auto_rectif), &ui);
    g_signal_connect(rectif_button, "toggled", G_CALLBACK(on_rectif_button),
            &ui);
    g_signal_connect(color_ok, "clicked", G_CALLBACK(on_color_ok), &ui);
    g_signal_connect(color_cancel, "clicked", G_CALLBACK(on_color_cancel), &ui);
    g_signal_connect(rectif_ok, "clicked", G_CALLBACK(on_rectif_ok), &ui);
    g_signal_connect(rectif_cancel, "clicked", G_CALLBACK(on_rectif_cancel),
            &ui);
    g_signal_connect(rectif_done, "clicked", G_CALLBACK(on_rectif_done), &ui);
    g_signal_connect(model_screen_size, "changed", G_CALLBACK(on_size_changed),
            &ui);
    g_signal_connect(model_type, "changed", G_CALLBACK(on_model_type_changed),
            &ui);

    g_signal_emit_by_name(model_screen_size, "changed");

    // Events
    gtk_widget_set_events(output_event_box, GDK_BUTTON_PRESS_MASK);

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
