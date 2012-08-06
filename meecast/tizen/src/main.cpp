/* vim: set sw=4 ts=4 et: */
/*
 * This file is part of Meecast for Tizen
 *
 * Copyright (C) 2012 Vlad Vasilyeu
 * 	for the code
 *
 * This software is free software; you can redistribute it and/or
 * modify it under the terms of the GNU  General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 *  General Public License for more details.
 *
 * You should have received a copy of the GNU  General Public
 * License along with this software; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
*/
/*******************************************************************************/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include "main.h"
/*******************************************************************************/

/* Global section */
FILE *file;
#define WIDTH  (480)
#define HEIGHT (800)
//#define WIDTH  (720)
//#define HEIGHT (1280)

#define DATA_XSD_PATH "/opt/apps/com.meecast.omweather/share/xsd/data.xsd"

Core::Config *
create_and_fill_config(){
    Core::Config *config;

    std::cerr<<"Create Config class: " << std::endl;
    try{
        config = Core::Config::Instance(Core::AbstractConfig::getConfigPath()+
                                       "config.xml",
                                       Core::AbstractConfig::prefix+
                                       Core::AbstractConfig::schemaPath+
                                       "config.xsd");
    }
    catch(const std::string &str){
        std::cerr<<"Error in Config class: "<< str <<std::endl;
        config =  Core::Config::Instance();
    }
    catch(const char *str){
        std::cerr<<"Error in Config class: "<< str <<std::endl;
        config =  Core::Config::Instance();
    }
    //std::cerr<<"End of creating Config class: " <<std::endl;
    config->saveConfig();
    std::cerr<<"End of creating Config class" <<std::endl;

    return config;

}

Core::DataParser*
current_data(std::string& str){
  Core::DataParser* dp;
  try{
        fprintf(stderr,"rrrrrrrrrrr %s\n", str.c_str());
        dp = Core::DataParser::Instance(str, DATA_XSD_PATH);
    }
    catch(const std::string &str){
        std::cerr<<"Error in DataParser class: "<< str <<std::endl;
        return NULL;
    }
    catch(const char *str){
        std::cerr<<"Error in DataParser class: "<< str <<std::endl;
        return NULL;
    }
    fprintf(stderr,"ooooorrrrrrrrrrr %p\n", dp);
    return dp;
}

static void
_on_delete(Ecore_Evas *ee)
{
   ecore_main_loop_quit();
}

static bool 
app_create(void *data)
{
	return TRUE;		//EXIT_SUCCESS
}

static void 
app_terminate(void *data)
{
//	struct appdata *ad = (struct appdata *)data;
//	_on_exit(ad);
}

static 
void app_pause(void *data){
	// Take necessary actions when application becomes invisible
}

static void 
app_resume(void *data){
	// Take necessary actions when application becomes visible.
}
static void 
app_win_del(void *data, Evas_Object * obj, void *event_info)
{
	elm_exit();
}

static Evas_Object 
*create_app_win(const char *name){
	Evas_Object *e;
	int w, h;
	e = elm_win_add(NULL, name, ELM_WIN_BASIC);
	if (e) {
		elm_win_title_set(e, name);
		elm_win_borderless_set(e, EINA_TRUE);
	    evas_object_smart_callback_add(e, "delete,request", app_win_del,
					       NULL);
		ecore_x_window_size_get(ecore_x_window_root_first_get(), &w, &h);
		evas_object_resize(e, w, h);
	}
	return e;
}

static bool 
app_launch(void *data){

	struct _App *app = (struct _App *)data;

	/*	Use elm_theme_extension_add() API before creating any widgets */
	//elm_theme_extension_add(NULL, MEECAST_THEME);

	/* main widnow */
	app->win = create_app_win("MeeCast");
	if (app->win == NULL) {
		return FALSE;
	}
	evas_object_show(app->win);

	elm_win_indicator_mode_set(app->win, ELM_WIN_INDICATOR_SHOW);
    
    /* Prepare config */
    app->config = create_and_fill_config();
    if ((app->config->stationsList().size() > 0) && app->config->current_station_id() > app->config->stationsList().size()) 
        app->dp = current_data(app->config->stationsList().at(app->config->current_station_id())->fileName());
    else 
        app->dp = NULL;

	/* load main view */
    create_main_window(app);

	/* register callback */
	//ecore_idler_add((Ecore_Task_Cb)_capture_idle_image, ad);
//	ecore_idler_add((Ecore_Task_Cb) _set_input_entry_focus, ad);
//	ecore_idler_add((Ecore_Task_Cb) _load_idle_view, ad);
//
	return TRUE;		//EXIT_SUCCESS
}

static void 
app_service(service_h service, void *data){

	struct _App *app = (struct _App *)data;

	if (app->win != NULL) { /* MeeCast has already launced. */
		elm_win_activate(app->win);
		return;
	}
	app_launch(app);
	evas_object_show(app->win);

}

int 
main(int argc, char *argv[])
{
    struct _App app;

	app_event_callback_s event_callback;

	event_callback.create = app_create;
	event_callback.terminate = app_terminate;
	event_callback.pause = app_pause;
	event_callback.resume = app_resume;
	event_callback.service = app_service;
	event_callback.low_memory = NULL;
	event_callback.low_battery = NULL;
	event_callback.device_orientation = NULL;
	event_callback.language_changed = NULL;
	event_callback.region_format_changed = NULL;

	memset(&app, 0x0, sizeof(struct _App));

	return app_efl_main(&argc, &argv, &event_callback, &app);
}


int
_main(void)
{
    int x, y, w, h;
    int i = 0, success = 0;
    struct _App app;

    app.dp = NULL;

    if (!ecore_evas_init())
        return EXIT_FAILURE;

    if (!edje_init())
        return EXIT_FAILURE;

    /* this will give you a window with an Evas canvas under the first
     * engine available */
    app.ee = ecore_evas_new(NULL, 0, 0, WIDTH, HEIGHT, NULL);
    if (!app.ee)
      goto error;

    ecore_evas_callback_delete_request_set(app.ee, _on_delete);
    ecore_evas_title_set(app.ee, "MeeCast");

    app.config = create_and_fill_config();
    /* Check time for previous updating */
    if ((app.config->stationsList().size() > 0) && app.config->current_station_id() > app.config->stationsList().size()) 
        app.dp = current_data(app.config->stationsList().at(app.config->current_station_id())->fileName());
    else 
        app.dp = NULL;
#if 0
    /* 25*60 = 30 minutes - minimal time between updates */ 
    if (app.dp && (abs(time(NULL) - app.dp->LastUpdate()) > 25*60)){
        /*update weather forecast*/
        for (i=0; i < app.config->stationsList().size();i++){
            if (app.config->stationsList().at(i)->updateData(true)){
                success ++;
            }
        }
    }
#endif
   create_main_window(&app);
   ecore_evas_show(app.ee);

   ecore_main_loop_begin();

   ecore_evas_free(app.ee);
   ecore_evas_shutdown();
   edje_shutdown();
   return 0;

error:
   fprintf(stderr, "You got to have at least one evas engine built"
                   " and linked up to ecore-evas for this example to run"
                   " properly.\n");
   ecore_evas_shutdown();
   return -1;

error_edj:
   fprintf(stderr, "Failed to load basic.edj!\n");

   ecore_evas_shutdown();
   return -2;
}

