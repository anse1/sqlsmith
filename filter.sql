--
-- PostgreSQL database dump
--

SET statement_timeout = 0;
SET lock_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SET check_function_bodies = false;
SET client_min_messages = warning;

SET search_path = public, pg_catalog;

SET default_tablespace = '';

SET default_with_oids = false;

--
-- Name: known; Type: TABLE; Schema: public; Owner: andreas; Tablespace: 
--

CREATE TABLE known (
    error text,
    match bigint DEFAULT 0
);


ALTER TABLE known OWNER TO andreas;

--
-- Name: known_re; Type: TABLE; Schema: public; Owner: andreas; Tablespace: 
--

CREATE TABLE known_re (
    re text,
    match bigint DEFAULT 0
);


ALTER TABLE known_re OWNER TO andreas;

--
-- Data for Name: known; Type: TABLE DATA; Schema: public; Owner: andreas
--

COPY known (error, match) FROM stdin;
ERROR:  cache lookup failed for index 12322	0
ERROR:  cache lookup failed for index 2691	0
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/chkpass.control": Cannot allocate memory	78
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/pg_stat_statements.control": Cannot allocate memory	65
ERROR:  cannot subtract infinite dates	1
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/pg_prewarm.control": Cannot allocate memory	62
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/tsm_system_time.control": Cannot allocate memory	58
ERROR:  out of dynamic memory in GUC_yy_create_buffer() at file "/home/smith/postgres/inst/master/share/extension/tsm_system_rows.control" line 1	126
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/autoinc.control": Cannot allocate memory	52
ERROR:  missing support function 1(34757,34757) in opfamily 1976	0
ERROR:  Cannot decrypt with public key	0
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/tsm_system_time--1.0.control": Cannot allocate memory	31
ERROR:  could not find member 1(34572,34572) of opfamily 1976	0
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/btree_gin.control": Cannot allocate memory	75
ERROR:  Refusing to encrypt with secret key	0
ERROR:  out of dynamic memory in GUC_yy_create_buffer() at file "/home/smith/postgres/inst/master/share/extension/isn.control" line 1	111
ERROR:  Some PX error (not specified)	1
ERROR:  invalid input value for enum insenum: "(0) with time zone"	0
ERROR:  value for "us" in source string is out of range	0
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/btree_gin--1.0.control": Cannot allocate memory	31
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/hstore--1.3.control": Cannot allocate memory	27
ERROR:  prewarm type cannot be null	22
ERROR:  cannot open BEGIN query as cursor	2
ERROR:  out of memory	3102034
ERROR:  could not find member 1(34554,34554) of opfamily 1976	0
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/pg_buffercache.control": Cannot allocate memory	61
ERROR:  output cannot be empty string	607
ERROR:  missing support function 1(34848,34848) in opfamily 1976	467
ERROR:  cache lookup failed for index 28418	0
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/cube--1.1.control": Cannot allocate memory	23
ERROR:  could not find commutator for operator 34793	0
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/dict_int.control": Cannot allocate memory	77
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/adminpack.control": Cannot allocate memory	63
Some PX error (not specified)	0
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/cube--1.0.control": Cannot allocate memory	21
ERROR:  could not find commutator for operator 34858	93
ERROR:  out of dynamic memory in GUC_yy_create_buffer() at file "/home/smith/postgres/inst/master/share/extension/postgres_fdw.control" line 1	147
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/pgcrypto--1.2.control": Cannot allocate memory	23
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/fuzzystrmatch.control": Cannot allocate memory	77
ERROR:  out of dynamic memory in GUC_yy_create_buffer() at file "/home/smith/postgres/inst/master/share/extension/autoinc.control" line 1	130
ERROR:  no current dictionary	418
ERROR:  invalid prewarm type	0
password is required	0
ERROR:  cache lookup failed for function 401	0
ERROR:  could not find commutator for operator 34777	0
ERROR:  out of dynamic memory in GUC_yy_create_buffer() at file "/home/smith/postgres/inst/master/share/extension/pageinspect.control" line 1	138
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/tcn--1.0.control": Cannot allocate memory	29
ERROR:  could not read file "backup_label": Cannot allocate memory	2156
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/file_fdw.control": Cannot allocate memory	71
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/pageinspect--1.3.control": Cannot allocate memory	0
ERROR:  collations are not supported by type "	0
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/pg_freespacemap.control": Cannot allocate memory	58
ERROR:  syntax error at end of input	17
ERROR:  cache lookup failed for index 16886	42
ERROR:  zero raised to a negative power is undefined	156
ERROR:  cache lookup failed for index 17179	21
ERROR:  cache lookup failed for index 1259	38
ERROR:  ts_rewrite query must return two tsquery columns	12
ERROR:  value for "w" in source string is out of range	14
ERROR:  cannot convert reltime "invalid" to interval	3
ERROR:  value for "D" in source string is out of range	9
ERROR:  cannot open CREATE RULE query as cursor	27
ERROR:  cache lookup failed for index 2600	10
ERROR:  distance in phrase operator should be non-negative and less than 16384	3
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/autoinc--1.0.control": Cannot allocate memory	21
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/tsm_system_rows.control": Cannot allocate memory	61
ERROR:  missing support function 1(34771,34771) in opfamily 1976	0
ERROR:  currtid cannot handle views with no CTID	15
ERROR:  out of dynamic memory in GUC_yy_create_buffer() at file "/home/smith/postgres/inst/master/share/extension/file_fdw.control" line 1	144
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/tsearch2--1.0.control": Cannot allocate memory	19
ERROR:  could not find member 1(35078,35078) of opfamily 1976	0
ERROR:  operator 1054 is not a member of opfamily 426	0
ERROR:  out of dynamic memory in GUC_yy_create_buffer() at file "/home/smith/postgres/inst/master/share/extension/insert_username.control" line 1	120
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/tcn.control": Cannot allocate memory	59
ERROR:  could not open stop-word file "/home/smith/postgres/inst/master/share/tsearch_data/english.stop": Cannot allocate memory	0
ERROR:  source string too short for "D" formatting field	0
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/seg.control": Cannot allocate memory	66
ERROR:  could not load library "/home/smith/postgres/src/test/regress/regress.so": /home/smith/postgres/src/test/regress/regress.so: cannot allocate version reference table: Cannot allocate memory	0
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/pgstattuple--1.3.control": Cannot allocate memory	30
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/isn.control": Cannot allocate memory	73
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/plpgsql--1.0.control": Cannot allocate memory	29
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/unaccent--1.0.control": Cannot allocate memory	32
ERROR:  could not find member 1(34761,34761) of opfamily 1976	0
ERROR:  cache lookup failed for function 1048	0
ERROR:  must be a member of the role whose process is being terminated	31
ERROR:  could not find member 1(34622,34622) of opfamily 1976	0
ERROR:  out of dynamic memory in GUC_yy_create_buffer() at file "/home/smith/postgres/inst/master/share/extension/btree_gist.control" line 1	125
ERROR:  missing support function 1(34838,34838) in opfamily 1976	1175
ERROR:  could not load library "/home/smith/postgres/src/test/regress/regress.so": /home/smith/postgres/src/test/regress/regress.so: cannot open shared object file	0
ERROR:  missing support function 1(34755,34755) in opfamily 1976	0
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/timetravel--1.0.control": Cannot allocate memory	19
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/moddatetime.control": Cannot allocate memory	53
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/isn--1.0.control": Cannot allocate memory	28
ERROR:  invalid regular expression: out of memory	107420
ERROR:  could not find commutator for operator 34592	0
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/refint--1.0.control": Cannot allocate memory	33
ERROR:  COALESCE could not convert type "char" to character	0
ERROR:  unrecognized storage manager name	0
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/failmalloc.control": Cannot allocate memory	0
ERROR:  cannot accept a value of type internal	0
ERROR:  could not find member 1(34838,34838) of opfamily 1976	3
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/pageinspect--1.4.control": Cannot allocate memory	26
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/citext--1.1.control": Cannot allocate memory	20
ERROR:  missing support function 1(34568,34568) in opfamily 1976	0
ERROR:  missing support function 1(34790,34790) in opfamily 1976	0
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/pg_trgm--1.2.control": Cannot allocate memory	33
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/dblink.control": Cannot allocate memory	61
ERROR:  operator 98 does not exist	0
ERROR:  value for "Q" in source string is out of range	2
ERROR:  invalid combination of date conventions	158
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/citext.control": Cannot allocate memory	65
ERROR:  type planets is not composite	1
ERROR:  value for "US" in source string is out of range	2
ERROR:  cannot convert reserved abstime value to date	247
ERROR:  could not identify a comparison function for type xid	66
ERROR:  OID out of range	769
ERROR:  cannot merge addresses from different families	327
ERROR:  value for "I" in source string is out of range	20
ERROR:  requested length cannot be negative	82
ERROR:  timestamp out of range	228
ERROR:  cannot open multi-query plan as cursor	137
ERROR:  payload string too long	4
ERROR:  cache lookup failed for index 16434	4
ERROR:  too many points requested	25
ERROR:  value for "d" in source string is out of range	41
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/unaccent.control": Cannot allocate memory	84
ERROR:  could not find commutator for operator 35098	0
ERROR:  missing support function 1(34769,34769) in opfamily 1976	0
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/timetravel.control": Cannot allocate memory	53
ERROR:  out of dynamic memory in GUC_yy_create_buffer() at file "/home/smith/postgres/inst/master/share/extension/pg_prewarm.control" line 1	153
ERROR:  missing support function 1(34594,34594) in opfamily 1976	0
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/postgres_fdw--1.0.control": Cannot allocate memory	28
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/fuzzystrmatch--1.0.control": Cannot allocate memory	25
ERROR:  out of dynamic memory in GUC_yy_create_buffer() at file "/home/smith/postgres/inst/master/share/extension/seg.control" line 1	142
ERROR:  could not find member 1(34759,34759) of opfamily 1976	0
ERROR:  out of dynamic memory in GUC_yy_create_buffer() at file "/home/smith/postgres/inst/master/share/extension/pg_stat_statements.control" line 1	135
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/earthdistance--1.0.control": Cannot allocate memory	27
ERROR:  cache lookup failed for index 28458	0
ERROR:  out of dynamic memory in GUC_yy_create_buffer() at file "/home/smith/postgres/inst/master/share/extension/ltree.control" line 1	141
ERROR:  could not load library "/home/smith/postgres/inst/master/lib/dict_snowball.so": /home/smith/postgres/inst/master/lib/dict_snowball.so: cannot allocate symbol search list: Cannot allocate memory	1
ERROR:  out of dynamic memory in GUC_yy_create_buffer() at file "/home/smith/postgres/inst/master/share/extension/intarray.control" line 1	139
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/pgrowlocks--1.1.control": Cannot allocate memory	24
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/earthdistance.control": Cannot allocate memory	77
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/intarray.control": Cannot allocate memory	77
ERROR:  out of dynamic memory in GUC_yy_create_buffer() at file "/home/smith/postgres/inst/master/share/extension/tcn.control" line 1	152
ERROR:  NOTIFY is not allowed in a non-volatile function	0
ERROR:  could not load library "/home/smith/postgres/inst/master/lib/plpgsql.so": /home/smith/postgres/inst/master/lib/plpgsql.so: cannot create shared object descriptor	0
ERROR:  out of dynamic memory in GUC_yy_create_buffer() at file "/home/smith/postgres/inst/master/share/extension/cube.control" line 1	133
ERROR:  out of dynamic memory in GUC_yy_create_buffer() at file "/home/smith/postgres/inst/master/share/extension/pg_trgm.control" line 1	126
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/file_fdw--1.0.control": Cannot allocate memory	33
ERROR:  could not find member 1(34840,34840) of opfamily 1976	274
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/postgres_fdw.control": Cannot allocate memory	49
ERROR:  out of dynamic memory in GUC_yy_create_buffer() at file "/home/smith/postgres/inst/master/share/extension/hstore.control" line 1	132
ERROR:  cache lookup failed for type 23	0
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/cube.control": Cannot allocate memory	86
ERROR:  out of dynamic memory in GUC_yy_create_buffer() at file "/home/smith/postgres/inst/master/share/extension/dict_xsyn.control" line 1	131
ERROR:  attempt to redefine parameter "plpgsql.variable_conflict"	0
ERROR:  out of dynamic memory in GUC_yy_create_buffer() at file "/home/smith/postgres/inst/master/share/extension/dict_int.control" line 1	138
ERROR:  source string too short for "mm" formatting field	0
ERROR:  source string too short for "CC" formatting field	0
ERROR:  out of dynamic memory in GUC_yy_create_buffer() at file "/home/smith/postgres/inst/master/share/extension/pg_freespacemap.control" line 1	137
ERROR:  could not find member 1(34775,34775) of opfamily 1976	0
ERROR:  could not find commutator for operator 34640	0
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/chkpass--1.0.control": Cannot allocate memory	30
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/lo--1.0.control": Cannot allocate memory	17
ERROR:  could not load library "/home/smith/postgres/inst/master/lib/dict_snowball.so": /home/smith/postgres/inst/master/lib/dict_snowball.so: cannot open shared object file	0
ERROR:  could not find commutator for operator 34616	0
ERROR:  function "close_sl" not implemented	145
ERROR:  cannot subtract infinite timestamps	89
ERROR:  invalid query	65
ERROR:  must be a member of the role whose query is being canceled	25
ERROR:  out of dynamic memory in GUC_yy_create_buffer() at file "/home/smith/postgres/inst/master/share/extension/tablefunc.control" line 1	140
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/pg_stat_statements--1.3.control": Cannot allocate memory	26
ERROR:  gtsvector_in not implemented	3427
ERROR:  conflicting values for "i" field in formatting string	21
ERROR:  CREATE RULE is not allowed in a non-volatile function	8
ERROR:  requested length too large	511
ERROR:  cannot open INSERT query as cursor	67
ERROR:  out of dynamic memory in GUC_yy_create_buffer() at file "/home/smith/postgres/inst/master/share/extension/chkpass.control" line 1	129
ERROR:  TABLESAMPLE clause can only be applied to tables and materialized views	0
ERROR:  missing support function 1(34616,34616) in opfamily 1976	0
ERROR:  missing support function 1(35074,35074) in opfamily 1976	0
ERROR:  could not find member 1(34773,34773) of opfamily 1976	0
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/intarray--1.1.control": Cannot allocate memory	36
ERROR:  could not find commutator for operator 35096	0
ERROR:  cache lookup failed for index 28505	8
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/pageinspect.control": Cannot allocate memory	57
ERROR:  cache lookup failed for type 16	0
ERROR:  cache lookup failed for function 2803	0
ERROR:  missing support function 1(34834,34834) in opfamily 1976	27
ERROR:  cache lookup failed for type 25	0
ERROR:  missing support function 1(34570,34570) in opfamily 1976	0
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/lo.control": Cannot allocate memory	73
ERROR:  cache lookup failed for index 17164	26
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/adminpack--1.0.control": Cannot allocate memory	28
ERROR:  out of dynamic memory in GUC_yy_create_buffer() at file "/home/smith/postgres/inst/master/share/extension/pgstattuple.control" line 1	144
ERROR:  could not load library "/home/smith/postgres/inst/master/lib/dict_snowball.so": /home/smith/postgres/inst/master/lib/dict_snowball.so: cannot create shared object descriptor	1
ERROR:  out of dynamic memory in GUC_yy_create_buffer() at file "/home/smith/postgres/inst/master/share/extension/pgrowlocks.control" line 1	133
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/tsm_system_rows--1.0.control": Cannot allocate memory	30
ERROR:  out of dynamic memory in GUC_yy_create_buffer() at file "/home/smith/postgres/inst/master/share/extension/dblink.control" line 1	147
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/intagg.control": Cannot allocate memory	56
ERROR:  could not find commutator for operator 34638	0
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/dict_xsyn.control": Cannot allocate memory	65
ERROR:  could not find array type for data type anynonarray	0
ERROR:  missing support function 1(35076,35076) in opfamily 1976	135
ERROR:  function "close_lb" not implemented	553
ERROR:  out of dynamic memory in GUC_yy_create_buffer() at file "/home/smith/postgres/inst/master/share/extension/unaccent.control" line 1	134
ERROR:  value for "J" in source string is out of range	2
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/ltree.control": Cannot allocate memory	69
ERROR:  btree opfamily 1976 has a partial set of operators for datatypes bigint and int8alias1	1
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/dict_int--1.0.control": Cannot allocate memory	29
ERROR:  missing support function 1(34618,34618) in opfamily 1976	0
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/failmalloc--1.0.control": Cannot allocate memory	0
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/pgrowlocks.control": Cannot allocate memory	79
ERROR:  out of dynamic memory in GUC_yy_create_buffer() at file "/home/smith/postgres/inst/master/share/extension/refint.control" line 1	134
ERROR:  could not load library "/home/smith/postgres/src/test/regress/regress.so": /home/smith/postgres/src/test/regress/regress.so: cannot create shared object descriptor	0
ERROR:  out of dynamic memory in GUC_yy_create_buffer() at file "/home/smith/postgres/inst/master/share/extension/failmalloc.control" line 1	0
ERROR:  could not load library "/home/smith/postgres/src/test/regress/regress.so": /home/smith/postgres/src/test/regress/regress.so: cannot allocate symbol search list: Cannot allocate memory	0
ERROR:  could not load library "/home/smith/postgres/inst/master/lib/dict_snowball.so": /home/smith/postgres/inst/master/lib/dict_snowball.so: cannot allocate version reference table: Cannot allocate memory": Cannot allocate memory	0
ERROR:  cache lookup failed for type 1042	0
ERROR:  could not find commutator for operator 34572	0
ERROR:  out of dynamic memory in GUC_yy_create_buffer() at file "/home/smith/postgres/inst/master/share/extension/earthdistance.control" line 1	142
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/dblink--1.1.control": Cannot allocate memory	24
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/tsearch2.control": Cannot allocate memory	66
ERROR:  UPDATE is not allowed in a non-volatile function	106
ERROR:  could not find member 1(34598,34598) of opfamily 1976	0
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/hstore.control": Cannot allocate memory	67
ERROR:  COALESCE could not convert type character to "char"	0
ERROR:  could not find member 1(34574,34574) of opfamily 1976	0
ERROR:  could not find commutator for operator 34790	0
ERROR:  count must be greater than zero	757
ERROR:  cannot delete from scalar	1
ERROR:  value for "j" in source string is out of range	4
ERROR:  cannot get array length of a scalar	42
ERROR:  cannot open CREATE INDEX query as cursor	19
ERROR:  out of dynamic memory in GUC_yy_create_buffer() at file "/home/smith/postgres/inst/master/share/extension/fuzzystrmatch.control" line 1	127
ERROR:  could not find commutator for operator 34779	0
ERROR:  out of dynamic memory in GUC_yy_create_buffer() at file "/home/smith/postgres/inst/master/share/extension/timetravel.control" line 1	150
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/pgstattuple.control": Cannot allocate memory	69
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/intagg--1.0.control": Cannot allocate memory	32
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/insert_username.control": Cannot allocate memory	72
ERROR:  out of dynamic memory in GUC_yy_create_buffer() at file "/home/smith/postgres/inst/master/share/extension/pgcrypto.control" line 1	136
ERROR:  out of dynamic memory in GUC_yy_create_buffer() at file "/home/smith/postgres/inst/master/share/extension/plpgsql.control" line 1	120
ERROR:  could not find commutator for operator 34812	0
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/refint.control": Cannot allocate memory	57
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/ltree--1.0.control": Cannot allocate memory	34
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/moddatetime--1.0.control": Cannot allocate memory	33
ERROR:  function substr(character, integer, integer) does not exist	0
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/pg_freespacemap--1.0.control": Cannot allocate memory	34
ERROR:  missing support function 1(34550,34550) in opfamily 1976	0
ERROR:  could not load library "/home/smith/postgres/inst/master/lib/plpgsql.so": /home/smith/postgres/inst/master/lib/plpgsql.so: cannot allocate version reference table: Cannot allocate memory	0
ERROR:  out of dynamic memory in GUC_yy_create_buffer() at file "/home/smith/postgres/inst/master/share/extension/tsm_system_time.control" line 1	135
ERROR:  cache lookup failed for type 17	0
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/tablefunc--1.0.control": Cannot allocate memory	21
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/seg--1.0.control": Cannot allocate memory	27
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/insert_username--1.0.control": Cannot allocate memory	32
ERROR:  out of dynamic memory in GUC_yy_create_buffer() at file "/home/smith/postgres/inst/master/share/extension/tsearch2.control" line 1	132
ERROR:  out of dynamic memory in GUC_yy_create_buffer() at file "/home/smith/postgres/inst/master/share/extension/moddatetime.control" line 1	112
ERROR:  out of dynamic memory in GUC_yy_create_buffer() at file "/home/smith/postgres/inst/master/share/extension/intagg.control" line 1	139
ERROR:  could not load library "/home/smith/postgres/inst/master/lib/plpgsql.so": /home/smith/postgres/inst/master/lib/plpgsql.so: cannot allocate symbol search list: Cannot allocate memory	0
ERROR:  could not read time zone file "Default": Cannot allocate memory	0
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/dict_xsyn--1.0.control": Cannot allocate memory	14
ERROR:  out of dynamic memory in GUC_yy_create_buffer() at file "/home/smith/postgres/inst/master/share/extension/citext.control" line 1	132
ERROR:  cannot open ROLLBACK query as cursor	0
ERROR:  date/time field value out of range: 	0
ERROR:  could not find member 1(34794,34794) of opfamily 1976	0
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/pg_trgm.control": Cannot allocate memory	58
ERROR:  could not find member 1(34842,34842) of opfamily 1976	174
ERROR:  cannot query or manipulate replication origin when max_replication_slots = 0	0
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/pg_buffercache--1.1.control": Cannot allocate memory	20
ERROR:  could not find member 1(34620,34620) of opfamily 1976	0
ERROR:  cache lookup failed for index 16862	36
ERROR:  out of dynamic memory in GUC_yy_create_buffer() at file "/home/smith/postgres/inst/master/share/extension/btree_gin.control" line 1	133
ERROR:  out of dynamic memory in GUC_yy_create_buffer() at file "/home/smith/postgres/inst/master/share/extension/adminpack.control" line 1	113
ERROR:  could not open relation with OID 2619	0
ERROR:  DELETE is not allowed in a non-volatile function	30
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/btree_gist--1.1.control": Cannot allocate memory	28
ERROR:  cache lookup failed for function 184	0
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/pgcrypto.control": Cannot allocate memory	63
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/btree_gist.control": Cannot allocate memory	63
ERROR:  missing support function 1(34836,34836) in opfamily 1976	2027
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/pg_prewarm--1.0.control": Cannot allocate memory	28
ERROR:  must request at least 2 points	459
ERROR:  conflicting values for "D" field in formatting string	1
ERROR:  value for "i" in source string is out of range	136
ERROR:  cache lookup failed for index 826	82
ERROR:  cannot subtract inet values of different sizes	697
ERROR:  LIKE pattern must not end with escape character	1332
ERROR:  date out of range for timestamp	14
ERROR:  interval out of range	225
ERROR:  out of dynamic memory in GUC_yy_create_buffer() at file "/home/smith/postgres/inst/master/share/extension/lo.control" line 1	111
ERROR:  could not find member 1(35080,35080) of opfamily 1976	0
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/plpgsql.control": Cannot allocate memory	84
ERROR:  out of dynamic memory in GUC_yy_create_buffer() at file "/home/smith/postgres/inst/master/share/extension/pg_buffercache.control" line 1	142
ERROR:  regular expression failed: out of memory	126027
ERROR:  could not open extension control file "/home/smith/postgres/inst/master/share/extension/tablefunc.control": Cannot allocate memory	66
ERROR:  ltree_gist_in() not implemented	583
ERROR:  Only V4 key packets are supported	184
ERROR:  <datatype>key_in() not implemented	2044
ERROR:  function "poly_distance" not implemented	153
ERROR:  Length not in range	1459
ERROR:  bad seg representation	528
ERROR:  argument exceeds the maximum length of 255 bytes	684
ERROR:  could not get commit timestamp data	1998
ERROR:  bigint out of range	938
ERROR:  Not implemented	445
ERROR:  invalid salt	219
ERROR:  No encryption key found	3015
ERROR:  bad cube representation	365
ERROR:  _intbig_in() not implemented	660
ERROR:  cannot use "S" and "PL" together	118
ERROR:  record type has not been registered	34755
ERROR:  Illegal argument to function	4771
ERROR:  Unexpected packet in key data	1330
ERROR:  wrong limit, should be between 0 and 1	751
ERROR:  only superuser may access generic file functions	15558
ERROR:  function reset_tsearch() is no longer supported	157654
ERROR:  relation cannot be null	24122
ERROR:  cannot AND inet values of different sizes	254
ERROR:  operator does not exist: language_handler = language_handler	2578
ERROR:  invalid regular expression: nfa has too many states	14
ERROR:  Corrupt ascii-armor	6814
ERROR:  Wrong key or corrupt data	16359
ERROR:  pg_stat_statements must be loaded via shared_preload_libraries	809919
ERROR:  levenshtein argument exceeds maximum length of 255 characters	2449
ERROR:  connection not available	1416458
ERROR:  gen_salt: Unknown salt algorithm	15705
ERROR:  cannot display a value of type fdw_handler	395426
ERROR:  password is required	6894422
ERROR:  invalid fork name	1100
ERROR:  result is out of range	571
ERROR:  cannot take square root of a negative number	5202
ERROR:  new bit must be 0 or 1	2856
ERROR:  invalid regular expression: parentheses () not balanced	5370
ERROR:  a negative number raised to a non-integer power yields a complex result	1257
ERROR:  cannot convert NaN to bigint	1821
ERROR:  negative substring length not allowed	3938
ERROR:  canceling statement due to user request	512
ERROR:  expected a right parenthesis	413
ERROR:  cannot use "V" and decimal point together	2278
ERROR:  cache lookup failed for index 2619	65
ERROR:  pg_extension_config_dump() can only be called from an SQL script executed by CREATE EXTENSION	9
ERROR:  cannot OR inet values of different sizes	201
ERROR:  out of shared memory	107246
ERROR:  unrecognized privilege type: "	18
ERROR:  cannot convert NaN to smallint	1545
ERROR:  cannot convert abstime "invalid" to timestamp	484
ERROR:  operand, lower bound, and upper bound cannot be NaN	110
ERROR:  direct link of phone line to wall slot not permitted	633
ERROR:  permission denied for relation pg_largeobject	24141782
ERROR:  WAL level not sufficient for making an online backup	1124915
ERROR:  invalid format specification for an interval value	2798
ERROR:  multiple decimal points	4172
ERROR:  syntax error at or near "ARRAY"	94884888
ERROR:  could not identify a comparison function for type aclitem[]	129
ERROR:  invalid regular expression: regular expression is too complex	13960
ERROR:  date out of range	10
ERROR:  no binary output function available for type aclitem	132
ERROR:  cannot convert circle with radius zero to polygon	179
ERROR:  link of phone to hub does not make sense	208
ERROR:  function "dist_lb" not implemented	384
ERROR:  cannot delete from object using integer index	11173
ERROR:  value out of range: underflow	668
ERROR:  pg_event_trigger_table_rewrite_reason() can only be called in a table_rewrite event trigger function	17449014
ERROR:  open path cannot be converted to polygon	21247
ERROR:  first argument of json_populate_record must be a row type	3641
ERROR:  WAL level not sufficient for creating a restore point	1369
ERROR:  cannot take logarithm of a negative number	6601
ERROR:  value out of range: overflow	15212
ERROR:  operator does not exist: json = json	12217
ERROR:  integer out of range	7290
ERROR:  cannot take logarithm of zero	27396
ERROR:  replication slots can only be used if max_replication_slots > 0	12761
ERROR:  "char" out of range	9809
ERROR:  cannot use "PR" and "S"/"PL"/"MI"/"SG" together	505
ERROR:  slotlink from patchslot to wallslot not permitted	14532
ERROR:  backlink between two phone lines does not make sense	12692
ERROR:  operator does not exist: city_budget = city_budget	5601
ERROR:  cannot use "S" and "PL"/"MI"/"SG"/"PR" together	373
ERROR:  channel name too long	2186
ERROR:  operator does not exist: txid_snapshot = txid_snapshot	2629
ERROR:  cannot convert NaN to integer	1651
ERROR:  cannot concatenate incompatible arrays	3470
ERROR:  invalid number of arguments: object must be matched key value pairs	52025
ERROR:  range lower bound must be less than or equal to range upper bound	41189
ERROR:  function "path_center" not implemented	31844
ERROR:  update or delete on table "rule_and_refint_t2" violates foreign key constraint "rule_and_refint_t3_id3a_fkey1" on table "rule_and_refint_t3"	160763
ERROR:  cannot compare arrays of different element types	80840
ERROR:  invalid name syntax	131259
ERROR:  SET requires parameter name	45897
ERROR:  first argument of jsonb_populate_record must be a row type	4265
ERROR:  field position must be greater than zero	1312
ERROR:  IF.orion.eth0        does not exist	6807
ERROR:  smallint out of range	71064
ERROR:  lower bound cannot equal upper bound	1537
ERROR:  unexpected duplicate for tablespace 0, relfilenode 0	8140
ERROR:  numeric field overflow	53181
ERROR:  cursor "c1" already in use	3573648
ERROR:  only superusers can query or manipulate replication origins	4585538
ERROR:  lastval is not yet defined in this session	1037441
ERROR:  null character not permitted	7658
ERROR:  lower bound of FOR loop cannot be null	25162
ERROR:  invalid escape string	40673
ERROR:  channel name cannot be empty	15074
ERROR:  function can only be called when server is in binary upgrade mode	92627
ERROR:  operator does not exist: xml = xml	136478
ERROR:  expected a left parenthesis	28360
ERROR:  invalid regular expression: brackets [] not balanced	84446
ERROR:  relation "tt" does not exist	1172601
ERROR:  argument declared "anyrange" is not a range type but type anyrange	3125178
ERROR:  cannot use "S" and "MI" together	486
ERROR:  failed to find conversion function from unknown to text	66725
ERROR:  unsupported XML feature	1925892
ERROR:  input is out of range	71592
ERROR:  cannot perform UPDATE RETURNING on relation "rules_src"	250281
ERROR:  update or delete on table "rule_and_refint_t1" violates foreign key constraint "rule_and_refint_t3_id3a_fkey" on table "rule_and_refint_t3"	160502
ERROR:  cannot assign XIDs during a parallel operation	220577
ERROR:  upper bound of FOR loop cannot be null	92787
ERROR:  no manual manipulation of HSlot	644419
ERROR:  operator does not exist: polygon = polygon	143683
ERROR:  cannot accept a value of type language_handler	1270946
ERROR:  could not determine which collation to use for string comparison	25292
ERROR:  cannot get array length of a non-array	148148
ERROR:  could not find array type for data type anyarray	1691677
ERROR:  cannot cast type unknown to anyenum	11105952
ERROR:  syntax error at or near "USER"	100298545
ERROR:  stack depth limit exceeded	6444903
ERROR:  recovery is not in progress	2227175
ERROR:  invalid range bound flags	129726
ERROR:  update or delete on table "customer" violates foreign key constraint "credit_card_cid_fkey" on table "credit_card"	92953
ERROR:  relation "created_table" already exists	978230
ERROR:  argument declared "anyarray" is not an array but type anyarray	162448
ERROR:  constraint in ON CONFLICT clause has no associated index	9986341
ERROR:  could not determine polymorphic type because input has type "unknown"	9275870
ERROR:  function returning record called in context that cannot accept type record	29486
ERROR:  cannot cast type text to anyrange	2228291
ERROR:  canceling statement due to statement timeout	4717135
ERROR:  INSERT with ON CONFLICT clause cannot be used with table that has INSERT or UPDATE rules	3895956
ERROR:  control reached end of function without RETURN	259065
ERROR:  update or delete on table "clstr_tst_s" violates foreign key constraint "clstr_tst_con" on table "clstr_tst"	164574
ERROR:  range constructor flags argument must not be null	368660
ERROR:  operator does not exist: point = point	2010371
ERROR:  cannot cast type text to anyarray	827168
ERROR:  Patchfield "PF0_2" does not exist	98294
ERROR:  type matched to anyenum is not an enum type: anyenum	431783
ERROR:  invalid regular expression: quantifier operand invalid	316657
ERROR:  PL/pgSQL functions cannot return type language_handler	1302204
ERROR:  cannot cast type text to anyenum	709805
ERROR:  operator does not exist: void = void	725818
ERROR:  cannot use "S" twice	15207
ERROR:  COALESCE could not convert type unknown to anyenum	135976
ERROR:  end of function	1318410
ERROR:  division by zero	1462841
ERROR:  value overflows numeric format	26544
\.


--
-- Data for Name: known_re; Type: TABLE DATA; Schema: public; Owner: andreas
--

COPY known_re (re, match) FROM stdin;
^ERROR:  unrecognized format() type specifier.*	0
^ERROR:  invalid range flags	0
^ERROR:  malformed range literal: "	1
^ERROR:  stack depth limit exceeded.*"infinite_recurse"	0
^ERROR:  could not create file "pg_snapshots.*Cannot allocate memory	5585
^ERROR:  unrecognized conversion type specifier 	31
^ERROR:  update or delete on table "[^"]+" violates foreign key constraint	0
^ERROR:  cannot insert into column "[^"]+" of view	0
^ERROR:  attribute number \\d+ not found in view targetlist	0
^ERROR:  \\d+ is outside the valid range for parameter	1
^ERROR:  relation .*does not exists	0
^ERROR:  currval of sequence "[^"]+" is not yet defined in this session	0
^ERROR:  index [^o]+out of valid range, 0..127	2634
^ERROR:  Cannot use ".*": Badly formatted type	1
^ERROR:  no operand in tsquery: 	0
^ERROR:  value ".*" is out of range for ISBN type	0
^ERROR:  more than one operator named	1
^ERROR:  Cannot use ".*": No such cipher algorithm	123
^ERROR:  value .* is out of range for ISSN type	7
ERROR:  value .* is out of range for (EAN13|ISSN|ISMN|UPC) type	16
^ERROR:  Cannot use .*: No such hash algorithm	3963
^ERROR:  text search parser .* does not exist	598
^ERROR:  connection .* not available	58770
\\nCONTEXT:  SQL function "make_table" statement	107
^ERROR:  unrecognized format() type specifier "	0
^ERROR:  timestamp out of range: 	21
^ERROR:  invalid byte sequence for encoding	2100
^ERROR:  parameter ".*" requires	38
^ERROR:  index row requires \\d+ bytes, maximum size is \\d+	28
^ERROR:  could not open relation with OID (\\d+)	1674
^ERROR:  wrong position info in tsvector: "	40
^ERROR:  unrecognized objtype abbreviation:	3175
^ERROR:  .*is out of range for type double precision	533
^ERROR:  invalid cidr value: 	10
^ERROR:  unrecognized key word: 	2244
^ERROR:  unrecognized storage manager name	1481
^ERROR:  cross-database references are not implemented	445
^ERROR:  syntax error in tsvector: "	235
^ERROR:  ".*" not supported for input	11
^ERROR:  invalid memory alloc request size \\d+	834
^ERROR:  ".*" is not a sequence	290
^ERROR:  unrecognized object class: 	650
^ERROR:  interval field value out of range	133
^ERROR:  hour ".*" is invalid for the 12-hour clock	104
^ERROR:  block number \\d+ is out of range for relation "	324
^ERROR:  could not identify a hash function for type	317
^ERROR:  invalid regular expression: 	82
^ERROR:  type .* is not a domain	1178
^ERROR:  type \\d+ is not a range type	10
^ERROR:  invalid encoding name 	18594
^ERROR:  text search configuration.*does not exist	2467
^ERROR:  invalid parameter list format: 	1081
^ERROR:  date/time field value out of range: 	1957
^ERROR:  type ".*" does not exist	3037
^ERROR:  parameter "[^"]+" cannot be changed without restarting the server	155
^ERROR:  bit string too long for type bit varying	1868
^ERROR:  HSlot slotname	47
^ERROR:  -\\d+ is less than zero	1689
^ERROR:  missing support function .*in opfamily	2204
^ERROR:  server .* does not exist	7119
^ERROR:  "time with time zone" units ".*" not recognized	1857
^ERROR:  interval time zone 	2428
^ERROR:  improper (qualified|relation) name \\(too many dotted names\\): 	367
^ERROR:  ".*" is not a number	821
^ERROR:  requested character too large for encoding	5765
^ERROR:  invalid octet value in	5
^ERROR:  syntax error in tsquery: 	15577
^ERROR: .*is not a valid binary digit	500
^ERROR: .*is not a valid encoding name	14174
^ERROR:  ".* must be ahead of ".*	358
^ERROR:  cache lookup failed for text search	6776
^ERROR:  parameter ".*" cannot be changed	587
^ERROR:  language validation function	420
^ERROR:  "-?\\d+" is out of range for type real	28
^ERROR:  schema ".*" does not exist	17293
^ERROR:  permission denied to set parameter	20
^ERROR:  ".*" is not an index	185
^ERROR:  parameter "[^"]+" cannot be changed now	393
^ERROR:  value ".*" is out of range for type 	2248
^ERROR:  timestamp units 	5126
^ERROR:  text search dictionary .* does not exist	1876
^ERROR:  interval units 	7670
^ERROR:  foreign-data wrapper "	7019
^ERROR:  column ".*" of relation ".*" does not exist	3044
^ERROR:  language .*does not exist	7053
^ERROR:  collations are not supported by type "	21793
^ERROR:  unrecognized reset target: 	2466
^ERROR:  could not read symbolic link ".*": No such file or directory	18325
^ERROR:  string is not a valid identifier: 	928
^ERROR:  invalid non-zero objectSubId for object class 	6004
^ERROR:  unrecognized weight: 	4694
^ERROR:  cannot retrieve commit timestamp for transaction	550
^ERROR:  column .* is of type oidvector	5866
^ERROR:  unrecognized encoding: 	15641
^ERROR:  malformed array literal	37297
^ERROR:  invalid regexp option: 	6430
^ERROR:  invalid type name	1884
^ERROR:  date out of range: "	173
^ERROR:  date out of range: 	151
^ERROR:  value .* is out of range for type oid	1567
^ERROR:  syntax error at or near 	20085
^ERROR:  "time" units "	1425
\\nCONTEXT:  invalid type name	12055
^ERROR:  IFace slotname	18839
^ERROR:  index row size \\d+ exceeds maximum \\d+ for index	18673
^ERROR:  timestamp with time zone units	13004
^ERROR:  invalid destination encoding name	5107
^ERROR:  column .*type rewritetype	642524
^ERROR:  relation ".*" does not exist	14929351
^ERROR:  function .* does not exist	30082005
^ERROR:  COALESCE types.*cannot be matched	58010
^ERROR:  index "hash_	109370
^ERROR:  null value in column "[^"]+" violates not-null constraint	19655669
^ERROR:  operator does not exist	7940408
^ERROR:  .*is not a valid encoding code	15619
^ERROR:  slotlink between two phones does not make sense	21865
^ERROR:  time zone 	10411
^ERROR:  invalid size: ".*"	22844
^ERROR:  column .*city_budget	729506
^ERROR:  timestamp\\(	10000
^ERROR:  function .*is not unique	467178
^ERROR:  value too long for type character	757795
^ERROR:  cache lookup failed for type	53924
^ERROR:  invalid mask length: 	5420
^ERROR:  index -?[0-9]+ out of valid range	37745
^ERROR:  bit string length \\d+ does not match type bit	14177
^ERROR:  database .* does not exist	95378
^ERROR:  setseed parameter 	3876
^ERROR:  could not find member .* of opfamily 1976	347
^ERROR:  invalid source encoding name 	11901
^ERROR:  argument list must have even number of elements	35837
^ERROR:  large object.*does not exist	71514
^ERROR:  cache lookup failed for constraint	81240
^ERROR:  invalid INTERVAL typmod: 	116744
^ERROR:  encoding conversion from 	75763
^ERROR:  cache lookup failed for function	275933
^ERROR:  invalid large-object descriptor: 	452672
^ERROR:  invalid input syntax for	194910
^ERROR:  value for domain [^ ]+ violates check constraint	310434
^ERROR:  invalid flags for opening a large object: 	35094
^ERROR:  duplicate key value violates unique constraint	1423906
^ERROR:  operator.*int8alias	299008
^ERROR:  operator.*test_type	175146
^ERROR:  conflicting key value violates exclusion constraint	22408
^ERROR:  unrecognized interval typmod	6824
^ERROR:  could not find tuple for trigger	81451
^ERROR:.*type tab1	633417
^ERROR:  bit index -?\\d+ out of valid range	25428
^ERROR:  bit index \\d+ out of valid range	24978
^ERROR:  unrecognized privilege type: "	116861
^ERROR:  column .* is of type .*\\[\\].* but expression is of type .*\\[\\]	524658
^ERROR:  insert or update on table "[^"]+" violates foreign key constraint	968932
^ERROR:  unrecognized configuration parameter	106336
^ERROR:  must be superuser	3584940
^ERROR:  tablespace .*does not exist	99419
^ERROR:  slotname must begin with	331953
^ERROR:  system "[^"]+" does not exist	320818
\\nCONTEXT:  PL/pgSQL function tg_iface_biu	341352
^ERROR:  illegal backlink beginning with	199785
\\nCONTEXT:  PL/pgSQL function tg_backlink_set	210698
^ERROR:  date field value out of range: 	71515
^ERROR:  Patchfield "[^"]*" does not exist	299392
\\nCONTEXT:  PL/pgSQL function tg_pslot_biu	300084
^ERROR:  illegal slotlink beginning with	193846
\\nCONTEXT:  PL/pgSQL function tg_slotlink_set	226252
^ERROR:  time field value out of range: 	8650
^ERROR:  role .* does not exist	48516
^ERROR:  new row for relation .* violates check constraint	2700411
^ERROR:  cache lookup failed for index 	90902
^ERROR:  invalid value 	6695901
^ERROR:  column.*test_type	1255248
^ERROR:  Room .* does not exist	311569
^ERROR:  cannot accept a value of type 	5440848
^ERROR:  column.*int8alias	4917520
\\nCONTEXT:  PL/pgSQL.*	19075506
\.


--
-- Name: known_error_idx; Type: INDEX; Schema: public; Owner: andreas; Tablespace: 
--

CREATE INDEX known_error_idx ON known USING btree (error);


--
-- Name: known; Type: ACL; Schema: public; Owner: andreas
--

REVOKE ALL ON TABLE known FROM PUBLIC;
REVOKE ALL ON TABLE known FROM andreas;
GRANT ALL ON TABLE known TO andreas;
GRANT SELECT,UPDATE ON TABLE known TO smith;


--
-- Name: known_re; Type: ACL; Schema: public; Owner: andreas
--

REVOKE ALL ON TABLE known_re FROM PUBLIC;
REVOKE ALL ON TABLE known_re FROM andreas;
GRANT ALL ON TABLE known_re TO andreas;
GRANT SELECT,UPDATE ON TABLE known_re TO smith;


--
-- PostgreSQL database dump complete
--

