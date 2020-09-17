select p.name from people p where p.id in
(select d.person_id from directors d join movies m on m.id = d.movie_id join ratings r on r.movie_id = m.id where r.rating >= 9);