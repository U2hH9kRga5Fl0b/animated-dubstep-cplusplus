/*
 * insight_viewer.h
 *
 *  Created on: Nov 6, 2014
 *      Author: thallock
 */

#ifndef INSIGHT_VIEWER_H_
#define INSIGHT_VIEWER_H_

#include "insight/insight_state.h"

void show_insight(const std::string& name, const insight_state* state, void *video, const std::string& file="");

void *create_video();
void destroy_video(void *video);

#endif /* INSIGHT_VIEWER_H_ */
