local file = require 'eco.file'
local ffi = require 'ffi'

ffi.cdef([[
    typedef struct md5_ctx {
        uint32_t lo, hi;
        uint32_t a, b, c, d;
        unsigned char buffer[64];
    } md5_ctx_t;

    void md5_begin(md5_ctx_t *ctx);
    void md5_hash(const void *data, size_t length, md5_ctx_t *ctx);
    void md5_end(void *resbuf, md5_ctx_t *ctx);
]])

local M = {}

local md5_methods = {}

function md5_methods:hash(data)
    self.lib.md5_hash(data, #data, self.ctx);
end

function md5_methods:final()
    local buf = ffi.new('uint8_t [16]')
    self.lib.md5_end(buf, self.ctx)

    local md5 = {}

    for i = 0, 15 do
        md5[#md5 + 1] = string.format('%02x', buf[i])
    end

    return table.concat(md5)
end

function md5_methods:sum(data)
    self:hash(data)
    return self:final()
end

function M.md5()
    local lib

    for name in file.dir('/lib') do
        if name:match('libubox.so') then
            lib = ffi.load(name)
            break
        end
    end

    local ctx = ffi.new('md5_ctx_t')

    lib.md5_begin(ctx)

    return setmetatable({ lib = lib, ctx = ctx }, { __index = md5_methods })
end

return M
