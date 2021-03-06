/* vim: set sw=4 ts=4 et: */
/*
 * This file is part of Other Maemo Weather(omweather)
 *
 * Copyright (C) 2006-2009 Vlad Vasiliev
 * Copyright (C) 2006-2009 Pavel Fialko
 * Copyright (C) 2008      Andrew Olmsted
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
*/
/*******************************************************************************/
#ifndef _weather_common_h
#define _weather_common_h 1
/*******************************************************************************/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <errno.h>
#include <pthread.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <gconf/gconf-client.h>
#include <libgnomevfs/gnome-vfs.h>
#include <sys/types.h>
#include <libosso.h>
#include <wchar.h>
#include <limits.h>
#include <libintl.h>
#include <locale.h>
#include <sqlite3.h>
#include <hildon/hildon-program.h>
#ifdef OS2008 
    #include <hildon/hildon-window.h>
    #include <hildon/hildon-banner.h>
    #include <libhildondesktop/hildon-desktop-picture.h>
    #include <libhildondesktop/hildon-desktop-home-item.h>
    #include <libhildondesktop/libhildondesktop/hildon-home-area.h>
    #include <X11/extensions/Xcomposite.h>
    #include <X11/extensions/Xdamage.h>
    #include <X11/extensions/Xrender.h>
#elif  NONMAEMO
    #include <hildon/hildon-banner.h>
#elif  OS2009
    #include <libhildondesktop/libhildondesktop.h>
    #include <hildon/hildon-banner.h>
    #include <hildon/hildon-window-stack.h>
    #include <hildon/hildon.h>
    #include <gconf/gconf-client.h>
#else
    #include <hildon-widgets/hildon-banner.h>
#endif
#ifdef ENABLE_GPS
    #include <location/location-gps-device.h>
    #include <location/location-gpsd-control.h>
#endif

#ifdef CLUTTER
    #include <clutter/clutter.h>
    #include <clutter-gtk/gtk-clutter-embed.h>
    #include <clutter-gtk/gtk-clutter-util.h>
#endif

#ifdef USE_CONIC
  #include <conic/conic.h>
#endif

#ifdef ENABLE_BROWSER_INTERFACE
  #include <tablet-browser-interface.h>
#endif

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
/*******************************************************************************/
/*#define _(String) dgettext (GETTEXT_PACKAGE, String)*/
#define GETTEXT_PACKAGE "omweather"
#include <glib/gi18n-lib.h>

#define Max_count_weather_day   10
#define DATABASEPATH        "/usr/share/omweather/db/"
#define SOURCESPATH         "/usr/share/omweather/sources/"
#define STYLEPATH           "/usr/share/omweather/"
#define COPYRIGHT_ICONS     "/usr/share/omweather/copyright_icons/"
#define COUNTRIESFILE       "countries.list"
#define REGIONSFILE         "regions.list"
#define LOCATIONSFILE       "locations.list"
#define ICONS_PATH          "/usr/share/omweather/icons/"
#define BUTTON_ICONS        "/usr/share/omweather/button_icons/"
#define IMAGES_PATH         "/usr/share/omweather/images/"

 #define START_FUNCTION        fprintf(stderr,"\n>>>>>>>>>Start %s()\n", __PRETTY_FUNCTION__);
#define END_FUNCTION		fprintf(stderr,"\n>>>>>>>>>End %s()\n", __PRETTY_FUNCTION__);
#define DEBUG_FUNCTION(string)  
/*
#define DEBUG_FUNCTION(string)  write_log(string)
#define START_FUNCTION          FILE *f; f=fopen("/tmp/omw.log","a+"); fprintf(f,"\n>>>>>>>>>Start %s()\n", __PRETTY_FUNCTION__);fflush(f);fclose(f);
#define END_FUNCTION          FILE *fileend; fileend=fopen("/tmp/omw.log","a+"); fprintf(fileend,"\n>>>>>>>>>End %s()\n", __PRETTY_FUNCTION__);fflush(fileend);fclose(fileend);
*/
#define OMWEATHER_ADD_STATION           1000
#define OMWEATHER_SEARCH_STATION        1001
#define OMWEATHER_CLOSE_STATION_WINDOW  1002
#define PRESET_BIG_FONT "Sans Bold 32"
#define PRESET_BIGMEDIUM_FONT "Sans Bold 24"
#define PRESET_MEDIUM_FONT "Sans Bold 21"
#define PRESET_DAY_FONT "Sans 16"
#define PRESET_TEMPERATURE_FONT "Sans 19"
#define PRESET_WIND_FONT "Sans 13"
#define PRESET_STATION_FONT "Sans 15"
/*#define PRESET_BIG_FONT "Bitstream Vera Sans Bold 38"*/
#define PRESET_BIG_FONT_COLOR_FRONT "#FFFFFF"
#define PRESET_BIG_FONT_COLOR_BACK "#000000"
#define PRESET_WIND_FONT_COLOR "#000000"
#define PRESET_FONT_COLOR_LOW_TEMP "#7D9CAD"
#define OMWEATHER_SIGNAL_INTERFACE "org.maemo.omweather"
#define OMWEATHER_SIGNAL_PATH "/org/maemo/omweather"
#define OMWEATHER_SIGNAL_RELOAD_CONFIG_INTERFACE "org.maemo.omweather.reload_config"
#define OMWEATHER_SIGNAL_RELOAD_CONFIG_PATH "/org/maemo/omweather/reload_config"
#define OMWEATHER_RELOAD_CONFIG "reload_config"
#define OMWEATHER_RELOAD_PLUGIN "reload_plugin"
#define MCE_MATCH_RULE "type='signal',interface='" MCE_SIGNAL_IF \
                        "',member='" MCE_DEVICE_ORIENTATION_SIG "'"

/*******************************************************************************/
enum { AUTOUPDATE, CHANGE_DAY_PART, DBUSINITEVENT, UPDATE_AFTER_CONNECTED,
       CHECK_GPS_POSITION};
enum { DOWNLOAD_UNCKNOWN_ERROR = -1, DOWNLOAD_OK = 0, DOWNLOAD_PROXY_ERROR = 5,
        DOWNLOAD_RESOLVE_ERROR = 6, DOWNLOAD_CONNECT_ERROR = 7,
        DOWNLOAD_ACCESS_DENIED = 9, DOWNLOAD_HTTP_ERROR = 22,
        DOWNLOAD_NOW_WORKING = 50, DOWNLOAD_FINISHED = 51,
        DOWNLOAD_TIMEOUT = 28, DOWNLOAD_WAKEUP_CONNECTION_FAILED = 52 };

enum { UNKNOWN_DIRECTION, TO_NORTH, TO_NORTH_EAST, TO_EAST, TO_SOUTH_EAST, TO_SOUTH, TO_SOUTH_WEST, TO_WEST, TO_NORTH_WEST };
enum { ONE_ROW, ONE_COLUMN, TWO_ROWS, TWO_COLUMNS, COMBINATION, PRESET_NOW, PRESET_NOW_PLUS_TWO, PRESET_NOW_PLUS_THREE_V,
       PRESET_NOW_PLUS_THREE_H, PRESET_NOW_PLUS_SEVEN, APPLICATION_MODE };
enum { FIRST_BUTTON, OTHER_BUTTON };
enum { RIGHT, LEFT, TOP, BOTTOM, NOTHING };
enum { METERS, KILOMETERS, MILES, SEA_MILES };
enum { METERS_S, KILOMETERS_H, MILES_H, BEAUFORT_SCALE };
enum { CELSIUS, FAHRENHEIT };
enum { INVALID_VALUE = 0, TINY, SMALL, MEDIUM, BIG, LARGE, GIANT, SUPER_GIANT };
enum { ICON, STATION_NAME };
enum { MB, INCH, MM };
enum { WEATHER_COM, RP5_RU, MAX_WEATHER_SOURCE_NUMBER };
enum { TINY_ICON_SIZE = 32, SMALL_ICON_SIZE = 48, MEDIUM_ICON_SIZE = 64,
       BIG_ICON_SIZE = 80, LARGE_ICON_SIZE = 96, GIANT_ICON_SIZE = 128,
       SUPER_GIANT_ICON_SIZE = 256};
enum { ZERO_PHASE = -1, FIRST_PHASE = 0, SECOND_PHASE, THIRD_PHASE, FOURTH_PHASE, FIFTH_PHASE, SIXTH_PHASE };
enum { SETTINGS_STATIONS_PAGE, SETTINGS_VISUALS_PAGE, SETTINGS_DISPLAY_PAGE,
       SETTINGS_UNITS_PAGE, SETTINGS_UPDATE_PAGE,
#if defined(OS2008) || defined(OS2009) || defined(NONMAEMO)
       SETTINGS_SENSOR_PAGE,
#endif
       ALERTS_PAGE, MAX_SETTINGS_PAGE_NUMBER, ABOUT_PAGE
};
enum { SIMPLE_MODE = 3, EXTENDED_MODE, MEDIUM_MODE };
enum { SHORT_CLICK, LONG_CLICK };
enum {COLLAPSED_VIEW_MODE, EXPANDED_VIEW_MODE};

/* station tab */
enum { STATE_ENABLE_GPS	= 1U,
       STATE_SOURCE	= 2U
};
/* visuals tab */
enum { STATE_ONE_ROW            = 1U,
       STATE_ONE_COLUMN         = 2U,
       STATE_TWO_ROWS           = 4U,
       STATE_TWO_COLUMNS        = 8U,
       STATE_COMBINATION        = 16U,
       STATE_THEME_OVERRIDE     = 32U,
       STATE_TRANSPARENCY       = 64U,
       STATE_FONT               = 128U,
       STATE_FONT_COLOR         = 256U,
       STATE_BACKGROUND_COLOR   = 512U,
       STATE_ICONSET            = 1024U,
       STATE_LONG               = 2048U,
       STATE_SHORT              = 4096U,
       STATE_PRESET_NOW               = 8192U,
       STATE_PRESET_NOW_PLUS_TWO      = 16384U,
       STATE_PRESET_NOW_PLUS_THREE_V  = 32768U,
       STATE_PRESET_NOW_PLUS_THREE_H  = 65536U,
       STATE_PRESET_NOW_PLUS_SEVEN    = 131072U
};
/* display tab */
enum { STATE_VISIBLE_ITEMS	= 1U,
       STATE_ICON_SIZE		= 2U,
       STATE_SHOW_CURRENT	= 4U,
       STATE_SWAP_TEMPERATURE	= 8U,
       STATE_SHOW_WIND		= 16U,
       STATE_SHOW_STATION_NAME	= 32U,
       STATE_SHOW_ARROWS	= 64U,
       STATE_RIGHT_POSITION	= 128U,
       STATE_LEFT_POSITION	= 256U,
       STATE_TOP_POSITION	= 512U,
       STATE_BOTTOM_POSITION	= 1024U,
       STATE_NOTHING_POSITION	= 2048U
};
/* units tab */
enum { STATE_CELCIUS		= 1U,
       STATE_FAHRENHEIT		= 2U,
       STATE_METERS		= 4U,
       STATE_KILOMETERS		= 8U,
       STATE_MILES		= 16U,
       STATE_SEA_MILES		= 32U,
       STATE_METERS_S		= 64U,
       STATE_KILOMETERS_H	= 128U,
       STATE_MILES_H		= 256U,
       STATE_PRESSURE_MB	= 512U,
       STATE_PRESSURE_IN	= 1024U,
       STATE_PRESSURE_MM	= 2048U,
       STATE_BEAUFORT_SCALE = 4096U
};
/* update tab */
enum { STATE_AUTO_CONNECT		= 1U,
       STATE_SWITCH_TO_NEXT		= 2U,
       STATE_VALID_TIME			= 4U,
       STATE_UPDATING_TIME		= 8U,
       STATE_SHOW_WEATHER_FOR_TWO_HOURS = 16U
};
/* sensor tab */
enum { STATE_USE_SENSOR		= 1U,
       STATE_SHOW_AT_NAME	= 2U,
       STATE_SHOW_AT_ICON	= 4U,
       STATE_SENSOR_READ_TIME	= 8U
};
/*******************************************************************************/
typedef struct weather_day_button_with_image{
    GtkWidget   *button;
    GtkWidget   *label;
    GtkWidget   *shadow_label;
    GtkWidget   *box;
    GtkWidget   *icon_image;
    GtkWidget   *wind;
    GtkWidget   *wind_text;
    GtkWidget   *wind_label;
    GtkWidget   *background;
}WDB;
/*******************************************************************************/
struct event_time{
    time_t	time;          	/* Time event */
    short	type_event; 	/* Type of event: Automatic update event AUTOUPDATE, 
				    time of day event DAYTIMEEVENT */ 
};
/*******************************************************************************/
struct time_update{ 
    gint	between_time;        /* Time in minutes  betwen two update weather */
    gchar	*name_between_time;  /* Human string it time */
};
/*******************************************************************************/
typedef	struct{
    char	name[50];
    long	start;
    long	end;
}Country_item;
/*******************************************************************************/
typedef	struct{
    char	name[50];
    long	start;
    long	end;
    double	minlat;
    double	minlon;
    double	maxlat;
    double	maxlon;
}Region_item;
/*******************************************************************************/
typedef struct{
    char    name[50];
    char    id0[10];
    char    id1[10];
    double  latitude;
    double  longtitude;
}Station;
/*******************************************************************************/
enum{ NAME_COLUMN = 0, ID0_COLUMN, ID1_COLUMN, LATITUDE_COLUMN,
	LONGTITUDE_COLUMN };
/*******************************************************************************/
typedef struct applet_config{
    gchar   *cache_dir_name;
    gchar   *icons_set_base;
    gchar   *icon_set;
    gchar   *font;
    gchar   *current_source;
    gchar   *current_country;
    gchar   *current_station_name;
    gchar   *current_station_id;
    gchar   *current_station_source;
    gchar   *iap_http_proxy_host;
    gchar   *cache_directory;
    gint    iap_http_proxy_port;
    gint    update_interval;
    gint    switch_time;
    gint    icons_size;
    gint    icons_layout;
    gint    text_position;
    gint    current_settings_page;
    gint    days_to_show;
    gint    previous_days_to_show;
    gint    distance_units;
    gint    wind_units;
    gint    clicking_type;
    gint    temperature_units;
    gint    pressure_units;
    gint    mode;
#if defined(OS2008) || defined(DEBUGTEMP) || defined(OS2009)
    gint        display_at;
    gboolean    use_sensor;
    guint       sensor_update_time;
    guint       alpha_comp;
    guint       corner_radius;
#endif
    guint       data_valid_interval;
    gboolean    transparency;
    gboolean    separate;
    gboolean    swap_hi_low_temperature;
    gboolean    show_station_name;
    gboolean    show_arrows;
    gboolean    downloading_after_connecting;
    gboolean    gps_station;
    gboolean    show_wind;
    gboolean    show_wind_gust;
    gboolean    show_weather_for_two_hours;
    GdkColor    font_color;
    GdkColor    background_color;
    gboolean    is_application_mode;
    gboolean	update_gsm;
    gboolean	update_wlan;
    gboolean	animation;
    gint        view_mode;
    gint        scale_in_popup;
    gchar       *mod;
}AppletConfig;
/*******************************************************************************/
#if defined CLUTTER || defined OS2009
typedef struct SuperOH{
#if defined CLUTTER 
  ClutterTimeline       *timeline;
  ClutterActor          *icon;
  ClutterGroup          *group;
  ClutterActor          *stage;
  ClutterScript         *script;
  ClutterActor          *bgtex;
#else
  gint                  timeline;
#endif
  GtkWidget             *image; /* Temp */
  gpointer              *function;
  GtkWidget             *clutter;
  GtkWidget             *icon_widget;
  GtkWidget             *window;
  GdkPixbuf             *bgpixb;
  GSList                *list_images;
  guint                 runtime;
  guint                 merge_id;
  guint                 duration;
  guint                 delay;
  gint                  icon_size;
  gchar                 *icon_name;
  gboolean              first_icon_in_list;
}SuperOH;
#endif
#ifdef HILDONANIMATION
#define LOAD_ACTOR 1
#define POSITION_ACTOR 2
#define OPACITY_ACTOR 3
#define SCALE_ACTOR 4
#define ROTATE_ACTOR 5
typedef struct Event{
    gchar       *event_type;
    gpointer    event;
    GtkWidget   *image;
    gint        number;
}Event;
/* Event for load actor */            
typedef struct Event_l{
  gchar                 *name;
  gint                  height;
  gint                  width;
}Event_l;
/* Event for changed Position of actor */            
typedef struct Event_p{
  gint                  x;
  gint                  y;
}Event_p;
/* Event for changed Opacity of actor */            
typedef struct Event_o{
  gint                  o;
}Event_o;
/* Event for changed Scale of actor */            
typedef struct Event_s{
  gdouble               x;
  gdouble               y;
}Event_s;
/* Event for changed Rotate of actor */            
typedef struct Event_r{
  gdouble               a;
  gint                  x;
  gint                  y;
  gint                  z;
  gint                  d;
}Event_r;





#endif
/*******************************************************************************/
typedef struct OMWeatherApplet{
    HildonProgram       *app;       /* handle to application */
    GtkWidget           *main_view; /* handle to app's view */
    osso_context_t	*osso;
    GHashTable		*hash,
                    *sources_units;
    GHashTable      *station_data;
    GHashTable      *detail_station_data;
    GtkWidget		*top_widget;
    GtkWidget		*main_window;
    GtkWidget		*popup_window;
    GtkWidget		*popup_window_more;
    GtkWidget		*settings_window;
    gint		button_pressed;
    AppletConfig	*config;
    gboolean		show_update_window;
    gboolean		iap_connected;
    gboolean		iap_connected_gsm;
    gboolean		iap_connected_wlan;
    gboolean		iap_connecting;
    gboolean            current_is_valid;
    glong		iap_connecting_timer;
    guint		timer;
    guint		timer_for_os2008;
    guint		switch_timer;
    guint		sensor_timer;
    guint		count_day;		/* days number from parser */
    guint		stations_tab_start_state;
    guint		stations_tab_current_state;
    guint		visuals_tab_start_state;
    guint		visuals_tab_current_state;
    guint		display_tab_start_state;
    guint		display_tab_current_state;
    guint		units_tab_start_state;
    guint		units_tab_current_state;
    guint		update_tab_start_state;
    guint		update_tab_current_state;
    guint		sensor_tab_start_state;
    guint		sensor_tab_current_state;
    guint		alerts_tab_start_state;
    guint		alerts_tab_current_state;
    GSList		*buttons;
    gboolean		dbus_is_initialize;
    gboolean		gps_must_be_current;
    gboolean		reload;
    gboolean		gps_need;
    GtkListStore	*user_stations_list;
    GtkListStore	*time_update_list;
    GtkListStore	*user_alerts_list;
    GtkListStore	*sources_list;
    gint		sources_number;
    GtkWidget 		*contextmenu;
    gboolean 		widget_first_start;
    gint                phase;
    gint 		widget_showing;
    gboolean 		home_item_flag_expose;
    gboolean 		area_button_pressed;
    gboolean 		area_button_release;
    gboolean		area_changed;
    GdkPixbuf 		*pixbuf;
    guint		signal_expose;
    guint		signal_area_expose;
    guint		signal_item_expose;
    guint		my_applet_signal_release;
    guint		signal_release;
    guint		signal_press;
    guint		signal_size_request;
    guint		signal_area_changed;
    GtkWidget 		*parent_parent;
    GtkWidget 		*parent;
    gpointer		*child_data;
    gint 		ax;
    gint		ay;
    gint		aw;
    gint		ah;
    GSList		*tab_of_window_popup;
    GSList		*handles;
    GSList              *sources;
    GSList              *clutter_objects_in_main_form;
    GSList              *clutter_objects_in_popup_form;
#ifdef ENABLE_GPS
    guint       gps_id_connection;
    guint       gps_id_gpsdevice;
    guint       gps_run;
    guint       gps_stop;
    LocationGPSDevice   *gps_device;
    LocationGPSDControl *gps_control;
    gboolean    gps_was_started;
    Station     gps_station;
    double      temporary_station_latitude;
    double      temporary_station_longtitude;
#endif
#ifdef OS2009
    gboolean    portrait_position;
    gboolean    popup_portrait_position;
    HDHomePluginItem *home_window;
#endif
#if defined(OS2008) || defined(DEBUGTEMP) || defined(OS2009)
    gfloat		sensor_data;
#endif
#ifdef USE_CONIC
    ConIcConnection     *connection;
#endif
#ifdef USE_DBUS
    DBusConnection      *dbus_conn;
    DBusConnection      *dbus_conn_session;
#endif
#ifdef CLUTTER
    ClutterScript       *clutter_script; /* Maybe it need be deleting */
#endif
#ifdef HILDONANIMATION
    GHashTable          *animation_hash; 
#endif
    pthread_t           update_thread_id;
    gboolean            applet_visible;
    gboolean            notanimation;
    GConfClient         *gconf_client;
    guint 		gps_notify_id1;
    guint 		gps_notify_id2;
}OMWeatherApp;
/*******************************************************************************/
typedef struct lists_struct{
    GtkWidget       *countries;
    GtkListStore    *countries_list;
    GtkWidget       *states;
    GtkListStore    *regions_list;
    GtkWidget       *stations;
    GtkListStore    *stations_list;
    GtkListStore    *sources_list;
    sqlite3         *database;
    gboolean        database_invalid;
}lists_struct_obj;
/*******************************************************************************/
extern	OMWeatherApp	*app;
/*******************************************************************************/
#ifdef OS2008 
G_BEGIN_DECLS

#define TYPE_OMWEATHER (omweather_get_type())
#define OMWEATHER(obj)	(G_TYPE_CHECK_INSTANCE_CAST((obj), \
			TYPE_OMWEATHER, OMWeather))
#define OMWEATHER_CLASS(klass) \
		(G_TYPE_CHECK_CLASS_CAST((klass), TYPE_OMWEATHER, OMWeather))
#define IS_OMWEATHERT(obj)	(G_TYPE_CHECK_INSTANCE_TYPE((obj), \
			TYPE_OMWEATHER))
#define IS_OMWEATHERT_CLASS(klass) \
		(G_TYPE_CHECK_CLASS_TYPE((klass), TYPE_OMWEATHER))
#define IS_OMWEATHER_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS((obj), \
			TYPE_OMWEATHER, OMWeather))
#define OMWEATHER_GET_PRIVATE(obj) \
		(G_TYPE_INSTANCE_GET_PRIVATE((obj), TYPE_OMWEATHER, \
				OMWeather))


typedef struct _OMWeatherPrivate OMWeatherPrivate;

/* Instance struct */
typedef struct {
	HildonDesktopHomeItem	parent;
	GtkWidget		*layout;
	GnomeVFSMonitorHandle	*fileMon;
	guint			updateTimeout;
	gboolean		queueRefresh;
	XRectangle		clip;
	OMWeatherPrivate	*priv;
}OMWeather;

/* Class struct */
typedef struct {
	HildonDesktopHomeItemClass parent_class;
} OMWeatherClass;


GType omweather_get_type(void);


G_END_DECLS
#endif

#ifdef OS2009

G_BEGIN_DECLS

#define OMWEATHER_TYPE_DESKTOP_WIDGET omweather_plugin_get_type()

#define OMWEATHER_DESKTOP_WIDGET(obj) \
    (G_TYPE_CHECK_INSTANCE_CAST ((obj), OMWEATHER_TYPE_DESKTOP_WIDGET, OmweatherPlugin))

#define OMWEATHER_DESKTOP_WIDGET_CLASS(klass) \
    (G_TYPE_CHECK_CLASS_CAST ((klass), OMWEATHER_TYPE_DESKTOP_WIDGET, OmweatherPlugintClass))

#define OMWEATHER_IS_DESKTOP_WIDGET(obj) \
    (G_TYPE_CHECK_INSTANCE_TYPE ((obj), OMWEATHER_TYPE_DESKTOP_WIDGET))

#define OMWEATHER_IS_DESKTOP_WIDGET_CLASS(klass) \
    (G_TYPE_CHECK_CLASS_TYPE ((klass), OMWEATHER_TYPE_DESKTOP_WIDGET))

#define OMWEATHER_DESKTOP_WIDGET_GET_CLASS(obj) \
          (G_TYPE_INSTANCE_GET_CLASS ((obj), OMWEATHER_TYPE_DESKTOP_WIDGET, OmweatherPluginClass))



typedef struct {
  HDHomePluginItem       parent;
} OmweatherPlugin;


typedef struct {
  HDHomePluginItemClass  parent_class;
} OmweatherPluginClass;

GType omweather_plugin_get_type (void);
/* OmweatherPlugin* omweather_plugin_widget_new (void); */


G_END_DECLS


#endif

#endif
