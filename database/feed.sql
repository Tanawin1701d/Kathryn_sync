use feed_db;
CREATE TABLE feed_tb (

uuid     CHAR(36) NOT NULL,
author   VARCHAR(64),
message  VARCHAR(1024),
likes    INT (10) UNSIGNED,
isdelete BOOLEAN,
primary key (uuid)
);
