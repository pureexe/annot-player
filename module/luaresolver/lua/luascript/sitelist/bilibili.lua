
---[[by lostangel 20100603]]
---[[edit 20110117 edit for bilibili embed tag changed]]
---[[add 20110119 bilibili login]]
---[[edit 20110215 bilibili subxml url changed]]
---[[edit 20110216 add parse for 6cn]]
---[[edit 20110402 bilibili subxml url changed]]
---[[edit 20110409 bilibili one line source html]]
---[[edit 20110414 bilibili for cannot extract multiple subpages.]]
---[[edit 20110415 bilibili for multiline vice descriptor]]
---[[edit 20110503 bilibili change ui]]
---[[edit 20110504 for batch one line source html]]
---[[edit 20110514 for bilibili inner video url parse]]
---[[edit 20110515 for login hint]]]
---[[edit 20110625 for bilibili.tv]]
---[[edit 20110731 for bilibili.tv batch parse for /avxxxxxx ori is /avxxxxxx/]]
---[[edit 20110815 for bilibili.tv rid (6cn) replace "=" for "/"]]
---[[edit 20110922 for suburl change to comment.bilibili.tv]]
---[[edit 20111206 for bilibili batch ]]
---[[edit 20111206 for bilibili no src html when batch ]]

require "lalib"
require "login"

--[[parse single bilibili url]]
function getTaskAttribute_bilibili ( str_url, str_tmpfile, pDlg, isNeedLogin, bSubOnly)
  local IsAutoLogin;
  if g_bilibili_login then
    IsAutoLogin = SUCCESS;
  else
    IsAutoLogin = FAILURE;
  end

  --dbgMessage("bilibili parse begin!");
  if pDlg~=nil then
    sShowMessage(pDlg, '开始解析..');
  end
  local int_acfpv = getACFPV ( str_url );

  if isNeedLogin == nil then
    isNeedLogin = true;
  end
  --[[check login]]
  if IsAutoLogin == SUCCESS and isNeedLogin==true then
    local loginstate = Login_Bilibili ( str_tmpfile );
    if loginstate == FAILURE then
      return nil;
    end
  end
  -------[[read flv id start]]
  local re = dlFile(str_tmpfile, str_url);
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

  --dbgMessage(pDlg, "dl ok");
  local file = io.open(str_tmpfile, "r");
  if file==nil
  then
    if pDlg~=nil then
      sShowMessage(pDlg, '读取原始页面错误。');
    end
    return;
  end

  --readin descriptor
  local str_line = readUntilFromUTF8(file, "<title>");
  local str_title_line = readIntoUntilFromUTF8(file, str_line, "</title>", str_line);
  local str_title = getMedText(str_title_line, "<title>", "</title>");

  --dbgMessage(str_title);

  if str_title == "信息提示." then
    dbgMessage("该视频需要您先使用IE登录bilibili，或开启使用acfunlocalizer的自动登录功能，否则可能出现错误。");
  end

  --readin vice descriptor
    
    -- FIXME jichi 2/1/2011:
    -- cus utf8_to_lua is not implemented, WCHAR matching is broken
  --readUntilFromUTF8(file, "主 页</a>", str_title_line);

  str_line = readUntilFromUTF8(file, "<div id=\"alist\"", str_title_line);
  --dbgMessage(str_line);
  str_line = getAfterText(str_line,"<div id=\"alist\"");
  local str_tmp_vd = "";
  local tbl_bilibili_vice_descriptor = {};
  local tbl_bilibili_vice_url = {};
  local vicedes_index = 1;
  while str_line~=nil and (string.find(str_line, "<option value='")~=nil or string.find(str_line, "</div>", 1, true)==nil)
  do

    --dbgMessage(str_line);
    --str_line = readIntoUntilFromUTF8(file, str_line, "<option value='", str_line);
    if str_line ~= nil and string.find(str_line, "<option value='", 1, true)==nil then
      local tmp_str_line = file:read("*l");
      str_line = utf8_to_lua(tmp_str_line);
    end
    --dbgMessage(str_line);
    if str_line~=nil and string.find(str_line, "<option value='", 1, true)~=nil
    then
      str_line = readIntoUntilFromUTF8(file, str_line, "</option>", str_line);
      local tmp_vdindex = string.format("%d", vicedes_index);
      --dbgMessage(str_line);
      tbl_bilibili_vice_url[tmp_vdindex] = getMedText(str_line, "<option value='", "'");
      tbl_bilibili_vice_descriptor[tmp_vdindex] = cutTags(getMedText(str_line, ">", "</option>"));
      --dbgMessage(tbl_bilibili_vice_descriptor[tmp_vdindex]);
      vicedes_index = vicedes_index+1;
      str_line = string.sub(str_line, string.find(str_line, "</option>")+string.len("</option>"));
    end
  end
  str_line = readUntilFromUTF8(file, "var pageno =", str_line);
  --dbgMessage(str_line);
  if str_line == nil then
    io.close(file);
    return;
  end
  str_line = "var pageno" .. getAfterText(str_line, "var pageno");
  --dbgMessage(str_line);
  if str_line~=nil and string.find(str_line, "var pageno", 1, true)~=nil
  then
    local str_page_index = getMedText(str_line, "'", "'");
    --dbgMessage(str_page_index);
    if str_page_index ~= nil then
      str_tmp_vd = tbl_bilibili_vice_descriptor[str_page_index];
      --dbgMessage(str_tmp_vd);
    end
  end
  --save descriptor
  local str_descriptor = "";

    -- jichi 2/2/2011: remove str_tmp_vd from title
  --if str_tmp_vd~=nil and str_tmp_vd ~= ""
  --then
  --  str_descriptor = str_title.."-"..str_tmp_vd;
  --else
    str_descriptor = str_title;
    --end
--dbgMessage(str_descriptor);
--dbgMessage(str_line);
  --find embed flash
  local str_embed_start = "<div class=\"scontent\" id=\"bofqi\">";
  --local str_embed_end = "</div><!-- /content -->";
  local str_embed_end = "</div>";
  str_line = readUntilFromUTF8(file, str_embed_start, str_line);--"<embed ");
  --str_line = readNextLineFromUTF8(file);
--dbgMessage(str_line);
  local str_embed = readIntoUntilFromUTF8(file, str_line, str_embed_end, str_line);--"</embed>");
  print(str_embed);
  --dbgMessage(str_embed);
  str_embed = getAfterText(str_embed, str_embed_start);
--dbgMessage(str_embed);
--dbgMessage(str_line);
  if str_embed==nil then
    if pDlg~=nil then
      sShowMessage(pDlg, "没有找到嵌入的flash播放器");
    end
    io.close(file);
    return;
  end

  --read foreign file
  local str_notsinaurl = "";
  if string.find(str_embed, "flashvars=\"file=")~=nil
  then
    str_notsinaurl = getMedText2end(str_embed, "flashvars=\"file=", "\"", "&amp;");
  elseif string.find(str_embed, "playerf.swf?file=")~=nil
  then
    str_notsinaurl = getMedText2end(str_embed, "playerf.swf?file=", "\"", "&amp;");
  elseif string.find(str_embed, "file=")~=nil
  then
    str_notsinaurl = getMedText2end(str_embed, "file=", "\"", "&amp;");
  end

--dbgMessage(str_embed);

  --read id
  local str_id = "";
  if string.find(str_embed, "flashvars=\"id=")~=nil
  then
    str_id = getMedText2end(str_embed, "flashvars=\"id=", "\"", "&amp;");
  elseif string.find(str_embed, "flashvars=\"avid=")~=nil
  then
    str_id = getMedText2end(str_embed, "flashvars=\"avid=", "\"", "&amp;");
  --elseif string.find(str_embed, "?id=")~=nil
  --then
  --  str_id = getMedText2end(str_embed, "?id=", "\"", "&amp;");
  elseif string.find(str_embed, "id=")~=nil
  then
    str_id = getMedText2end(str_embed, "id=", "\"", "&amp;");
  end

  --dbgMessage(str_id);




  if string.find(str_id, "levelup", 1, true)~=nil then
    --str_notsinaurl = "http://pl.bilibili.us/uploads/"..string.gsub(str_id, "levelup", "/")..".flv";
    str_notsinaurl = "http://pl.bilibili.tv/"..string.gsub(str_id, "levelup", "/")..".flv";
  end

  --certain foreign sitelink
  local int_foreignlinksite = fls["realurl"];
  if str_notsinaurl=="" then
    if string.find(str_embed, "ykid=", 1,true)~=nil then
      int_foreignlinksite = fls["youku"];
    elseif string.find(str_embed, "qid=", 1,true)~=nil then
      int_foreignlinksite = fls["qq"];
    elseif string.find(str_embed, "uid=", 1, true)~=nil then
      int_foreignlinksite = fls["tudou"];
    elseif string.find(str_embed, "rid=", 1, true)~=nil then
      int_foreignlinksite = fls["6cn"];
    else
      int_foreignlinksite = fls["sina"];
    end
  end


  --read id ok.close file
  io.close(file);

--dbgMessage(str_id);

  if str_id == ""
  then
    if pDlg~=nil then
      sShowMessage(pDlg, '解析FLV ID错误。');
    end
    return;
  end
  ------------------------------------------------------------[[read flv id end]]



  --define subxmlurl
  local str_subxmlurl = "";
  --str_subxmlurl = "http://d.bilibili.us/"..str_id.."?r=968";
  --str_subxmlurl = "http://www.bilibili.tv/dm," .. str_id .. "?r=126";
  str_subxmlurl = "http://comment.bilibili.tv/dm," .. str_id;

  --realurls
  local int_realurlnum = 0;
  local tbl_realurls = {};
  local tbl_durations = {};

  if not bSubOnly then
    if int_foreignlinksite == fls["sina"] -- is sina flv
    then
      --fetch dynamic url
      int_realurlnum, tbl_realurls, tbl_durations = getRealUrls(str_id, str_tmpfile, pDlg);
    elseif int_foreignlinksite == fls["youku"]
    then
      int_realurlnum, tbl_realurls = getRealUrls_youku(str_id, str_tmpfile, pDlg);
    elseif int_foreignlinksite == fls["qq"]
    then
      int_realurlnum, tbl_realurls = getRealUrls_QQ(str_id, str_tmpfile, pDlg);
    elseif int_foreignlinksite == fls["tudou"]
    then
      int_realurlnum, tbl_realurls = getRealUrls_tudou(str_id, str_tmpfile, pDlg);
    elseif int_foreignlinksite == fls["6cn"]
    then
      str_id_tmp = string.gsub(str_id, "=","/");
      int_realurlnum, tbl_realurls = getRealUrls_6cn(str_id_tmp, str_tmpfile, pDlg);
    else
      int_realurlnum = 1;
      tbl_realurls = {};
      tbl_realurls[string.format("%d",0)] = str_notsinaurl;
    end
  end

  if pDlg~=nil then
    sShowMessage(pDlg, '完成解析..');
  end

  local _, _, str_biliid = string.find(str_url, "video/av(%d+)");
  if str_biliid == nil then
    _, _, str_biliid = string.find(str_url, "plus/view.php%?aid=(%d+)");
  end


  local tbl_subxmlurls={};
  tbl_subxmlurls["0"] = str_subxmlurl;

  local tbl_ta = {};
  tbl_ta["acfpv"] = int_acfpv;
  --tbl_ta["descriptor"] = "bili"..str_biliid.." - "..str_descriptor;
    -- jichi 2/3/2011: remove prefix id
  tbl_ta["descriptor"] = str_descriptor;
  tbl_ta["subxmlurl"] = tbl_subxmlurls;
  tbl_ta["realurlnum"] = int_realurlnum;
  tbl_ta["realurls"] = tbl_realurls;
  tbl_ta["durations"] = tbl_durations;
  tbl_ta["sizes"] = {};
  tbl_ta["oriurl"] = str_url;

  local tbl_resig = {};
  tbl_resig[string.format("%d",0)] = tbl_ta;
  return tbl_resig;
end

--[[parse every video in acfun url]]
function getTaskAttributeBatch_bilibili ( str_url, str_tmpfile , pDlg)
  local IsAutoLogin;
  if g_bilibili_login then
    IsAutoLogin = SUCCESS;
  else
    IsAutoLogin = FAILURE;
  end

  if pDlg~=nil then
    sShowMessage(pDlg, '开始解析..');
  end

  --[[check login]]
  if IsAutoLogin == SUCCESS  then
    local loginstate = Login_Bilibili ( str_tmpfile );
    if loginstate == FAILURE then
      return;
    end
  end

  local re = dlFile(str_tmpfile, str_url);
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

  --readin descriptor
  local str_line = readUntilFromUTF8(file, "<title>", str_line);
  local str_title_line = readIntoUntilFromUTF8(file, str_line, "</title>", str_line);
  local str_title = getMedText(str_title_line, "<title>", "</title>");

  --readin vice descriptor
    -- FIXME jichi 2/1/2011:
    -- cus utf8_to_lua is not implemented, WCHAR matching is broken
  --readUntilFromUTF8(file, "主 页</a>", str_title_line);
  --str_line = readUntilFromUTF8(file, "主 页</a>", str_line);
  --dbgMessage(str_line);
  str_line = readUntilFromUTF8(file, "<div id=\"alist\"", str_line);
  --dbgMessage(str_line);

  str_line = getAfterText(str_line,"<div id=\"alist\"");
  --str_line = "";
  local str_tmp_vd = "";
  local tbl_descriptors = {};
  local tbl_shorturls = {};
  local index = 0;
  --dbgMessage(str_line);
  while str_line~=nil and (string.find(str_line, "<option value='")~=nil or string.find(str_line, "</div>")==nil)
  do

    if str_line~=nil and string.find(str_line, "<option value='")==nil then
      local tmp_str_line = file:read("*l");
      str_line = utf8_to_lua(tmp_str_line);
    end

    --dbgMessage(str_line);
    if str_line~=nil and string.find(str_line, "<option value='")~=nil
    then
      str_line = readIntoUntilFromUTF8(file, str_line, "</option>", str_line);
      local tmp_vdindex = string.format("%d", index);
      tbl_shorturls[tmp_vdindex] = getMedText(str_line, "<option value='", "'");

      if tbl_shorturls[tmp_vdindex]~=nil and string.find(tbl_shorturls[tmp_vdindex], "/", 1,true) ~= nil then
        local bg_t,ed_t = string.find(string.reverse(tbl_shorturls[tmp_vdindex]),"/",1,true);
        ed_t = string.len(tbl_shorturls[tmp_vdindex])+1-ed_t;
        tbl_shorturls[tmp_vdindex] = string.sub(tbl_shorturls[tmp_vdindex], ed_t+1) ;
      end

      tbl_descriptors[tmp_vdindex] = getMedText(str_line, ">", "</option>");
      index = index+1;
      str_line = string.sub(str_line, string.find(str_line, "</option>") + string.len("</optio"));
    end
  end

  --local tmpstr = string.format("index = %d", index);
  --dbgMessage(tmpstr);


  --readin pairs ok. close file
  io.close(file);

  --dbgMessage(tbl_shorturls[str_index]);
  --dbgMessage(str_url);
  if index==0 then
  --if string.find(str_url, "/", 1,true)~=nil then
    local str_index = string.format("%d", index);
    tbl_descriptors[str_index] = str_title;
    local bg_t,ed_t = string.find(string.reverse(str_url),"/",1,true);
    ed_t = string.len(str_url)+1-ed_t;
    tbl_shorturls[str_index] = string.sub(str_url, ed_t+1) ;
    --dbgMessage(tbl_shorturls[str_index]);
    --dbgMessage(str_url);
    if string.find(tbl_shorturls[str_index], "index",1,true) == nil then
      tbl_shorturls[str_index] = "";
    end
    index = index+1;
  end


  --------[[parse every url in pairs]]

  --local bg,ed = string.find(string.reverse(str_url),"/",1,true);
  --ed = string.len(str_url)+1-ed;
  --local urlprefix = string.sub(str_url, 1, ed);
  local _, _, str_biliid = string.find(str_url, "video/av(%d+)");
  local urlprefix = nil;
  if str_biliid == nil then
    _, _, str_biliid = string.find(str_url, "plus/view.php%?aid=(%d+)");
    --urlprefix = "http://bilibili.us";
  --else
    --urlprefix = "http://bilibili.us/video/av"..str_biliid.."/";
  end
  urlprefix = "http://www.bilibili.tv/video/av"..str_biliid.."/"

  --dbgMessage(urlprefix);

  local tbl_re = {};
  local index2= 0;
  for ti = 0, index-1, 1 do
    local str_index = string.format("%d", ti);
    --dbgMessage(str_index);
    --dbgMessage(tbl_shorturls[str_index]);
    sShowMessage(pDlg, string.format("正在解析地址(%d/%d)\"%s\",请等待..",ti+1,index,tbl_shorturls[str_index]));
    for tj = 0, 5, 1 do
      local str_son_url = urlprefix..tbl_shorturls[str_index];
      --dbgMessage(str_son_url);
      local tbl_sig = getTaskAttribute_bilibili(str_son_url, str_tmpfile, str_servername, false);
      if tbl_sig~=nil then
        --tbl_sig["0"]["descriptor"] = --[[str_title]]tbl_sig["0"]["descriptor"].."-"..tbl_descriptors[str_index];
        local str_index2 = string.format("%d",index2);
        tbl_re[str_index2] = deepcopy(tbl_sig)["0"];--dumpmytable(tbl_sig["0"]);--
        index2 = index2+1;
        break;
      end
    end

  end

  sShowMessage(pDlg, string.format("解析完毕, 共有%d个视频",index2));

  return tbl_re;


end
