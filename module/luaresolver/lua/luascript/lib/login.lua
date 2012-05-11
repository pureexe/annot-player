-- login.lua
-- 2011/12/8

require "lalib"
--require "loginconfig"

function Login_Bilibili( str_tmp_file )
	--local str_login_url = "https://secure.bilibili.us/member/index_do.php";
	local str_login_url = "https://secure.bilibili.tv/member/index_do.php";
	local post_str = string.format(
		--"fmdo=login&dopost=login&refurl=http%%3A%%2F%%2Fbilibili.tv%%2F&keeptime=604800&userid=%s&pwd=%s&keeptime=604800"
		"fmdo=login&dopost=login&gourl=&keeptime=604800&userid=%s&pwd=%s&keeptime=604800"
		,username["bilibili"],decrypt(password["bilibili"]));
	--dbgMessage(post_str);
	re = postdlFile("", str_login_url, post_str,
		"Content-type: application/x-www-form-urlencoded\r\nReferer: https://secure.bilibili.tv/login.php");

	--check login state
	if re~=0
	then
		if pDlg~=nil then
			sShowMessage(pDlg, '登录错误。');
		end
		dbgMessage("error login.");
		return FAILURE;
	else
		if pDlg~=nil then
			sShowMessage(pDlg, '已登录');
		end
	end

    return SUCCESS;

	--local file = io.open(str_tmp_file, "r");
	--if file==nil
	--then
	--	if pDlg~=nil then
	--		sShowMessage(pDlg, '登录bilibili网站出现错误');
	--	end
	--	return FAILURE;
	--end

	--local str_line = readUntilFromUTF8(file, "成功登录");

	--local login_state = FAILURE;
	--if str_line ~= 0 then
	--	login_state = SUCCESS;
	--	--dbgMessage("ok.");
	--else
	--	login_state = FAILURE;
	--end

	----readin  ok. close file
	--io.close(file);

	--return login_state;
end

function Login_Nico( str_tmp_file )
	local str_login_url = "https://secure.nicovideo.jp/secure/login?site=niconico";
	local post_str = string.format(
		"next_url=&mail=%s&password=%s"
		--,encodeUrl(username["nico"]),decrypt(password["nico"]));
		,username["nico"],decrypt(password["nico"]));
	--dbgMessage(post_str);
	re = postdlFile("", str_login_url, post_str,
		"Content-type: application/x-www-form-urlencoded\r\nReferer: https://secure.nicovideo.jp/secure/login_form");
	--dbgMessage("login ok.");

	--check login state
	if re~=0
	then
		if pDlg~=nil then
			sShowMessage(pDlg, '登录错误。');
		end
		dbgMessage("error login.");
		return FAILURE;
	else
		if pDlg~=nil then
			sShowMessage(pDlg, '已登录');
		end
	end

--[[	local file = io.open(str_tmp_file, "r");
	if file==nil
	then
		if pDlg~=nil then
			sShowMessage(pDlg, '登录bilibili网站出现错误');
		end
		return FAILURE;
	end

	local str_line = readUntilFromUTF8(file, "成功登录");]]

--	local login_state = SUCCESS;
--[[	if str_line ~= 0 then
		login_state = SUCCESS;
	else
		login_state = FAILURE;
	end

	--readin  ok. close file
	io.close(file);]]

	return SUCCESS;
end
