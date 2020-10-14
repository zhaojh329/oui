local M = {}

function M.get()
    local entries = {}
    local r, lines = pcall(io.lines, "/etc/crontabs/root")
    if r then
        for line in lines do
            local min, hour, day, month, week, cmd = line:match("(%S+) +(%S+) +(%S+) +(%S+) +(%S+) +(%S+)")
            entries[#entries + 1] = {
                min = min,
                hour = hour,
                day = day,
                month = month,
                week = week,
                command = cmd
            }
        end
    end
    return {entries = entries}
end

function M.set(params)
    if type(params.data) ~= "string" then error("invalid argument") end

    os.execute("mkdir -p /etc/crontabs")
    utils.writefile("/etc/crontabs/root", params.data)
end

return M
