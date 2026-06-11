function main(Data)
   local MyDB = db.connect{api=db.SQLITE, name='tests/db/demo.db'}
   local R = MyDB:query{sql='SELECT * FROM Kin LIMIT 50;'}

   R.Eli😊
end   