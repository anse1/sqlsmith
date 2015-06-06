-- schema for --log-to

create table error (
    rev text,    -- revision of sqlsmith
    msg text,    -- error message
    query text,  -- failed query
    target text, -- conninfo of the target
    
    -- not referenced by sqlsmith:
    client inet default inet_client_addr(),
    t timestamptz default now()
);

create view base_error as
       select rev, (regexp_split_to_array(msg,'\n'))[1] as error, query from error;

create view report as
       select count(1), error from base_error group by 2 order by count desc;
