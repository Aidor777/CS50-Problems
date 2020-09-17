select p.name from people p where p.id in
(select distinct s.person_id from stars s join movies m on m.id = s.movie_id where m.year = 2004)
order by p.birth;