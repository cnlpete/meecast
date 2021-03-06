/* vim: set sw=4 ts=4 et: */
/* This file is part of Other Maemo Weather(omweather)
 *
 * Copyright (C) 2006-2010 Vlad Vasiliev
 * Copyright (C) 2006-2010 Pavel Fialko
 * 	for the code
 *        
 * Copyright (C) 2008 Andrew Zhilin
 *		      az@pocketpcrussia.com 
 *	for default icon set (Glance)
 *
 * This software is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU weather-config.h General Public
 * License along with this software; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
*/
/*******************************************************************************/
#include "weather-gps.h"
#include <errno.h>
#include <math.h>
#ifdef RELEASE
#undef DEBUGFUNCTIONCALL
#endif
/*******************************************************************************/
#ifdef ENABLE_GPS
#define PI   (3.14159265358979323846)
#define EARTH_RADIUS (3443.91847)
#define deg2rad(deg) ((deg) * (PI / 180.0))
/*******************************************************************************/
void
get_nearest_station(double lat, double lon, Station *result){
    Region_item         region;
    GtkListStore        *stations_list = NULL,
                        *regions_list = NULL;
    GtkTreeIter         iter,
                        iter_region;
    gboolean            valid = FALSE,
                        valid_region;
    gint                regions_number,
                        region_id;
    gchar               *station_name = NULL,
                        *station_id0 = NULL,
                        *region_name;
    double              station_latitude,
                        station_longtitude,
                        distance,
                        min_distance = 40000.0;
    sqlite3             *database = NULL;
#ifdef DEBUGFUNCTIONCALL
    START_FUNCTION;
#endif
    /* Check only weather.com datase yet */
    database = open_database(DATABASEPATH, "weather.com.db");
    if(!database)
        return;
    regions_list = create_regions_list(database, 0, &regions_number);
    if(!regions_list)
        return;
    valid_region = gtk_tree_model_get_iter_first(GTK_TREE_MODEL
                                              (regions_list), &iter_region);
    /* Reading region settings */
    while(valid_region){
        gtk_tree_model_get(GTK_TREE_MODEL(regions_list),
                                   &iter_region,
                                   0, &region_name,
                                   1, &region_id,
                                   2, &region.maxlon,
                                   3, &region.maxlat,
                                   4, &region.minlon,
                                   5, &region.minlat,
                                   -1);
        /* Checking insiding point in to region */
        if (lat >= (region.minlat -1) && lat <= (region.maxlat + 1)
            && lon >= (region.minlon - 1) && lon <= (region.maxlon + 1) ) {
            stations_list = create_stations_list(database, region_id);
            valid =
                gtk_tree_model_get_iter_first(GTK_TREE_MODEL
                                              (stations_list), &iter);

            while (valid) {
                gtk_tree_model_get(GTK_TREE_MODEL(stations_list),
                                   &iter,
                                   NAME_COLUMN, &station_name,
                                   ID0_COLUMN, &station_id0,
                                   2, &station_longtitude,
                                   3, &station_latitude, -1);
                /* Calculating distance */
                distance =
                    calculate_distance(lat, lon, station_latitude,
                                       station_longtitude);

                if (distance < min_distance) {

                    /* New minimal distance */
                    min_distance = distance;

                    /* Copying to result */
                    memset(result->name, 0, sizeof(result->name));
                    memcpy(result->name, station_name,
                           ((sizeof(result->name) - 1) >
                            (int)(strlen(station_name))
                            ? (int)(strlen(station_name))
                            : (sizeof(result->name) - 1)));

                    /* Add word (GPS) */
                    if (strlen(result->name) < (sizeof(result->name) - 5)) {
                        result->name[strlen(result->name)] = '(';
                        result->name[strlen(result->name)] = 'G';
                        result->name[strlen(result->name)] = 'P';
                        result->name[strlen(result->name)] = 'S';
                        result->name[strlen(result->name)] = ')';
                    } else {
                        result->name[sizeof(result->name) - 5] = '(';
                        result->name[sizeof(result->name) - 4] = 'G';
                        result->name[sizeof(result->name) - 3] = 'P';
                        result->name[sizeof(result->name) - 2] = 'S';
                        result->name[sizeof(result->name) - 1] = ')';
                    }
                    memset(result->id0, 0, sizeof(result->id0));
                    memcpy(result->id0, station_id0,
                           ((sizeof(result->id0) - 1) >
                            (int)(strlen(station_id0))
                            ? (int)(strlen(station_id0))
                            : (sizeof(result->id0) - 1)));
                    result->latitude = station_latitude;
                    result->longtitude = station_longtitude;
                }
                valid =
                    gtk_tree_model_iter_next(GTK_TREE_MODEL
                                             (stations_list), &iter);
            }

            /* Clearing station list */
            if (stations_list) {
                gtk_list_store_clear(stations_list);
                stations_list = NULL;
            }
        }
        valid_region =
            gtk_tree_model_iter_next(GTK_TREE_MODEL
                                     (regions_list), &iter_region);
    }
    close_database(database);
#ifdef DEBUGFUNCTIONCALL
    END_FUNCTION;
#endif
}
/*******************************************************************************/ 
static void 
gps_location_started (LocationGPSDControl *control, gpointer userdata){
 initial_gps_connect();
}
/*******************************************************************************/ 
static void 
gps_location_stopped (LocationGPSDControl *control, gpointer userdata){
 deinitial_gps_connect();
}
/*******************************************************************************/ 
void
key_changed_callback(GConfClient* client,
		     guint cnxn_id,
		     const gchar* key,
		     GConfValue* value,
		     gboolean is_default,
		     gpointer user_data)
{
#ifdef DEBUGFUNCTIONCALL
    START_FUNCTION;
#endif
	gps_location_changed(NULL, NULL);
#ifdef DEBUGFUNCTIONCALL
    END_FUNCTION;
#endif
}
/*******************************************************************************/ 
void
initial_gps_control(void){
#ifdef DEBUGFUNCTIONCALL
    START_FUNCTION;
#endif
#ifdef OS2009
    gconf_client_add_dir(app->gconf_client,
                              "/system/nokia/location/lastknown",
                              GCONF_CLIENT_PRELOAD_NONE,
    			      NULL);
    app->gps_notify_id1 = gconf_client_notify_add(app->gconf_client, "/system/nokia/location/lastknown/longitude",
	                                key_changed_callback,
     	                                NULL,
		                        NULL, NULL);
    app->gps_notify_id2 = gconf_client_notify_add(app->gconf_client, "/system/nokia/location/lastknown/latitude",
	                                key_changed_callback,
     	                                NULL,
		                        NULL, NULL);
#else
    /* This "if" is hack for liblocation for Fremantle*/
    if (g_type_from_name("LocationGPSDControl") == 0){
        app->gps_was_started = FALSE;
        app->gps_control = location_gpsd_control_get_default();
        app->gps_run = g_signal_connect(app->gps_control, "gpsd_running", G_CALLBACK (gps_location_started), NULL);
        app->gps_stop = g_signal_connect(app->gps_control, "gpsd_stopped", G_CALLBACK (gps_location_stopped), NULL);
    }else
        fprintf(stderr, "Problem with gpsd\n");
#endif
#ifdef DEBUGFUNCTIONCALL
    END_FUNCTION;
#endif
}
/*******************************************************************************/ 
void
deinitial_gps_control(void){
#ifdef DEBUGFUNCTIONCALL
    START_FUNCTION;
#endif
#ifdef OS2009
    if (app->gps_notify_id1 != 0)
    	gconf_client_notify_remove (app->gconf_client, app->gps_notify_id1);
    if (app->gps_notify_id2 != 0)
    	gconf_client_notify_remove (app->gconf_client, app->gps_notify_id2);
#else
    deinitial_gps_connect();
    if (app->gps_control){
        g_signal_handler_disconnect (app->gps_control,app->gps_run);
        g_signal_handler_disconnect (app->gps_control,app->gps_stop);
        g_object_unref(app->gps_control);
    }
#endif
#ifdef DEBUGFUNCTIONCALL
    END_FUNCTION;
#endif
}
/*******************************************************************************/
void
gps_location_changed(LocationGPSDevice * device, gpointer userdata){
#ifdef DEBUGFUNCTIONCALL
    START_FUNCTION;
#endif
    if(!app->config->gps_station)
        return;

#ifdef OS2009
    if ((gconf_client_get_float (app->gconf_client, "/system/nokia/location/lastknown/longitude", NULL) == 0.0) &&
	(gconf_client_get_float (app->gconf_client, "/system/nokia/location/lastknown/latitude", NULL) == 0.0))
        return;

    app->temporary_station_longtitude =
	    gconf_client_get_float (app->gconf_client, "/system/nokia/location/lastknown/longitude", NULL);
    app->temporary_station_latitude =
	    gconf_client_get_float (app->gconf_client, "/system/nokia/location/lastknown/latitude", NULL);
#else
    if(device->fix->fields & LOCATION_GPS_DEVICE_LATLONG_SET){
        app->temporary_station_latitude = device->fix->latitude;
        app->temporary_station_longtitude = device->fix->longitude;
    }
#endif
}
/*******************************************************************************/
void
initial_gps_connect(void){
#ifdef DEBUGFUNCTIONCALL
    START_FUNCTION;
#endif

#ifdef OS2009
    return;
#else
    /* This "if" is hack for liblocation for Fremantle*/
    if (g_type_from_name("LocationGPSDevice") == 0){
        app->gps_device = g_object_new (LOCATION_TYPE_GPS_DEVICE, NULL);
        app->gps_id_connection = g_signal_connect (app->gps_device, "changed", G_CALLBACK (gps_location_changed), NULL);
    }
#endif
#ifdef DEBUGFUNCTIONCALL
    END_FUNCTION;
#endif
}
/*******************************************************************************/
void
deinitial_gps_connect(void){
#ifdef DEBUGFUNCTIONCALL
    START_FUNCTION;
#endif
    if(app->gps_device){
        g_signal_handler_disconnect (app->gps_device,app->gps_id_connection);
        g_object_unref(app->gps_device);
    }
#ifdef DEBUGFUNCTIONCALL
    END_FUNCTION;
#endif
}
/*******************************************************************************/
gboolean
check_needing_of_gps_station(void){
#ifdef DEBUGFUNCTIONCALL
    START_FUNCTION;
#endif
    gboolean valid;
    GtkTreeIter iter;
    gboolean is_gps = FALSE;

    valid =
        gtk_tree_model_get_iter_first(GTK_TREE_MODEL
                                      (app->user_stations_list), &iter);
    while (valid) {
        gtk_tree_model_get(GTK_TREE_MODEL(app->user_stations_list),
                           &iter,
                           2, &is_gps,
                           -1);
        if (is_gps) {
            return TRUE;
        } else
            valid =
                gtk_tree_model_iter_next(GTK_TREE_MODEL
                                         (app->user_stations_list), &iter);
    }
    return FALSE;
#ifdef DEBUGFUNCTIONCALL
    END_FUNCTION;
#endif
}
/*******************************************************************************/
void
delete_all_gps_stations(void){
    gboolean valid;
    GtkTreeIter iter;
    gchar *station_name = NULL, *station_code = NULL;
    gboolean is_gps = FALSE;
#ifdef DEBUGFUNCTIONCALL
    START_FUNCTION;
#endif
    valid =
        gtk_tree_model_get_iter_first(GTK_TREE_MODEL
                                      (app->user_stations_list), &iter);
    while (valid) {
        gtk_tree_model_get(GTK_TREE_MODEL(app->user_stations_list),
                           &iter,
                           NAME_COLUMN, &station_name,
                           ID0_COLUMN, &station_code,
                           2, &is_gps,
                           -1);
        if (is_gps) {
            if (app->config->current_station_id &&
                !strcmp(app->config->current_station_id, station_code)
                && app->config->current_station_name
                && !strcmp(app->config->current_station_name,
                           station_name)) {
                /* deleting current station */
                app->gps_must_be_current = TRUE;
                g_free(app->config->current_station_id);
                g_free(app->config->current_station_name);
                app->config->current_station_id = NULL;
                app->config->current_station_name = NULL;
                app->config->previous_days_to_show =
                    app->config->days_to_show;
            } else
                app->gps_must_be_current = FALSE;
            valid = gtk_list_store_remove(app->user_stations_list, &iter);
        } else
            valid =
                gtk_tree_model_iter_next(GTK_TREE_MODEL
                                         (app->user_stations_list), &iter);

    }
    /* Set new current_station */
    if (!app->config->current_station_id) {
        valid =
            gtk_tree_model_get_iter_first(GTK_TREE_MODEL
                                          (app->user_stations_list),
                                          &iter);
        if (valid) {
            gtk_tree_model_get(GTK_TREE_MODEL(app->user_stations_list),
                               &iter,
                               NAME_COLUMN, &station_name,
                               ID0_COLUMN, &station_code,
                               2, &is_gps, -1);
            app->config->current_station_id = g_strdup(station_code);
            app->config->current_station_name = g_strdup(station_name);
        }
    }
}
#endif
