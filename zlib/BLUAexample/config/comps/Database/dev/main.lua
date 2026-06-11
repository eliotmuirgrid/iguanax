
function testExecute(Conn)
   Conn:execute{sql="DROP TABLE foo"}
   Conn:execute{sql="DROP TABLE bar"}
   local TablesLeft = Conn:query{sql="SELECT name from sqlite_master where type='table'"}
   assert(#TablesLeft == 0)
end

function testMerge(Conn)
   local T = db.tables{vdb='schema.vdb'}
   T.foo[1].id = "1"
   T.foo[1].val = "qwerty"
   T.bar[1].col1 = "hello"
   T.bar[1].col2 = "world"

   Conn:merge{data=T}
   -- Do some selects to make sure the information was correctly populated
   local Q1 = Conn:query{sql="SELECT * FROM foo;"}
   assert(#Q1 == 1)
   assert(tostring(Q1[1].id) == '1')
   assert(tostring(Q1[1].val) == 'qwerty')

   local Q2 = Conn:query{sql="SELECT * FROM bar;"}
   assert(#Q2 == 1)
   assert(tostring(Q2[1].col1) == 'hello')
   assert(tostring(Q2[1].col2) == 'world')
end

function testConnInfo(Conn)
   local info = Conn:info()
   assert(info.api == 1013)
   assert(info.username == '')
   assert(info.password == '')
end

function main()
   local C = db.connect{api=db.SQLITE, name="test.db"}
   -- schema has 2 tables, foo: "foo"("id" TEXT, "val" TEXT); 
   -- and bar: "bar"("col1" TEXT, "col2" TEXT);
   C:execute{sql=[[CREATE TABLE "foo"("id" TEXT, "val" TEXT);]]}
   C:execute{sql=[[CREATE TABLE "bar"("col1" TEXT, "col2" TEXT);]]}
  -- testConnInfo(C)
   testMerge(C)
   testExecute(C)
   C:close()
end
