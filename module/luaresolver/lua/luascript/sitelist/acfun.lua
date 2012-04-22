
---[[by lostangel 20100528]]
---[[edit 20100827]]
---[[edit 20110117 adding parsing youku flv]]
---[[edit 20110222 editing xmlserver]]
---[[edit 20110402 editing xmllock and return struct]]
---[[edit 20110415 editing for seperate subid/ and acplayert.swf]]
---[[edit 20110507 for http://www.acfun.cn/html/zj/20100205/75086.html another mid= in wrong location ]]
---[[edit 20110527 for acfun_xml_servername value]]
---[[edit 20110705 for acfun.tv/v/acxxxxx]]
---[[edit 20110710 for acfun.tv/plus/view.php?aid=xxx&pageno=xxx]]
---[[edit 20110809 for cid= commentid equal to mid= messageid]]
---[[edit 20111102 for acfun new flashvar embed id]]
---[[edit 20111106 for new acfun sub]]

require "lalib"

-- jichi 2/4/2011: replace with permanent name
--acfun_xml_servername = '124.228.254.234';--'www.sjfan.com.cn';
acfun_xml_servername = "www.sjfan.com.cn";
acfun_comment_servername = 'comment.acfun.tv';--'122.224.11.162';--

--[[parse single acfun url]]
function getTaskAttribute_acfun ( str_url, str_tmpfile ,str_servername, pDlg, bSubOnly)
	if pDlg~=nil then
		sShowMessage(pDlg, '开始解析..');
	end
	local int_acfpv = getACFPV ( str_url, str_servername );

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
	local str_title_line = readIntoUntilFromUTF8(file, str_line, "</title>");
	local str_title = getMedText(str_title_line, "<title>", "</title>");

	--readin vice descriptor
	--readUntil(file, "主页</a>");
    readUntilFromUTF8(file, "</div><!--Tool -->");
	str_line = "";
	local str_tmp_vd = "";
	--while str_line~=nil and string.find(str_line, "</tr>")==nil
	while str_line~=nil and string.find(str_line, "</div>")==nil
	do
		str_line = file:read("*l");
		--if str_line~=nil and string.find(str_line, "<option value='")~=nil
        str_line = utf8_to_lua(str_line);
        if str_line~=nil and string.find(str_line, "<a class=\"")~=nil
		then
			--if str_tmp_vd=="" or string.find(str_line, "selected>")~=nil
            if str_tmp_vd=="" or string.find(str_line, "pager-active")~=nil
			then
			--str_tmp_vd = getMedText(str_line, ">", "</option>");
            str_tmp_vd = getMedText(str_line, "\">", "</a>");
			end
		end
	end
	--save descriptor
	local str_descriptor = "";
    -- jichi 2/3/2012: remove str_tmp_vd from title
	--if str_tmp_vd ~= ""
	--then
    --str_descriptor = str_title.."-"..str_tmp_vd;
	--else
		str_descriptor = str_title;
    --end

	--dbgMessage(str_title);
	--dbgMessage(str_tmp_vd);
	--dbgMessage(str_descriptor);

	--find embed flash
	--str_line = readUntil(file, "<embed ");
	--local str_embed = readIntoUntil(file, str_line, "</td>");--"</embed>");
    str_line = readUntilFromUTF8(file, "<embed ");
    local str_embed = readIntoUntilFromUTF8(file, str_line, "</div>");--"</td>");--"</embed>");
	print(str_embed);
	if str_embed==nil then
		if pDlg~=nil then
			sShowMessage(pDlg, "没有找到嵌入的flash播放器");
		end
		io.close(file);
		return;
	end


	--read foreign file
	local str_notsinaurl = "";
	if string.find(str_embed, "flashvars=\"file=", 1, true)~=nil
	then
		str_notsinaurl = getMedText2end(str_embed, "flashvars=\"file=", "\"", "&amp;");
	elseif string.find(str_embed, "playerf.swf?file=")~=nil
	then
		str_notsinaurl = getMedText2end(str_embed, "playerf.swf?file=", "\"", "&amp;");
	elseif string.find(str_embed, "file=")~=nil
	then
		str_notsinaurl = getMedText2end(str_embed, "file=", "\"", "&amp;");
	end

	--certain foreign sitelink
	local int_foreignlinksite = fls["realurl"];
	if str_notsinaurl=="" then
		if string.find(str_embed, "type2=qq", 1,true)~=nil then
			int_foreignlinksite = fls["qq"];
		elseif string.find(str_embed, "type2=youku", 1,true)~=nil then
			int_foreignlinksite = fls["youku"];
		else
			int_foreignlinksite = fls["sina"];
		end
	end

	--certain acfpv
	if string.find(str_embed, "flvplayer/acplayer.swf")~=nil or string.find(str_embed, "flvplayer/acplayert.swf")~=nil
	then
		int_acfpv = 0; -- ACFPV_ORI
	end

	--read id
	local str_id = "";
	if string.find(str_embed, "flashvars=\"id=")~=nil
	then
		str_id = getMedText2end(str_embed, "flashvars=\"id=", "\"", "&amp;");
	elseif string.find(str_embed, "flashvars=\"avid=")~=nil
	then
		str_id = getMedText2end(str_embed, "flashvars=\"avid=", "\"", "&amp;");
	elseif string.find(str_embed, "?id=")~=nil
	then
		str_id = getMedText2end(str_embed, "?id=", "\"", "&amp;");
	elseif string.find(str_embed, "id=")~=nil
	then
		str_embed_tmp = getAfterText(str_embed, "flashvars=");
        if str_embed_tmp==nil 
 		then
 			str_embed_tmp = getAfterText(str_embed, "src=");
 		end
		str_id = getMedText2end(str_embed_tmp, "id=", "\"", "&amp;");
	end

	local str_subid = str_id;
	--if there is a seperate subid
	if string.find(str_embed, "mid=")~= nil then
		str_subid = getMedText2end(str_embed, "mid=", "\"", "&amp;");
	elseif string.find(str_embed, "cid=")~= nil then
		str_subid = getMedText2end(str_embed, "cid=", "\"", "&amp;");
	end

	--read id ok.close file
	io.close(file);

	if str_id == ""
	then
		if pDlg~=nil then
			sShowMessage(pDlg, '解析FLV ID错误。');
		end
		return;
	end
	------------------------------------------------------------[[read flv id end]]


	--dbgMessage(str_id);

	--define subxmlurl
	local str_subxmlurl = "";
	local str_subxmlurl_lock = "";
	local str_subxmlurl_super = "";
	if int_acfpv==1 --ACFPV_NEW
	then
		str_subxmlurl = "http://"..acfun_xml_servername.."/newflvplayer/xmldata/"..str_subid.."/comment_on.xml?r=1";
		str_subxmlurl_lock = "http://"..acfun_xml_servername.."/newflvplayer/xmldata/"..str_subid.."/comment_lock.xml?r=1";
		str_subxmlurl_super = "http://"..acfun_xml_servername.."/newflvplayer/xmldata/"..str_subid.."/comment_super.xml";
		str_subxmlurl_20111106 = "http://"..acfun_comment_servername.."/".. str_subid ..".json?clientID=0.9264421034604311";
		str_subxmlurl_lock_20111106 = "http://"..acfun_comment_servername.."/".. str_subid .."_lock.json?clientID=0.4721592585556209";
	else --ACFPV_ORI
		str_subxmlurl = "http://"..acfun_xml_servername.."/flvplayer/xmldata/"..str_subid.."/comment_on.xml?a=1";
	end

    -- jichi 2/4/2012: Enforce JSON format
	str_subxmlurl = "http://comment.acfun.tv/".. str_subid ..".json?clientID=0.9264421034604311";

	--realurls
	local int_realurlnum = 0;
	local tbl_realurls = {};
    local tbl_durations = {};

    if not bSubOnly then
	  --if str_notsinaurl=="" -- is sina flv
	  if int_foreignlinksite == fls["sina"]
	  then
	  	--fetch dynamic url
	  	int_realurlnum, tbl_realurls, tbl_durations = getRealUrls(str_id, str_tmpfile, pDlg);
	  elseif int_foreignlinksite == fls["qq"]
	  then
	  	int_realurlnum, tbl_realurls = getRealUrls_QQ(str_id, str_tmpfile, pDlg);
	  elseif int_foreignlinksite == fls["youku"]
	  then
	  	int_realurlnum, tbl_realurls = getRealUrls_youku(str_id, str_tmpfile, pDlg);
	  else
	  	int_realurlnum = 1;
	  	tbl_realurls = {};
	  	tbl_realurls[string.format("%d",0)] = str_notsinaurl;
	  end
    end

	if pDlg~=nil then
		sShowMessage(pDlg, '完成解析..');
	end

	local tbl_subxmlurls={};
	tbl_subxmlurls["0"] = str_subxmlurl;
	if str_subxmlurl_lock ~= "" then
		tbl_subxmlurls["1"] = str_subxmlurl_lock;
		tbl_subxmlurls["2"] = str_subxmlurl_super;
		tbl_subxmlurls["3"] = str_subxmlurl_20111106;
		tbl_subxmlurls["4"] = str_subxmlurl_lock_20111106;
	end

	local _, _, str_acfid = string.find(str_url, "/([%d_]+).html");
	local str_acfid_subfix = nil;
	if str_acfid == nil
	then
		_,_,str_acfid = string.find(str_url, "/[a]?[c]?([%d_]+)/");
		_,_, str_acfid_subfix = string.find(str_url, "/index([%d_]*).html");
		if str_acfid_subfix ~= nil then
			str_acfid = str_acfid .. str_acfid_subfix;
		end
	end
	--for http://acfun.tv/plus/view.php?aid=xxxxxxx
	if str_acfid == nil
	then
		_,_,str_acfid = string.find(str_url, "/view.php%?aid=([%d_]+)");
		_,_,str_acfid_subfix = string.find(str_url, "pageno=([%d]+)");
		if str_acfid_subfix ~= nil then
			str_acfid = str_acfid .. str_acfid_subfix;
		end
	end

	local tbl_ta = {};
	tbl_ta["acfpv"] = int_acfpv;
    -- jichi 2/3/2012: remove acfid from title
	--tbl_ta["descriptor"] = "ac"..str_acfid.." - "..str_descriptor;
	tbl_ta["descriptor"] = str_descriptor;
	--tbl_ta["subxmlurl"] = str_subxmlurl;
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
function getTaskAttributeBatch_acfun ( str_url, str_tmpfile ,str_servername, pDlg)
	if pDlg~=nil then
		sShowMessage(pDlg, '开始解析..');
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
	--local str_line = readUntil(file, "<title>");
	--local str_title_line = readIntoUntil(file, str_line, "</title>");
	local str_line = readUntilFromUTF8(file, "<title>");
	local str_title_line = readIntoUntilFromUTF8(file, str_line, "</title>");
	local str_title = getMedText(str_title_line, "<title>", "</title>");

	--readin vice descriptor
	--readUntil(file, "主页</a>");
    readUntilFromUTF8(file, "</div><!--Tool -->");

	str_line = "";
	local tbl_descriptors = {};
	local tbl_shorturls = {};
	local index = 0;
	--while str_line~=nil and string.find(str_line, "</tr>")==nil
    while str_line~=nil and string.find(str_line, "</div>")==nil--"</tr>")==nil
	do
		str_line = file:read("*l");
		--if str_line~=nil and string.find(str_line, "<option value='")~=nil
        str_line = utf8_to_lua(str_line);
		if str_line~=nil and string.find(str_line, "<a class")~=nil
		then
			--local str_tmp_vd = getMedText(str_line, ">", "</option>");
			--local str_tmp_url = getMedText(str_line, "<option value='", "'");
            local str_tmp_vd = getMedText(str_line, "\">", "</a>");
			local str_tmp_url = getMedText(str_line, "href=\"", "\">");
			local str_index = string.format("%d", index);
			tbl_descriptors[str_index] = --[[str_title.."-"..]]str_tmp_vd;
			tbl_shorturls[str_index] = str_tmp_url;
			index = index+1;
		end
	end

	--readin pairs ok. close file
	io.close(file);

	if index==0 then
		local str_index = string.format("%d", index);
		tbl_descriptors[str_index] = str_title;
		local bg_t,ed_t = string.find(string.reverse(str_url),"/",1,true);
		ed_t = string.len(str_url)+1-ed_t;
		tbl_shorturls[str_index] = string.sub(str_url, ed_t+1) ;
		index = index+1;
	end

	--dbgMessage(string.format("%d", index));

	--------[[parse every url in pairs]]

	--local bg,ed = string.find(string.reverse(str_url),"/",1,true);
	--ed = string.len(str_url)+1-ed;
	--local urlprefix = string.sub(str_url, 1, ed);
	local urlprefix = "http://www.acfun.tv/v/"

	local tbl_re = {};
	local index2= 0;
	for ti = 0, index-1, 1 do
		local str_index = string.format("%d", ti);
		sShowMessage(pDlg, string.format("正在解析地址(%d/%d)\"%s\",请等待..",ti+1,index,tbl_shorturls[str_index]));
		for tj = 0, 5, 1 do
			local str_son_url = urlprefix..tbl_shorturls[str_index];
			--dbgMessage(str_son_url);
			local tbl_sig = getTaskAttribute_acfun(str_son_url, str_tmpfile, str_servername, nil);
			if tbl_sig~=nil then
				--dbgMessage("has video");
				--dbgMessage(tbl_sig["0"]["descriptor"]);
				--dbgMessage(tbl_descriptors[str_index]);
				--tbl_sig["0"]["descriptor"] = tbl_sig["0"]["descriptor"].." - "..tbl_descriptors[str_index];
				local str_index2 = string.format("%d",index2);
				tbl_re[str_index2] = deepcopy(tbl_sig)["0"];--dumpmytable(tbl_sig["0"]);--
				index2 = index2+1;
				break;
			else
				--dbgMessage("parse error");
			end
		end

	end

	sShowMessage(pDlg, string.format("解析完毕, 共有%d个视频",index2));

	return tbl_re;


end
