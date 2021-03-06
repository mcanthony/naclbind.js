/* Copyright 2014 Ben Smith. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef RUN_H_
#define RUN_H_

#include <ppapi/c/pp_var.h>

#ifndef NB_ONE_FILE
#include "bool.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct NB_Queue;
struct NB_Response;

void nb_run_message_loop(struct NB_Queue* queue);
struct NB_Response* nb_run_message_loop_for_response(struct NB_Queue* queue,
                                                     int id,
                                                     int cb_id);
NB_Bool nb_request_run(struct NB_Queue* queue,
                       struct PP_Var request_var,
                       struct PP_Var* response_var);

#ifdef __cplusplus
}
#endif

#endif /* RUN_H_ */
