#ifndef ANNOTDBDEFS_H
#define ANNOTDBDEFS_H

// annotdbdefs.h
// 9/13/2011

#define ANNOTDB_VERSION "0.1.9"

// - SQLite helpers -

#ifdef ANNOTDB_WITH_DATETIME
  // See: http://www.sqlite.org/lang_datefunc.html
# define ANNOTDB_UNIX_TIMESTAMP(_t) "strftime('%s'," _t ")"
# define ANNOTDB_FROM_UNIXTIME(_ts) "datetime(" _ts ",'unixepoch')"
# define ANNOTDB_DATETIME           "DATETIME"
#else
# define ANNOTDB_UNIX_TIMESTAMP(_t) _t
# define ANNOTDB_FROM_UNIXTIME(_ts) _ts
# define ANNOTDB_DATETIME           "BIGINT"
#endif // ANNOTDB_WITH_DATETIME

// - Creation -

#define ANNOTDB_CREATE_TABLE_USER \
  "CREATE TABLE user(" \
    "user_id "          "BIGINT NOT NULL PRIMARY KEY," /* AUTO_INCREMENT */ \
    "user_name "        "VARCHAR(64) NOT NULL UNIQUE," /* 64 is the maximum allowed for email username */ \
    "user_password "    "CHAR(40),"     /* SHA1 */ \
    "user_nickname "    "VARCHAR(255)," \
    "user_email "       "VARCHAR(160) DEFAULT NULL," /* 320 = 64 + 1 + 255 is the maximum, but not allowd to be unique by mysql */ \
    "user_status "      "INT NOT NULL DEFAULT 0," \
    "user_flags "       "BIGINT UNSIGNED NOT NULL DEFAULT 0," \
    "user_lang "        "INT NOT NULL DEFAULT 0," \
    "user_create_time " ANNOTDB_DATETIME " NOT NULL," \
    "user_create_ip "   "BIGINT," \
    "user_login_time "  ANNOTDB_DATETIME " NOT NULL," \
    "user_login_ip "    "BIGINT," \
    "user_login_count "   "BIGINT NOT NULL DEFAULT 0," \
    "user_blessed_count " "INT UNSIGNED NOT NULL DEFAULT 0," \
    "user_cursed_count "  "INT UNSIGNED NOT NULL DEFAULT 0," \
    "user_blocked_count " "INT UNSIGNED NOT NULL DEFAULT 0," \
    "user_annot_count "   "INT UNSIGNED NOT NULL DEFAULT 0" \
    /*"INDEX(user_name)"*/ \
  ")"

#define ANNOTDB_CREATE_TABLE_TOKEN \
  "CREATE TABLE token(" \
    "token_id "         "BIGINT NOT NULL," /* PRIMARY KEY AUTO_INCREMENT */ \
    "user_id "          "BIGINT NOT NULL DEFAULT 0," \
    "token_type "       "INT NOT NULL DEFAULT 0," \
    "token_digest "     "CHAR(32) DEFAULT NULL," /* MD5, since unhex not supported in SQLite */ \
    "token_url "        "VARCHAR(255) DEFAULT NULL," \
    "token_section "    "INT NOT NULL DEFAULT 0," \
    "token_status "     "INT NOT NULL DEFAULT 0," \
    "token_flags "      "BIGINT UNSIGNED NOT NULL DEFAULT 0," \
    "token_create_time " ANNOTDB_DATETIME " NOT NULL," \
    "token_create_ip "  "BIGINT," \
    "token_blessed_count " "INT UNSIGNED NOT NULL DEFAULT 0," \
    "token_cursed_count "  "INT UNSIGNED NOT NULL DEFAULT 0," \
    "token_blocked_count " "INT UNSIGNED NOT NULL DEFAULT 0," \
    "token_visited_count " "INT UNSIGNED NOT NULL DEFAULT 0," \
    "token_annot_count "   "INT UNSIGNED NOT NULL DEFAULT 0" \
    /*"INDEX(token_digest)"*/ \
    /*"FOREIGN KEY(user_id) REFERENCES user(user_id) "*/ \
    /*  "ON DELETE CASCADE ON UPDATE CASCADE"*/ \
  ")"

#define ANNOTDB_CREATE_TABLE_ALIAS \
  "CREATE TABLE alias(" \
    "alias_id "         "BIGINT NOT NULL," /* PRIMARY KEY AUTO_INCREMENT */ \
    "token_id "         "BIGINT NOT NULL DEFAULT 0," /* AUTO_INCREMENT */ \
    "token_digest "     "CHAR(32) DEFAULT NULL," /* MD5, since unhex not supported in SQLite */ \
    "token_section "    "INT NOT NULL DEFAULT 0," \
    "user_id "          "BIGINT NOT NULL DEFAULT 0," \
    "alias_type "       "INT NOT NULL DEFAULT 0," \
    "alias_text "       "VARCHAR(255)," \
    "alias_flags "      "BIGINT UNSIGNED NOT NULL DEFAULT 0," \
    "alias_status "     "INT NOT NULL DEFAULT 0," \
    "alias_lang "       "INT NOT NULL DEFAULT 0," \
    "alias_update_time " ANNOTDB_DATETIME " NOT NULL," \
    "alias_update_ip "  "BIGINT," \
    "alias_blessed_count " "INT UNSIGNED NOT NULL DEFAULT 0," \
    "alias_cursed_count "  "INT UNSIGNED NOT NULL DEFAULT 0," \
    "alias_blocked_count " "INT UNSIGNED NOT NULL DEFAULT 0" \
    /*"INDEX(token_digest)"*/ \
    /*"FOREIGN KEY(user_id) REFERENCES token(token_id) "*/ \
    /*  "ON DELETE CASCADE ON UPDATE CASCADE"*/ \
    /*"FOREIGN KEY(user_id) REFERENCES user(user_id) "*/ \
    /*  "ON DELETE CASCADE ON UPDATE CASCADE"*/ \
  ")"


#define ANNOTDB_CREATE_TABLE_ANNOT \
  "CREATE TABLE annot(" \
    "annot_id "         "BIGINT NOT NULL DEFAULT 0," /* PRIMARY KEY AUTO_INCREMENT */ \
    "token_id "         "BIGINT NOT NULL DEFAULT 0," \
    "token_digest "     "CHAR(32) DEFAULT NULL," /* MD5, since unhex not supported in SQLite */ \
    "token_section "    "INT NOT NULL DEFAULT 0," \
    "user_id "          "BIGINT NOT NULL DEFAULT 0," \
    "user_alias "       "VARCHAR(64)," /* the same length as user_name */ \
    "annot_status "     "INT NOT NULL DEFAULT 0," \
    "annot_flags "      "BIGINT UNSIGNED NOT NULL DEFAULT 0," \
    "annot_lang "       "INT NOT NULL DEFAULT 0," \
    "annot_create_time " ANNOTDB_DATETIME " NOT NULL," \
    "annot_create_ip "  "BIGINT," \
    "annot_update_time " ANNOTDB_DATETIME "," \
    "annot_update_ip "  "BIGINT," \
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

#define ANNOTDB_INSERT_USER(_user, _query) \
{ \
  _query.prepare( \
    "INSERT INTO user(" \
      "user_status,"            /* 0 */ \
      "user_flags,"             /* 1 */ \
      "user_id,"                /* 2 */ \
      "user_name,"              /* 3 */ \
      "user_password,"          /* 4 */ \
      "user_nickname,"          /* 5 */ \
      "user_email,"             /* 6 */ \
      "user_lang,"              /* 7 */ \
      "user_create_time,"       /* 8 */ \
      "user_create_ip,"         /* 9 */ \
      "user_login_time,"        /* 10 */ \
      "user_login_ip,"          /* 11 */ \
      "user_login_count,"       /* 12 */ \
      "user_blessed_count,"     /* 13 */ \
      "user_cursed_count,"      /* 14 */ \
      "user_blocked_count,"     /* 15 */ \
      "user_annot_count"        /* 16 */ \
    ") VALUES (" \
      "?,"      /* 0: user_status */ \
      "?,"      /* 1: user_flags */ \
      "?,"      /* 2: user_id */ \
      "?,"      /* 3: user_name */ \
      "?,"      /* 4: user_password */ \
      "?,"      /* 5: user_nickname */ \
      "?,"      /* 6: user_email */ \
      "?,"      /* 7: user_lang */ \
      ANNOTDB_FROM_UNIXTIME("?") "," /* 8: user_create_time */ \
      "?,"      /* 9: user_create_ip */ \
      ANNOTDB_FROM_UNIXTIME("?") "," /* 10: user_login_time */ \
      "?,"      /* 11: user_login_ip */ \
      "?,"      /* 12: user_login_count */ \
      "?,"      /* 13: user_blessed_count */ \
      "?,"      /* 14: user_cursed_count */ \
      "?,"      /* 15: user_blocked_count */ \
      "?"       /* 16: user_annot_count */ \
    ")" \
  ); \
  (_query).addBindValue((_user).status());       /* 0 */ \
  (_query).addBindValue((_user).flags());        /* 1 */ \
  (_query).addBindValue((_user).id());           /* 2 */ \
  (_query).addBindValue((_user).name());         /* 3 */ \
  (_query).addBindValue((_user).password());     /* 4 */ \
  (_query).addBindValue((_user).nickname());     /* 5 */ \
  (_query).addBindValue((_user).email());        /* 6 */ \
  (_query).addBindValue((_user).language());     /* 7 */ \
  (_query).addBindValue((_user).createTime());   /* 8 */ \
  (_query).addBindValue((_user).createIp());     /* 9 */ \
  (_query).addBindValue((_user).loginTime());    /* 10 */ \
  (_query).addBindValue((_user).loginIp());      /* 11 */ \
  (_query).addBindValue((_user).loginCount());   /* 12 */ \
  (_query).addBindValue((_user).blessCount());   /* 13 */ \
  (_query).addBindValue((_user).curseCount());   /* 14 */ \
  (_query).addBindValue((_user).blockCount());   /* 15 */ \
  (_query).addBindValue((_user).annotCount());   /* 16 */ \
}

#define ANNOTDB_INSERT_TOKEN(_token, _query) \
{ \
  _query.prepare( \
    "INSERT INTO token(" \
      "token_status,"           /* 0 */ \
      "token_flags,"            /* 1 */ \
      "token_id,"               /* 2 */ \
      "token_type,"             /* 3 */ \
      "user_id,"                /* 4 */ \
      "token_digest,"           /* 5 */ \
      "token_url,"              /* 6 */ \
      "token_section,"          /* 7 */ \
      "token_create_time,"      /* 8 */ \
      "token_create_ip,"        /* 9 */ \
      "token_blessed_count,"    /* 10 */ \
      "token_cursed_count,"     /* 11 */ \
      "token_blocked_count,"    /* 12 */ \
      "token_visited_count,"    /* 13 */ \
      "token_annot_count"       /* 14 */ \
    ") VALUES (" \
      "?,"      /* 0: token_status */ \
      "?,"      /* 1: token_flags */ \
      "?,"      /* 2: token_id */ \
      "?,"      /* 3: token_type */ \
      "?,"      /* 4: user_id */ \
      "?,"      /* 5: token_digest */ \
      "?,"      /* 6: token_digest */ \
      "?,"      /* 7: token_section */ \
      ANNOTDB_FROM_UNIXTIME("?") "," /* 8: token_create_time */ \
      "?,"      /* 9: token_create_ip */ \
      "?,"      /* 10: token_blessed_count */ \
      "?,"      /* 11: token_cursed_count */ \
      "?,"      /* 12: token_blocked_count */ \
      "?,"      /* 13: token_visited_count */ \
      "?"       /* 14: token_annot_count */ \
    ")" \
  ); \
  (_query).addBindValue((_token).status());       /* 0 */ \
  (_query).addBindValue((_token).flags());        /* 1 */ \
  (_query).addBindValue((_token).id());           /* 2 */ \
  (_query).addBindValue((_token).type());         /* 3 */ \
  (_query).addBindValue((_token).userId());       /* 4 */ \
  (_query).addBindValue((_token).digest());       /* 5 */ \
  (_query).addBindValue((_token).url());          /* 6 */ \
  (_query).addBindValue((_token).section());      /* 7 */ \
  (_query).addBindValue((_token).createTime());   /* 8 */ \
  (_query).addBindValue((_token).createIp());     /* 9 */ \
  (_query).addBindValue((_token).blessCount());   /* 10 */ \
  (_query).addBindValue((_token).curseCount());   /* 11 */ \
  (_query).addBindValue((_token).blockCount());   /* 12 */ \
  (_query).addBindValue((_token).visitCount());   /* 13 */ \
  (_query).addBindValue((_token).annotCount());   /* 14 */ \
}

#define ANNOTDB_INSERT_ALIAS(_alias, _query) \
{ \
  _query.prepare( \
    "INSERT INTO alias(" \
      "alias_status,"           /* 0 */ \
      "alias_flags,"            /* 1 */ \
      "alias_id,"               /* 2 */ \
      "token_id,"               /* 3 */ \
      "token_digest,"           /* 4 */ \
      "token_section,"          /* 5 */ \
      "user_id,"                /* 6 */ \
      "alias_type,"             /* 7 */ \
      "alias_text,"             /* 8 */ \
      "alias_lang,"             /* 9 */ \
      "alias_update_time,"      /* 10 */ \
      "alias_update_ip,"        /* 11 */ \
      "alias_blessed_count,"    /* 12 */ \
      "alias_cursed_count,"     /* 13 */ \
      "alias_blocked_count"     /* 14 */ \
    ") VALUES (" \
      "?,"      /* 0: alias_status */ \
      "?,"      /* 1: alias_flags */ \
      "?,"      /* 2: alias_id */ \
      "?,"      /* 3: token_id */ \
      "?,"      /* 4: token_digest */ \
      "?,"      /* 5: token_section */ \
      "?,"      /* 6: user_id */ \
      "?,"      /* 7: alias_type */ \
      "?,"      /* 8: alias_text */ \
      "?,"      /* 9: alias_lang */ \
      ANNOTDB_FROM_UNIXTIME("?") "," /* 10: alias_update_time */ \
      "?,"      /* 11: alias_update_ip */ \
      "?,"      /* 12: alias_blessed_count */ \
      "?,"      /* 13: alias_cursed_count */ \
      "?"       /* 14: alias_blocked_count */ \
    ")" \
  ); \
  (_query).addBindValue((_alias).status());       /* 0 */ \
  (_query).addBindValue((_alias).flags());        /* 1 */ \
  (_query).addBindValue((_alias).id());           /* 2 */ \
  (_query).addBindValue((_alias).tokenId());      /* 3 */ \
  (_query).addBindValue((_alias).tokenDigest());  /* 4 */ \
  (_query).addBindValue((_alias).tokenSection()); /* 5 */ \
  (_query).addBindValue((_alias).userId());       /* 6 */ \
  (_query).addBindValue((_alias).type());         /* 7 */ \
  (_query).addBindValue((_alias).text());         /* 8 */ \
  (_query).addBindValue((_alias).language());     /* 9 */ \
  (_query).addBindValue((_alias).updateTime());   /* 10 */ \
  (_query).addBindValue((_alias).updateIp());     /* 11 */ \
  (_query).addBindValue((_alias).blessCount());   /* 12 */ \
  (_query).addBindValue((_alias).curseCount());   /* 13 */ \
  (_query).addBindValue((_alias).blockCount());   /* 14 */ \
}

#define ANNOTDB_INSERT_ANNOT(_annot, _query) \
{ \
  _query.prepare( \
    "INSERT INTO annot(" \
      "annot_status,"   /* 0 */ \
      "annot_flags,"    /* 1 */ \
      "annot_id,"       /* 2 */ \
      "token_id,"       /* 3 */ \
      "token_digest,"   /* 4 */ \
      "token_section,"  /* 5 */ \
      "user_id,"        /* 6 */ \
      "user_alias,"     /* 7 */ \
      "annot_pos,"      /* 8 */ \
      "annot_pos_type," /* 9 */ \
      "annot_time,"     /* 10 */ \
      "annot_text,"     /* 11 */ \
      "annot_lang,"     /* l2 */ \
      "annot_create_time,"      /* 13 */ \
      "annot_create_ip,"        /* 14 */ \
      "annot_update_time,"      /* 15 */ \
      "annot_update_ip,"        /* 16 */ \
      "annot_blessed_count,"    /* 17 */ \
      "annot_cursed_count,"     /* 18 */ \
      "annot_blocked_count"     /* 19 */ \
    ") VALUES (" \
      "?,"      /* 0: annot_status */ \
      "?,"      /* 1: annot_flags */ \
      "?,"      /* 2: annot_id */ \
      "?,"      /* 3: token_id */ \
      "?,"      /* 4: token_digest */ \
      "?,"      /* 5: token_section */ \
      "?,"      /* 6: user_id */ \
      "?,"      /* 7: user_alias */ \
      "?,"      /* 8: annot_pos */ \
      "?,"      /* 9: annot_pos_type */ \
      "?,"      /* 10: annot_time */ \
      "?,"      /* 11: annot_text */ \
      "?,"      /* 12: annot_lang */ \
      ANNOTDB_FROM_UNIXTIME("?") "," /* 13: annot_create_time */ \
      "?,"      /* 14: annot_create_ip */ \
      ANNOTDB_FROM_UNIXTIME("?") "," /* 15: annot_update_time */ \
      "?,"      /* 16: annot_update_ip */ \
      "?,"      /* 17: annot_blessed_count */ \
      "?,"      /* 18: annot_cursed_count */ \
      "?"       /* 19: annot_blocked_count */ \
    ")" \
  ); \
  (_query).addBindValue((_annot).status());       /* 0 */ \
  (_query).addBindValue((_annot).flags());        /* 1 */ \
  (_query).addBindValue((_annot).id());           /* 2 */ \
  (_query).addBindValue((_annot).tokenId());      /* 3 */ \
  (_query).addBindValue((_annot).tokenDigest());  /* 4 */ \
  (_query).addBindValue((_annot).tokenSection()); /* 5 */ \
  (_query).addBindValue((_annot).userId());       /* 6 */ \
  (_query).addBindValue((_annot).userAlias());    /* 7 */ \
  (_query).addBindValue((_annot).pos());          /* 8 */ \
  (_query).addBindValue((_annot).posType());      /* 9 */ \
  (_query).addBindValue((_annot).time());         /* 10 */ \
  (_query).addBindValue((_annot).text());         /* 11 */ \
  (_query).addBindValue((_annot).language());     /* 12 */ \
  (_query).addBindValue((_annot).createTime());   /* 13 */ \
  (_query).addBindValue((_annot).createIp());     /* 14 */ \
  (_query).addBindValue((_annot).updateTime());   /* 15 */ \
  (_query).addBindValue((_annot).updateIp());     /* 16 */ \
  (_query).addBindValue((_annot).blessCount());   /* 17 */ \
  (_query).addBindValue((_annot).curseCount());   /* 18 */ \
  (_query).addBindValue((_annot).blockCount());   /* 19 */ \
}

// - Queries -

#define ANNOTDB_SELECT_USER \
  "SELECT " \
    "user_status,"    /* 0 */ \
    "user_flags,"     /* 1 */ \
    "user_id,"        /* 2 */ \
    "user_name,"      /* 3 */ \
    "user_password,"  /* 4 */ \
    "user_nickname,"  /* 5 */ \
    "user_email,"     /* 6 */ \
    "user_lang,"      /* 7 */ \
    ANNOTDB_UNIX_TIMESTAMP("user_create_time") "," /* 8 */ \
    "user_create_ip,"     /* 9 */ \
    ANNOTDB_UNIX_TIMESTAMP("user_login_time") ","  /* 10 */ \
    "user_login_ip,"      /* 11 */ \
    "user_login_count,"   /* 12 */ \
    "user_blessed_count," /* 13 */ \
    "user_cursed_count,"  /* 14 */ \
    "user_blocked_count," /* 15 */ \
    "user_annot_count "   /* 16 */ \
  "FROM user "

#define ANNOTDB_SET_USER(_user, _query) \
{ \
  (_user).setStatus((_query).value(0).toInt()); \
  (_user).setFlags((_query).value(1).toULongLong()); \
  (_user).setId((_query).value(2).toLongLong()); \
  (_user).setName((_query).value(3).toString()); \
  (_user).setPassword((_query).value(4).toString()); \
  (_user).setNickname((_query).value(5).toString()); \
  (_user).setEmail((_query).value(6).toString()); \
  (_user).setLanguage((_query).value(7).toInt()); \
  (_user).setCreateTime((_query).value(8).toLongLong()); \
  (_user).setCreateIp((_query).value(9).toLongLong()); \
  (_user).setLoginTime((_query).value(10).toLongLong()); \
  (_user).setLoginIp((_query).value(11).toLongLong()); \
  (_user).setLoginCount((_query).value(12).toLongLong()); \
  (_user).setBlessCount((_query).value(13).toUInt()); \
  (_user).setCurseCount((_query).value(14).toUInt()); \
  (_user).setBlockCount((_query).value(15).toUInt()); \
  (_user).setAnnotCount((_query).value(16).toUInt()); \
}

#define ANNOTDB_SELECT_TOKEN \
  "SELECT " \
    "token_status,"        /* 0 */ \
    "token_flags,"         /* 1 */ \
    "token_id,"            /* 2 */ \
    "token_type,"          /* 3 */ \
    "user_id,"             /* 4 */ \
    "token_digest,"        /* 5 */ \
    "token_url,"           /* 6 */ \
    "token_section,"       /* 7 */ \
    ANNOTDB_UNIX_TIMESTAMP("token_create_time") "," /* 8 */ \
    "token_create_ip,"     /* 9 */ \
    "token_blessed_count," /* 10 */ \
    "token_cursed_count,"  /* 11 */ \
    "token_blocked_count," /* 12 */ \
    "token_visited_count," /* 13 */ \
    "token_annot_count "   /* 14 */ \
  "FROM token "

#define ANNOTDB_SET_TOKEN(_token, _query) \
{ \
  (_token).setStatus((_query).value(0).toInt()); \
  (_token).setFlags((_query).value(1).toULongLong()); \
  (_token).setId((_query).value(2).toLongLong()); \
  (_token).setType((_query).value(3).toInt()); \
  (_token).setUserId((_query).value(4).toLongLong()); \
  (_token).setDigest((_query).value(5).toString()); \
  (_token).setUrl((_query).value(6).toString()); \
  (_token).setSection((_query).value(7).toInt()); \
  (_token).setCreateTime((_query).value(8).toLongLong()); \
  (_token).setCreateIp((_query).value(9).toLongLong()); \
  (_token).setBlessCount((_query).value(10).toUInt()); \
  (_token).setCurseCount((_query).value(11).toUInt()); \
  (_token).setBlockCount((_query).value(12).toUInt()); \
  (_token).setVisitCount((_query).value(13).toUInt()); \
  (_token).setAnnotCount((_query).value(14).toUInt()); \
}

#define ANNOTDB_SELECT_ALIAS \
  "SELECT " \
    "alias_status,"        /* 0 */ \
    "alias_flags,"         /* 1 */ \
    "alias_id,"            /* 2 */ \
    "token_id,"            /* 3 */ \
    "token_digest,"        /* 4 */ \
    "token_section,"       /* 5 */ \
    "user_id,"             /* 6 */ \
    "alias_type,"          /* 7 */ \
    "alias_text,"          /* 8 */ \
    "alias_lang,"          /* 9 */ \
    ANNOTDB_UNIX_TIMESTAMP("alias_update_time") "," /* 10 */ \
    "alias_update_ip,"     /* 11 */ \
    "alias_blessed_count," /* 12 */ \
    "alias_cursed_count,"  /* 13 */ \
    "alias_blocked_count " /* 14 */ \
  "FROM alias "

#define ANNOTDB_SET_ALIAS(_alias, _query) \
{ \
  (_alias).setStatus((_query).value(0).toInt()); \
  (_alias).setFlags((_query).value(1).toULongLong()); \
  (_alias).setId((_query).value(2).toLongLong()); \
  (_alias).setTokenId((_query).value(3).toLongLong()); \
  (_alias).setTokenDigest((_query).value(4).toString()); \
  (_alias).setTokenSection((_query).value(5).toInt()); \
  (_alias).setUserId((_query).value(6).toLongLong()); \
  (_alias).setType((_query).value(7).toInt()); \
  (_alias).setText((_query).value(8).toString()); \
  (_alias).setLanguage((_query).value(9).toInt()); \
  (_alias).setUpdateTime((_query).value(10).toLongLong()); \
  (_alias).setUpdateIp((_query).value(11).toLongLong()); \
  (_alias).setBlessCount((_query).value(12).toUInt()); \
  (_alias).setCurseCount((_query).value(13).toUInt()); \
  (_alias).setBlockCount((_query).value(14).toUInt()); \
}

#define ANNOTDB_SELECT_ANNOTATION \
  "SELECT " \
    "annot_status,"    /* 0 */ \
    "annot_flags,"     /* 1 */ \
    "annot_id,"        /* 2 */ \
    "token_id,"        /* 3 */ \
    "token_digest,"    /* 4 */ \
    "token_section,"   /* 5 */ \
    "user_id,"         /* 6 */ \
    "user_alias,"      /* 7 */ \
    "annot_pos,"       /* 8 */ \
    "annot_pos_type,"  /* 9 */ \
    "annot_time,"      /* 10 */ \
    "annot_text,"      /* 11 */ \
    "annot_lang,"      /* 12 */ \
    ANNOTDB_UNIX_TIMESTAMP("annot_create_time") "," /* 13 */ \
    "annot_create_ip," /* 14 */ \
    ANNOTDB_UNIX_TIMESTAMP("annot_update_time") "," /* 15 */ \
    "annot_update_ip," /* 16 */ \
    "annot_blessed_count," /* 17 */ \
    "annot_cursed_count,"  /* 18 */ \
    "annot_blocked_count " /* 19 */ \
  "FROM annot "

#define ANNOTDB_SET_ANNOTATION(_annot, _query) \
{ \
  (_annot).setStatus((_query).value(0).toInt()); \
  (_annot).setFlags((_query).value(1).toULongLong()); \
  (_annot).setId((_query).value(2).toLongLong()); \
  (_annot).setTokenId((_query).value(3).toLongLong()); \
  (_annot).setTokenDigest((_query).value(4).toString()); \
  (_annot).setTokenSection((_query).value(5).toInt()); \
  (_annot).setUserId((_query).value(6).toLongLong()); \
  (_annot).setUserAlias((_query).value(7).toString()); \
  (_annot).setPos((_query).value(8).toLongLong()); \
  (_annot).setPosType((_query).value(9).toInt()); \
  (_annot).setTime((_query).value(10).toInt()); \
  (_annot).setText((_query).value(11).toString()); \
  (_annot).setLanguage((_query).value(12).toInt()); \
  (_annot).setCreateTime((_query).value(13).toLongLong()); \
  (_annot).setCreateIp((_query).value(14).toLongLong()); \
  (_annot).setUpdateTime((_query).value(15).toLongLong()); \
  (_annot).setUpdateIp((_query).value(16).toLongLong()); \
  (_annot).setBlessCount((_query).value(17).toUInt()); \
  (_annot).setCurseCount((_query).value(18).toUInt()); \
  (_annot).setBlockCount((_query).value(19).toUInt()); \
}

#endif // ANNOTDBDEFS_H
