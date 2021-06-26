insert into student2 values (56789,'abcd',50);
select * from student2 where id=56789 and name="abcd";

select * from student2 where name = "name90000";

drop index autoIndex_2;
select * from student2 where name = "name90000";
create index stuindex on student2(name);

select * from student2 where score = 100;

select name score from student2 where id>1080199975 and id<=1080199999 and score>60 and score<90;

select * from student2 where  id>1080199975 and id<1080199980 and name <>'name99977';

delete from student2 where  id>1080199975 and id<1080199980 and name <>'name99977';

select * from student2 where  id>1080199975 and id<1080199980;

delete from student2;
select * from student2;
