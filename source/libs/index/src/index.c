/*
 * Copyright (c) 2019 TAOS Data, Inc. <jhtao@taosdata.com>
 *
 * This program is free software: you can use, redistribute, and/or modify
 * it under the terms of the GNU Affero General Public License, version 3 * or later ("AGPL"), as published by the Free
 * Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "index.h"
#include "indexCache.h"
#include "indexComm.h"
#include "indexInt.h"
#include "indexTfile.h"
#include "indexUtil.h"
#include "tdef.h"
#include "tsched.h"

#ifdef USE_LUCENE
#include "lucene++/Lucene_c.h"
#endif

#define INDEX_NUM_OF_THREADS 4
#define INDEX_QUEUE_SIZE     200

void* indexQhandle = NULL;

void indexInit() {
  // refactor later
  indexQhandle = taosInitScheduler(INDEX_QUEUE_SIZE, INDEX_NUM_OF_THREADS, "index");
}
void indexCleanUp() {
  // refacto later
  taosCleanUpScheduler(indexQhandle);
}

typedef struct SIdxColInfo {
  int colId;  // generated by index internal
  int cVersion;
} SIdxColInfo;

static TdThreadOnce isInit = PTHREAD_ONCE_INIT;
// static void           indexInit();
static int indexTermSearch(SIndex* sIdx, SIndexTermQuery* term, SArray** result);

static void indexInterResultsDestroy(SArray* results);
static int  indexMergeFinalResults(SArray* interResults, EIndexOperatorType oType, SArray* finalResult);

static int indexGenTFile(SIndex* index, IndexCache* cache, SArray* batch);

// merge cache and tfile by opera type
static void indexMergeCacheAndTFile(SArray* result, IterateValue* icache, IterateValue* iTfv, SIdxTempResult* helper);

// static int32_t indexSerialTermKey(SIndexTerm* itm, char* buf);
// int32_t        indexSerialKey(ICacheKey* key, char* buf);

int indexOpen(SIndexOpts* opts, const char* path, SIndex** index) {
  taosThreadOnce(&isInit, indexInit);
  SIndex* sIdx = taosMemoryCalloc(1, sizeof(SIndex));
  if (sIdx == NULL) {
    return -1;
  }

#ifdef USE_LUCENE
  index_t* index = index_open(path);
  sIdx->index = index;
#endif

#ifdef USE_INVERTED_INDEX
  // sIdx->cache = (void*)indexCacheCreate(sIdx);
  sIdx->tindex = indexTFileCreate(path);
  if (sIdx->tindex == NULL) {
    goto END;
  }

  sIdx->colObj = taosHashInit(8, taosGetDefaultHashFunction(TSDB_DATA_TYPE_BINARY), true, HASH_ENTRY_LOCK);
  sIdx->cVersion = 1;
  sIdx->path = tstrdup(path);
  taosThreadMutexInit(&sIdx->mtx, NULL);
  *index = sIdx;
  return 0;
#endif

END:
  if (sIdx != NULL) {
    indexClose(sIdx);
  }

  *index = NULL;
  return -1;
}

void indexClose(SIndex* sIdx) {
#ifdef USE_LUCENE
  index_close(sIdex->index);
  sIdx->index = NULL;
#endif

#ifdef USE_INVERTED_INDEX
  void* iter = taosHashIterate(sIdx->colObj, NULL);
  while (iter) {
    IndexCache** pCache = iter;
    if (*pCache) {
      indexCacheUnRef(*pCache);
    }
    iter = taosHashIterate(sIdx->colObj, iter);
  }
  taosHashCleanup(sIdx->colObj);
  taosThreadMutexDestroy(&sIdx->mtx);
  indexTFileDestroy(sIdx->tindex);
#endif
  taosMemoryFree(sIdx->path);
  taosMemoryFree(sIdx);
  return;
}

int indexPut(SIndex* index, SIndexMultiTerm* fVals, uint64_t uid) {
#ifdef USE_LUCENE
  index_document_t* doc = index_document_create();

  char buf[16] = {0};
  sprintf(buf, "%d", uid);

  for (int i = 0; i < taosArrayGetSize(fVals); i++) {
    SIndexTerm* p = taosArrayGetP(fVals, i);
    index_document_add(doc, (const char*)(p->key), p->nKey, (const char*)(p->val), p->nVal, 1);
  }
  index_document_add(doc, NULL, 0, buf, strlen(buf), 0);

  index_put(index->index, doc);
  index_document_destroy(doc);
#endif

#ifdef USE_INVERTED_INDEX

  // TODO(yihao): reduce the lock range
  taosThreadMutexLock(&index->mtx);
  for (int i = 0; i < taosArrayGetSize(fVals); i++) {
    SIndexTerm* p = taosArrayGetP(fVals, i);

    char      buf[128] = {0};
    ICacheKey key = {.suid = p->suid, .colName = p->colName, .nColName = strlen(p->colName), .colType = p->colType};
    int32_t   sz = indexSerialCacheKey(&key, buf);

    IndexCache** cache = taosHashGet(index->colObj, buf, sz);
    if (cache == NULL) {
      IndexCache* pCache = indexCacheCreate(index, p->suid, p->colName, p->colType);
      taosHashPut(index->colObj, buf, sz, &pCache, sizeof(void*));
    }
  }
  taosThreadMutexUnlock(&index->mtx);

  for (int i = 0; i < taosArrayGetSize(fVals); i++) {
    SIndexTerm* p = taosArrayGetP(fVals, i);

    char      buf[128] = {0};
    ICacheKey key = {.suid = p->suid, .colName = p->colName, .nColName = strlen(p->colName), .colType = p->colType};
    int32_t   sz = indexSerialCacheKey(&key, buf);

    IndexCache** cache = taosHashGet(index->colObj, buf, sz);
    assert(*cache != NULL);
    int ret = indexCachePut(*cache, p, uid);
    if (ret != 0) {
      return ret;
    }
  }

#endif
  return 0;
}
int indexSearch(SIndex* index, SIndexMultiTermQuery* multiQuerys, SArray* result) {
#ifdef USE_INVERTED_INDEX
  EIndexOperatorType opera = multiQuerys->opera;  // relation of querys

  SArray* iRslts = taosArrayInit(4, POINTER_BYTES);
  int     nQuery = taosArrayGetSize(multiQuerys->query);
  for (size_t i = 0; i < nQuery; i++) {
    SIndexTermQuery* qterm = taosArrayGet(multiQuerys->query, i);
    SArray*          trslt = NULL;
    indexTermSearch(index, qterm, &trslt);
    taosArrayPush(iRslts, (void*)&trslt);
  }
  indexMergeFinalResults(iRslts, opera, result);
  indexInterResultsDestroy(iRslts);

#endif
  return 0;
}

int indexDelete(SIndex* index, SIndexMultiTermQuery* query) {
#ifdef USE_INVERTED_INDEX

#endif

  return 1;
}
int indexRebuild(SIndex* index, SIndexOpts* opts) {
#ifdef USE_INVERTED_INDEX
#endif

  return 0;
}

SIndexOpts* indexOptsCreate() {
#ifdef USE_LUCENE
#endif
  return NULL;
}
void indexOptsDestroy(SIndexOpts* opts) {
#ifdef USE_LUCENE
#endif
  return;
}
/*
 * @param: oper
 *
 */
SIndexMultiTermQuery* indexMultiTermQueryCreate(EIndexOperatorType opera) {
  SIndexMultiTermQuery* mtq = (SIndexMultiTermQuery*)taosMemoryMalloc(sizeof(SIndexMultiTermQuery));
  if (mtq == NULL) {
    return NULL;
  }
  mtq->opera = opera;
  mtq->query = taosArrayInit(4, sizeof(SIndexTermQuery));
  return mtq;
}
void indexMultiTermQueryDestroy(SIndexMultiTermQuery* pQuery) {
  for (int i = 0; i < taosArrayGetSize(pQuery->query); i++) {
    SIndexTermQuery* p = (SIndexTermQuery*)taosArrayGet(pQuery->query, i);
    indexTermDestroy(p->term);
  }
  taosArrayDestroy(pQuery->query);
  taosMemoryFree(pQuery);
};
int indexMultiTermQueryAdd(SIndexMultiTermQuery* pQuery, SIndexTerm* term, EIndexQueryType qType) {
  SIndexTermQuery q = {.qType = qType, .term = term};
  taosArrayPush(pQuery->query, &q);
  return 0;
}

SIndexTerm* indexTermCreate(int64_t suid, SIndexOperOnColumn oper, int8_t queryType, uint8_t colType,
                            const char* colName, int32_t nColName, const char* colVal, int32_t nColVal) {
  SIndexTerm* tm = (SIndexTerm*)taosMemoryCalloc(1, (sizeof(SIndexTerm)));
  if (tm == NULL) {
    return NULL;
  }

  tm->suid = suid;
  tm->operType = oper;
  tm->colType = colType;

  tm->colName = (char*)taosMemoryCalloc(1, nColName + 1);
  memcpy(tm->colName, colName, nColName);
  tm->nColName = nColName;

  tm->colVal = (char*)taosMemoryCalloc(1, nColVal + 1);
  memcpy(tm->colVal, colVal, nColVal);
  tm->nColVal = nColVal;
  tm->qType = queryType;

  return tm;
}
void indexTermDestroy(SIndexTerm* p) {
  taosMemoryFree(p->colName);
  taosMemoryFree(p->colVal);
  taosMemoryFree(p);
}

SIndexMultiTerm* indexMultiTermCreate() { return taosArrayInit(4, sizeof(SIndexTerm*)); }

int indexMultiTermAdd(SIndexMultiTerm* terms, SIndexTerm* term) {
  taosArrayPush(terms, &term);
  return 0;
}
void indexMultiTermDestroy(SIndexMultiTerm* terms) {
  for (int32_t i = 0; i < taosArrayGetSize(terms); i++) {
    SIndexTerm* p = taosArrayGetP(terms, i);
    indexTermDestroy(p);
  }
  taosArrayDestroy(terms);
}

static int indexTermSearch(SIndex* sIdx, SIndexTermQuery* query, SArray** result) {
  SIndexTerm* term = query->term;
  const char* colName = term->colName;
  int32_t     nColName = term->nColName;

  // Get col info
  IndexCache* cache = NULL;

  char      buf[128] = {0};
  ICacheKey key = {
      .suid = term->suid, .colName = term->colName, .nColName = strlen(term->colName), .colType = term->colType};
  int32_t sz = indexSerialCacheKey(&key, buf);

  taosThreadMutexLock(&sIdx->mtx);
  IndexCache** pCache = taosHashGet(sIdx->colObj, buf, sz);
  cache = (pCache == NULL) ? NULL : *pCache;
  taosThreadMutexUnlock(&sIdx->mtx);

  *result = taosArrayInit(4, sizeof(uint64_t));
  // TODO: iterator mem and tidex
  STermValueType s = kTypeValue;

  int64_t st = taosGetTimestampUs();

  SIdxTempResult* tr = sIdxTempResultCreate();
  if (0 == indexCacheSearch(cache, query, tr, &s)) {
    if (s == kTypeDeletion) {
      indexInfo("col: %s already drop by", term->colName);
      // coloum already drop by other oper, no need to query tindex
      return 0;
    } else {
      st = taosGetTimestampUs();
      if (0 != indexTFileSearch(sIdx->tindex, query, tr)) {
        indexError("corrupt at index(TFile) col:%s val: %s", term->colName, term->colVal);
        goto END;
      }
      int64_t tfCost = taosGetTimestampUs() - st;
      indexInfo("tfile search cost: %" PRIu64 "us", tfCost);
    }
  } else {
    indexError("corrupt at index(cache) col:%s val: %s", term->colName, term->colVal);
    goto END;
  }
  int64_t cost = taosGetTimestampUs() - st;
  indexInfo("search cost: %" PRIu64 "us", cost);

  sIdxTempResultMergeTo(*result, tr);

  sIdxTempResultDestroy(tr);
  return 0;
END:
  sIdxTempResultDestroy(tr);
  return -1;
}
static void indexInterResultsDestroy(SArray* results) {
  if (results == NULL) {
    return;
  }

  size_t sz = taosArrayGetSize(results);
  for (size_t i = 0; i < sz; i++) {
    SArray* p = taosArrayGetP(results, i);
    taosArrayDestroy(p);
  }
  taosArrayDestroy(results);
}

static int indexMergeFinalResults(SArray* interResults, EIndexOperatorType oType, SArray* fResults) {
  // refactor, merge interResults into fResults by oType
  for (int i = 0; i < taosArrayGetSize(interResults); i--) {
    SArray* t = taosArrayGetP(interResults, i);
    taosArraySort(t, uidCompare);
    taosArrayRemoveDuplicate(t, uidCompare, NULL);
  }

  if (oType == MUST) {
    iIntersection(interResults, fResults);
  } else if (oType == SHOULD) {
    iUnion(interResults, fResults);
  } else if (oType == NOT) {
    // just one column index, enhance later
    taosArrayAddAll(fResults, interResults);
    // not use currently
  }
  return 0;
}

static void indexMayMergeTempToFinalResult(SArray* result, TFileValue* tfv, SIdxTempResult* tr) {
  int32_t sz = taosArrayGetSize(result);
  if (sz > 0) {
    TFileValue* lv = taosArrayGetP(result, sz - 1);
    if (tfv != NULL && strcmp(lv->colVal, tfv->colVal) != 0) {
      sIdxTempResultMergeTo(lv->tableId, tr);
      sIdxTempResultClear(tr);

      taosArrayPush(result, &tfv);
    } else if (tfv == NULL) {
      // handle last iterator
      sIdxTempResultMergeTo(lv->tableId, tr);
    } else {
      // temp result saved in help
      tfileValueDestroy(tfv);
    }
  } else {
    taosArrayPush(result, &tfv);
  }
}
static void indexMergeCacheAndTFile(SArray* result, IterateValue* cv, IterateValue* tv, SIdxTempResult* tr) {
  char*       colVal = (cv != NULL) ? cv->colVal : tv->colVal;
  TFileValue* tfv = tfileValueCreate(colVal);

  indexMayMergeTempToFinalResult(result, tfv, tr);

  if (cv != NULL) {
    uint64_t id = *(uint64_t*)taosArrayGet(cv->val, 0);
    uint32_t ver = cv->ver;
    if (cv->type == ADD_VALUE) {
      INDEX_MERGE_ADD_DEL(tr->deled, tr->added, id)
    } else if (cv->type == DEL_VALUE) {
      INDEX_MERGE_ADD_DEL(tr->added, tr->deled, id)
    }
  }
  if (tv != NULL) {
    taosArrayAddAll(tr->total, tv->val);
  }
}
static void indexDestroyFinalResult(SArray* result) {
  int32_t sz = result ? taosArrayGetSize(result) : 0;
  for (size_t i = 0; i < sz; i++) {
    TFileValue* tv = taosArrayGetP(result, i);
    tfileValueDestroy(tv);
  }
  taosArrayDestroy(result);
}

int indexFlushCacheToTFile(SIndex* sIdx, void* cache) {
  if (sIdx == NULL) {
    return -1;
  }
  indexInfo("suid %" PRIu64 " merge cache into tindex", sIdx->suid);

  int64_t st = taosGetTimestampUs();

  IndexCache*  pCache = (IndexCache*)cache;
  TFileReader* pReader = tfileGetReaderByCol(sIdx->tindex, pCache->suid, pCache->colName);
  if (pReader == NULL) {
    indexWarn("empty tfile reader found");
  }
  // handle flush
  Iterate* cacheIter = indexCacheIteratorCreate(pCache);
  Iterate* tfileIter = tfileIteratorCreate(pReader);
  if (tfileIter == NULL) {
    indexWarn("empty tfile reader iterator");
  }

  SArray* result = taosArrayInit(1024, sizeof(void*));

  bool cn = cacheIter ? cacheIter->next(cacheIter) : false;
  bool tn = tfileIter ? tfileIter->next(tfileIter) : false;

  SIdxTempResult* tr = sIdxTempResultCreate();
  while (cn == true || tn == true) {
    IterateValue* cv = (cn == true) ? cacheIter->getValue(cacheIter) : NULL;
    IterateValue* tv = (tn == true) ? tfileIter->getValue(tfileIter) : NULL;

    int comp = 0;
    if (cn == true && tn == true) {
      comp = strcmp(cv->colVal, tv->colVal);
    } else if (cn == true) {
      comp = -1;
    } else {
      comp = 1;
    }
    if (comp == 0) {
      indexMergeCacheAndTFile(result, cv, tv, tr);
      cn = cacheIter->next(cacheIter);
      tn = tfileIter->next(tfileIter);
    } else if (comp < 0) {
      indexMergeCacheAndTFile(result, cv, NULL, tr);
      cn = cacheIter->next(cacheIter);
    } else {
      indexMergeCacheAndTFile(result, NULL, tv, tr);
      tn = tfileIter->next(tfileIter);
    }
  }
  indexMayMergeTempToFinalResult(result, NULL, tr);
  sIdxTempResultDestroy(tr);

  int ret = indexGenTFile(sIdx, pCache, result);
  indexDestroyFinalResult(result);

  indexCacheDestroyImm(pCache);

  indexCacheIteratorDestroy(cacheIter);
  tfileIteratorDestroy(tfileIter);

  tfileReaderUnRef(pReader);
  indexCacheUnRef(pCache);

  int64_t cost = taosGetTimestampUs() - st;
  if (ret != 0) {
    indexError("failed to merge, time cost: %" PRId64 "ms", cost / 1000);
  } else {
    indexInfo("success to merge , time cost: %" PRId64 "ms", cost / 1000);
  }
  return ret;
}
void iterateValueDestroy(IterateValue* value, bool destroy) {
  if (destroy) {
    taosArrayDestroy(value->val);
    value->val = NULL;
  } else {
    if (value->val != NULL) {
      taosArrayClear(value->val);
    }
  }
  taosMemoryFree(value->colVal);
  value->colVal = NULL;
}
static int indexGenTFile(SIndex* sIdx, IndexCache* cache, SArray* batch) {
  int32_t version = CACHE_VERSION(cache);
  uint8_t colType = cache->type;

  TFileWriter* tw = tfileWriterOpen(sIdx->path, cache->suid, version, cache->colName, colType);
  if (tw == NULL) {
    indexError("failed to open file to write");
    return -1;
  }

  int ret = tfileWriterPut(tw, batch, true);
  if (ret != 0) {
    indexError("failed to write into tindex ");
    goto END;
  }
  tfileWriterClose(tw);

  TFileReader* reader = tfileReaderOpen(sIdx->path, cache->suid, version, cache->colName);
  if (reader == NULL) {
    return -1;
  }

  TFileHeader* header = &reader->header;
  ICacheKey    key = {.suid = cache->suid, .colName = header->colName, .nColName = strlen(header->colName)};

  taosThreadMutexLock(&sIdx->mtx);
  IndexTFile* ifile = (IndexTFile*)sIdx->tindex;
  tfileCachePut(ifile->cache, &key, reader);
  taosThreadMutexUnlock(&sIdx->mtx);
  return ret;
END:
  if (tw != NULL) {
    writerCtxDestroy(tw->ctx, true);
    taosMemoryFree(tw);
  }
  return -1;
}

int32_t indexSerialCacheKey(ICacheKey* key, char* buf) {
  bool hasJson = INDEX_TYPE_CONTAIN_EXTERN_TYPE(key->colType, TSDB_DATA_TYPE_JSON);

  char* p = buf;
  SERIALIZE_MEM_TO_BUF(buf, key, suid);
  SERIALIZE_VAR_TO_BUF(buf, '_', char);
  // SERIALIZE_MEM_TO_BUF(buf, key, colType);
  // SERIALIZE_VAR_TO_BUF(buf, '_', char);
  if (hasJson) {
    SERIALIZE_STR_VAR_TO_BUF(buf, JSON_COLUMN, strlen(JSON_COLUMN));
  } else {
    SERIALIZE_STR_MEM_TO_BUF(buf, key, colName, key->nColName);
  }
  return buf - p;
}
