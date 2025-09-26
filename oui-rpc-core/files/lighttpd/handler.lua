local req_attr = lighty.r.req_attr
local resp_header = lighty.r.resp_header

local function handle_gzip()
    local accept_encoding = lighty.r.req_header['Accept-Encoding']

    if not accept_encoding or not accept_encoding:find('gzip') then
        return
    end

    local orig_path = req_attr['physical.path']
    local gzip_path = orig_path .. '.gz'

    if not lighty.c.stat(gzip_path) then
        return
    end

    req_attr['physical.path'] = gzip_path
    resp_header['Content-Encoding'] = 'gzip'

    local content_type

    if orig_path:match('%.css$') then
        content_type = 'text/css'
    elseif orig_path:match('%.js$') then
        content_type = 'application/javascript'
    end

    if content_type then
        resp_header['Content-Type'] = content_type
    end

    return true
end

if handle_gzip() then
    return
end
