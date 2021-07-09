-- schema for --log-to

create table instance (
    id bigserial primary key,
    rev text,      -- sqlsmith git revision
    target text,   -- target connstr
    hostname text, -- hostname of sqlsmith instance
    version text,   -- target version()
    seed text,   -- RNG seed

    -- not referenced by sqlsmith:
    t timestamptz default now(),
    client inet default inet_client_addr(),
    port integer default inet_client_port()
);

comment on table instance is 'details about an sqlsmith instance';

create table error (
    id bigint references instance(id),
    msg text,    -- error message
    query text,  -- failed query
    target text, -- conninfo of the target
    sqlstate text, -- sqlstate of error
    
    -- not referenced by sqlsmith:
    t timestamptz default now(),
    errid bigserial primary key
);

comment on table error is 'observed errors';

create table stat (
   id bigint references instance(id),
   generated bigint,    -- number of generated ASTs
   level float,         -- avg. height of ASTs
   nodes float,         -- avg. number of nodes in ASTs
   retries float,       -- avg. number of retries needed for ASTs
   updated timestamptz,
   impedance jsonb      -- impedance report
);

comment on table stat is 'statistics about ASTs and productions';

-- grant role smith just enough rights to do the logging
create role smith login;
grant insert,select on table instance to smith;
grant insert on table error to smith;
grant update,insert,select on table stat to smith;
grant usage on all sequences in schema public to smith;

-- stuff beyond this line is not referenced by sqlsmith

create or replace function firstline(msg text) returns text as $$
    select split_part(msg, E'\n', 1);
$$ language sql immutable;

create view base_error as
       select id, firstline(msg) as error, query, t, errid from error;

comment on view base_error is 'like error, but truncate msg to first line';

drop view if exists report;
create view report as
       select count(1), max(t) as last_seen, error
       from base_error group by 3 order by count desc;


create or replace view state_report as
 SELECT count(1) AS count,
    sqlstate,
    min(substring(firstline(e.msg),1,80)) AS sample,
    array_agg(DISTINCT i.hostname) AS hosts
   FROM error e
     JOIN instance i ON i.id = e.id
  WHERE e.t > (now() - '24:00:00'::interval)
  GROUP BY sqlstate
  ORDER BY (count(1));

comment on view state_report is 'an sqlstate-grouped report';

comment on view report is 'same report as sqlsmith''s verbose output';

drop view if exists report24h;
create view report24h as
       select count(1), error, max(e.t) as last_seen
       from base_error e join instance i on (e.id = i.id)
       where i.t > now() - interval '1 days'
       	     group by 2 order by count desc;

create view instance_activity as
       select i.hostname, i.target, max(e.t)
       	      from instance i join error e on (i.id = e.id)
	      group by i.hostname, i.target
              order by max desc;

comment on view instance_activity is 'time of last error message from instance';

create view instance_speed as
    select hostname,
        generated/extract(epoch from (updated-t)) as "queries/s"
    from stat natural join instance
    where updated > now() - interval '1 minutes';

comment on view instance_speed is 'query speed of recently active instances';

-- Filtering boring errors

create table boring_sqlstates (sqlstate text primary key);
comment on table boring_sqlstates is 'sqlstates to reject';
grant select on boring_sqlstates to public;

create table known(error text);
comment on table known is 'error messages to reject';
\copy known from known.txt

create table known_re(re text);
comment on table known_re is 'regular expressions to match error messages to reject';
\copy known_re from known_re.txt

create or replace function discard_known() returns trigger as $$
begin
	if exists (select 1 from boring_sqlstates b where new.sqlstate = b.sqlstate)
	   or exists (select 1 from known where firstline(new.msg) = error)
	then
	   return NULL;
        end if;
	
	if new.msg ~ ANY (select re from known_re)
        then
	   return NULL;
	end if;
	return new;
end
$$ language plpgsql;

create trigger discard_known before insert on error
    for each row execute procedure discard_known();

-- YMMV.
create index on error(t);

-- Following views are used for debugging sqlsmith
create view impedance as
    select id, generated, level, nodes, updated,
    	   prod, ok, bad, js.retries, limited, failed
    from stat, jsonb_to_recordset(impedance->'impedance')
    	 js(prod text, ok bigint, bad bigint, retries bigint, limited bigint, failed bigint)
    where impedance is not null;

comment on view impedance is 'stat table with normalized jsonb';

create view impedance_report as
  select rev, prod,
  	 sum(generated) as generated, sum(ok) as ok,
	 sum(bad) as bad,
	 sum(retries) as retries,
	 sum(limited)as limited,
	 sum(failed) as failed
  from impedance natural join instance
  where rev = (select max(rev) from instance where version ~* 'postgres')
  group by rev, prod
  order by retries;

comment on view impedance_report is 'impedance report for latest revision';
