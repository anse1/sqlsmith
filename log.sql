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
