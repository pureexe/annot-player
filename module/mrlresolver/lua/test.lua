--[[ luascript.lua
2/1/2011
luascript update 2/1/2011
TODO: Support batch mode.
]]

-- Debug --
DEBUG = true;
if DEBUG then
  dprint = print;
  require "dumper";
  function dump(var) dprint(DataDumper(var)) end
else
  function dprint() end;
  function dump() end;
end

-- Modules --

printf = print;
function print() end;
require "luascript/add";

function sShowMessage() end;

function dlFile(filesavepath, fileurl)
  dprint("fileurl:", fileurl);
  --dprint("filesavepath:", filesavepath);
  if string.find(fileurl, "bilibili.tv") ~= nil then
    --cmd = "wget -q \""..fileurl.."\" -O - | gunzip >\""..filesavepath.."\"";
    cmd = "curl -s \""..fileurl.."\"|gzip -d >\""..filesavepath.."\"";
  else
    --cmd = "wget -q \""..fileurl.."\" -O \""..filesavepath.."\"";
    cmd = "curl -s \""..fileurl.."\">\""..filesavepath.."\"";
  end
  os.execute(cmd);
  return 0;
end

function utf8_to_lua(utf8string)
  --dprint(utf8string)
  return utf8string
end

-- Entry --
--g_acfpv = nil;
g_title = nil;
g_ref = nil;
g_mrls = nil;
g_mrls_size = nil;
g_sub = nil;
function load(url, tmpfile)
  --local t = getTaskAttribute("http://www.bilibili.tv/video/av205838/", tmpfile, "acfun.tv");
  local t = getTaskAttribute(url, tmpfile, "acfun.tv");
  os.remove(tmpfile);
  if t == nil then
    return 1;
  end

  dump(t);
  local p = "0";
  --g_acfpv = t["acfpv"];
  g_title = t[p]["descriptor"];
  g_ref = t[p]["oriurl"];
  g_mrls = t[p]["realurls"];
  g_mrls_size = t[p]["realurlnum"];
  g_sub = t[p]["subxmlurl"]["0"];
  return 0;
end

-- EOF
--[[
-- Test --
function test()
  local tmpfile = "tmp";
  local tbl_re = getTaskAttribute("http://www.bilibili.tv/video/av205838/", tmpfile, "acfun.tv");
  --local tbl_re = getTaskAttributeBatch("http://www.bilibili.tv/video/av55775/", tmpfile, "acfun.tv");
  --local tbl_re = getTaskAttribute("http://v.youku.com/v_show/id_XMzQ3OTc4MTg4.html", tmpfile, "acfun.tv");
  os.remove(tmpfile);
  
  if tbl_re ~= nil then
    dump(tbl_re);
  end
end

if DEBUG then
  test();
end
]]
