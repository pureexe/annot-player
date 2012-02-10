
---[[by lostangel 20100529]]
---[[edit 20101212]]
---[[edit 20110402]]
require "lalib"

--[[parse single acfun url]]
function getTaskAttribute_mikufans ( str_url, str_tmpfile , pDlg)
	if pDlg~=nil then
		sShowMessage(pDlg, '开始解析..');
	end
	local int_acfpv = getACFPV ( str_url );

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
	--dbgMessage(str_title);


	--find embed flash
	--str_line = readUntilFromUTF8(file, "<object type=\"application/x-shockwave-flash\" ");
	--str_line = readUntilFromUTF8(file, "swfobject.embedSWF");
	str_line = readUntilFromUTF8(file, "var params = {");
	local str_embed = readIntoUntilFromUTF8(file, str_line, "</script>"--[["</object>"]]);
	print(str_embed);
	if str_embed==nil then
		if pDlg~=nil then
			sShowMessage(pDlg, "没有找到嵌入的flash播放器");
		end
		io.close(file);
		return;
	end

	--dbgMessage(str_embed);

	--read foreign file
	local str_notsinaurl = "";
	--if string.find(str_embed, "flashvars=\"file=", 1, true)~=nil
	if string.find(str_embed, "flashvars: \"file=", 1, true)~=nil
	then
		--dbgMessage("1");
		--str_notsinaurl = getMedText2end(str_embed, "flashvars=\"file=", "\"", "&");
		str_notsinaurl = getMedText2end(str_embed, "flashvars: \"file=", "\"", "&");
	--[[elseif string.find(str_embed, "playerf.swf?file=", 1, true)~=nil
	then
		--dbgMessage("2");
		str_notsinaurl = getMedText2end(str_embed, "playerf.swf?file=", "\"", "&");
	elseif string.find(str_embed, "mukioplayer.swf?file=", 1, true)~=nil
	then
		--dbgMessage("3");
		str_notsinaurl = getMedText2end(str_embed, "mukioplayer.swf?file=", "\"", "&");
	elseif string.find(str_embed, ".swf?file=", 1, true)~=nil
	then
		--dbgMessage("3");
		str_notsinaurl = getMedText2end(str_embed, ".swf?file=", "\"", "&");
	]]
	end

	--dbgMessage(str_notsinaurl);


	--read id
	local str_id = "";
	if string.find(str_embed, "flashvars=\"id=", 1,true)~=nil
	then
		str_id = getMedText2end(str_embed, "flashvars=\"id=", "\"", "&");
	elseif string.find(str_embed, "flashvars=\"avid=", 1,true)~=nil
	then
		str_id = getMedText2end(str_embed, "flashvars=\"avid=", "\"", "&");
	elseif string.find(str_embed, "?id=", 1,true)~=nil
	then
		str_id = getMedText2end(str_embed, "?id=", "\"", "&");
	elseif string.find(str_embed, "id=", 1,true)~=nil
	then
		str_id = getMedText2end(str_embed, "id=", "\"", "&");
	end

	--dbgMessage(str_id);

	--read vice descriptor
	local str_descriptor = "";
	--dbgMessage("before readline");
	str_line = readUntilFromUTF8(file, "<div class='breaklist'>");
	--dbgMessage("after readuntil");
	str_line = readIntoUntilFromUTF8(file, str_line, "</div>");
	--dbgMessage("after readinto");
	if str_line~=nil then
		local str_tmp_des = getMedText(str_line, "<strong>", "</strong>");
		if str_tmp_des~=nil then
			str_descriptor = str_title.." - "..str_tmp_des;
		end
	else
		str_descriptor = str_title;
	end
	--dbgMessage("vice descriptor read ok.");

	--if multipart add subfix for descriptor
	local _, _, str_partnum = string.find(str_url, "%?partEX=(%d+)$");
	if str_partnum~=nil then
		str_descriptor = str_descriptor .. " - part"..str_partnum;
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

	--dbgMessage(str_id);
	------------------------------------------------------------[[read flv id end]]



	--define subxmlurl
	local str_subxmlurl = "";
	str_subxmlurl = "http://danmaku.us/newflvplayer/xmldata/"..str_id.."/Comment_on.xml";
	if string.find(str_url, "Bilibili", 1, true)~=nil
	then
		str_subxmlurl = "http://danmaku.us/pmfun/uploads/Bilibili2/"..str_id..".xml";
	end

	--realurls
	local int_realurlnum = 0;
	local tbl_realurls = {};
	if str_notsinaurl=="" -- is sina flv
	then
		--fetch dynamic url
		int_realurlnum, tbl_readurls = getRealUrls(str_id, str_tmpfile, pDlg);
	else
		int_realurlnum = 1;
		tbl_readurls = {};
		tbl_readurls[string.format("%d",0)] = str_notsinaurl;
	end

	if pDlg~=nil then
		sShowMessage(pDlg, '完成解析..');
	end

	local tbl_subxmlurls = {};
	tbl_subxmlurls["0"] = str_subxmlurl;

	local tbl_ta = {};
	tbl_ta["acfpv"] = int_acfpv;
	tbl_ta["descriptor"] = str_descriptor;
	tbl_ta["subxmlurl"] = tbl_subxmlurls;
	tbl_ta["realurlnum"] = int_realurlnum;
	tbl_ta["realurls"] = tbl_readurls;
	tbl_ta["oriurl"] = str_url;

	local tbl_resig = {};
	tbl_resig[string.format("%d",0)] = tbl_ta;
	return tbl_resig;
end

--[[parse every video in acfun url]]
function getTaskAttributeBatch_mikufans ( str_url, str_tmpfile , pDlg)
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
	local str_line = readUntilFromUTF8(file, "<title>");
	local str_title_line = readIntoUntilFromUTF8(file, str_line, "</title>");
	local str_title = getMedText(str_title_line, "<title>", "</title>");

	--readin vice descriptor
	str_line = readUntilFromUTF8(file, --[["<div class='breaklist'>"]]"<p>切换Part  :");
	str_line = readIntoUntilFromUTF8(file, str_line, --[["</div>"]]"</p>");
	--dbgMessage(str_line);
	local tbl_shorturls = {};
	local index = 0;
	if str_line~=nil then
		--local str_tmp_des = getMedText(str_line, "<strong>", "</strong>");
		--local str_index = string.format("%s", str_tmp_des);
		--index  = index+1;
		--tbl_shorturls[str_index] = str_url;
		--dbgMessage(tbl_shorturls[str_index]);
		--if str_tmp_des~=nil then
			local int_next_pos = 1;
			local str_sub_url = "";
			while true do
				str_sub_url, int_next_pos = getMedText(str_line, "<A class=wikilink href=\"", "\"", int_next_pos);
				--dbgMessage(str_sub_url);
				if str_sub_url==nil then
					break;
				end
				if int_next_pos~=nil then
					str_index, int_next_pos = getMedText(str_line, ">P", "</A>", int_next_pos);
					if str_index~=nil and str_sub_url~=nil then
						tbl_shorturls[str_index] = str_sub_url;
						--dbgMessage(tbl_shorturls[str_index]);
						index = index+1;
					end
				end
			end
		--end
	else
		str_descriptor = str_title;
	end

	--readin pairs ok. close file
	io.close(file);

	if index==0 then
		--dbgMessage("index=0");
		local str_index = string.format("%d", index+1);
		--dbgMessage(str_index);
		--tbl_descriptors[str_index] = str_title;
		local bg_t,ed_t = string.find(string.reverse(str_url),"/",1,true);
		ed_t = string.len(str_url)+1-ed_t;
		tbl_shorturls[str_index] = str_url;--string.sub(str_url, ed_t+1) ;
		index = index+1;
	end

	--------[[parse every url in pairs]]

	--local bg,ed = string.find(string.reverse(str_url),"/",1,true);
	--ed = string.len(str_url)+1-ed;
	--local urlprefix = string.sub(str_url, 1, ed);

	local tbl_re = {};
	local index2= 0;
	for ti = 1, index, 1 do
		local str_index = string.format("%d", ti);
		local tmpstr = string.format("%d/%d", ti, index);
		--dbgMessage(tmpstr);
		--dbgMessage(tbl_shorturls[str_index]);
		sShowMessage(pDlg, string.format("正在解析地址(%d/%d)\"%s\",请等待..",ti,index,tbl_shorturls[str_index]));
		for tj = 0, 5, 1 do
			local str_son_url = tbl_shorturls[str_index];
			local tbl_sig = getTaskAttribute_mikufans(str_son_url, str_tmpfile, str_servername, nil);
			if tbl_sig~=nil then
				--tbl_sig["0"]["descriptor"] = tbl_descriptors[str_index];
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
