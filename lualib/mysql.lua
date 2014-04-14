package.cpath = package.cpath .. ";luaclib/?.so"
local driver = require "mysql.driver"

local mysql = {}
local db_meta = {
    __tostring = function (self)
        return "[mysql db : " .. self.name .. "]"
    end,
    __gc = function (self)
        self:close()
    end,
}

function mysql.connect (host,user,pass,dbname,dbport)
    local db = {
        conn = driver.connect(host,user,pass,dbname,dbport),
        name = dbname,
        close = function (db)
            return driver.disconnect(db.conn)
        end,
        query = function (db,sql)
            return driver.query(db.conn,sql)
        end,
        insert = function (db,sql)
            return driver.insert(db.conn,sql)
        end,
        update = function (db,sql)
            return driver.update(db.conn,sql)
        end,
        character_set = function (db,character_type)
            return driver.character_set(db.conn,character_type)
        end,
    }
    return setmetatable(db, db_meta)
end

return mysql

