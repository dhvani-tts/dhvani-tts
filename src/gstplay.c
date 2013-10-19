#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "gstplay.h"
#include "debug.h"

static gboolean bus0_callback(GstBus *bus0,
			      GstMessage *msg,
			      gpointer *data)
{
  GMainLoop *loop0 = (GMainLoop *) data;
  switch(GST_MESSAGE_TYPE(msg))
    {
    case GST_MESSAGE_EOS:
      {
	DHVANI_DEBUG("got EOS..\n");
	g_main_loop_quit(loop0);
      }
      break;
    case GST_MESSAGE_ERROR:
      {
	gchar *debug = NULL;
	GError *error = NULL;
	gst_message_parse_error(msg, &error, &debug);
	DHVANI_ERROR("%s: %s [%d]\n%s\n",
		     g_quark_to_string(error->domain),
		     error->message,
		     error->code,
		     debug);
	g_error_free(error);
	g_free(debug);
	g_main_loop_quit(loop0);
      }
      break;
    }
  return TRUE;
}

int gstplay(const char *filename)
{
  GstElement *pipeline0 = NULL;
  GstElement *filesrc0 = NULL;
  GstElement *audioparse0 = NULL;
  GstElement *autoaudiosink0 = NULL;
  GstBus *bus0 = NULL;
  GMainLoop *loop0 = NULL;
  guint bus_watch_id;

  gst_init(NULL, NULL);

  loop0 = g_main_loop_new(NULL, FALSE);
  pipeline0 = gst_pipeline_new("pipeline0");
  filesrc0 = gst_element_factory_make("filesrc", "filesrc0");
  audioparse0 = gst_element_factory_make("audioparse", "audioparse0");
  autoaudiosink0 = gst_element_factory_make("autoaudiosink", "autoaudiosink0");
  bus0 = gst_pipeline_get_bus(GST_PIPELINE(pipeline0));

  if(!loop0 || !pipeline0 || !filesrc0 || !audioparse0 || !autoaudiosink0 || !bus0)
    {
      DHVANI_ERROR("objects not created!!\n");
      exit(EXIT_FAILURE);
    }

  g_object_set(G_OBJECT(filesrc0), "location", filename, NULL);
  g_object_set(G_OBJECT(audioparse0), "format", 0, NULL);
  g_object_set(G_OBJECT(audioparse0), "raw-format", 4, NULL);
  g_object_set(G_OBJECT(audioparse0), "rate", 16000, NULL);
  g_object_set(G_OBJECT(audioparse0), "channels", 1, NULL);

  bus_watch_id = gst_bus_add_watch(bus0, (GstBusFunc) bus0_callback, loop0);
  gst_bin_add_many(GST_BIN(pipeline0), filesrc0, audioparse0, autoaudiosink0, NULL);
  gst_element_link_many(filesrc0, audioparse0, autoaudiosink0, NULL);

  DHVANI_DEBUG("playing %s\n", filename);
  gst_element_set_state(pipeline0, GST_STATE_PLAYING);
  g_main_loop_run(loop0);
  gst_element_set_state(pipeline0, GST_STATE_NULL);
  DHVANI_DEBUG("finished playing %s\n", filename);
  gst_object_unref(GST_OBJECT(pipeline0));
  g_source_remove(bus_watch_id);
  gst_object_unref(bus0);
  g_main_loop_unref(loop0);

  return 0;
}
