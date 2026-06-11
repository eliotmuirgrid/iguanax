function main(Data)
   local T = db.tables{vdb="tests/db/table.vdb"}
   T.Kin[1].GivenName = "Eliot"
   T.Kin[1].LastName = "Muir"
   T.Kin[1].Relation = "Friend"
   
   T.Kin[1].😊
end