#ifndef DB_CONFIG_H
#define DB_CONFIG_H

// db_config.h
// 9/13/2011

// - SQLite helpers -

// See: http://www.sqlite.org/lang_datefunc.html
#define DB_UNIX_TIMESTAMP(_t)   "strftime('%s'," _t ")"
#define DB_FROM_UNIXTIME(_ts)   "datetime(" _ts ",'unixepoch')"

// - Creation -

#define DB_CREATE_TABLE_USER \
  "CREATE TABLE user(" \
    "user_id "          "BIGINT NOT NULL PRIMARY KEY," /* AUTO_INCREMENT */ \
    "group_id "         "BIGINT NOT NULL DEFAULT 0," \
    "user_name "        "VARCHAR(64) NOT NULL UNIQUE," /* 64 is the maximum allowed for email username */ \
    "user_password "    "CHAR(40),"     /* SHA1 */ \
    "user_nickname "    "VARCHAR(255)," \
    "user_status "      "INT NOT NULL DEFAULT 0," \
    "user_flags "       "BIGINT UNSIGNED NOT NULL DEFAULT 0," \
    "user_language "    "INT NOT NULL DEFAULT 0," \
    "user_create_time " "DATETIME NOT NULL," \
    "user_login_time " "DATETIME NOT NULL," \
    "user_blessed_count " "INT UNSIGNED NOT NULL DEFAULT 0," \
    "user_cursed_count "  "INT UNSIGNED NOT NULL DEFAULT 0," \
    "user_blocked_count " "INT UNSIGNED NOT NULL DEFAULT 0," \
    "user_annot_count "   "INT UNSIGNED NOT NULL DEFAULT 0" \
    /*"INDEX(user_name)"*/ \
  ")"
  //"user_email "       "VARCHAR(160) NOT NULL UNIQUE," /* 320 = 64 + 1 + 255 is the maximum, but not allowd to be unique by mysql */

#define DB_CREATE_TABLE_TOKEN \
  "CREATE TABLE token(" \
    "token_id "         "BIGINT NOT NULL," /* PRIMARY KEY AUTO_INCREMENT */ \
    "user_id "          "BIGINT NOT NULL DEFAULT 0," \
    "token_type "       "INT NOT NULL DEFAULT 0," \
    "token_digest "     "CHAR(32) NOT NULL," /* MD5, since unhex not supported in SQLite */ \
    "token_digest_type ""INT NOT NULL DEFAULT 0," \
    "token_status "     "INT NOT NULL DEFAULT 0," \
    "token_flags "      "BIGINT UNSIGNED NOT NULL DEFAULT 0," \
    "token_create_time " "DATETIME NOT NULL," \
    "token_blessed_count " "INT UNSIGNED NOT NULL DEFAULT 0," \
    "token_cursed_count "  "INT UNSIGNED NOT NULL DEFAULT 0," \
    "token_blocked_count " "INT UNSIGNED NOT NULL DEFAULT 0," \
    "token_visited_count " "INT UNSIGNED NOT NULL DEFAULT 0," \
    "token_annot_count "   "INT UNSIGNED NOT NULL DEFAULT 0" \
    /*"INDEX(token_digest)"*/ \
    /*"FOREIGN KEY(user_id) REFERENCES user(user_id) "*/ \
    /*  "ON DELETE CASCADE ON UPDATE CASCADE"*/ \
  ")"

#define DB_CREATE_TABLE_ALIAS \
  "CREATE TABLE alias(" \
    "alias_id "         "BIGINT NOT NULL," /* PRIMARY KEY AUTO_INCREMENT */ \
    "token_id "         "BIGINT NOT NULL DEFAULT 0," /* AUTO_INCREMENT */ \
    "token_digest "     "CHAR(32)," /* MD5, since unhex not supported in SQLite */ \
    "token_digest_type ""INT NOT NULL DEFAULT 0," \
    "user_id "          "BIGINT NOT NULL DEFAULT 0," \
    "alias_type "       "INT NOT NULL DEFAULT 0," \
    "alias_text "       "VARCHAR(255)," \
    "alias_flags "      "BIGINT UNSIGNED NOT NULL DEFAULT 0," \
    "alias_status "     "INT NOT NULL DEFAULT 0," \
    "alias_language "   "INT NOT NULL DEFAULT 0," \
    "alias_update_time " "DATETIME NOT NULL," \
    "alias_blessed_count " "INT UNSIGNED NOT NULL DEFAULT 0," \
    "alias_cursed_count "  "INT UNSIGNED NOT NULL DEFAULT 0," \
    "alias_blocked_count " "INT UNSIGNED NOT NULL DEFAULT 0" \
    /*"INDEX(token_digest)"*/ \
    /*"FOREIGN KEY(user_id) REFERENCES token(token_id) "*/ \
    /*  "ON DELETE CASCADE ON UPDATE CASCADE"*/ \
    /*"FOREIGN KEY(user_id) REFERENCES user(user_id) "*/ \
    /*  "ON DELETE CASCADE ON UPDATE CASCADE"*/ \
  ")"


#define DB_CREATE_TABLE_ANNOT \
  "CREATE TABLE annot(" \
    "annot_id "         "BIGINT NOT NULL DEFAULT 0," /* PRIMARY KEY AUTO_INCREMENT */ \
    "token_id "         "BIGINT NOT NULL DEFAULT 0," \
    "token_digest "     "CHAR(32)," /* MD5, since unhex not supported in SQLite */ \
    "token_digest_type ""INT NOT NULL DEFAULT 0," \
    "user_id "          "BIGINT NOT NULL DEFAULT 0," \
    "user_alias "       "VARCHAR(64)," /* the same length as user_name */ \
    "annot_status "     "INT NOT NULL DEFAULT 0," \
    "annot_flags "      "BIGINT UNSIGNED NOT NULL DEFAULT 0," \
    "annot_language "   "INT NOT NULL DEFAULT 0," \
    "annot_create_time " "DATETIME NOT NULL," \
    "annot_update_time " "DATETIME," \
    "annot_pos "        "BIGINT NOT NULL DEFAULT 0," \
    "annot_pos_type "   "INT NOT NULL DEFAULT 0," \
    "annot_time "       "INT NOT NULL DEFAULT 0," \
    "annot_text "       "VARCHAR(255)," \
    "annot_blessed_count " "INT UNSIGNED NOT NULL DEFAULT 0," \
    "annot_cursed_count "  "INT UNSIGNED NOT NULL DEFAULT 0," \
    "annot_blocked_count " "INT UNSIGNED NOT NULL DEFAULT 0" \
    /*"INDEX(token_id),"*/ \
    /*"FOREIGN KEY(token_id) REFERENCES token(token_id) "*/ \
    /*  "ON DELETE CASCADE ON UPDATE CASCADE" */ \
    /*"FOREIGN KEY(user_id) REFERENCES user(user_id) "*/ \
    /*  "ON DELETE CASCADE ON UPDATE CASCADE" */ \
  ")"

// - Insertions -

#define DB_INSERT_USER(_user, _query) \
{ \
  _query.prepare( \
    "INSERT INTO user(" \
      "user_status,"            /* 0 */ \
      "user_flags,"             /* 1 */ \
      "user_id,"                /* 2 */ \
      "group_id,"               /* 3 */ \
      "user_name,"              /* 4 */ \
      "user_password,"          /* 5 */ \
      "user_nickname,"          /* 6 */ \
      "user_language,"          /* 7 */ \
      "user_create_time,"       /* 8 */ \
      "user_login_time,"        /* 9 */ \
      "user_blessed_count,"     /* 10 */ \
      "user_cursed_count,"      /* 11 */ \
      "user_blocked_count,"     /* 12 */ \
      "user_annot_count"        /* 13 */ \
    ") VALUES (" \
      "?,"      /* 0: user_status */ \
      "?,"      /* 1: user_flags */ \
      "?,"      /* 2: user_id */ \
      "?,"      /* 3: group_id */ \
      "?,"      /* 4: user_name */ \
      "?,"      /* 5: user_password */ \
      "?,"      /* 6: user_nickname */ \
      "?,"      /* 7: user_language */ \
      DB_FROM_UNIXTIME("?") "," /* 8: user_create_time */ \
      DB_FROM_UNIXTIME("?") "," /* 9: user_login_time */ \
      "?,"      /* 10: user_blessed_count */ \
      "?,"      /* 11: user_cursed_count */ \
      "?,"      /* 12: user_blocked_count */ \
      "?"       /* 13: user_annot_count */ \
    ")" \
  ); \
  (_query).addBindValue((_user).status());       /* 0 */ \
  (_query).addBindValue((_user).flags());        /* 1 */ \
  (_query).addBindValue((_user).id());           /* 2 */ \
  (_query).addBindValue((_user).groupId());      /* 3 */ \
  (_query).addBindValue((_user).name());         /* 4 */ \
  (_query).addBindValue((_user).password());     /* 5 */ \
  (_query).addBindValue((_user).nickname());     /* 6 */ \
  (_query).addBindValue((_user).language());     /* 7 */ \
  (_query).addBindValue((_user).createTime());   /* 8 */ \
  (_query).addBindValue((_user).loginTime());    /* 9 */ \
  (_query).addBindValue((_user).blessedCount()); /* 10 */ \
  (_query).addBindValue((_user).cursedCount());  /* 11 */ \
  (_query).addBindValue((_user).blockedCount()); /* 12 */ \
  (_query).addBindValue((_user).annotCount());   /* 13 */ \
}

#define DB_INSERT_TOKEN(_token, _query) \
{ \
  _query.prepare( \
    "INSERT INTO token(" \
      "token_status,"           /* 0 */ \
      "token_flags,"            /* 1 */ \
      "token_id,"               /* 2 */ \
      "token_type,"             /* 3 */ \
      "user_id,"                /* 4 */ \
      "token_digest,"           /* 5 */ \
      "token_digest_type,"      /* 6 */ \
      "token_create_time,"      /* 7 */ \
      "token_blessed_count,"    /* 8 */ \
      "token_cursed_count,"     /* 9 */ \
      "token_blocked_count,"    /* 10 */ \
      "token_visited_count,"    /* 11 */ \
      "token_annot_count"       /* 12 */ \
    ") VALUES (" \
      "?,"      /* 0: token_status */ \
      "?,"      /* 1: token_flags */ \
      "?,"      /* 2: token_id */ \
      "?,"      /* 3: token_type */ \
      "?,"      /* 4: user_id */ \
      "?,"      /* 5: token_digest */ \
      "?,"      /* 6: token_digest_type */ \
      DB_FROM_UNIXTIME("?") "," /* 7: token_create_time */ \
      "?,"      /* 8: token_blessed_count */ \
      "?,"      /* 9: token_cursed_count */ \
      "?,"      /* 10: token_blocked_count */ \
      "?,"      /* 11: token_visited_count */ \
      "?"       /* 12: token_annot_count */ \
    ")" \
  ); \
  (_query).addBindValue((_token).status());       /* 0 */ \
  (_query).addBindValue((_token).flags());        /* 1 */ \
  (_query).addBindValue((_token).id());           /* 2 */ \
  (_query).addBindValue((_token).type());         /* 3 */ \
  (_query).addBindValue((_token).userId());       /* 4 */ \
  (_query).addBindValue((_token).digest());       /* 5 */ \
  (_query).addBindValue((_token).digestType());   /* 6 */ \
  (_query).addBindValue((_token).createTime());   /* 7 */ \
  (_query).addBindValue((_token).blessedCount()); /* 8 */ \
  (_query).addBindValue((_token).cursedCount());  /* 9 */ \
  (_query).addBindValue((_token).blockedCount()); /* 10 */ \
  (_query).addBindValue((_token).visitedCount()); /* 11 */ \
  (_query).addBindValue((_token).annotCount());   /* 12 */ \
}

#define DB_INSERT_ALIAS(_alias, _query) \
{ \
  _query.prepare( \
    "INSERT INTO alias(" \
      "alias_status,"           /* 0 */ \
      "alias_flags,"            /* 1 */ \
      "alias_id,"               /* 2 */ \
      "token_id,"               /* 3 */ \
      "token_digest,"           /* 4 */ \
      "token_digest_type,"      /* 5 */ \
      "user_id,"                /* 6 */ \
      "alias_type,"             /* 7 */ \
      "alias_text,"             /* 8 */ \
      "alias_language,"         /* 9 */ \
      "alias_update_time,"      /* 10 */ \
      "alias_blessed_count,"    /* 11 */ \
      "alias_cursed_count,"     /* 12 */ \
      "alias_blocked_count,"    /* 13 */ \
    ") VALUES (" \
      "?,"      /* 0: alias_status */ \
      "?,"      /* 1: alias_flags */ \
      "?,"      /* 2: alias_id */ \
      "?,"      /* 3: token_id */ \
      "?,"      /* 4: token_digest */ \
      "?,"      /* 5: token_digest_type */ \
      "?,"      /* 6: user_id */ \
      "?,"      /* 7: alias_type */ \
      "?,"      /* 8: alias_text */ \
      "?,"      /* 9: alias_language */ \
      DB_FROM_UNIXTIME("?") "," /* 10: alias_update_time */ \
      "?,"      /* 11: alias_blessed_count */ \
      "?,"      /* 12: alias_cursed_count */ \
      "?,"      /* 13: alias_blocked_count */ \
    ")" \
  ); \
  (_query).addBindValue((_alias).status());       /* 0 */ \
  (_query).addBindValue((_alias).flags());        /* 1 */ \
  (_query).addBindValue((_alias).id());           /* 2 */ \
  (_query).addBindValue((_alias).tokenId());      /* 3 */ \
  (_query).addBindValue((_alias).tokenDigest());  /* 4 */ \
  (_query).addBindValue((_alias).tokenDigestType()); /* 5 */ \
  (_query).addBindValue((_alias).userId());       /* 6 */ \
  (_query).addBindValue((_alias).type());         /* 7 */ \
  (_query).addBindValue((_alias).text());         /* 8 */ \
  (_query).addBindValue((_alias).language());     /* 9 */ \
  (_query).addBindValue((_alias).updateTime());   /* 10 */ \
  (_query).addBindValue((_alias).blessedCount()); /* 11 */ \
  (_query).addBindValue((_alias).cursedCount());  /* 12 */ \
  (_query).addBindValue((_alias).blockedCount()); /* 13 */ \
}

#define DB_INSERT_ANNOT(_annot, _query) \
{ \
  _query.prepare( \
    "INSERT INTO annot(" \
      "annot_status,"   /* 0 */ \
      "annot_flags,"    /* 1 */ \
      "annot_id,"       /* 2 */ \
      "token_id,"       /* 3 */ \
      "token_digest,"   /* 4 */ \
      "token_digest_type," /* 5 */ \
      "user_id,"        /* 6 */ \
      "user_alias,"     /* 7 */ \
      "annot_pos,"      /* 8 */ \
      "annot_pos_type," /* 9 */ \
      "annot_time,"     /* 10 */ \
      "annot_text,"     /* 11 */ \
      "annot_language," /* l2 */ \
      "annot_create_time,"      /* 13 */ \
      "annot_update_time,"      /* 14 */ \
      "annot_blessed_count,"    /* 15 */ \
      "annot_cursed_count,"     /* 16 */ \
      "annot_blocked_count "    /* 17 */ \
    ") VALUES (" \
      "?,"      /* 0: annot_status */ \
      "?,"      /* 1: annot_flags */ \
      "?,"      /* 2: annot_id */ \
      "?,"      /* 3: token_id */ \
      "?,"      /* 4: token_digest */ \
      "?,"      /* 5: token_digest_type */ \
      "?,"      /* 6: user_id */ \
      "?,"      /* 7: user_alias */ \
      "?,"      /* 8: annot_pos */ \
      "?,"      /* 9: annot_pos_type */ \
      "?,"      /* 10: annot_time */ \
      "?,"      /* 11: annot_text */ \
      "?,"      /* 12: annot_language */ \
      DB_FROM_UNIXTIME("?") "," /* 13: annot_create_time */ \
      DB_FROM_UNIXTIME("?") "," /* 14: annot_update_time */ \
      "?,"      /* 15: annot_blessed_count */ \
      "?,"      /* 16: annot_cursed_count */ \
      "?"       /* 17: annot_blocked_count */ \
    ")" \
  ); \
  (_query).addBindValue((_annot).status());       /* 0 */ \
  (_query).addBindValue((_annot).flags());        /* 1 */ \
  (_query).addBindValue((_annot).id());           /* 2 */ \
  (_query).addBindValue((_annot).tokenId());      /* 3 */ \
  (_query).addBindValue((_annot).tokenDigest());  /* 4 */ \
  (_query).addBindValue((_annot).tokenDigestType()); /* 5 */ \
  (_query).addBindValue((_annot).userId());       /* 6 */ \
  (_query).addBindValue((_annot).userAlias());    /* 7 */ \
  (_query).addBindValue((_annot).pos());          /* 8 */ \
  (_query).addBindValue((_annot).posType());      /* 9 */ \
  (_query).addBindValue((_annot).time());         /* 10 */ \
  (_query).addBindValue((_annot).text());         /* 11 */ \
  (_query).addBindValue((_annot).language());     /* 12 */ \
  (_query).addBindValue((_annot).createTime());   /* 13 */ \
  (_query).addBindValue((_annot).updateTime());   /* 14 */ \
  (_query).addBindValue((_annot).blessedCount()); /* 15 */ \
  (_query).addBindValue((_annot).cursedCount());  /* 16 */ \
  (_query).addBindValue((_annot).blockedCount()); /* 17 */ \
}

// - Queries -

#define DB_SELECT_USER \
  "SELECT " \
    "user_status,"    /* 0 */ \
    "user_flags,"     /* 1 */ \
    "user_id,"        /* 2 */ \
    "group_id,"       /* 3 */ \
    "user_name,"      /* 4 */ \
    "user_password,"  /* 5 */ \
    "user_nickname,"  /* 6 */ \
    "user_language,"  /* 7 */ \
    DB_UNIX_TIMESTAMP("user_create_time") "," /* 8 */ \
    DB_UNIX_TIMESTAMP("user_login_time") ","  /* 9 */ \
    "user_blessed_count," /* 10 */ \
    "user_cursed_count,"  /* 11 */ \
    "user_blocked_count," /* 12 */ \
    "user_annot_count "   /* 13 */ \
  "FROM user "

#define DB_SET_USER(_user, _query) \
{ \
  bool ok; \
  (_user).setStatus((_query).value(0).toInt(&ok)); \
  Q_ASSERT(ok); \
  (_user).setFlags((_query).value(1).toULongLong(&ok)); \
  Q_ASSERT(ok); \
  (_user).setId((_query).value(2).toLongLong(&ok)); \
  Q_ASSERT(ok); \
  (_user).setGroupId((_query).value(3).toLongLong(&ok)); \
  Q_ASSERT(ok); \
  (_user).setName((_query).value(4).toString()); \
  (_user).setPassword((_query).value(5).toString()); \
  (_user).setNickname((_query).value(6).toString()); \
  (_user).setLanguage((_query).value(7).toInt(&ok)); \
  Q_ASSERT(ok); \
  (_user).setCreateTime((_query).value(8).toULongLong(&ok)); \
  Q_ASSERT(ok); \
  (_user).setLoginTime((_query).value(9).toULongLong(&ok)); \
  Q_ASSERT(ok); \
  (_user).setBlessedCount((_query).value(10).toUInt(&ok)); \
  Q_ASSERT(ok); \
  (_user).setCursedCount((_query).value(11).toUInt(&ok)); \
  Q_ASSERT(ok); \
  (_user).setBlockedCount((_query).value(12).toUInt(&ok)); \
  Q_ASSERT(ok); \
  (_user).setAnnotCount((_query).value(13).toUInt(&ok)); \
  Q_ASSERT(ok); \
}

#define DB_SELECT_TOKEN \
  "SELECT " \
    "token_status,"        /* 0 */ \
    "token_flags,"         /* 1 */ \
    "token_id,"            /* 2 */ \
    "token_type,"          /* 3 */ \
    "user_id,"             /* 4 */ \
    "token_digest,"        /* 5 */ \
    "token_digest_type,"   /* 6 */ \
    "strftime('%s',token_create_time)," /* 7 */ \
    "token_blessed_count," /* 8 */ \
    "token_cursed_count,"  /* 9 */ \
    "token_blocked_count," /* 10 */ \
    "token_visited_count," /* 11 */ \
    "token_annot_count "   /* 12 */ \
  "FROM token "

#define DB_SET_TOKEN(_token, _query) \
{ \
  bool ok; \
  (_token).setStatus((_query).value(0).toInt(&ok)); \
  Q_ASSERT(ok); \
  (_token).setFlags((_query).value(1).toULongLong(&ok)); \
  Q_ASSERT(ok); \
  (_token).setId((_query).value(2).toLongLong()); \
  Q_ASSERT(ok); \
  (_token).setType((_query).value(3).toInt(&ok)); \
  Q_ASSERT(ok); \
  (_token).setUserId((_query).value(4).toLongLong()); \
  Q_ASSERT(ok); \
  (_token).setDigest((_query).value(5).toString()); \
  (_token).setDigestType((_query).value(6).toInt(&ok)); \
  Q_ASSERT(ok); \
  (_token).setCreateTime((_query).value(7).toLongLong(&ok)); \
  Q_ASSERT(ok); \
  (_token).setBlessedCount((_query).value(8).toUInt(&ok)); \
  Q_ASSERT(ok); \
  (_token).setCursedCount((_query).value(9).toUInt(&ok)); \
  Q_ASSERT(ok); \
  (_token).setBlockedCount((_query).value(10).toUInt(&ok)); \
  Q_ASSERT(ok); \
  (_token).setVisitedCount((_query).value(11).toUInt(&ok)); \
  Q_ASSERT(ok); \
  (_token).setAnnotCount((_query).value(12).toUInt(&ok)); \
  Q_ASSERT(ok); \
}

#define DB_SELECT_ALIAS \
  "SELECT " \
    "alias_status,"        /* 0 */ \
    "alias_flags,"         /* 1 */ \
    "alias_id,"            /* 2 */ \
    "token_id,"            /* 3 */ \
    "token_digest,"        /* 4 */ \
    "token_digest_type,"   /* 5 */ \
    "user_id,"             /* 6 */ \
    "alias_type,"          /* 7 */ \
    "alias_text,"          /* 8 */ \
    "alias_language,"      /* 9 */ \
    "strftime('%s',alias_update_time)," /* 10 */ \
    "alias_blessed_count," /* 11 */ \
    "alias_cursed_count,"  /* 12 */ \
    "alias_blocked_count " /* 13 */ \
  "FROM alias "

#define DB_SET_ALIAS(_alias, _query) \
{ \
  bool ok; \
  (_alias).setStatus((_query).value(0).toInt(&ok)); \
  Q_ASSERT(ok); \
  (_alias).setFlags((_query).value(1).toULongLong(&ok)); \
  Q_ASSERT(ok); \
  (_alias).setId((_query).value(2).toLongLong()); \
  Q_ASSERT(ok); \
  (_alias).setTokenId((_query).value(3).toLongLong()); \
  Q_ASSERT(ok); \
  (_alias).setTokenDigest((_query).value(4).toString()); \
  (_alias).setTokenDigestType((_query).value(5).toInt(&ok)); \
  Q_ASSERT(ok); \
  (_alias).setUserId((_query).value(6).toLongLong()); \
  Q_ASSERT(ok); \
  (_alias).setType((_query).value(7).toInt(&ok)); \
  Q_ASSERT(ok); \
  (_alias).setText((_query).value(8).toString()); \
  (_alias).setLanguage((_query).value(9).toInt(&ok)); \
  Q_ASSERT(ok); \
  (_alias).setUpdateTime((_query).value(10).toLongLong(&ok)); \
  Q_ASSERT(ok); \
  (_alias).setBlessedCount((_query).value(11).toUInt(&ok)); \
  Q_ASSERT(ok); \
  (_alias).setCursedCount((_query).value(12).toUInt(&ok)); \
  Q_ASSERT(ok); \
  (_alias).setBlockedCount((_query).value(13).toUInt(&ok)); \
  Q_ASSERT(ok); \
}

#define DB_SELECT_ANNOTATION \
  "SELECT " \
    "annot_status,"    /* 0 */ \
    "annot_flags,"     /* 1 */ \
    "annot_id,"        /* 2 */ \
    "token_id,"        /* 3 */ \
    "token_digest,"    /* 4 */ \
    "token_digest_type," /* 5 */ \
    "user_id,"         /* 6 */ \
    "user_alias,"      /* 7 */ \
    "annot_pos,"       /* 8 */ \
    "annot_pos_type,"  /* 9 */ \
    "annot_time,"      /* 10 */ \
    "annot_text,"      /* 11 */ \
    "annot_language,"  /* 12 */ \
    DB_UNIX_TIMESTAMP("annot_create_time") "," /* 13 */ \
    DB_UNIX_TIMESTAMP("annot_update_time") "," /* 14 */ \
    "annot_blessed_count," /* 15 */ \
    "annot_cursed_count,"  /* 16 */ \
    "annot_blocked_count " /* 17 */ \
  "FROM annot "

#define DB_SET_ANNOTATION(_annot, _query) \
{ \
  bool ok; \
  (_annot).setStatus((_query).value(0).toInt(&ok)); \
  Q_ASSERT(ok); \
  (_annot).setFlags((_query).value(1).toULongLong(&ok)); \
  Q_ASSERT(ok); \
  (_annot).setId((_query).value(2).toLongLong(&ok)); \
  Q_ASSERT(ok); \
  (_annot).setTokenId((_query).value(3).toLongLong(&ok)); \
  Q_ASSERT(ok); \
  (_annot).setTokenDigest((_query).value(4).toString()); \
  (_annot).setTokenDigestType((_query).value(5).toInt(&ok)); \
  Q_ASSERT(ok); \
  (_annot).setUserId((_query).value(6).toLongLong(&ok)); \
  Q_ASSERT(ok); \
  (_annot).setUserAlias((_query).value(7).toString()); \
  (_annot).setPos((_query).value(8).toLongLong(&ok)); \
  Q_ASSERT(ok); \
  (_annot).setPosType((_query).value(9).toInt(&ok)); \
  Q_ASSERT(ok); \
  (_annot).setTime((_query).value(10).toInt(&ok)); \
  Q_ASSERT(ok); \
  (_annot).setText((_query).value(11).toString()); \
  (_annot).setLanguage((_query).value(12).toInt(&ok)); \
  Q_ASSERT(ok); \
  (_annot).setCreateTime((_query).value(13).toLongLong(&ok)); \
  Q_ASSERT(ok); \
  (_annot).setUpdateTime((_query).value(14).toLongLong(&ok)); \
  Q_ASSERT(ok); \
  (_annot).setBlessedCount((_query).value(15).toUInt(&ok)); \
  Q_ASSERT(ok); \
  (_annot).setCursedCount((_query).value(16).toUInt(&ok)); \
  Q_ASSERT(ok); \
  (_annot).setBlockedCount((_query).value(17).toUInt(&ok)); \
  Q_ASSERT(ok); \
}

#endif // DB_CONFIG_H
