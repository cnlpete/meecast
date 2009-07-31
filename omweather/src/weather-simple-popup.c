/* vim: set sw=4 ts=4 et: */
/*
 * This file is part of Other Maemo Weather(omweather)
 *
 * Copyright (C) 2006-2009 Vlad Vasiliev
 * Copyright (C) 2006-2009 Pavel Fialko
 *
 * Copyright (C) 2008 Andrew Zhilin
 *		      az@pocketpcrussia.com 
 *	for default icon set (Glance)
 *
 * This software is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this software; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
*/
/*******************************************************************************/
#include "weather-simple-popup.h"
#include "weather-simple-status.h"
#include "weather-utils.h"
#include "weather-home.h"
#include "weather-hash.h"
#include "weather-popup.h"
#include "weather-data.h"
/*******************************************************************************/
void
weather_simple_window_popup(GtkWidget *widget, gpointer user_data){
    GtkWidget       *window = NULL,
                    *main_vbox = NULL;
#if defined OS2009
    HildonAppMenu  *menu;
#endif
    GtkWidget *menu_item1;
    GtkWidget *menu_item2;
    GtkWidget *menu_item3;
    GtkWidget *menu_item4;
    GtkAction *test1;
    GtkAction *test2;
    GtkAction *test3;
    GtkAction *test4;
//#ifdef DEBUGFUNCTIONCALL
    START_FUNCTION;
//#endif

#if defined OS2009
    window = hildon_stackable_window_new();
#else
    window = hildon_window_new();
#endif
    gtk_window_set_title(GTK_WINDOW(window), _("Forecast"));
    main_vbox = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(window), main_vbox);
    gtk_widget_show(window);
    gtk_box_pack_start(GTK_BOX(main_vbox), create_top_buttons_box(), FALSE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(main_vbox), create_collapsed_view(main_vbox), TRUE, TRUE, 0);

#if defined OS2009
    menu = hildon_app_menu_new();
    menu_item1 = gtk_button_new_with_label(_("Settings"));
    g_signal_connect(G_OBJECT(menu_item1), "button-release-event",
                     G_CALLBACK(simple_settings_button_handler),
                     (gpointer)window);

    gtk_widget_show_all(GTK_WIDGET(menu_item1));
    hildon_app_menu_append(menu, menu_item1);
    gtk_widget_show_all(GTK_WIDGET(menu));
    hildon_window_set_app_menu(HILDON_WINDOW(window), menu);
#endif

    if(app->popup_window)
        gtk_widget_destroy(app->popup_window);
    app->popup_window = window;
    gtk_widget_show_all(GTK_WIDGET(window));
    g_signal_connect((gpointer)app->popup_window, "destroy_event",
                        G_CALLBACK(destroy_popup_window), GINT_TO_POINTER(1));
    g_signal_connect((gpointer)app->popup_window, "delete_event",
                        G_CALLBACK(destroy_popup_window), GINT_TO_POINTER(1));
}
/*******************************************************************************/
gchar*
get_next_station_name(const gchar *current_station_name, GtkListStore *user_stations_list){
    GtkTreeIter     iter;
    gboolean        valid,
                    ready = FALSE;
    gchar           *station_name = NULL;
    GtkTreePath     *path;
#ifdef DEBUGFUNCTIONCALL
    START_FUNCTION;
#endif
    if(!current_station_name)
        return NULL;

    path = gtk_tree_path_new_first();
    valid = gtk_tree_model_get_iter(GTK_TREE_MODEL(app->user_stations_list),
                                    &iter, path);
    while(valid){
        gtk_tree_model_get(GTK_TREE_MODEL(app->user_stations_list),
                            &iter, NAME_COLUMN, &station_name, -1);
        if(ready)
            return station_name;
        else{
            if((current_station_name) && (station_name) &&
                  !strcmp(current_station_name, station_name))
                ready = TRUE;
            g_free(station_name);
            gtk_tree_path_next(path);

            valid = gtk_tree_model_get_iter(GTK_TREE_MODEL(app->user_stations_list),
                                        &iter, path);
            if(!valid)
                valid = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(app->user_stations_list),
                                        &iter);
        }
    }
    gtk_tree_path_free(path);
#ifdef DEBUGFUNCTIONCALL
    END_FUNCTION;
#endif

    return NULL;
}
/*******************************************************************************/
GtkWidget*
create_top_buttons_box(void){
    GtkWidget       *buttons_box = NULL,
                    *station_button = NULL,
                    *update_button = NULL;
    gchar           buffer[255],
                    full_filename[2048];
    struct stat     statv;
//#ifdef DEBUGFUNCTIONCALL
    START_FUNCTION;
//#endif
 
/* buttons */
    buttons_box = gtk_hbox_new(TRUE, 0);
    /* station button */
    *buffer = 0;
    snprintf(buffer, sizeof(buffer) - 1, "%s: %s",
                _("next"),
                get_next_station_name(app->config->current_station_name,
                                        app->user_stations_list));
    station_button = create_button_with_2_line_text(app->config->current_station_name,
                                                    buffer, 18, 12);
    g_signal_connect(G_OBJECT(station_button), "button-release-event",
                            G_CALLBACK(change_station_next), GINT_TO_POINTER(1));
    gtk_widget_set_size_request(station_button, -1, 80);
    /* prepare last update time*/
    if(app->station_data){
        *full_filename = 0;
        snprintf(full_filename, sizeof(full_filename) - 1, "%s/%s.xml",
                    app->config->cache_dir_name, app->config->current_station_id);
        if(stat(full_filename, &statv))
            snprintf(buffer, sizeof(buffer) - 1, "%s%s", _("Last update: "), _("Unknown"));
        else{
            snprintf(buffer, sizeof(buffer) - 1, _("Last update: "));
            strftime(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer) - 1,
                    "%X %x", localtime(&statv.st_mtime));
        }
    }
    else{
        *buffer = 0;
        snprintf(buffer, sizeof(buffer) - 1, "%s%s", _("Last update: "), _("Unknown"));
    }
    /* update button */
    update_button = create_button_with_2_line_text(_("Update"), buffer, 18, 12);
    gtk_widget_set_size_request(update_button, -1, 80);
    g_signal_connect(G_OBJECT(update_button), "button-release-event",
                        G_CALLBACK(refresh_button_handler), NULL);


    gtk_box_pack_start(GTK_BOX(buttons_box), station_button, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(buttons_box), update_button, TRUE, TRUE, 0);
    return buttons_box;
}
/*******************************************************************************/
GtkWidget*
create_collapsed_view(GtkWidget *vbox){
    GtkWidget       *scrolled_window = NULL,
                    *main_vbox = NULL,
                    *line_hbox = NULL,
                    *line_text = NULL,
                    *line = NULL,
                    *label = NULL,
                    *vscrollbar = NULL;
    GdkPixbuf       *icon_buffer;
    GtkWidget       *icon_image;
    const gchar     *wind_units_str[] = { "m/s", "km/h", "mi/h" };
    gchar           buffer[1024],
                    tmp[512],
                    icon[2048],
                    symbol = 'C',
                    *comma = NULL;
    GSList          *days = NULL;
    GHashTable      *day = NULL;
    gint            i = 0,
                    hi_temp,
                    low_temp;
    struct tm       tmp_time_date_struct = {0};
#ifdef DEBUGFUNCTIONCALL
    START_FUNCTION;
#endif
    /* scrolled window */
    scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolled_window),
                                            GTK_SHADOW_NONE);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
                                   GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    vscrollbar = gtk_scrolled_window_get_vscrollbar(scrolled_window);
/*    gtk_widget_set_size_request(vscrollbar, 50, -1);*/
    /* pack childs to the scrolled window */
    main_vbox = gtk_vbox_new(FALSE, 5);
    gtk_scrolled_window_add_with_viewport(scrolled_window, main_vbox);


    days = (GSList*)g_hash_table_lookup(app->station_data, "forecast");
    if(days){
        while(days){
            day = (GHashTable*)(days->data);
            line = gtk_event_box_new();
            g_object_set_data(G_OBJECT(line), "scrolled_window", (gpointer)scrolled_window);
            g_object_set_data(G_OBJECT(line), "vbox", (gpointer)vbox);
            g_signal_connect(G_OBJECT(line), "button-release-event",
                                G_CALLBACK(show_expanded_day_button_handler),
                                 GINT_TO_POINTER(i));
            /* line box */
            line_hbox = gtk_hbox_new(FALSE, 0);
            gtk_container_add(GTK_CONTAINER(line), line_hbox);
            /* icon */
            *buffer = 0;
            snprintf(icon, sizeof(icon) - 1, "%s%s.png", app->config->icons_set_base,
                        (char*)g_hash_table_lookup(day, "day_icon"));
            icon_buffer = gdk_pixbuf_new_from_file_at_size(icon, SMALL_ICON_SIZE,
                                                            SMALL_ICON_SIZE, NULL);
            icon_image = create_icon_widget(icon_buffer, icon, SMALL_ICON_SIZE, &app->clutter_objects_in_popup_form);
            if(icon_image){
                gtk_box_pack_start(GTK_BOX(line_hbox), icon_image, FALSE, TRUE, 0);
                gtk_box_pack_start(GTK_BOX(main_vbox), line, TRUE, TRUE, 0);
                gtk_box_pack_start(GTK_BOX(main_vbox), gtk_hseparator_new(), TRUE, TRUE, 0);
            }
            /* day label */
            *buffer = 0;
            sprintf(buffer,"%s %s", (char*)g_hash_table_lookup(day, "day_name"),
                        (char*)g_hash_table_lookup(day, "day_date"));
            strptime(buffer, "%A %b %d", &tmp_time_date_struct);
            *buffer = 0;
            strftime(tmp, sizeof(tmp) - 1, "%A, %d %B", &tmp_time_date_struct);
            if(i == 0){
                comma = strchr(tmp, ',');
                if(comma)
                    snprintf(buffer, sizeof(buffer) - 1, "<span weight=\"bold\">%s%s", _("Today"), comma);
                else
                    snprintf(buffer, sizeof(buffer) - 1, "<span weight=\"bold\">%s%s", _("Today"), tmp);
            }
            else
                snprintf(buffer, sizeof(buffer) - 1, "<span weight=\"bold\">%s", tmp);
            snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer) - 1, "</span>");
            /* day text */
            *tmp = 0;
            snprintf(tmp, sizeof(tmp) - 1, "\n%s ", _("d:"));
            /* prepare temperature */
            if(!g_hash_table_lookup(day, "day_hi_temperature") ||
                   !strcmp(g_hash_table_lookup(day, "day_hi_temperature"), "N/A")){
                hi_temp = INT_MAX;
            }
            else
                hi_temp = atoi(g_hash_table_lookup(day, "day_hi_temperature"));
            if(!g_hash_table_lookup(day, "day_low_temperature") ||
                    !strcmp(g_hash_table_lookup(day, "day_low_temperature"), "N/A")){
                low_temp = INT_MAX;
            }
            else
                low_temp = atoi(g_hash_table_lookup(day, "day_low_temperature"));
            if(app->config->temperature_units == FAHRENHEIT){
                if(hi_temp != INT_MAX)
                    hi_temp = c2f(hi_temp);
                if(low_temp != INT_MAX)
                    low_temp = c2f(low_temp);
                symbol = 'F';
            }
            /* day temperature */
            if(hi_temp == INT_MAX)
                snprintf(tmp + strlen(tmp), sizeof(tmp) - strlen(tmp) - 1, "%s, ",
                            (char*)hash_table_find("N/A", FALSE));
            else
                snprintf(tmp + strlen(tmp), sizeof(tmp) - strlen(tmp) - 1, "%d\302\260%c, ",
                            hi_temp, symbol);
            /* day title */
            if(g_hash_table_lookup(day, "day_title"))
                snprintf(tmp + strlen(tmp), sizeof(tmp) - strlen(tmp) - 1, "%s, ",
                            (char*)hash_table_find(g_hash_table_lookup(day, "day_title"), FALSE));
            /* wind speed */
            if(g_hash_table_lookup(day, "day_wind_speed"))
                snprintf(tmp + strlen(tmp), sizeof(tmp) - strlen(tmp) - 1,"%.2f %s ",
                            convert_wind_units(app->config->wind_units, atof(g_hash_table_lookup(day, "day_wind_speed"))),
                            (char*)hash_table_find((gpointer)wind_units_str[app->config->wind_units], FALSE));
            /* wind direction */
            if(g_hash_table_lookup(day, "day_wind_title"))
                snprintf(tmp + strlen(tmp), sizeof(tmp) - strlen(tmp) - 1, "(%s); ",
                            (char*)hash_table_find(g_hash_table_lookup(day, "day_wind_title"), FALSE));
            snprintf(tmp + strlen(tmp), sizeof(tmp) - strlen(tmp) - 1, "%s ", _("n:"));
            /* night temperature */
            if(low_temp == INT_MAX)
                snprintf(tmp + strlen(tmp), sizeof(tmp) - strlen(tmp) - 1, "%s, ",
                            (char*)hash_table_find("N/A", FALSE));
            else
                snprintf(tmp + strlen(tmp), sizeof(tmp) - strlen(tmp) - 1, "%d\302\260%c, ",
                            low_temp, symbol);
            /* night title */
            if(g_hash_table_lookup(day, "night_title"))
                snprintf(tmp + strlen(tmp), sizeof(tmp) - strlen(tmp) - 1, "%s, ",
                            (char*)hash_table_find(g_hash_table_lookup(day, "night_title"), FALSE));
            /* wind speed */
            if(g_hash_table_lookup(day, "night_wind_speed"))
                snprintf(tmp + strlen(tmp), sizeof(tmp) - strlen(tmp) - 1,"%.2f %s ",
                            convert_wind_units(app->config->wind_units, atof(g_hash_table_lookup(day, "night_wind_speed"))),
                            (char*)hash_table_find((gpointer)wind_units_str[app->config->wind_units], FALSE));
            /* wind direction */
            if(g_hash_table_lookup(day, "night_wind_title"))
                snprintf(tmp + strlen(tmp), sizeof(tmp) - strlen(tmp) - 1, "(%s)",
                            (char*)hash_table_find(g_hash_table_lookup(day, "night_wind_title"), FALSE));

            snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer) - 1, "<i>%s</i>", tmp);

            line_text = gtk_label_new(NULL);
            gtk_label_set_justify(GTK_LABEL(line_text), GTK_JUSTIFY_FILL);
            gtk_label_set_markup(GTK_LABEL(line_text), buffer);
            set_font(line_text, NULL, 12);
            gtk_box_pack_start(GTK_BOX(line_hbox), line_text, FALSE, TRUE, 10);
            /* next day */
            days = g_slist_next(days);
            i++;
        }
    }
    else{ /* no weather data */
        gtk_box_pack_start(GTK_BOX(main_vbox),
                            label = gtk_label_new(_("No weather data for this station.")), TRUE, TRUE, 0);
        set_font(label, NULL, 24);
    }
    gtk_widget_show_all(scrolled_window);
    return scrolled_window;
}
/*******************************************************************************/
GtkWidget*
create_weather_expanded_view(GtkWidget *button, GdkEventButton *event, gpointer user_data){

    gint                day_number = (gint)user_data,
                        i = 0;
    GHashTable          *current = NULL,
                        *location = NULL,
                        *day = NULL;
    GSList              *days = NULL;
    GtkWidget           *day_widget = NULL,
                        *current_widget = NULL,
                        *main_vbox = NULL,
                        *window = NULL;
    gchar               *day_name = NULL;
    time_t              current_time = 0,
                        diff_time,
                        current_data_last_update = 0;
    GtkWidget       *scrolled_window = NULL,
                    *vscrollbar = NULL,
                    *view = NULL;
//#ifdef DEBUGFUNCTIONCALL
    START_FUNCTION;
//#endif
    /* scrolled window */
    scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolled_window),
                                            GTK_SHADOW_NONE);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
                                       GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_size_request (scrolled_window, -1, 300);
    vscrollbar = gtk_scrolled_window_get_vscrollbar(scrolled_window);

    main_vbox = gtk_vbox_new(FALSE, 5);
    gtk_scrolled_window_add_with_viewport(scrolled_window, main_vbox);

    if(!app->station_data)
        return;
    location = (GHashTable*)g_hash_table_lookup(app->station_data, "location");
    current = (GHashTable*)g_hash_table_lookup(app->station_data, "current");
    days = (GSList*)g_hash_table_lookup(app->station_data, "forecast");
    if(!current || !days || !location)
        return;
    for(i = 0; days && i != day_number; i++)      /* search needed day */
        days = g_slist_next(days);
    if(i != day_number)         /* if needed day was not found */
        return;
    day = (GHashTable*)(days->data);
    day_widget = create_day_tab(current, day, &day_name);
    if(!day_widget)
        return;

//    gtk_window_set_title(GTK_WINDOW(window), _("Expanded forecast"));

    if(day_number == 0){ /* if selected Today, than adding Now, if it aviable */
        /* prepare for Now data */
        current_time = time(NULL); /* get current day */
        /* correct time for current location */
        diff_time = calculate_diff_time(atol(g_hash_table_lookup(location, "station_time_zone")));
#ifndef RELEASE
        fprintf(stderr, "\n>>>>>>>Diff time=%li<<<<<<\n", diff_time);
#endif
        current_time += diff_time;
        current_data_last_update = last_update_time_new(current);
        /* Check a valid time for current weather */
        if( (current_data_last_update >
                ( current_time - app->config->data_valid_interval)) &&
                (current_data_last_update < ( current_time + app->config->data_valid_interval))){
            current_widget = create_current_tab(current);
            if(current){
                gtk_box_pack_start(GTK_BOX(main_vbox), current_widget, FALSE, TRUE, 0);
                gtk_box_pack_start(GTK_BOX(main_vbox), gtk_hseparator_new(), FALSE, TRUE, 0);
            }
        }
    }
    gtk_box_pack_start(GTK_BOX(main_vbox), day_widget, TRUE, TRUE, 0);
    gtk_widget_show_all(main_vbox);
    gtk_widget_show_all(scrolled_window);
    g_free(day_name);
    fprintf(stderr, "\n>>>>>>>>>>>>>>>>>>Day number %d\n", day_number);
    return scrolled_window;

}
/*******************************************************************************/
void
show_expanded_day_button_handler(GtkWidget *button, GdkEventButton *event,
                                                            gpointer user_data){
    GtkWidget *view;
#ifdef DEBUGFUNCTIONCALL
    START_FUNCTION;
#endif
//    view = create_weather_for_two_hours_collapsed_view(button, event, user_data);
    view = create_weather_expanded_view(button, event, user_data);
    gtk_widget_destroy(g_object_get_data(G_OBJECT(button), "scrolled_window"));
    gtk_box_pack_start(GTK_BOX(g_object_get_data(G_OBJECT(button), "vbox")), view, FALSE, TRUE, 0);
}
/*******************************************************************************/
GtkWidget*
create_weather_for_two_hours_collapsed_view(GtkWidget *button, GdkEventButton *event, gpointer user_data){
    GtkWidget       *scrolled_window = NULL,
                    *main_vbox = NULL,
                    *line = NULL,
                    *line_hbox = NULL,
                    *line_text = NULL,
                    *vscrollbar = NULL,
                    *icon_text_hbox = NULL;
    GdkPixbuf       *icon;
    GtkWidget       *icon_image;
    const gchar     *wind_units_str[] = {"m/s", "km/h", "mi/h" };
    gchar           buffer[1024],
                    tmp[512];
    GHashTable      *hour_weather = NULL;
    GSList          *hours_weather = NULL;
    gint            i = 0;

#ifdef DEBUGFUNCTIONCALL
    START_FUNCTION;
#endif

    /* scrolled window */
    scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolled_window),
                                            GTK_SHADOW_NONE);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
                                       GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_size_request (scrolled_window, -1, 300);
    vscrollbar = gtk_scrolled_window_get_vscrollbar(scrolled_window);

    main_vbox = gtk_vbox_new(FALSE, 5);
    gtk_scrolled_window_add_with_viewport(scrolled_window, main_vbox);

    hours_weather = g_hash_table_lookup(g_hash_table_lookup(app->station_data, "detail"), "hours_data");

    if(hours_weather){
        while(hours_weather){
            line = gtk_event_box_new();
            hour_weather = (GHashTable*)hours_weather->data;
            icon_text_hbox = gtk_hbox_new(FALSE, 0);
            *buffer = 0;
            g_object_set_data(G_OBJECT(line), "scrolled_window", (gpointer)scrolled_window); 
            line_hbox = gtk_hbox_new(FALSE, 0);
            gtk_container_add(GTK_CONTAINER(line), line_hbox);
            /*icon*/
            sprintf(buffer,"%s%s.png", app->config->icons_set_base,
                                     (char*)g_hash_table_lookup(hour_weather, "hour_icon"));
            icon = gdk_pixbuf_new_from_file_at_size(buffer, SMALL_ICON_SIZE,
                                                                    SMALL_ICON_SIZE, NULL);
            icon_image = create_icon_widget(icon, buffer, SMALL_ICON_SIZE, 
                                                    &app->clutter_objects_in_popup_form);
            if(icon_image){
                gtk_box_pack_start(GTK_BOX(line_hbox), icon_image, FALSE, TRUE, 0);
                gtk_box_pack_start(GTK_BOX(main_vbox), line, TRUE, TRUE, 0);
                gtk_box_pack_start(GTK_BOX(main_vbox), gtk_hseparator_new(), TRUE, TRUE, 0);
            }

            *buffer = 0;
            *tmp = 0;
            /* hour */
            sprintf(tmp + strlen(tmp), "%s", _("Forecast at: "));
            sprintf(tmp + strlen(tmp), "%s:00\n",
                                        (char*)g_hash_table_lookup(hour_weather, "hours"));

            if(tmp){
                snprintf(buffer, sizeof(buffer) - 1, "<span weight=\"bold\">%s", tmp);
                snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer) - 1, 
                                                                                   "</span>");
            }

            *tmp = 0;
            /* temperature */
            sprintf(tmp, " %d\302\260",
                        ((app->config->temperature_units == CELSIUS) ?
                       ( atoi(g_hash_table_lookup(hour_weather, "hour_temperature"))) :
                  ( (int)c2f(atoi(g_hash_table_lookup(hour_weather, "hour_temperature"))))));
            (app->config->temperature_units == CELSIUS) ? ( strcat(tmp, _("C")))
                                               : ( strcat(tmp, _("F")));

            /* feels like */
            sprintf(tmp + strlen(tmp), " (feels like  %d\302\260",
                             (app->config->temperature_units == CELSIUS) ?
                             (atoi(g_hash_table_lookup(hour_weather, "hour_feels_like"))) :
                    ((int)c2f(atoi(g_hash_table_lookup(hour_weather, "hour_feels_like")))));
            (app->config->temperature_units == CELSIUS) ? ( strcat(tmp, _("C")))
                                                             : ( strcat(tmp, _("F")));
            sprintf(tmp + strlen(tmp), ")");
            
            /* title */
            if(g_hash_table_lookup(hour_weather, "hour_title"))
                snprintf(tmp + strlen(tmp), sizeof(tmp) - strlen(tmp) - 1," ,%s, ",
              (char*)hash_table_find(g_hash_table_lookup(hour_weather, "hour_title"), FALSE));

            /* wind speed */
            if(g_hash_table_lookup(hour_weather, "hour_wind_speed"))
                snprintf(tmp + strlen(tmp), sizeof(tmp) - strlen(tmp) - 1,"%.2f %s ",
                                   convert_wind_units(app->config->wind_units, 
                                   atof(g_hash_table_lookup(hour_weather, "hour_wind_speed"))),
           (char*)hash_table_find((gpointer)wind_units_str[app->config->wind_units], FALSE));

           /* wind direction */
           if(g_hash_table_lookup(hour_weather, "hour_wind_direction"))
                        snprintf(tmp + strlen(tmp), sizeof(tmp) - strlen(tmp) - 1, "(%s)",
                                    (char*)hash_table_find(g_hash_table_lookup(hour_weather,
                                                            "hour_wind_direction"), FALSE));

            /* humidity */
            if(g_hash_table_lookup(hour_weather, "hour_humidity"))
                sprintf(tmp + strlen(tmp), ",  %d%%",
                                  atoi(g_hash_table_lookup(hour_weather, "hour_humidity")));
            
            snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer) - 1, "<i>%s</i>",
                tmp);

            line_text = gtk_label_new(NULL);
            gtk_label_set_justify(GTK_LABEL(line_text), GTK_JUSTIFY_FILL);
            gtk_label_set_markup(GTK_LABEL(line_text), buffer);
            set_font(line_text, NULL, 12);
            gtk_box_pack_start(GTK_BOX(line_hbox), line_text, FALSE, TRUE, 10);
            
            hours_weather = g_slist_next(hours_weather);
            if(!hours_weather)
               break;
        }
    }

 gtk_widget_show_all(scrolled_window);
 return scrolled_window;
}
/*******************************************************************************/
