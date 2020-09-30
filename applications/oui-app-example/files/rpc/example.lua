local M = {}

function M.echo(params)
	params.text = os.date('%Y-%m-%d %H:%M:%S') .. ": " .. params.text
	return params
end

return M