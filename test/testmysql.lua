package.path = package.path .. ";lualib/?.lua"
local mysql = require "mysql"

local db = mysql.connect("192.168.1.25","root","root","mysql",3306)
print ("db=",db)
local tbl = db:query("select * from user")
for k,v in pairs(tbl) do
    print(k,v.Host,v.User)
end

