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

-- Modules --

printf = print;
function print() end;
require "luascript/add";

function sShowMessage() end;

function utf8_to_lua(utf8string)
  --dprint(utf8string)
  return utf8string
end

-- curl(url, path)
function dlFile(filesavepath, fileurl)
  --return cc.curl(fileurl, filesavepath);
  -- FIXME: return value not worked
  os.remove(filesavepath);
  cc.curl(fileurl, filesavepath);
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

-- Entry --
--g_acfpv = nil;
g_title = nil;
g_refurl = nil;
g_mrls = nil;
g_mrls_size = nil;
g_suburl = nil;
function resolve_media(url, tmpnam)
  --local t = getTaskAttribute("http://www.bilibili.tv/video/av205838/", tmpnam, "acfun.tv");
  local t = getTaskAttribute(url, tmpnam, "acfun.tv");
  os.remove(tmpnam);
  if t == nil then
    return 1;
  end

  dump(t);
  local p = "0";
  --g_acfpv = t["acfpv"];
  g_title = t[p]["descriptor"];
  g_refurl = t[p]["oriurl"];
  g_mrls = t[p]["realurls"];
  g_mrls_size = t[p]["realurlnum"];
  if t[p]["subxmlurl"] ~= nil then
    g_suburl = t[p]["subxmlurl"]["0"];
  end
  return 0;
end

--[[
-- Test --
function test()
  local tmpnam = "tmp";
  local tbl_re = getTaskAttribute("http://www.bilibili.tv/video/av205838/", tmpnam, "acfun.tv");
  --local tbl_re = getTaskAttributeBatch("http://www.bilibili.tv/video/av55775/", tmpnam, "acfun.tv");
  --local tbl_re = getTaskAttribute("http://v.youku.com/v_show/id_XMzQ3OTc4MTg4.html", tmpnam, "acfun.tv");
  os.remove(tmpnam);

  if tbl_re ~= nil then
    dump(tbl_re);
  end
end

if DEBUG then
  test();
end
]]

-- EOF
