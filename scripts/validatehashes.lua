local md5 = require("md5")
local lfs = require("lfs")

local root = arg[1] or "../HappyWars"
local validHashes = require("hashes")
local hashes = {}

local function iterdir(path, relpath)
    relpath = relpath or ""
    for file in lfs.dir(path) do
        if file ~= "." and file ~= ".." then
            local f = path .. "/" .. file
            local attr = lfs.attributes(f)
            if attr.mode == "directory" then
                iterdir(f, relpath .. "/" .. file)
            else
                local fd = io.open(f, "r")
                if fd ~= nil then
                    local content = fd:read("*a")
                    hashes[relpath .. "/" .. file] = md5.sumhexa(content)
                    fd:close()
                end
            end
        end
    end
end

iterdir(root)

local invalidCount = 0
local missingCount = 0

for path, hash in pairs(validHashes) do
    local ourHash = hashes[path]
    if ourHash == nil then
        print("Missing file " .. path)
        missingCount = missingCount + 1
    elseif ourHash ~= hash then
        print("Invalid hash for file " .. path)
        invalidCount = invalidCount + 1
    end
end

print(missingCount .. " missing file(s)")
print(invalidCount .. " invalid file(s)")
if invalidCount == 0 and missingCount == 0 then
    print("All files validated successfully.")
end
