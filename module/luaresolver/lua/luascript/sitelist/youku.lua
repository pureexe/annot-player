
---[[by lostangel 20110402]]
--[[edit lostangel 20110405 add batch code]]

require "lalib"

--[[parse single youku url]]
function getTaskAttribute_youku ( str_url, str_tmpfile ,str_servername, pDlg)
	if pDlg~=nil then
		sShowMessage(pDlg, '开始解析..');
	end
	local int_acfpv = getACFPV ( str_url );

	-------[[read flv id start]]

	local _, _, str_id = string.find(str_url, "/id_([^%.]+)%.");--"/id_([^\.]+)\./");


	--[[get descriptor]]
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
	local str_descriptor = str_title;
	--dbgMessage(str_descriptor);

	--readin id
	str_line = readUntilFromUTF8(file, "var videoId2");
	str_id = getMedText(str_line, "var videoId2= '", "';");

	io.close(file);
	--[[end get descriptor]]

	if str_id==nil then
		return;
	end

	local int_realurlnum, tbl_realurls = getRealUrls_youku(str_id, str_tmpfile, pDlg);

	if pDlg~=nil then
		sShowMessage(pDlg, '完成解析..');
	end

	local tbl_subxmlurls = {};

	local tbl_ta = {};
	tbl_ta["acfpv"] = int_acfpv;
	--tbl_ta["descriptor"] = "youku"..str_id.." - "..str_descriptor;
	tbl_ta["descriptor"] = str_descriptor;
	tbl_ta["subxmlurl"] = tbl_subxmlurls;--str_subxmlurl;
	tbl_ta["realurlnum"] = int_realurlnum;
	tbl_ta["realurls"] = tbl_realurls;
	tbl_ta["oriurl"] = str_url;
	--dbgMessage(tbl_realurls["0"]);
	--dbgMessage(tbl_ta["realurls"]["0"]);
	--dbgMessage(string.format("%d videos.", int_realurlnum));

	local tbl_resig = {};
	tbl_resig[string.format("%d",0)] = tbl_ta;
	return tbl_resig;
end





--[[parse every video in acfun url]]
function getTaskAttributeBatch_youku ( str_url, str_tmpfile , pDlg)
	if pDlg~=nil then
		sShowMessage(pDlg, '开始解析..');
	end

	local _, _, str_listid = string.find(str_url, "v_playlist/f(%d+)o");--http://v.youku.com/v_playlist/f5688358o1p1.html
	if str_listid==nil then
		_,_, str_listid = string.find(str_url, "playlist_show/id_(%d+)[%._]");--http://www.youku.com/playlist_show/id_5688358.html
	end

	if str_listid == nil then
		return;
	end

	local str_listurl = "http://www.youku.com/playlist_show/id_".. str_listid .. ".html";

	local re = dlFile(str_tmpfile, str_listurl);
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
	local str_line  = readUntilFromUTF8(file, "<li class=\"v_link\">");


	local tbl_descriptors = {};
	local tbl_shorturls = {};
	local index = 0;
	while str_line~=nil
	do
		local str_tmp_vd = getMedText(str_line, "title=\"", "\"");
		local str_tmp_url = getMedText(str_line, "href=\"", "\"");
		local str_index = string.format("%d", index);
		tbl_descriptors[str_index] = str_tmp_vd;
		tbl_shorturls[str_index] = str_tmp_url;
		index = index+1;

		str_line = readUntilFromUTF8(file, "<li class=\"v_link\">");
	end

	--readin pairs ok. close file
	io.close(file);


	--------[[parse every url in pairs]]


	local tbl_re = {};
	local index2= 0;
	for ti = 0, index-1, 1 do
		local str_index = string.format("%d", ti);
		sShowMessage(pDlg, string.format("正在解析地址(%d/%d)\"%s\",请等待..",ti+1,index,tbl_shorturls[str_index]));
		for tj = 0, 5, 1 do
			local str_son_url = tbl_shorturls[str_index];
			--dbgMessage(str_son_url);
			local tbl_sig = getTaskAttribute_youku(str_son_url, str_tmpfile, pDlg);
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

