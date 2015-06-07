-- schema for --log-to

create table instance (
    id bigserial primary key,
    rev text,      -- sqlsmith git revision
    target text,   -- target connstr
    hostname text, -- hostname of sqlsmith instance

    -- not referenced by sqlsmith:
    t timestamptz default now(),
    client inet default inet_client_addr()
);

comment on table instance is 'details about an sqlsmith instance';

create table error (
    id bigint references instance(id),
    msg text,    -- error message
    query text,  -- failed query
    target text, -- conninfo of the target
    
    -- not referenced by sqlsmith:
    t timestamptz default now()
    errid serial primary key;
);

comment on table error is 'observed errors';

create table stat (
   id bigint references instance(id),
   generated bigint,    -- number of generated ASTs
   level float,         -- avg. height of ASTs
   nodes float,         -- avg. number of nodes in ASTs
   retries float,       -- avg. number of retries needed for ASTs
   updated timestamptz
);

comment on table stat is 'statistics about ASTs';

-- stuff beyond this line is not referenced by sqlsmith

create view base_error as
       select id, (regexp_split_to_array(msg,'\n'))[1] as error, query from error;

comment on view base_error is 'like error, but truncate msg to first line';

create view report as
       select count(1), error from base_error group by 2 order by count desc;

comment on view report is 'same report as sqlsmith''s verbose output';

create view instance_activity as
       select hostname, target, max(e.t)
       	      from instance i join error e on (i.id = e.id)
	      group by hostname, target
              order by max desc;

comment on view instance_activity is 'time of last error message from instance';

create view instance_speed as
    select hostname,
        generated/extract(epoch from (updated-t)) as "queries/s"
    from stat natural join instance
    where updated > now() - interval '1 minutes';

comment on view instance_speed is 'query speed of recently active instances';
