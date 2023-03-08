use jor_db;

DROP TABLE IF EXISTS jor_tb;
CREATE TABLE jor_tb (

uuid              CHAR(36),
needFeed          BOOLEAN,
needImage         BOOLEAN,
shouldDeleted     BOOLEAN,
shouldDeleteImage BOOLEAN,
primary key (uuid)

);
