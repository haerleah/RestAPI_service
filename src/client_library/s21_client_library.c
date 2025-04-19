#include "s21_client_library.h"

static char* get_handling_url() {
  static char* handling_url = NULL;
  if (handling_url == NULL) {
    handling_url = calloc(100, sizeof(char));
  }
  return handling_url;
}

static ResponseStatus_t map_status(long response_status) {
  switch (response_status) {
    case 200:
      return STATUS_OK;
      break;
    case 400:
      return STATUS_BAD_REQUEST;
      break;
    case 404:
      return STATUS_NOT_FOUND;
      break;
    case 409:
      return STATUS_CONFLICT;
      break;
    case 500:
      return STATUS_INTERNAL_SERVER_ERROR;
      break;
    default:
      return STATUS_OTHER;
      break;
  }
}

static GameStatus_t map_game_status(const char* status_string) {
  if (strcmp(status_string, "Moving") == 0) return MOVING;
  if (strcmp(status_string, "Spawn") == 0) return SPAWN;
  if (strcmp(status_string, "Exit") == 0) return EXIT;
  if (strcmp(status_string, "Pause") == 0) return PAUSE;
  if (strcmp(status_string, "Start") == 0) return START;
  if (strcmp(status_string, "Gameover") == 0) return GAMEOVER;
  if (strcmp(status_string, "Shifting") == 0) return SHIFTING;
  if (strcmp(status_string, "Attaching") == 0) return ATTACHING;
  return -1;
}

static size_t write_callback(void* contents, size_t size, size_t nmemb,
                             void* userp) {
  size_t realsize = size * nmemb;
  MemoryStruct* mem = (MemoryStruct*)userp;

  char* ptr = realloc(mem->memory, mem->size + realsize + 1);  // +1 под '\0'
  if (!ptr) {
    fprintf(stderr, "Ошибка выделения памяти\n");
    return 0;
  }

  mem->memory = ptr;
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = '\0';

  return realsize;
}

void init_library(char* server_url) {
  char* handling_url = get_handling_url();
  curl_global_init(CURL_GLOBAL_DEFAULT);
  strcpy(handling_url, server_url);
}

void clear_library() {
  curl_global_cleanup();
  free(get_handling_url());
}

Response_t get_accesible_games(GameType_t** result) {
  CURL* curl_handle = curl_easy_init();
  Response_t response = {STATUS_OK, ""};
  char* status_api_url = calloc(100, sizeof(char));
  MemoryStruct chunk;
  chunk.memory = malloc(1);
  chunk.size = 0;

  strcat(status_api_url, get_handling_url());
  strcat(status_api_url, "/api/games");

  curl_easy_setopt(curl_handle, CURLOPT_URL, status_api_url);
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_callback);
  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void*)&chunk);
  curl_easy_perform(curl_handle);

  long response_code;
  curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &response_code);

  json_object* parsed_json = NULL;
  parsed_json = json_tokener_parse(chunk.memory);

  if (response_code != 200) {
    response.response_status = map_status(response_code);
    json_object* error_message_object;
    json_object_object_get_ex(parsed_json, "message", &error_message_object);
    if (error_message_object != NULL)
      strcpy(response.message, json_object_get_string(error_message_object));
    json_object_put(parsed_json);
    free(chunk.memory);
    free(status_api_url);
    return response;
  }

  if (parsed_json && json_object_get_type(parsed_json) == json_type_array) {
    size_t lenght = json_object_array_length(parsed_json);
    *result = calloc(lenght, sizeof(GameType_t));
    for (size_t i = 0; i < lenght; ++i) {
      json_object* item = json_object_array_get_idx(parsed_json, i);
      json_object* name;
      json_object* id;
      json_object_object_get_ex(item, "name", &name);
      json_object_object_get_ex(item, "id", &id);
      (*result)[i].identifier = json_object_get_int(id);
      const char* name_from_json = json_object_get_string(name);
      (*result)[i].name = calloc(strlen(name_from_json) + 1, sizeof(char));
      strcpy((*result)[i].name, name_from_json);
    }
  }
  json_object_put(parsed_json);
  free(chunk.memory);
  free(status_api_url);
  curl_easy_cleanup(curl_handle);
  return response;
}

Response_t select_game(GameType_t type) {
  CURL* curl_handle = curl_easy_init();
  Response_t response = {STATUS_OK, ""};
  char* select_api_url = calloc(100, sizeof(char));
  MemoryStruct chunk;
  chunk.memory = malloc(1);
  chunk.size = 0;

  strcat(select_api_url, get_handling_url());
  strcat(select_api_url, "/api/games/");
  char identified[3];
  snprintf(identified, sizeof(identified), "%d", type.identifier);
  strcat(select_api_url, identified);

  curl_easy_setopt(curl_handle, CURLOPT_URL, select_api_url);
  curl_easy_setopt(curl_handle, CURLOPT_POST, 1L);
  curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, "\n");
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_callback);
  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void*)&chunk);
  curl_easy_perform(curl_handle);

  long response_code;
  curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &response_code);

  if (response_code != 200) {
    json_object* parsed_json;
    parsed_json = json_tokener_parse(chunk.memory);
    response.response_status = map_status(response_code);
    json_object* error_message_object;
    json_object_object_get_ex(parsed_json, "message", &error_message_object);
    strcpy(response.message, json_object_get_string(error_message_object));
    json_object_put(parsed_json);
  }

  free(select_api_url);
  free(chunk.memory);
  curl_easy_cleanup(curl_handle);
  return response;
}

Response_t submit_action(UserAction_t action, bool hold) {
  CURL* curl_handle = curl_easy_init();
  Response_t response = {STATUS_OK, ""};
  char* actions_api_url = calloc(100, sizeof(char));
  MemoryStruct chunk;
  chunk.memory = malloc(1);
  chunk.size = 0;

  strcat(actions_api_url, get_handling_url());
  strcat(actions_api_url, "/api/actions");

  json_object* action_json = json_object_new_object();
  json_object_object_add(action_json, "id", json_object_new_int(action + 1));
  json_object_object_add(action_json, "hold", json_object_new_boolean(hold));

  const char* json_string = json_object_to_json_string(action_json);
  struct curl_slist* headers = NULL;
  headers = curl_slist_append(headers, "Content-Type: application/json");

  curl_easy_setopt(curl_handle, CURLOPT_URL, actions_api_url);
  curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(curl_handle, CURLOPT_POST, 1L);
  curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, json_string);
  curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDSIZE, strlen(json_string));
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_callback);
  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void*)&chunk);
  // curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, 1L);
  curl_easy_perform(curl_handle);

  long response_code;
  curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &response_code);

  if (response_code != 200) {
    json_object* parsed_json;
    parsed_json = json_tokener_parse(chunk.memory);
    response.response_status = map_status(response_code);
    json_object* error_message_object;
    json_object_object_get_ex(parsed_json, "message", &error_message_object);
    if (error_message_object != NULL)
      strcpy(response.message, json_object_get_string(error_message_object));
    json_object_put(parsed_json);
  }

  json_object_put(action_json);
  free(actions_api_url);
  free(chunk.memory);
  curl_slist_free_all(headers);
  curl_easy_cleanup(curl_handle);
  return response;
}

Response_t get_current_state(GameInfo_t* result) {
  CURL* curl_handle = curl_easy_init();
  Response_t response = {STATUS_OK, ""};
  char* state_api_url = calloc(100, sizeof(char));
  MemoryStruct chunk;
  chunk.memory = malloc(1);
  chunk.size = 0;

  strcat(state_api_url, get_handling_url());
  strcat(state_api_url, "/api/state");

  curl_easy_setopt(curl_handle, CURLOPT_URL, state_api_url);
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_callback);
  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void*)&chunk);
  curl_easy_perform(curl_handle);

  long response_code;
  curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &response_code);

  json_object* parsed_json = NULL;
  parsed_json = json_tokener_parse(chunk.memory);

  if (response_code != 200) {
    result->field = NULL;
    result->next = NULL;
    response.response_status = map_status(response_code);
    json_object* error_message_object;
    json_object_object_get_ex(parsed_json, "message", &error_message_object);
    strcpy(response.message, json_object_get_string(error_message_object));
    json_object_put(parsed_json);
    free(chunk.memory);
    free(state_api_url);
    return response;
  }

  if (parsed_json) {
    /* Парсинг основного поля */
    json_object* field_2d_array = NULL;
    json_object_object_get_ex(parsed_json, "field", &field_2d_array);

    if (field_2d_array &&
        json_object_get_type(field_2d_array) == json_type_array) {
      size_t field_rows_size = json_object_array_length(field_2d_array);
      result->field = calloc(field_rows_size, sizeof(int*));
      for (size_t i = 0; i < field_rows_size; ++i) {
        json_object* field_row = json_object_array_get_idx(field_2d_array, i);
        if (json_object_get_type(field_row) == json_type_array) {
          size_t field_col_size = json_object_array_length(field_row);
          result->field[i] = calloc(field_col_size, sizeof(int));
          for (size_t j = 0; j < field_col_size; ++j) {
            json_object* field_item = json_object_array_get_idx(field_row, j);
            result->field[i][j] = json_object_get_int64(field_item);
          }
        }
      }
    } else {
      result->field = NULL;
    }

    /* Парсинг следующей фигуры */
    json_object* next_2d_array = NULL;
    json_object_object_get_ex(parsed_json, "next", &next_2d_array);

    if (next_2d_array &&
        json_object_get_type(next_2d_array) == json_type_array) {
      size_t field_rows_size = json_object_array_length(next_2d_array);
      result->next = calloc(field_rows_size, sizeof(int*));
      for (size_t i = 0; i < field_rows_size; ++i) {
        json_object* field_row = json_object_array_get_idx(next_2d_array, i);
        if (json_object_get_type(field_row) == json_type_array) {
          size_t field_col_size = json_object_array_length(field_row);
          result->next[i] = calloc(field_col_size, sizeof(int));
          for (size_t j = 0; j < field_col_size; ++j) {
            json_object* field_item = json_object_array_get_idx(field_row, j);
            result->next[i][j] = json_object_get_int64(field_item);
          }
        }
      }
    } else {
      result->next = NULL;
    }

    /* Парсинг счета */
    json_object* score_object = NULL;
    json_object_object_get_ex(parsed_json, "score", &score_object);
    result->score = json_object_get_int(score_object);

    /* Парсинг максимального счета */
    json_object* highest_score_object = NULL;
    json_object_object_get_ex(parsed_json, "highScore", &highest_score_object);
    result->high_score = json_object_get_int(highest_score_object);

    /* Парсинг уровня */
    json_object* level_object = NULL;
    json_object_object_get_ex(parsed_json, "level", &level_object);
    result->level = json_object_get_int(level_object);

    /* Парсинг скорости */
    json_object* speed_object = NULL;
    json_object_object_get_ex(parsed_json, "speed", &speed_object);
    result->speed = json_object_get_int(speed_object);

    /* Парсинг флага паузы */
    json_object* pause_object = NULL;
    json_object_object_get_ex(parsed_json, "pause", &pause_object);
    const char* pause_string = json_object_get_string(pause_object);
    if (strcmp(pause_string, "true") == 0)
      result->pause = 1;
    else if (strcmp(pause_string, "false") == 0)
      result->pause = 0;
  }

  json_object_put(parsed_json);
  free(chunk.memory);
  free(state_api_url);
  curl_easy_cleanup(curl_handle);
  return response;
}

Response_t get_current_game_status(GameStatus_t* result) {
  CURL* curl_handle = curl_easy_init();
  Response_t response = {STATUS_OK, ""};
  char* status_api_url = calloc(100, sizeof(char));
  MemoryStruct chunk;
  chunk.memory = malloc(1);
  chunk.size = 0;

  strcat(status_api_url, get_handling_url());
  strcat(status_api_url, "/api/status");

  curl_easy_setopt(curl_handle, CURLOPT_URL, status_api_url);
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_callback);
  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void*)&chunk);
  curl_easy_perform(curl_handle);

  long response_code;
  curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &response_code);

  json_object* parsed_json = NULL;
  parsed_json = json_tokener_parse(chunk.memory);

  if (response_code != 200) {
    response.response_status = map_status(response_code);
    json_object* error_message_object;
    json_object_object_get_ex(parsed_json, "message", &error_message_object);
    strcpy(response.message, json_object_get_string(error_message_object));
    json_object_put(parsed_json);
    free(chunk.memory);
    free(status_api_url);
    return response;
  }

  json_object* status_object = NULL;
  json_object_object_get_ex(parsed_json, "status", &status_object);
  const char* status_string = json_object_get_string(status_object);
  *result = map_game_status(status_string);

  json_object_put(parsed_json);
  free(chunk.memory);
  free(status_api_url);
  curl_easy_cleanup(curl_handle);
  return response;
}
