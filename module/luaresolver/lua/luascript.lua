--[[ luascript.lua
2/1/2011
luascript update 2/1/2011
TODO: Support batch mode.
]]

-- Debug --
--[[
DEBUG = false;
if DEBUG then
  dprint = print;
  require "dumper";
  function dump(var) dprint(DataDumper(var)) end
else
  function dprint() end;
  function dump() end;
end
]]
function dprint() end;
function dump() end;

-- Global variables required by ImageCreator --

username = {};
password = {};

-- Functions --

printf = print;
function print() end;
require "add";

function sShowMessage() end;

function utf8_to_lua(utf8string)
  --dprint(utf8string)
  return utf8string
end

function encrypt(s) return s end
function decrypt(s) return s end

-- See: http://stackoverflow.com/questions/4990990/lua-check-if-a-file-exists
function file_readable(name)
  local file = io.open(name,"r");
   if file~=nil then
     io.close(file);
     return true;
   else 
     return false;
   end
end
function file_size(name)
  local file = io.open(name,"r");
  local size = -1;
  if file~=nil then
    size = file:seek("end");
    io.close(file);
  end
  return size;
end

-- require clib.dlget(url, path)
function dlFile(filesavepath, fileurl)
  --return clib.dlget(fileurl, filesavepath);
  -- FIXME: return value not worked
  os.remove(filesavepath);
  clib.dlget(filesavepath, fileurl);
--[[
  if not string.match(fileurl, "tp://") then
    return -1;
  end
  dprint("fileurl:", fileurl);
  dprint("filesavepath:", filesavepath);
  if string.match(fileurl, "bilibili.tv") then
    cmd = "curl -s \""..fileurl.."\"|gzip -d >\""..filesavepath.."\"";
  else
    cmd = "curl -s \""..fileurl.."\">\""..filesavepath.."\"";
  end
  os.execute(cmd);
]]
  return 0;
end

-- require clib.dlpost(url, path)
function postdlFile(filesavepath, posturl, postdata, postheader)
  --return clib.dlget(fileurl, filesavepath);
  -- FIXME: return value not worked
  os.remove(filesavepath);
  clib.dlpost(filesavepath, posturl, postdata, postheader);

  return 0;
end

-- Entry --
g_debug = {};
g_siteid = 0;
g_title = nil;
g_refurl = nil;
g_mrls = nil;
g_mrls_size = nil;
g_sizes = nil;
g_durations = nil;
g_suburl = nil;
function resolve(url, tmpnam, bSubOnly)
  --local t = getTaskAttribute("http://www.bilibili.tv/video/av205838/", tmpnam, "acfun.tv", bSubOnly);
  local t = getTaskAttribute(url, tmpnam, "acfun.tv", bSubOnly);
  if t == nil then
    return 1;
  end

  dump(t);
  local p = "0";
  g_siteid = t[p]["acfpv"];
  g_title = t[p]["descriptor"];
  g_refurl = t[p]["oriurl"];
  if not bSubOnly then
    g_mrls = t[p]["realurls"];
    g_mrls_size = t[p]["realurlnum"];
    g_sizes = t[p]["sizes"];
    g_durations = t[p]["durations"];
  end
  if t[p]["subxmlurl"] ~= nil then
    g_suburl = t[p]["subxmlurl"]["0"];
  end
  --if not (g_suburl and string.match(g_suburl, "file://")) then
  --  os.remove(tmpnam);
  --end
  return 0;
end

function resolve_media(url, tmpnam)
  return resolve(url, tmpnam, false);
end

function resolve_subtitle(url, tmpnam)
  return resolve(url, tmpnam, true);
end

function set_account(entry, name, passwd)
  username[entry] = name;
  password[entry] = passwd;
end

g_nicovideo_login = false;
function set_nicovideo_account(name, passwd)
  set_account("nico", name, passwd);
  g_nicovideo_login = true;
end

g_bilibili_login = false;
function set_bilibili_account(name, passwd)
  set_account("bilibili", name, passwd);
  g_bilibili_login = true;
end
  
--[[
-- Test --
function test()
  local tmpnam = "tmp";
  local tbl_re = getTaskAttribute("http://www.bilibili.tv/video/av205838/", tmpnam, "acfun.tv");
  --local tbl_re = getTaskAttributeBatch("http://www.bilibili.tv/video/av55775/", tmpnam, "acfun.tv");
  --local tbl_re = getTaskAttribute("http://v.youku.com/v_show/id_XMzQ3OTc4MTg4.html", tmpnam, "acfun.tv");
  --os.remove(tmpnam);

  if tbl_re ~= nil then
    dump(tbl_re);
  end
end

if DEBUG then
  test();
end
]]

-- EOF
