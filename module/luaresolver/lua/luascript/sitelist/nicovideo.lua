
---[[by lostangel 20100918]]
--[[edit lostangel 20110216 add autologin]]
--[[edit lostangel 20110402 for subxml return struct]]

-- See: http://d.hatena.ne.jp/MineAP/20100819/1282201560

require "lalib"

local nico_sublice_num = 2000; -- change this to set how many comments you will download for one video.

--[[parse single nico url]]
function getTaskAttribute_nico ( str_url, str_tmpfile , pDlg)
  local IsAutoLogin;
  if g_nicovideo_login then
    IsAutoLogin = SUCCESS;
  else
    IsAutoLogin = FAILURE;
  end

  if pDlg~=nil then
    sShowMessage(pDlg, '开始解析..');
  end
  local int_acfpv = getACFPV ( str_url );

  --[[check login]]
  if IsAutoLogin == SUCCESS then
    local loginstate = Login_Nico ( str_tmpfile );
    if loginstate == FAILURE then
      return nil;
    end
  end

  -------[[read flv id start]]

  local _, _, str_id = string.find(str_url, "watch/([^%.]+)$"--[["watch/([sn]m[^%.]+)$"]]);--"/id_([^\.]+)\./");
  if str_id==nil then
    return;
  end

  local istwnico = 0;
  if string.find(str_url, "http://tw.nicovideo.jp", 1, true) ~= nil then
    istwnico = 1;
  end

  --get descriptor

  -- jichi 3/2/2012: cookies from original URL is essential, get twice
  -- tw.nicovideo. will be broken here. fuck taiwan!
  local nico_url = "http://www.nicovideo.jp/watch/"..str_id;
  local re;
  --re = dlFile(str_tmpfile, nico_url);

  --local nico_proxy_url = "http://nico.galstars.net/?t=www&u=%2Fwatch%2F"..str_id;
  --local nico_proxy_url = "http://nico.galstars.net/?t=www&u=/watch/"..str_id;
  --local nico_proxy_url = "http://sakuradite.com/nico/www/watch/"..str_id;
  local nico_proxy_url = "http://210.175.52.140/nico/www/watch/"..str_id;
  re = dlFile(str_tmpfile, nico_proxy_url); -- bypass banning
  --if re ~= 0 then
  --  re = dlFile(str_tmpfile, nico_url); -- redundant, in case galstars.net is down
  --end
  local forbidden = false;
  if re~=0
  then
    if pDlg~=nil then
      sShowMessage(pDlg, '读取原始页面错误。');
    end
    forbidden = true;
    --return;
  else
    if pDlg~=nil then
      sShowMessage(pDlg, '已读取原始页面，正在分析...');
    end
  end

  local file = io.open(str_tmpfile, "r");
  if file==nil
  then
    if pDlg~=nil then
      sShowMessage(pDlg, '读取原始页面错误。');
    end
    forbidden = true;
    --return;
  end

  --readin descriptor
  local str_line = "";
  local str_title_line = "";
  local str_title = "";
  local str_tw_vid = nil;

  local str_descriptor = "";

  if not forbidden then
    str_line = readUntilFromUTF8(file, "<title>");
    str_title_line = readIntoUntilFromUTF8(file, str_line, "</title>");
    str_title = getMedText(str_title_line, "<title>", "</title>");

    str_descriptor = str_title;

    --if is twnico, should readin vid
    local str_tw_vid = nil;
    if istwnico==1 then
      str_line = readUntilFromUTF8(file, "var Video");
      --dbgMessage(str_line);
      local str_vid_line = readIntoUntilFromUTF8(file, str_line, "v:");
      --dbgMessage(str_vid_line);
      str_tw_vid = getMedText(str_vid_line, "'", "'");
      --dbgMessage(str_tw_vid);
    end

    --get descriptor end.close file
    io.close(file);
  end



  if str_title == nil then
    re = dlFile(str_tmpfile, nico_url); -- bypass banning

    forbidden = false;
    file = io.open(str_tmpfile, "r");
    if file==nil
    then
      forbidden = true;
      --return;
    end

    if file ~= nil then
      str_line = readUntilFromUTF8(file, "<title>");
      str_title_line = readIntoUntilFromUTF8(file, str_line, "</title>");
      str_title = getMedText(str_title_line, "<title>", "</title>");

      str_descriptor = str_title;

      io.close(file);
    end
  end

  --parse url
  if istwnico==1 then
    if str_tw_vid == nil then
      if not forbidden then
        local file = io.open(str_tmpfile, "r");
        if file==nil
        then
          if pDlg~=nil then
            sShowMessage(pDlg, '读取原始页面错误。');
          end
          return;
        end
        str_line = readUntilFromUTF8(file, "請不要短時間內連續的進入本網站");
        dbgMessage(str_line);
        io.close(file);
      end
      return;
    end
    local str_getflv_url = "http://tw.nicovideo.jp/api/getflv?v="..str_tw_vid;
    re = dlFile(str_tmpfile, str_getflv_url);
  else
    local str_getflv_url = "http://flapi.nicovideo.jp/api/getflv";
    --local str_getflv_url = "http://nico.galstars.net/?t=&u=/api/getflv";
    --local str_post_data = string.format("v=%s&ts=%s&as=1", str_id, tostring(os.time()));
    local str_post_data = string.format("v=%s&as=1", str_id);
    re = postdlFile(str_tmpfile, str_getflv_url, str_post_data, "Referer: http\r\nContent-type: application/x-www-form-urlencoded");
    --dbgMessage("post ok.");
  end
  if re~=0
  then
    if pDlg~=nil then
      sShowMessage(pDlg, '读取跳转页面错误。');
    end
    return;
  else
    if pDlg~=nil then
      sShowMessage(pDlg, '已读取跳转页面，正在分析...');
    end
  end

  local file = io.open(str_tmpfile, "r");
  if file==nil
  then
    if pDlg~=nil then
      sShowMessage(pDlg, '读取跳转页面错误。');
    end
    return;
  end
  -- See: http://d.hatena.ne.jp/MineAP/20100819/1282201560
  str_line = file:read("*l");
  local str_threadid = getMedText(str_line, "thread_id=", "&");
  local str_length = getMedText(str_line, "l=", "&");
  local str_real_url = decodeUrl(getMedText(str_line, "url=", "&"));
  local str_sub_url = decodeUrl(getMedText(str_line, "ms=", "&"));
  local str_userid = getMedText(str_line, "user_id=", "&");
  --dbgMessage(str_line);
  local str_needskey = getMedText(str_line, "needs_key=", "&");
  if str_needskey == nil then
    str_needskey = "0";
  end

  --dbgMessage(str_real_url);
  --dbgMessage(str_sub_url);

  if str_threadid == nil or str_real_url == nil or str_sub_url == nil or str_userid == nil then
    io.close(file);
    return;
  end
  --dbgMessage(str_needskey);

  -- parse ok close file
  io.close(file);

  local str_threadkey = nil;
  local str_force_184 = nil;
  --getthreadkey
  if str_needskey == "1" then
    local str_getthreadkeyurl = "http://flapi.nicovideo.jp/api/getthreadkey?thread="..str_threadid;
    local re = dlFile(str_tmpfile, str_getthreadkeyurl);
    if re~=0
    then
      if pDlg~=nil then
        sShowMessage(pDlg, '读取原始页面错误。');
      end
      return;
    else
      if pDlg~=nil then
        sShowMessage(pDlg, '已读取原始页面，正在分析...');
      end
    end

    local file = io.open(str_tmpfile, "r");
    if file==nil
    then
      if pDlg~=nil then
        sShowMessage(pDlg, '读取原始页面错误。');
      end
      return;
    end

    local str_line_threadkey = file:read("*l");
    str_threadkey = getMedText(str_line_threadkey, "threadkey=", "&");
    str_force_184 = getMedText(str_line_threadkey, "force_184=", "&");
    if str_force_184==nil and string.find(str_line_threadkey, "force_184=", 1, true)~=nil
    then
      local int_force184index = string.find(str_line_threadkey, "force_184=", 1, true) + string.len("force_184=");
      str_force_184 = string.sub(str_line_threadkey, int_force184index, int_force184index+1);
    end

    io.close(file);
  end


  --dl sub xml
    -- See: http://blog.hitobashira.org/?eid=21
    -- See: http://d.hatena.ne.jp/gifnksm/?of=5
  str_getflv_url = str_sub_url;
  if str_needskey == "1" then
    str_post_data = string.format('<packet><thread thread="%s" version="20061206" res_from="-%d" user_id="%s" threadkey="%s" force_184="%s"/><thread thread="%s" version="20061206" res_from="-%d" threadkey="%s" force_184="%s" fork="1" click_revision="-1"/></packet>'
    , str_threadid, nico_sublice_num, str_userid, str_threadkey, str_force_184
    , str_threadid, nico_sublice_num, str_threadkey, str_force_184);
  else
    str_post_data = string.format('<packet><thread thread="%s" version="20061206" res_from="-%d" user_id="%s"/><thread thread="%s" version="20061206" res_from="-%d" fork="1" click_revision="-1"/></packet>'
    , str_threadid, nico_sublice_num, str_userid
    , str_threadid, nico_sublice_num);
  end
  --dbgMessage(str_post_data);
  re = postdlFile(str_tmpfile, str_getflv_url, str_post_data, "Content-type: text/xml");
  if re~=0
  then
    if pDlg~=nil then
      sShowMessage(pDlg, '读取字幕文件错误。');
    end
  else
    if pDlg~=nil then
      sShowMessage(pDlg, '已读取字幕文件');
    end
  end

  -- create realurls.
  local tbl_realurls = {};
  tbl_realurls["0"] = str_real_url;
  local int_realurlnum = 1;

  local tbl_durations = {};
  tbl_durations["0"] = str_length * 1000;


  --local int_realurlnum, tbl_realurls = getRealUrls_QQ(str_id, str_tmpfile, pDlg);


  if pDlg~=nil then
    sShowMessage(pDlg, '完成解析..');
  end

  local tbl_subxmlurls = {};

  tbl_subxmlurls["0"] = "file:///" .. str_tmpfile;

  local tbl_ta = {};
  tbl_ta["acfpv"] = int_acfpv;
  --tbl_ta["descriptor"] = str_id.." - "..str_descriptor;
  tbl_ta["descriptor"] = str_descriptor;
  tbl_ta["subxmlurl"] = tbl_subxmlurls;--str_subxmlurl;
  tbl_ta["realurlnum"] = int_realurlnum;
  tbl_ta["realurls"] = tbl_realurls;
  tbl_ta["sizes"] = {};
  tbl_ta["durations"] = tbl_durations;
  tbl_ta["oriurl"] = nico_url;
  --dbgMessage(tbl_realurls["0"]);
  --dbgMessage(tbl_ta["realurls"]["0"]);
  --dbgMessage(string.format("%d videos.", int_realurlnum));

  local tbl_resig = {};
  tbl_resig[string.format("%d",0)] = tbl_ta;
  return tbl_resig;
end

