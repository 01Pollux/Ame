file_utils = {}

function file_utils:path_from_root(path)
    return _script_root_dir .. "/" .. path
end

function file_utils:path_from_tmp(path)
    return _script_root_dir .. "/Tmp/" .. path
end
 
--This function finds the filename when given a complete path
function file_utils:get_file_name(path)
   local start, finish = path:find('[%w%s!-={-|]+[_%.].+')
   return path:sub(start,#path)
end

function file_utils:get_file_name_without_extension(path)
   local start, finish = path:find('[%w%s!-={-|]+[_%.].+')
   local file_name = path:sub(start,#path)
   local start, finish = file_name:find('[%w%s!-={-|]+[_%.].+')
   return file_name:sub(start,finish-4)
end

function file_utils:get_file_extension(path)
   local start, finish = path:find('[%w%s!-={-|]+[_%.].+')
   local file_name = path:sub(start,#path)
   local start, finish = file_name:find('[%w%s!-={-|]+[_%.].+')
   return file_name:sub(finish+1,#file_name)
end

function file_utils:get_file_path(path)
   local start, finish = path:find('[%w%s!-={-|]+[_%.].+')
   return path:sub(1,start-1)
end
