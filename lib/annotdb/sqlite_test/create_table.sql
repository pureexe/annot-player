/*
 * create_table.sql
 * for SQLite
 * Since 9/12/2011
 *
 * Difference against MySQL follows:
 * - Not supported: AUTO_INCREMENT, INDEX
 */

CREATE TABLE user(
  user_id           BIGINT NOT NULL PRIMARY KEY /*AUTO_INCREMENT*/,
  group_id          BIGINT NOT NULL DEFAULT 0,
  user_name         VARCHAR(64) NOT NULL UNIQUE,    -- 64 is the maximum allowed for email username
  user_password     CHAR(40),       -- SHA1
  user_nickname     VARCHAR(255) NOT NULL,
  user_email        VARCHAR(160) NOT NULL UNIQUE,   -- 320 = 64 + 1 + 255 is the maximum, but not allowd to be unique by mysql
  user_status       INT NOT NULL DEFAULT 0,
  user_flags        BIGINT UNSIGNED NOT NULL DEFAULT 0,
  user_language     INT NOT NULL DEFAULT 0,
  user_create_time  DATETIME NOT NULL,
  user_login_time   DATETIME,
  user_blessed_count    INT UNSIGNED NOT NULL DEFAULT 0,
  user_cursed_count     INT UNSIGNED NOT NULL DEFAULT 0,
  user_blocked_count    INT UNSIGNED NOT NULL DEFAULT 0
  -- INDEX(user_name)
);

CREATE TABLE token(
  token_id          BIGINT NOT NULL DEFAULT 0 /*PRIMARY KEY AUTO_INCREMENT*/,
  user_id           BIGINT NOT NULL DEFAULT 0,
  token_digest      CHAR(32) NOT NULL,  -- MD5
  token_tt          INT NOT NULL DEFAULT 0,
  token_status      INT NOT NULL DEFAULT 0,
  token_flags       BIGINT UNSIGNED NOT NULL DEFAULT 0,
  token_create_time DATETIME NOT NULL,
  token_blessed_count   INT UNSIGNED NOT NULL DEFAULT 0,
  token_cursed_count    INT UNSIGNED NOT NULL DEFAULT 0,
  token_blocked_count   INT UNSIGNED NOT NULL DEFAULT 0,
  token_visited_count   INT UNSIGNED NOT NULL DEFAULT 0
  -- INDEX(token_digest)
  -- FOREIGN KEY(user_id) REFERENCES user(user_id)
  --   ON DELETE CASCADE ON UPDATE CASCADE
);

CREATE TABLE alias(
  alias_id          BIGINT NOT NULL DEFAULT 0 /*PRIMARY KEY AUTO_INCREMENT*/,
  token_id          BIGINT NOT NULL DEFAULT 0,
  token_digest      CHAR(32),     -- MD5
  user_id           BIGINT NOT NULL DEFAULT 0,
  alias_tt          INT NOT NULL DEFAULT 0,
  alias_type        INT NOT NULL DEFAULT 0,
  alias_text        VARCHAR(255),
  alias_status      INT NOT NULL DEFAULT 0,
  alias_flags       BIGINT UNSIGNED NOT NULL DEFAULT 0,
  alias_language    INT NOT NULL DEFAULT 0,
  alias_update_time DATETIME NOT NULL,
  alias_blessed_count   INT UNSIGNED NOT NULL DEFAULT 0,
  alias_cursed_count    INT UNSIGNED NOT NULL DEFAULT 0,
  alias_blocked_count   INT UNSIGNED NOT NULL DEFAULT 0
  -- INDEX(token_id)
  -- FOREIGN KEY(token_id) REFERENCES media_token(token_id)
  --   ON DELETE CASCADE ON UPDATE CASCADE
  -- FOREIGN KEY(user_id) REFERENCES user(user_id)
  --   ON DELETE CASCADE ON UPDATE CASCADE
);

CREATE TABLE annot(
  annot_id          BIGINT NOT NULL DEFAULT 0 /*PRIMARY KEY AUTO_INCREMENT*/,
  token_id          BIGINT NOT NULL DEFAULT 0,
  token_digest      CHAR(32),     -- MD5
  user_id           BIGINT NOT NULL DEFAULT 0,
  user_alias        VARCHAR(64),   -- the same length as user_name
  annot_tt          INT NOT NULL DEFAULT 0,
  annot_status      INT NOT NULL DEFAULT 0,
  annot_flags       BIGINT UNSIGNED NOT NULL DEFAULT 0,
  annot_language    INT NOT NULL DEFAULT 0,
  annot_create_time DATETIME NOT NULL,
  annot_update_time DATETIME,
  annot_pos         BIGINT NOT NULL DEFAULT 0,
  annot_pos_type    INT NOT NULL DEFAULT 0,
  annot_text        VARCHAR(255),
  annot_blessed_count   INT UNSIGNED NOT NULL DEFAULT 0,
  annot_cursed_count    INT UNSIGNED NOT NULL DEFAULT 0,
  annot_blocked_count   INT UNSIGNED NOT NULL DEFAULT 0
  -- INDEX(token_id)
  -- FOREIGN KEY(token_id) REFERENCES token(token_id)
  --  ON DELETE CASCADE ON UPDATE CASCADE
  -- FOREIGN KEY(user_id) REFERENCES user(user_id)
  --  ON DELETE CASCADE ON UPDATE CASCADE
);

-- EOF

/*
CREATE TABLE event(
  sender_id         BIGINT NOT NULL,
  sender_type       INT NOT NULL DEFAULT 0,
  receiver_id       BIGINT NOT NULL,
  receiver_type     INT NOT NULL DEFAULT 0,
  group_id          BIGINT NOT NULL DEFAULT 0,
  event_type        INT NOT NULL,
  event_flags       BIGINT UNSIGNED NOT NULL DEFAULT 0,
  event_time        DATETIME NOT NULL
);
*/
