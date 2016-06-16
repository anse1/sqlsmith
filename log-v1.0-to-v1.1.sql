-- update SQLsmith logging schema from 1.0 to 1.1

alter table stat add column impedance jsonb;

create view impedance as
    select id, generated, level, nodes, updated, retries, prod, ok, bad
    from stat, jsonb_to_recordset(impedance->'impedance')
        js(prod text, ok int, bad int)
    where impedance is not null;

comment on view impedance is 'stat table with normalized jsonb';
