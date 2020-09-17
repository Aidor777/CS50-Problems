select p.name from people p where p.id in
(select distinct p.id from people p join stars s on s.person_id = p.id where s.movie_id in
(select m.id from movies m join stars s on s.movie_id = m.id join people p on p.id = s.person_id where p.id in
(select p.id from people p where p.name = 'Kevin Bacon' and p.birth = 1958)))
and p.id not in (select p.id from people p where p.name = 'Kevin Bacon' and p.birth = 1958);