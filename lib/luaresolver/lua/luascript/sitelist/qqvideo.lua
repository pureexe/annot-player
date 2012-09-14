
---[[by lostangel 20110402]]

require "lalib"

--[[parse single acfun url]]
function getTaskAttribute_qq ( str_url, str_tmpfile ,str_servername, pDlg)
	if pDlg~=nil then
		sShowMessage(pDlg, '开始解析..');
	end
	local int_acfpv = getACFPV ( str_url );

	-------[[read flv id start]]

	local _, _, str_id = string.find(str_url, "%?v=([^%.]+)$");--"/id_([^\.]+)\./");
	if str_id==nil then
		return;
	end
	local str_descriptor = str_id;

	local int_realurlnum, tbl_realurls = getRealUrls_QQ(str_id, str_tmpfile, pDlg);




	if pDlg~=nil then
		sShowMessage(pDlg, '完成解析..');
	end

	local tbl_subxmlurls = {};

	local tbl_ta = {};
	tbl_ta["acfpv"] = int_acfpv;
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

