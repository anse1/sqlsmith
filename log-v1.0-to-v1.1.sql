-- upgrade SQLsmith logging schema from 1.0 to 1.1

alter table stat add column impedance jsonb;
alter table instance add column seed text;

create or replace view impedance as
 SELECT stat.id,
    stat.generated,
    stat.level,
    stat.nodes,
    stat.updated,
    js.prod,
    js.ok,
    js.bad,
    js.retries,
    js.limited,
    js.failed
   FROM stat,
    LATERAL jsonb_to_recordset(stat.impedance -> 'impedance')
    	    js(prod text,
	       ok integer,
	       bad integer,
	       retries integer,
	       limited integer,
	       failed integer)
  WHERE stat.impedance IS NOT NULL;

comment on view impedance is 'stat table with normalized jsonb';

create view impedance_report as
 SELECT instance.rev,
    impedance.prod,
    sum(impedance.generated) AS generated,
    sum(impedance.ok) AS ok,
    sum(impedance.bad) AS bad,
    sum(impedance.retries) AS retries,
    sum(impedance.limited) AS limited,
    sum(impedance.failed) AS failed
   FROM impedance
     JOIN instance USING (id)
  WHERE instance.rev = (( SELECT instance_1.rev
           FROM instance instance_1
          ORDER BY instance_1.t DESC
         LIMIT 1))
  GROUP BY instance.rev, impedance.prod
  ORDER BY sum(impedance.retries);

comment on view impedance_report is 'impedance report for latest revision';
