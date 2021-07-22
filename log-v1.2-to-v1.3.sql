-- upgrade SQLsmith logging schema from 1.2 to 1.3

alter table error add column code char;

create view base_error as
    select id, firstline(msg) as error, query, code, t, errid from error;

comment on view base_error is 'like error, but truncate msg to first line';