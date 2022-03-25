/*
 * Copyright (c) 2019 TAOS Data, Inc. <jhtao@taosdata.com>
 *
 * This program is free software: you can use, redistribute, and/or modify
 * it under the terms of the GNU Affero General Public License, version 3
 * or later ("AGPL"), as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "tdatablock.h"
#include "tmsg.h"
#include "tmsgcb.h"
#include "trpc.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _TSTREAM_H_
#define _TSTREAM_H_

int32_t streamExecTask(SStreamTask* pTask, SMsgCb* pMsgCb, const void* input, int32_t inputType, int32_t workId);

#ifdef __cplusplus
}
#endif

#endif /* ifndef _TSTREAM_H_ */
