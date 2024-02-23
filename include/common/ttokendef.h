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

#ifndef _TD_COMMON_TOKEN_H_
#define _TD_COMMON_TOKEN_H_

#define TK_OR                               1
#define TK_AND                              2
#define TK_UNION                            3
#define TK_ALL                              4
#define TK_MINUS                            5
#define TK_EXCEPT                           6
#define TK_INTERSECT                        7
#define TK_NK_BITAND                        8
#define TK_NK_BITOR                         9
#define TK_NK_LSHIFT                       10
#define TK_NK_RSHIFT                       11
#define TK_NK_PLUS                         12
#define TK_NK_MINUS                        13
#define TK_NK_STAR                         14
#define TK_NK_SLASH                        15
#define TK_NK_REM                          16
#define TK_NK_CONCAT                       17
#define TK_CREATE                          18
#define TK_ACCOUNT                         19
#define TK_NK_ID                           20
#define TK_PASS                            21
#define TK_NK_STRING                       22
#define TK_ALTER                           23
#define TK_PPS                             24
#define TK_TSERIES                         25
#define TK_STORAGE                         26
#define TK_STREAMS                         27
#define TK_QTIME                           28
#define TK_DBS                             29
#define TK_USERS                           30
#define TK_CONNS                           31
#define TK_STATE                           32
#define TK_NK_COMMA                        33
#define TK_HOST                            34
#define TK_USER                            35
#define TK_ENABLE                          36
#define TK_NK_INTEGER                      37
#define TK_SYSINFO                         38
#define TK_ADD                             39
#define TK_DROP                            40
#define TK_GRANT                           41
#define TK_ON                              42
#define TK_TO                              43
#define TK_REVOKE                          44
#define TK_FROM                            45
#define TK_SUBSCRIBE                       46
#define TK_READ                            47
#define TK_WRITE                           48
#define TK_NK_DOT                          49
#define TK_WITH                            50
#define TK_DNODE                           51
#define TK_PORT                            52
#define TK_DNODES                          53
#define TK_RESTORE                         54
#define TK_NK_IPTOKEN                      55
#define TK_FORCE                           56
#define TK_UNSAFE                          57
#define TK_CLUSTER                         58
#define TK_LOCAL                           59
#define TK_QNODE                           60
#define TK_BNODE                           61
#define TK_SNODE                           62
#define TK_MNODE                           63
#define TK_VNODE                           64
#define TK_DATABASE                        65
#define TK_USE                             66
#define TK_FLUSH                           67
#define TK_TRIM                            68
#define TK_COMPACT                         69
#define TK_IF                              70
#define TK_NOT                             71
#define TK_EXISTS                          72
#define TK_BUFFER                          73
#define TK_CACHEMODEL                      74
#define TK_CACHESIZE                       75
#define TK_COMP                            76
#define TK_DURATION                        77
#define TK_NK_VARIABLE                     78
#define TK_MAXROWS                         79
#define TK_MINROWS                         80
#define TK_KEEP                            81
#define TK_PAGES                           82
#define TK_PAGESIZE                        83
#define TK_TSDB_PAGESIZE                   84
#define TK_PRECISION                       85
#define TK_REPLICA                         86
#define TK_VGROUPS                         87
#define TK_SINGLE_STABLE                   88
#define TK_RETENTIONS                      89
#define TK_SCHEMALESS                      90
#define TK_WAL_LEVEL                       91
#define TK_WAL_FSYNC_PERIOD                92
#define TK_WAL_RETENTION_PERIOD            93
#define TK_WAL_RETENTION_SIZE              94
#define TK_WAL_ROLL_PERIOD                 95
#define TK_WAL_SEGMENT_SIZE                96
#define TK_STT_TRIGGER                     97
#define TK_TABLE_PREFIX                    98
#define TK_TABLE_SUFFIX                    99
#define TK_KEEP_TIME_OFFSET               100
#define TK_NK_COLON                       101
#define TK_BWLIMIT                        102
#define TK_START                          103
#define TK_TIMESTAMP                      104
#define TK_END                            105
#define TK_TABLE                          106
#define TK_NK_LP                          107
#define TK_NK_RP                          108
#define TK_STABLE                         109
#define TK_COLUMN                         110
#define TK_MODIFY                         111
#define TK_RENAME                         112
#define TK_TAG                            113
#define TK_SET                            114
#define TK_NK_EQ                          115
#define TK_USING                          116
#define TK_TAGS                           117
#define TK_BOOL                           118
#define TK_TINYINT                        119
#define TK_SMALLINT                       120
#define TK_INT                            121
#define TK_INTEGER                        122
#define TK_BIGINT                         123
#define TK_FLOAT                          124
#define TK_DOUBLE                         125
#define TK_BINARY                         126
#define TK_NCHAR                          127
#define TK_UNSIGNED                       128
#define TK_JSON                           129
#define TK_VARCHAR                        130
#define TK_MEDIUMBLOB                     131
#define TK_BLOB                           132
#define TK_VARBINARY                      133
#define TK_GEOMETRY                       134
#define TK_DECIMAL                        135
#define TK_COMMENT                        136
#define TK_MAX_DELAY                      137
#define TK_WATERMARK                      138
#define TK_ROLLUP                         139
#define TK_TTL                            140
#define TK_SMA                            141
#define TK_DELETE_MARK                    142
#define TK_FIRST                          143
#define TK_LAST                           144
#define TK_SHOW                           145
#define TK_PRIVILEGES                     146
#define TK_DATABASES                      147
#define TK_TABLES                         148
#define TK_STABLES                        149
#define TK_MNODES                         150
#define TK_QNODES                         151
#define TK_FUNCTIONS                      152
#define TK_INDEXES                        153
#define TK_ACCOUNTS                       154
#define TK_APPS                           155
#define TK_CONNECTIONS                    156
#define TK_LICENCES                       157
#define TK_GRANTS                         158
#define TK_FULL                           159
#define TK_LOGS                           160
#define TK_MACHINES                       161
#define TK_QUERIES                        162
#define TK_SCORES                         163
#define TK_TOPICS                         164
#define TK_VARIABLES                      165
#define TK_BNODES                         166
#define TK_SNODES                         167
#define TK_TRANSACTIONS                   168
#define TK_DISTRIBUTED                    169
#define TK_CONSUMERS                      170
#define TK_SUBSCRIPTIONS                  171
#define TK_VNODES                         172
#define TK_ALIVE                          173
#define TK_VIEWS                          174
#define TK_VIEW                           175
#define TK_COMPACTS                       176
#define TK_NORMAL                         177
#define TK_CHILD                          178
#define TK_LIKE                           179
#define TK_TBNAME                         180
#define TK_QTAGS                          181
#define TK_AS                             182
#define TK_SYSTEM                         183
#define TK_INDEX                          184
#define TK_FUNCTION                       185
#define TK_INTERVAL                       186
#define TK_COUNT                          187
#define TK_LAST_ROW                       188
#define TK_META                           189
#define TK_ONLY                           190
#define TK_TOPIC                          191
#define TK_CONSUMER                       192
#define TK_GROUP                          193
#define TK_DESC                           194
#define TK_DESCRIBE                       195
#define TK_RESET                          196
#define TK_QUERY                          197
#define TK_CACHE                          198
#define TK_EXPLAIN                        199
#define TK_ANALYZE                        200
#define TK_VERBOSE                        201
#define TK_NK_BOOL                        202
#define TK_RATIO                          203
#define TK_NK_FLOAT                       204
#define TK_OUTPUTTYPE                     205
#define TK_AGGREGATE                      206
#define TK_BUFSIZE                        207
#define TK_LANGUAGE                       208
#define TK_REPLACE                        209
#define TK_STREAM                         210
#define TK_INTO                           211
#define TK_PAUSE                          212
#define TK_RESUME                         213
#define TK_TRIGGER                        214
#define TK_AT_ONCE                        215
#define TK_WINDOW_CLOSE                   216
#define TK_IGNORE                         217
#define TK_EXPIRED                        218
#define TK_FILL_HISTORY                   219
#define TK_UPDATE                         220
#define TK_SUBTABLE                       221
#define TK_UNTREATED                      222
#define TK_KILL                           223
#define TK_CONNECTION                     224
#define TK_TRANSACTION                    225
#define TK_BALANCE                        226
#define TK_VGROUP                         227
#define TK_LEADER                         228
#define TK_MERGE                          229
#define TK_REDISTRIBUTE                   230
#define TK_SPLIT                          231
#define TK_DELETE                         232
#define TK_INSERT                         233
#define TK_NULL                           234
#define TK_NK_QUESTION                    235
#define TK_NK_ALIAS                       236
#define TK_NK_ARROW                       237
#define TK_ROWTS                          238
#define TK_QSTART                         239
#define TK_QEND                           240
#define TK_QDURATION                      241
#define TK_WSTART                         242
#define TK_WEND                           243
#define TK_WDURATION                      244
#define TK_IROWTS                         245
#define TK_ISFILLED                       246
#define TK_CAST                           247
#define TK_NOW                            248
#define TK_TODAY                          249
#define TK_TIMEZONE                       250
#define TK_CLIENT_VERSION                 251
#define TK_SERVER_VERSION                 252
#define TK_SERVER_STATUS                  253
#define TK_CURRENT_USER                   254
#define TK_CASE                           255
#define TK_WHEN                           256
#define TK_THEN                           257
#define TK_ELSE                           258
#define TK_BETWEEN                        259
#define TK_IS                             260
#define TK_NK_LT                          261
#define TK_NK_GT                          262
#define TK_NK_LE                          263
#define TK_NK_GE                          264
#define TK_NK_NE                          265
#define TK_MATCH                          266
#define TK_NMATCH                         267
#define TK_CONTAINS                       268
#define TK_IN                             269
#define TK_JOIN                           270
#define TK_INNER                          271
#define TK_LEFT                           272
#define TK_RIGHT                          273
#define TK_OUTER                          274
#define TK_SEMI                           275
#define TK_ANTI                           276
#define TK_ASOF                           277
#define TK_WINDOW                         278
#define TK_WINDOW_OFFSET                  279
#define TK_JLIMIT                         280
#define TK_SELECT                         281
#define TK_NK_HINT                        282
#define TK_DISTINCT                       283
#define TK_WHERE                          284
#define TK_PARTITION                      285
#define TK_BY                             286
#define TK_SESSION                        287
#define TK_STATE_WINDOW                   288
#define TK_EVENT_WINDOW                   289
#define TK_COUNT_WINDOW                   290
#define TK_SLIDING                        291
#define TK_FILL                           292
#define TK_VALUE                          293
#define TK_VALUE_F                        294
#define TK_NONE                           295
#define TK_PREV                           296
#define TK_NULL_F                         297
#define TK_LINEAR                         298
#define TK_NEXT                           299
#define TK_HAVING                         300
#define TK_RANGE                          301
#define TK_EVERY                          302
#define TK_ORDER                          303
#define TK_SLIMIT                         304
#define TK_SOFFSET                        305
#define TK_LIMIT                          306
#define TK_OFFSET                         307
#define TK_ASC                            308
#define TK_NULLS                          309
#define TK_ABORT                          310
#define TK_AFTER                          311
#define TK_ATTACH                         312
#define TK_BEFORE                         313
#define TK_BEGIN                          314
#define TK_BITAND                         315
#define TK_BITNOT                         316
#define TK_BITOR                          317
#define TK_BLOCKS                         318
#define TK_CHANGE                         319
#define TK_COMMA                          320
#define TK_CONCAT                         321
#define TK_CONFLICT                       322
#define TK_COPY                           323
#define TK_DEFERRED                       324
#define TK_DELIMITERS                     325
#define TK_DETACH                         326
#define TK_DIVIDE                         327
#define TK_DOT                            328
#define TK_EACH                           329
#define TK_FAIL                           330
#define TK_FILE                           331
#define TK_FOR                            332
#define TK_GLOB                           333
#define TK_ID                             334
#define TK_IMMEDIATE                      335
#define TK_IMPORT                         336
#define TK_INITIALLY                      337
#define TK_INSTEAD                        338
#define TK_ISNULL                         339
#define TK_KEY                            340
#define TK_MODULES                        341
#define TK_NK_BITNOT                      342
#define TK_NK_SEMI                        343
#define TK_NOTNULL                        344
#define TK_OF                             345
#define TK_PLUS                           346
#define TK_PRIVILEGE                      347
#define TK_RAISE                          348
#define TK_RESTRICT                       349
#define TK_ROW                            350
#define TK_STAR                           351
#define TK_STATEMENT                      352
#define TK_STRICT                         353
#define TK_STRING                         354
#define TK_TIMES                          355
#define TK_VALUES                         356
#define TK_VARIABLE                       357
#define TK_WAL                            358



#define TK_NK_SPACE         600
#define TK_NK_COMMENT       601
#define TK_NK_ILLEGAL       602
#define TK_NK_HEX           603  // hex number  0x123
#define TK_NK_OCT           604  // oct number
#define TK_NK_BIN           605  // bin format data 0b111
#define TK_BATCH_SCAN       606
#define TK_NO_BATCH_SCAN    607
#define TK_SORT_FOR_GROUP   608
#define TK_PARTITION_FIRST  609
#define TK_PARA_TABLES_SORT  610


#define TK_NK_NIL 65535

#endif /*_TD_COMMON_TOKEN_H_*/
