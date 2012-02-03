/*
 ============================================================================
 Name        : hev-scgi-handler-lophone.c
 Author      : Heiher <admin@heiher.info>
 Version     : 0.0.1
 Copyright   : Copyright (C) 2011 everyone.
 Description : 
 ============================================================================
 */

#include <string.h>
#include <gmodule.h>

#include "hev-scgi-handler.h"
#include "hev-scgi-task.h"
#include "hev-scgi-request.h"
#include "hev-scgi-response.h"

#include "hev-lophone.h"
#include "hev-modem.h"
#include "hev-modem-cdma-phone.h"

#define HEV_SCGI_HANDLER_HELLO_NAME		"HevSCGIHandlerLOPhone"
#define HEV_SCGI_HANDLER_HELLO_VERSION	"0.0.1"
#define HEV_SCGI_HANDLER_HELLO_PATTERN	"^/lophone($|/.*)"

static void hev_scgi_handler_res_set_header(GHashTable *hash_table, const gchar *status);
static void hev_scgi_handler_res_write_header_handler(gpointer data, gpointer user_data);
static void hev_scgi_handler_res_output_stream_write_async_handler(GObject *source_object,
			GAsyncResult *res, gpointer user_data);
static void hev_scgi_handler_modem_get_info_async_handler(GObject *source_object,
			GAsyncResult *res, gpointer user_data);
static void hev_scgi_handler_modem_cdma_phone_get_status_async_handler(GObject *source_object,
			GAsyncResult *res, gpointer user_data);
static void hev_scgi_handler_modem_cdma_phone_lp_get_status_async_handler(GObject *source_object,
			GAsyncResult *res, gpointer user_data);
static void hev_scgi_handler_modem_cdma_phone_call_async_handler(GObject *source_object,
			GAsyncResult *res, gpointer user_data);
static void hev_scgi_handler_modem_cdma_phone_end_async_handler(GObject *source_object,
			GAsyncResult *res, gpointer user_data);
static void hev_scgi_handler_modem_cdma_phone_answer_async_handler(GObject *source_object,
			GAsyncResult *res, gpointer user_data);
static void hev_scgi_handler_modem_cdma_phone_get_status_id_async_handler(GObject *source_object,
			GAsyncResult *res, gpointer user_data);
static void hev_scgi_handler_modem_cdma_phone_send_dtmf_async_handler(GObject *source_object,
			GAsyncResult *res, gpointer user_data);
static void hev_scgi_handler_modem_cdma_phone_g_signal_handler(GDBusProxy *proxy,
			gchar *sender_name, gchar *signal_name,	GVariant *parameters, gpointer user_data);
static gboolean hev_scgi_handler_timeout_handler(gpointer user_data);

G_MODULE_EXPORT gboolean hev_scgi_handler_module_init(HevSCGIHandler *self)
{
	GObject *lophone = NULL;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	lophone = hev_lophone_new();
	if(!HEV_IS_LOPHONE(lophone))
	{
		g_critical("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
		return FALSE;
	}
	g_object_set_data(G_OBJECT(self), "LOPhone", lophone);

	return TRUE;
}

G_MODULE_EXPORT void hev_scgi_handler_module_finalize(HevSCGIHandler *self)
{
	GObject *lophone = NULL;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	lophone = g_object_get_data(G_OBJECT(self), "LOPhone");
	g_object_unref(lophone);
}

G_MODULE_EXPORT const gchar * hev_scgi_handler_module_get_name(HevSCGIHandler *self)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
	return HEV_SCGI_HANDLER_HELLO_NAME;
}

G_MODULE_EXPORT const gchar * hev_scgi_handler_module_get_version(HevSCGIHandler *self)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
	return HEV_SCGI_HANDLER_HELLO_VERSION;
}

G_MODULE_EXPORT const gchar * hev_scgi_handler_module_get_pattern(HevSCGIHandler *self)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
	return HEV_SCGI_HANDLER_HELLO_PATTERN;
}

G_MODULE_EXPORT void hev_scgi_handler_module_handle(HevSCGIHandler *self, GObject *scgi_task)
{
	GObject *lophone = NULL;
	GObject *scgi_request = NULL, *scgi_response = NULL;
	GHashTable *req_hash_table = NULL, *res_hash_table = NULL;
	GOutputStream *res_output_stream = NULL;
	const gchar *document_uri= NULL, *query_string = NULL;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	lophone = g_object_get_data(G_OBJECT(self), "LOPhone");

	scgi_request = hev_scgi_task_get_request(HEV_SCGI_TASK(scgi_task));
	scgi_response = hev_scgi_task_get_response(HEV_SCGI_TASK(scgi_task));

	req_hash_table =
		hev_scgi_request_get_header_hash_table(HEV_SCGI_REQUEST(scgi_request));
	res_hash_table =
		hev_scgi_response_get_header_hash_table(HEV_SCGI_RESPONSE(scgi_response));

	res_output_stream =
		hev_scgi_response_get_output_stream(HEV_SCGI_RESPONSE(scgi_response));

	document_uri = g_hash_table_lookup(req_hash_table, "DOCUMENT_URI");
	query_string = g_hash_table_lookup(req_hash_table, "QUERY_STRING");
	g_return_if_fail(document_uri);

	/* ListModems */
	if(g_regex_match_simple("^/lophone/listmodems$",
					document_uri, 0, 0))
	{
		GString *outbuffer = g_string_new(NULL);
		GList *l = NULL, *list = NULL;

		g_hash_table_insert(res_hash_table, "Status",
					g_strdup("200 OK"));
		g_hash_table_insert(res_hash_table, "Content-Type",
					g_strdup("text/plain"));

		list = hev_lophone_get_modems(HEV_LOPHONE(lophone));
		for(l=list; l; l=g_list_next(l))
		  g_string_append_printf(outbuffer, "%s\r\n", l->data);
		g_list_free(list);
		g_object_set_data(scgi_task, "outbuffer", outbuffer);

		hev_scgi_response_write_header(HEV_SCGI_RESPONSE(scgi_response),
					hev_scgi_handler_res_write_header_handler, scgi_task);
		g_object_ref(scgi_task);
	}
	/* GetModemInfo */
	else if(g_regex_match_simple("^/lophone/getmodeminfo$",
					document_uri, 0, 0))
	{
		GString *outbuffer = g_string_new(NULL);
		GRegex *regex = NULL;
		GMatchInfo *match_info = NULL;

		g_object_set_data(scgi_task, "outbuffer", outbuffer);

		regex = g_regex_new("^modem=([/\\w]+)&?", 0, 0, NULL);
		if(g_regex_match(regex, query_string, 0, &match_info))
		{
			gchar *path = g_match_info_fetch(match_info, 1);

			if(path)
			{
				GObject *modem = NULL;
				
				modem = hev_lophone_lookup_modem(HEV_LOPHONE(lophone),
							path);
				if(modem)
				{
					hev_modem_get_info(HEV_MODEM(modem), NULL,
								hev_scgi_handler_modem_get_info_async_handler,
								scgi_task);
					g_object_ref(scgi_task);
				}
				else
				{
					hev_scgi_handler_res_set_header(res_hash_table,
								"400 Bad Request");
					g_string_printf(outbuffer, "400 Bad Request");
					hev_scgi_response_write_header(HEV_SCGI_RESPONSE(scgi_response),
								hev_scgi_handler_res_write_header_handler, scgi_task);
					g_object_ref(scgi_task);
				}

				g_free(path);
			}
			else
			{
				hev_scgi_handler_res_set_header(res_hash_table,
							"400 Bad Request");
				g_string_printf(outbuffer, "400 Bad Request");
				hev_scgi_response_write_header(HEV_SCGI_RESPONSE(scgi_response),
							hev_scgi_handler_res_write_header_handler, scgi_task);
				g_object_ref(scgi_task);
			}

			g_match_info_free(match_info);
		}
		else
		{
			hev_scgi_handler_res_set_header(res_hash_table,
						"400 Bad Request");
			g_string_printf(outbuffer, "400 Bad Request");
			hev_scgi_response_write_header(HEV_SCGI_RESPONSE(scgi_response),
						hev_scgi_handler_res_write_header_handler, scgi_task);
			g_object_ref(scgi_task);
		}
		g_regex_unref(regex);
	}
	/* GetModemStatus */
	else if(g_regex_match_simple("^/lophone/getmodemstatus$",
					document_uri, 0, 0))
	{
		GString *outbuffer = g_string_new(NULL);
		GRegex *regex = NULL;
		GMatchInfo *match_info = NULL;

		g_object_set_data(scgi_task, "outbuffer", outbuffer);

		regex = g_regex_new("^modem=([/\\w]+)&?", 0, 0, NULL);
		if(g_regex_match(regex, query_string, 0, &match_info))
		{
			gchar *path = g_match_info_fetch(match_info, 1);

			if(path)
			{
				GObject *modem = NULL;
				
				modem = hev_lophone_lookup_modem(HEV_LOPHONE(lophone),
							path);
				if(modem)
				{
					GObject *cdma_phone = NULL;

					cdma_phone = hev_modem_get_cdma_phone(HEV_MODEM(modem));
					hev_modem_cdma_phone_get_status(HEV_MODEM_CDMA_PHONE(cdma_phone), NULL,
								hev_scgi_handler_modem_cdma_phone_get_status_async_handler,
								scgi_task);
					g_object_ref(scgi_task);
				}
				else
				{
					hev_scgi_handler_res_set_header(res_hash_table,
								"400 Bad Request");
					g_string_printf(outbuffer, "400 Bad Request");
					hev_scgi_response_write_header(HEV_SCGI_RESPONSE(scgi_response),
								hev_scgi_handler_res_write_header_handler, scgi_task);
					g_object_ref(scgi_task);
				}

				g_free(path);
			}
			else
			{
				hev_scgi_handler_res_set_header(res_hash_table,
							"400 Bad Request");
				g_string_printf(outbuffer, "400 Bad Request");
				hev_scgi_response_write_header(HEV_SCGI_RESPONSE(scgi_response),
							hev_scgi_handler_res_write_header_handler, scgi_task);
				g_object_ref(scgi_task);
			}

			g_match_info_free(match_info);
		}
		else
		{
			hev_scgi_handler_res_set_header(res_hash_table,
						"400 Bad Request");
			g_string_printf(outbuffer, "400 Bad Request");
			hev_scgi_response_write_header(HEV_SCGI_RESPONSE(scgi_response),
						hev_scgi_handler_res_write_header_handler, scgi_task);
			g_object_ref(scgi_task);
		}
		g_regex_unref(regex);
	}
	/* GetModemStatus (Long Polling) */
	else if(g_regex_match_simple("^/lophone/lpgetmodemstatus$",
					document_uri, 0, 0))
	{
		GString *outbuffer = g_string_new(NULL);
		GRegex *regex = NULL;
		GMatchInfo *match_info = NULL;

		g_object_set_data(scgi_task, "outbuffer", outbuffer);

		regex = g_regex_new("^modem=([/\\w]+)&status=(\\w+)&?", 0, 0, NULL);
		if(g_regex_match(regex, query_string, 0, &match_info))
		{
			gchar *path = g_match_info_fetch(match_info, 1);
			gchar *status = g_match_info_fetch(match_info, 2);

			if(path && status)
			{
				GObject *modem = NULL;
				
				modem = hev_lophone_lookup_modem(HEV_LOPHONE(lophone),
							path);
				if(modem)
				{
					static gchar buffer[16];
					GObject *cdma_phone = NULL;
					guint *timeout = g_malloc(sizeof(guint));

					cdma_phone = hev_modem_get_cdma_phone(HEV_MODEM(modem));
					g_object_set_data_full(scgi_task, "status", status,
								g_free);
					g_object_set_data_full(scgi_task, "timeout", timeout,
								g_free);
					*timeout = g_timeout_add(30 * 1000,
								hev_scgi_handler_timeout_handler, scgi_task);
					hev_modem_cdma_phone_get_status(HEV_MODEM_CDMA_PHONE(cdma_phone), NULL,
								hev_scgi_handler_modem_cdma_phone_lp_get_status_async_handler,
								scgi_task);
					g_object_ref(scgi_task);
				}
				else
				{
					hev_scgi_handler_res_set_header(res_hash_table,
								"400 Bad Request");
					g_string_printf(outbuffer, "400 Bad Request");
					hev_scgi_response_write_header(HEV_SCGI_RESPONSE(scgi_response),
								hev_scgi_handler_res_write_header_handler, scgi_task);
					g_object_ref(scgi_task);
				}
			}
			else
			{
				hev_scgi_handler_res_set_header(res_hash_table,
							"400 Bad Request");
				g_string_printf(outbuffer, "400 Bad Request");
				hev_scgi_response_write_header(HEV_SCGI_RESPONSE(scgi_response),
							hev_scgi_handler_res_write_header_handler, scgi_task);
				g_object_ref(scgi_task);
			}

			g_free(path);

			g_match_info_free(match_info);
		}
		else
		{
			hev_scgi_handler_res_set_header(res_hash_table,
						"400 Bad Request");
			g_string_printf(outbuffer, "400 Bad Request");
			hev_scgi_response_write_header(HEV_SCGI_RESPONSE(scgi_response),
						hev_scgi_handler_res_write_header_handler, scgi_task);
			g_object_ref(scgi_task);
		}
		g_regex_unref(regex);
	}
	/* PhoneCall */
	else if(g_regex_match_simple("^/lophone/phonecall$",
					document_uri, 0, 0))
	{
		GString *outbuffer = g_string_new(NULL);
		GRegex *regex = NULL;
		GMatchInfo *match_info = NULL;

		g_object_set_data(scgi_task, "outbuffer", outbuffer);

		regex = g_regex_new("^modem=([/\\w]+)&number=([\\d]+)&?", 0, 0, NULL);
		if(g_regex_match(regex, query_string, 0, &match_info))
		{
			gchar *path = g_match_info_fetch(match_info, 1);
			gchar *number = g_match_info_fetch(match_info, 2);

			if(path && number)
			{
				GObject *modem = NULL;
				
				modem = hev_lophone_lookup_modem(HEV_LOPHONE(lophone),
							path);
				if(modem)
				{
					GObject *cdma_phone = NULL;

					cdma_phone = hev_modem_get_cdma_phone(HEV_MODEM(modem));
					hev_modem_cdma_phone_call(HEV_MODEM_CDMA_PHONE(cdma_phone),
								number, NULL,
								hev_scgi_handler_modem_cdma_phone_call_async_handler,
								scgi_task);
					g_object_ref(scgi_task);
				}
				else
				{
					hev_scgi_handler_res_set_header(res_hash_table,
								"400 Bad Request");
					g_string_printf(outbuffer, "400 Bad Request");
					hev_scgi_response_write_header(HEV_SCGI_RESPONSE(scgi_response),
								hev_scgi_handler_res_write_header_handler, scgi_task);
					g_object_ref(scgi_task);
				}
			}
			else
			{
				hev_scgi_handler_res_set_header(res_hash_table,
							"400 Bad Request");
				g_string_printf(outbuffer, "400 Bad Request");
				hev_scgi_response_write_header(HEV_SCGI_RESPONSE(scgi_response),
							hev_scgi_handler_res_write_header_handler, scgi_task);
				g_object_ref(scgi_task);
			}

			g_free(path);
			g_free(number);
			g_match_info_free(match_info);
		}
		else
		{
			hev_scgi_handler_res_set_header(res_hash_table,
						"400 Bad Request");
			g_string_printf(outbuffer, "400 Bad Request");
			hev_scgi_response_write_header(HEV_SCGI_RESPONSE(scgi_response),
						hev_scgi_handler_res_write_header_handler, scgi_task);
			g_object_ref(scgi_task);
		}
		g_regex_unref(regex);
	}
	/* PhoneEnd */
	else if(g_regex_match_simple("^/lophone/phoneend$",
					document_uri, 0, 0))
	{
		GString *outbuffer = g_string_new(NULL);
		GRegex *regex = NULL;
		GMatchInfo *match_info = NULL;

		g_object_set_data(scgi_task, "outbuffer", outbuffer);

		regex = g_regex_new("^modem=([/\\w]+)&?", 0, 0, NULL);
		if(g_regex_match(regex, query_string, 0, &match_info))
		{
			gchar *path = g_match_info_fetch(match_info, 1);

			if(path)
			{
				GObject *modem = NULL;
				
				modem = hev_lophone_lookup_modem(HEV_LOPHONE(lophone),
							path);
				if(modem)
				{
					GObject *cdma_phone = NULL;

					cdma_phone = hev_modem_get_cdma_phone(HEV_MODEM(modem));
					hev_modem_cdma_phone_end(HEV_MODEM_CDMA_PHONE(cdma_phone), NULL,
								hev_scgi_handler_modem_cdma_phone_end_async_handler,
								scgi_task);
					g_object_ref(scgi_task);
				}
				else
				{
					hev_scgi_handler_res_set_header(res_hash_table,
								"400 Bad Request");
					g_string_printf(outbuffer, "400 Bad Request");
					hev_scgi_response_write_header(HEV_SCGI_RESPONSE(scgi_response),
								hev_scgi_handler_res_write_header_handler, scgi_task);
					g_object_ref(scgi_task);
				}

				g_free(path);
			}
			else
			{
				hev_scgi_handler_res_set_header(res_hash_table,
							"400 Bad Request");
				g_string_printf(outbuffer, "400 Bad Request");
				hev_scgi_response_write_header(HEV_SCGI_RESPONSE(scgi_response),
							hev_scgi_handler_res_write_header_handler, scgi_task);
				g_object_ref(scgi_task);
			}

			g_match_info_free(match_info);
		}
		else
		{
			hev_scgi_handler_res_set_header(res_hash_table,
						"400 Bad Request");
			g_string_printf(outbuffer, "400 Bad Request");
			hev_scgi_response_write_header(HEV_SCGI_RESPONSE(scgi_response),
						hev_scgi_handler_res_write_header_handler, scgi_task);
			g_object_ref(scgi_task);
		}
		g_regex_unref(regex);
	}
	/* PhoneAnswer */
	else if(g_regex_match_simple("^/lophone/phoneanswer$",
					document_uri, 0, 0))
	{
		GString *outbuffer = g_string_new(NULL);
		GRegex *regex = NULL;
		GMatchInfo *match_info = NULL;

		g_object_set_data(scgi_task, "outbuffer", outbuffer);

		regex = g_regex_new("^modem=([/\\w]+)&?", 0, 0, NULL);
		if(g_regex_match(regex, query_string, 0, &match_info))
		{
			gchar *path = g_match_info_fetch(match_info, 1);

			if(path)
			{
				GObject *modem = NULL;
				
				modem = hev_lophone_lookup_modem(HEV_LOPHONE(lophone),
							path);
				if(modem)
				{
					GObject *cdma_phone = NULL;

					cdma_phone = hev_modem_get_cdma_phone(HEV_MODEM(modem));
					hev_modem_cdma_phone_answer(HEV_MODEM_CDMA_PHONE(cdma_phone), NULL,
								hev_scgi_handler_modem_cdma_phone_answer_async_handler,
								scgi_task);
					g_object_ref(scgi_task);
				}
				else
				{
					hev_scgi_handler_res_set_header(res_hash_table,
								"400 Bad Request");
					g_string_printf(outbuffer, "400 Bad Request");
					hev_scgi_response_write_header(HEV_SCGI_RESPONSE(scgi_response),
								hev_scgi_handler_res_write_header_handler, scgi_task);
					g_object_ref(scgi_task);
				}

				g_free(path);
			}
			else
			{
				hev_scgi_handler_res_set_header(res_hash_table,
							"400 Bad Request");
				g_string_printf(outbuffer, "400 Bad Request");
				hev_scgi_response_write_header(HEV_SCGI_RESPONSE(scgi_response),
							hev_scgi_handler_res_write_header_handler, scgi_task);
				g_object_ref(scgi_task);
			}

			g_match_info_free(match_info);
		}
		else
		{
			hev_scgi_handler_res_set_header(res_hash_table,
						"400 Bad Request");
			g_string_printf(outbuffer, "400 Bad Request");
			hev_scgi_response_write_header(HEV_SCGI_RESPONSE(scgi_response),
						hev_scgi_handler_res_write_header_handler, scgi_task);
			g_object_ref(scgi_task);
		}
		g_regex_unref(regex);
	}
	/* PhoneSendDTMF */
	else if(g_regex_match_simple("^/lophone/phonesenddtmf$",
					document_uri, 0, 0))
	{
		GString *outbuffer = g_string_new(NULL);
		GRegex *regex = NULL;
		GMatchInfo *match_info = NULL;

		g_object_set_data(scgi_task, "outbuffer", outbuffer);

		regex = g_regex_new("^modem=([/\\w]+)&digit=([\\d%%abcdefABCDEF]+)&?", 0, 0, NULL);
		if(g_regex_match(regex, query_string, 0, &match_info))
		{
			gchar *path = g_match_info_fetch(match_info, 1);
			gchar *digit = g_match_info_fetch(match_info, 2);

			if(path && digit)
			{
				GObject *modem = NULL;
				
				modem = hev_lophone_lookup_modem(HEV_LOPHONE(lophone),
							path);
				if(modem)
				{
					GObject *cdma_phone = NULL;

					cdma_phone = hev_modem_get_cdma_phone(HEV_MODEM(modem));
					g_object_set_data_full(scgi_task, "digit", digit, g_free);
					hev_modem_cdma_phone_get_status(HEV_MODEM_CDMA_PHONE(cdma_phone), NULL,
								hev_scgi_handler_modem_cdma_phone_get_status_id_async_handler,
								scgi_task);
					g_object_ref(scgi_task);
				}
				else
				{
					hev_scgi_handler_res_set_header(res_hash_table,
								"400 Bad Request");
					g_string_printf(outbuffer, "400 Bad Request");
					hev_scgi_response_write_header(HEV_SCGI_RESPONSE(scgi_response),
								hev_scgi_handler_res_write_header_handler, scgi_task);
					g_object_ref(scgi_task);
				}
			}
			else
			{
				hev_scgi_handler_res_set_header(res_hash_table,
							"400 Bad Request");
				g_string_printf(outbuffer, "400 Bad Request");
				hev_scgi_response_write_header(HEV_SCGI_RESPONSE(scgi_response),
							hev_scgi_handler_res_write_header_handler, scgi_task);
				g_object_ref(scgi_task);
			}

			g_free(path);
			g_match_info_free(match_info);
		}
		else
		{
			hev_scgi_handler_res_set_header(res_hash_table,
						"400 Bad Request");
			g_string_printf(outbuffer, "400 Bad Request");
			hev_scgi_response_write_header(HEV_SCGI_RESPONSE(scgi_response),
						hev_scgi_handler_res_write_header_handler, scgi_task);
			g_object_ref(scgi_task);
		}
		g_regex_unref(regex);
	}
	/* Not Implemented */
	else
	{
		GString *outbuffer = g_string_new(NULL);

		hev_scgi_handler_res_set_header(res_hash_table,
					"503 Not Implemented");
		g_string_printf(outbuffer, "503 Not Implemented");
		g_object_set_data(scgi_task, "outbuffer", outbuffer);

		hev_scgi_response_write_header(HEV_SCGI_RESPONSE(scgi_response),
					hev_scgi_handler_res_write_header_handler, scgi_task);
		g_object_ref(scgi_task);
	}
}

static void hev_scgi_handler_res_set_header(GHashTable *hash_table, const gchar *status)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	g_hash_table_insert(hash_table, "Status",
				g_strdup(status));
	g_hash_table_insert(hash_table, "Content-Type",
				g_strdup("text/plain"));
}

static void hev_scgi_handler_res_write_header_handler(gpointer data, gpointer user_data)
{
	GObject *handler = NULL;
	GObject *scgi_task = G_OBJECT(user_data);
	GObject *scgi_response = NULL;
	GOutputStream *res_output_stream = NULL;
	GString *outbuffer = NULL;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	handler = hev_scgi_task_get_handler(HEV_SCGI_TASK(scgi_task));

	scgi_response = hev_scgi_task_get_response(HEV_SCGI_TASK(scgi_task));
	res_output_stream =
		hev_scgi_response_get_output_stream(HEV_SCGI_RESPONSE(scgi_response));

	outbuffer = g_object_get_data(scgi_task, "outbuffer");
	g_output_stream_write_async(res_output_stream,
				outbuffer->str, outbuffer->len, G_PRIORITY_DEFAULT, NULL,
				hev_scgi_handler_res_output_stream_write_async_handler,
				scgi_task);
}

static void hev_scgi_handler_res_output_stream_write_async_handler(GObject *source_object,
			GAsyncResult *res, gpointer user_data)
{
	GObject *scgi_task = G_OBJECT(user_data);
	GString *outbuffer = NULL;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	g_output_stream_write_finish(G_OUTPUT_STREAM(source_object),
				res, NULL);
	outbuffer = g_object_get_data(scgi_task, "outbuffer");

	g_string_free(outbuffer, TRUE);
	g_object_unref(scgi_task);
}

static void hev_scgi_handler_modem_get_info_async_handler(GObject *source_object,
			GAsyncResult *res, gpointer user_data)
{
	GObject *scgi_task = G_OBJECT(user_data);
	GObject *scgi_response = NULL;
	GHashTable *res_hash_table = NULL;
	GVariant *retval = NULL;
	GString *outbuffer = NULL;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	scgi_response = hev_scgi_task_get_response(HEV_SCGI_TASK(scgi_task));
	res_hash_table =
		hev_scgi_response_get_header_hash_table(HEV_SCGI_RESPONSE(scgi_response));
	outbuffer = g_object_get_data(scgi_task, "outbuffer");
	retval = hev_modem_get_info_finish(HEV_MODEM(source_object), res, NULL);
	if(retval)
	{
		GVariant *infos = NULL;
		gsize i = 0, len = 0;

		hev_scgi_handler_res_set_header(res_hash_table,
					"200 OK");
		infos = g_variant_get_child_value(retval, 0);
		len = g_variant_n_children(infos);
		for(i=0; i<len; i++)
		{
			GVariant *value = g_variant_get_child_value(infos, i);
			g_string_append_printf(outbuffer, "%s\r\n",
						g_variant_get_string(value, NULL));
		}

		g_variant_unref(retval);
	}
	else
	{
		hev_scgi_handler_res_set_header(res_hash_table,
					"400 Bad Request");
		g_string_printf(outbuffer, "400 Bad Request");
	}

	hev_scgi_response_write_header(HEV_SCGI_RESPONSE(scgi_response),
				hev_scgi_handler_res_write_header_handler, scgi_task);
}

static void hev_scgi_handler_modem_cdma_phone_get_status_async_handler(GObject *source_object,
			GAsyncResult *res, gpointer user_data)
{
	GObject *scgi_task = G_OBJECT(user_data);
	GObject *scgi_response = NULL;
	GHashTable *res_hash_table = NULL;
	GVariant *retval = NULL;
	GString *outbuffer = NULL;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	scgi_response = hev_scgi_task_get_response(HEV_SCGI_TASK(scgi_task));
	res_hash_table =
		hev_scgi_response_get_header_hash_table(HEV_SCGI_RESPONSE(scgi_response));
	outbuffer = g_object_get_data(scgi_task, "outbuffer");
	retval = hev_modem_cdma_phone_get_status_finish(HEV_MODEM_CDMA_PHONE(source_object),
				res, NULL);
	if(retval)
	{
		GVariant *values = NULL;
		GVariant *dir = NULL, *state = NULL;
		GVariant *number = NULL, *priority = NULL;

		hev_scgi_handler_res_set_header(res_hash_table,
					"200 OK");
		values = g_variant_get_child_value(retval, 0);
		dir = g_variant_lookup_value(values, "dir", NULL);
		state = g_variant_lookup_value(values, "state", NULL);
		number = g_variant_lookup_value(values, "number", NULL);
		priority = g_variant_lookup_value(values, "priority", NULL);

		if((0==strlen(g_variant_get_string(number, NULL))) &&
					(0==g_variant_get_uint32(priority)))
		{
			g_string_printf(outbuffer, "idle");
		}
		else
		{
			if(1 == g_variant_get_uint32(dir))
			{
				if(0 == g_variant_get_uint32(state))
				  g_string_printf(outbuffer, "connected\r\n%s",
							  g_variant_get_string(number, NULL));
				else
				  g_string_printf(outbuffer, "callingin\r\n%s",
							  g_variant_get_string(number, NULL));
			}
			else
			{
				if(0 == g_variant_get_uint32(state))
				  g_string_printf(outbuffer, "connected\r\n%s",
							  g_variant_get_string(number, NULL));
				else
				  g_string_printf(outbuffer, "callingout\r\n%s",
							  g_variant_get_string(number, NULL));
			}
		}

		g_variant_unref(retval);
	}
	else
	{
		hev_scgi_handler_res_set_header(res_hash_table,
					"500 Internal Server Error");
		g_string_printf(outbuffer, "500 Internal Server Error");
	}

	hev_scgi_response_write_header(HEV_SCGI_RESPONSE(scgi_response),
				hev_scgi_handler_res_write_header_handler, scgi_task);
}

static void hev_scgi_handler_modem_cdma_phone_lp_get_status_async_handler(GObject *source_object,
			GAsyncResult *res, gpointer user_data)
{
	GObject *scgi_task = G_OBJECT(user_data);
	GObject *scgi_response = NULL;
	GHashTable *res_hash_table = NULL;
	GVariant *retval = NULL;
	GString *outbuffer = NULL;
	gchar *status = NULL, *new_status = NULL;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	scgi_response = hev_scgi_task_get_response(HEV_SCGI_TASK(scgi_task));
	res_hash_table =
		hev_scgi_response_get_header_hash_table(HEV_SCGI_RESPONSE(scgi_response));
	outbuffer = g_object_get_data(scgi_task, "outbuffer");
	status = g_object_get_data(scgi_task, "status");
	retval = hev_modem_cdma_phone_get_status_finish(HEV_MODEM_CDMA_PHONE(source_object),
				res, NULL);
	if(retval)
	{
		GVariant *values = NULL;
		GVariant *dir = NULL, *state = NULL;
		GVariant *number = NULL, *priority = NULL;

		values = g_variant_get_child_value(retval, 0);
		dir = g_variant_lookup_value(values, "dir", NULL);
		state = g_variant_lookup_value(values, "state", NULL);
		number = g_variant_lookup_value(values, "number", NULL);
		priority = g_variant_lookup_value(values, "priority", NULL);

		if((0==strlen(g_variant_get_string(number, NULL))) &&
					(0==g_variant_get_uint32(priority)))
		{
			new_status = "idle";
			g_string_printf(outbuffer, "idle");
		}
		else
		{
			if(1 == g_variant_get_uint32(dir))
			{
				if(0 == g_variant_get_uint32(state))
				{
					new_status = "connected";
					g_string_printf(outbuffer, "connected");
				}
				else
				{
					new_status = "callingin";
					g_string_printf(outbuffer, "callingin");
				}
			}
			else
			{
				if(0 == g_variant_get_uint32(state))
				{
					new_status = "connected";
					g_string_printf(outbuffer, "connected");
				}
				else
				{
					new_status = "callingout";
					g_string_printf(outbuffer, "callingout");
				}
			}
		}

		if(0 == g_strcmp0(status, new_status))
		{
			gulong *handler = g_malloc0(sizeof(gulong));

			*handler = g_signal_connect(source_object, "g-signal",
						G_CALLBACK(hev_scgi_handler_modem_cdma_phone_g_signal_handler),
						scgi_task);
			g_object_set_data(scgi_task, "cdma_phone", source_object);
			g_object_set_data_full(scgi_task, "handler", handler, g_free);
		}
		else
		{
			hev_scgi_handler_res_set_header(res_hash_table,
						"200 OK");
			hev_scgi_response_write_header(HEV_SCGI_RESPONSE(scgi_response),
						hev_scgi_handler_res_write_header_handler, scgi_task);
		}

		g_variant_unref(retval);
	}
	else
	{
		hev_scgi_handler_res_set_header(res_hash_table,
					"500 Internal Server Error");
		g_string_printf(outbuffer, "500 Internal Server Error");

		hev_scgi_response_write_header(HEV_SCGI_RESPONSE(scgi_response),
					hev_scgi_handler_res_write_header_handler, scgi_task);
	}
}

static void hev_scgi_handler_modem_cdma_phone_call_async_handler(GObject *source_object,
			GAsyncResult *res, gpointer user_data)
{
	GObject *scgi_task = G_OBJECT(user_data);
	GObject *scgi_response = NULL;
	GHashTable *res_hash_table = NULL;
	GVariant *retval = NULL;
	GString *outbuffer = NULL;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	scgi_response = hev_scgi_task_get_response(HEV_SCGI_TASK(scgi_task));
	res_hash_table =
		hev_scgi_response_get_header_hash_table(HEV_SCGI_RESPONSE(scgi_response));
	outbuffer = g_object_get_data(scgi_task, "outbuffer");
	retval = hev_modem_cdma_phone_call_finish(HEV_MODEM_CDMA_PHONE(source_object),
				res, NULL);
	if(retval)
	{
		hev_scgi_handler_res_set_header(res_hash_table,
					"200 OK");
		g_string_printf(outbuffer, "200 OK");

		g_variant_unref(retval);
	}
	else
	{
		hev_scgi_handler_res_set_header(res_hash_table,
					"500 Internal Server Error");
		g_string_printf(outbuffer, "500 Internal Server Error");
	}

	hev_scgi_response_write_header(HEV_SCGI_RESPONSE(scgi_response),
				hev_scgi_handler_res_write_header_handler, scgi_task);
}

static void hev_scgi_handler_modem_cdma_phone_end_async_handler(GObject *source_object,
			GAsyncResult *res, gpointer user_data)
{
	GObject *scgi_task = G_OBJECT(user_data);
	GObject *scgi_response = NULL;
	GHashTable *res_hash_table = NULL;
	GVariant *retval = NULL;
	GString *outbuffer = NULL;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	scgi_response = hev_scgi_task_get_response(HEV_SCGI_TASK(scgi_task));
	res_hash_table =
		hev_scgi_response_get_header_hash_table(HEV_SCGI_RESPONSE(scgi_response));
	outbuffer = g_object_get_data(scgi_task, "outbuffer");
	retval = hev_modem_cdma_phone_end_finish(HEV_MODEM_CDMA_PHONE(source_object),
				res, NULL);
	if(retval)
	{
		hev_scgi_handler_res_set_header(res_hash_table,
					"200 OK");
		g_string_printf(outbuffer, "200 OK");

		g_variant_unref(retval);
	}
	else
	{
		hev_scgi_handler_res_set_header(res_hash_table,
					"500 Internal Server Error");
		g_string_printf(outbuffer, "500 Internal Server Error");
	}

	hev_scgi_response_write_header(HEV_SCGI_RESPONSE(scgi_response),
				hev_scgi_handler_res_write_header_handler, scgi_task);
}

static void hev_scgi_handler_modem_cdma_phone_answer_async_handler(GObject *source_object,
			GAsyncResult *res, gpointer user_data)
{
	GObject *scgi_task = G_OBJECT(user_data);
	GObject *scgi_response = NULL;
	GHashTable *res_hash_table = NULL;
	GVariant *retval = NULL;
	GString *outbuffer = NULL;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	scgi_response = hev_scgi_task_get_response(HEV_SCGI_TASK(scgi_task));
	res_hash_table =
		hev_scgi_response_get_header_hash_table(HEV_SCGI_RESPONSE(scgi_response));
	outbuffer = g_object_get_data(scgi_task, "outbuffer");
	retval = hev_modem_cdma_phone_answer_finish(HEV_MODEM_CDMA_PHONE(source_object),
				res, NULL);
	if(retval)
	{
		hev_scgi_handler_res_set_header(res_hash_table,
					"200 OK");
		g_string_printf(outbuffer, "200 OK");

		g_variant_unref(retval);
	}
	else
	{
		hev_scgi_handler_res_set_header(res_hash_table,
					"500 Internal Server Error");
		g_string_printf(outbuffer, "500 Internal Server Error");
	}

	hev_scgi_response_write_header(HEV_SCGI_RESPONSE(scgi_response),
				hev_scgi_handler_res_write_header_handler, scgi_task);
}

static void hev_scgi_handler_modem_cdma_phone_get_status_id_async_handler(GObject *source_object,
			GAsyncResult *res, gpointer user_data)
{
	GObject *scgi_task = G_OBJECT(user_data);
	GObject *scgi_response = NULL;
	GHashTable *res_hash_table = NULL;
	GVariant *retval = NULL;
	GString *outbuffer = NULL;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	scgi_response = hev_scgi_task_get_response(HEV_SCGI_TASK(scgi_task));
	res_hash_table =
		hev_scgi_response_get_header_hash_table(HEV_SCGI_RESPONSE(scgi_response));
	outbuffer = g_object_get_data(scgi_task, "outbuffer");
	retval = hev_modem_cdma_phone_get_status_finish(HEV_MODEM_CDMA_PHONE(source_object),
				res, NULL);
	if(retval)
	{
		GVariant *values = NULL;
		GVariant *id = NULL;
		gchar *sid = NULL, * digit = NULL;

		values = g_variant_get_child_value(retval, 0);
		id = g_variant_lookup_value(values, "id", NULL);
		sid = g_strdup_printf("%u", g_variant_get_uint32(id));

		digit = g_object_get_data(scgi_task, "digit");
		digit = g_uri_unescape_string(digit, NULL);

		hev_modem_cdma_phone_send_dtmf(HEV_MODEM_CDMA_PHONE(source_object),
					sid, digit, NULL,
					hev_scgi_handler_modem_cdma_phone_send_dtmf_async_handler,
					scgi_task);

		g_free(sid);
		g_free(digit);
		g_variant_unref(retval);
	}
	else
	{
		hev_scgi_handler_res_set_header(res_hash_table,
					"500 Internal Server Error");
		g_string_printf(outbuffer, "500 Internal Server Error");

		hev_scgi_response_write_header(HEV_SCGI_RESPONSE(scgi_response),
					hev_scgi_handler_res_write_header_handler, scgi_task);
	}
}

static void hev_scgi_handler_modem_cdma_phone_send_dtmf_async_handler(GObject *source_object,
			GAsyncResult *res, gpointer user_data)
{
	GObject *scgi_task = G_OBJECT(user_data);
	GObject *scgi_response = NULL;
	GHashTable *res_hash_table = NULL;
	GVariant *retval = NULL;
	GString *outbuffer = NULL;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	scgi_response = hev_scgi_task_get_response(HEV_SCGI_TASK(scgi_task));
	res_hash_table =
		hev_scgi_response_get_header_hash_table(HEV_SCGI_RESPONSE(scgi_response));
	outbuffer = g_object_get_data(scgi_task, "outbuffer");
	retval = hev_modem_cdma_phone_send_dtmf_finish(HEV_MODEM_CDMA_PHONE(source_object),
				res, NULL);
	if(retval)
	{
		hev_scgi_handler_res_set_header(res_hash_table,
					"200 OK");
		g_string_printf(outbuffer, "200 OK");

		g_variant_unref(retval);
	}
	else
	{
		hev_scgi_handler_res_set_header(res_hash_table,
					"500 Internal Server Error");
		g_string_printf(outbuffer, "500 Internal Server Error");
	}

	hev_scgi_response_write_header(HEV_SCGI_RESPONSE(scgi_response),
				hev_scgi_handler_res_write_header_handler, scgi_task);
}

static void hev_scgi_handler_modem_cdma_phone_g_signal_handler(GDBusProxy *proxy,
			gchar *sender_name, gchar *signal_name,	GVariant *parameters, gpointer user_data)
{
	GObject *scgi_task = G_OBJECT(user_data);
	GObject *scgi_response = NULL;
	GHashTable *res_hash_table = NULL;
	gulong *handler = g_object_get_data(scgi_task, "handler");
	guint *timeout = g_object_get_data(scgi_task, "timeout");
	GString *outbuffer = g_object_get_data(scgi_task, "outbuffer");

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	g_signal_handler_disconnect(proxy, *handler);
	if(timeout)
	  g_source_remove(*timeout);

	scgi_response = hev_scgi_task_get_response(HEV_SCGI_TASK(scgi_task));
	res_hash_table =
		hev_scgi_response_get_header_hash_table(HEV_SCGI_RESPONSE(scgi_response));

	if(0==g_strcmp0(signal_name, "Ring") ||
				(0==g_strcmp0(signal_name, "Clip")))
	{
		hev_scgi_handler_res_set_header(res_hash_table,
					"200 OK");
		g_string_printf(outbuffer, "callingin");
	}
	else if(0 == g_strcmp0(signal_name, "Orig"))
	{
		hev_scgi_handler_res_set_header(res_hash_table,
					"200 OK");
		g_string_printf(outbuffer, "callingout");
	}
	else if(0 == g_strcmp0(signal_name, "Conn"))
	{
		hev_scgi_handler_res_set_header(res_hash_table,
					"200 OK");
		g_string_printf(outbuffer, "connected");
	}
	else if(0 == g_strcmp0(signal_name, "Cend"))
	{
		hev_scgi_handler_res_set_header(res_hash_table,
					"200 OK");
		g_string_printf(outbuffer, "idle");
	}
	else
	{
		hev_scgi_handler_res_set_header(res_hash_table,
					"500 Internal Server Error");
	}

	hev_scgi_response_write_header(HEV_SCGI_RESPONSE(scgi_response),
				hev_scgi_handler_res_write_header_handler, scgi_task);
}

static gboolean hev_scgi_handler_timeout_handler(gpointer user_data)
{
	GObject *scgi_task = G_OBJECT(user_data);
	gulong *handler = g_object_get_data(scgi_task, "handler");
	GObject *cdma_phone = g_object_get_data(scgi_task, "cdma_phone");

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	if(handler)
	  g_signal_handler_disconnect(cdma_phone, *handler);

	g_object_unref(scgi_task);

	return FALSE;
}

