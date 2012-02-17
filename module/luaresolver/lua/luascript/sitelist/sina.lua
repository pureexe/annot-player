
---[[by lostangel 20101117]]
---[[edit 20101117]]
--[[edit 20110314 for video tag]]
--[[edit lostangel 20110402 for subxml return struct]]

require "lalib"

--[[parse single sina url]]
function getTaskAttribute_sina ( str_url, str_tmpfile , pDlg)
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
	--str_line = readUntilFromUTF8(file, "var video = {");
	str_line = readUntilFromUTF8(file, "video : {");
	local str_embed = readIntoUntilFromUTF8(file, str_line, "</script>"--[["</object>"]]);
	print(str_embed);
	if str_embed==nil then
		if pDlg~=nil then
			sShowMessage(pDlg, "没有找到vid");
		end
		io.close(file);
		return;
	end

	--dbgMessage(str_embed);


	--read id
	local str_id = "";
	str_id = getMedText(str_embed, "vid :'", "',");

	--dbgMessage(str_id);


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
	--local str_subxmlurl = "";
	--str_subxmlurl = "http://danmaku.us/newflvplayer/xmldata/"..str_id.."/Comment_on.xml";

	--realurls
	local int_realurlnum = 0;
	local tbl_realurls = {};
	local tbl_durations = {};
	--fetch dynamic url
	int_realurlnum, tbl_readurls, tbl_durations = getRealUrls(str_id, str_tmpfile, pDlg);


	if pDlg~=nil then
		sShowMessage(pDlg, '完成解析..');
	end

	local tbl_subxmlurls={};

	local tbl_ta = {};
	tbl_ta["acfpv"] = int_acfpv;
	--tbl_ta["descriptor"] = "sina" .. str_id .. " - " .. str_title;
	tbl_ta["descriptor"] = str_title;
	tbl_ta["subxmlurl"] = tbl_subxmlurls;
	tbl_ta["realurlnum"] = int_realurlnum;
	tbl_ta["realurls"] = tbl_readurls;
	tbl_ta["durations"] = tbl_durations;
	tbl_ta["sizes"] = {};
	tbl_ta["oriurl"] = str_url;

	local tbl_resig = {};
	tbl_resig[string.format("%d",0)] = tbl_ta;
	return tbl_resig;
end

