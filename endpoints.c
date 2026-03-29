#include <stdlib.h>
#include <time.h>
#include <ulfius.h>
#include <jansson.h>
#include "endpoints.h"

static const char *QUOTES_FILE = "cytaty.json";

static int callback_helloworld(const struct _u_request *request,
                               struct _u_response *response,
                               void *user_data) {
    (void)request;
    (void)user_data;

    ulfius_set_string_body_response(response, 200, "Hello World from bombapi!");
    return U_CALLBACK_CONTINUE;
}

static json_t *load_quotes_file(void) {
    json_error_t error;
    return json_load_file(QUOTES_FILE, 0, &error);
}

static json_t *build_random_quote_response(const char *quote_type) {
    json_t *root = load_quotes_file();
    json_t *selected = NULL;
    json_t *item = NULL;
    size_t index = 0;
    size_t match_count = 0;
    size_t target = 0;
    json_t *response = NULL;
    const char *mission = NULL;
    json_t *entries = NULL;

    if (root == NULL || !json_is_array(root)) {
        if (root != NULL) {
            json_decref(root);
        }
        return NULL;
    }

    json_array_foreach(root, index, item) {
        const char *type = json_string_value(json_object_get(item, "type"));
        if (type != NULL && o_strcmp(type, quote_type) == 0) {
            match_count++;
        }
    }

    if (match_count == 0) {
        json_decref(root);
        return NULL;
    }

    target = (size_t)(rand() % (int)match_count);
    match_count = 0;

    json_array_foreach(root, index, item) {
        const char *type = json_string_value(json_object_get(item, "type"));
        if (type != NULL && o_strcmp(type, quote_type) == 0) {
            if (match_count == target) {
                selected = item;
                break;
            }
            match_count++;
        }
    }

    if (selected == NULL) {
        json_decref(root);
        return NULL;
    }

    response = json_object();
    if (response == NULL) {
        json_decref(root);
        return NULL;
    }

    json_object_set_new(response, "id", json_integer(json_integer_value(json_object_get(selected, "id"))));

    mission = json_string_value(json_object_get(selected, "mission"));
    json_object_set_new(response, "mission", json_string(mission != NULL ? mission : "-"));
    json_object_set_new(response, "type", json_string(quote_type));

    entries = json_object_get(selected, "entries");
    if (entries != NULL && json_is_array(entries)) {
        json_object_set_new(response, "entries", json_deep_copy(entries));
    } else {
        json_object_set_new(response, "entries", json_array());
    }

    json_decref(root);
    return response;
}

static int callback_random_dialogue(const struct _u_request *request,
                                    struct _u_response *response,
                                    void *user_data) {
    json_t *payload = NULL;

    (void)request;
    (void)user_data;

    payload = build_random_quote_response("dialogue");
    if (payload == NULL) {
        ulfius_set_string_body_response(response, 404, "No dialogue entries found");
        return U_CALLBACK_CONTINUE;
    }

    ulfius_set_json_body_response(response, 200, payload);
    json_decref(payload);
    return U_CALLBACK_CONTINUE;
}

static int callback_random_phrase(const struct _u_request *request,
                                  struct _u_response *response,
                                  void *user_data) {
    json_t *payload = NULL;

    (void)request;
    (void)user_data;

    payload = build_random_quote_response("phrase");
    if (payload == NULL) {
        ulfius_set_string_body_response(response, 404, "No phrase entries found");
        return U_CALLBACK_CONTINUE;
    }

    ulfius_set_json_body_response(response, 200, payload);
    json_decref(payload);
    return U_CALLBACK_CONTINUE;
}

void register_endpoints(struct _u_instance *instance) {
    srand((unsigned int)time(NULL));

    ulfius_add_endpoint_by_val(instance, "GET", "/bombapi", NULL, 0,
                               callback_helloworld, NULL);
    ulfius_add_endpoint_by_val(instance, "GET", "/bombapi/dialogue", NULL, 0,
                               callback_random_dialogue, NULL);
    ulfius_add_endpoint_by_val(instance, "GET", "/bombapi/phrase", NULL, 0,
                               callback_random_phrase, NULL);
}
