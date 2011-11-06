:: generate_clientagent.cmd
:: 10/5/2011
:: Generate client side server
:: See: http://www.cnblogs.com/seamancode/archive/2011/07/29/wso2_wsf_cpp_hello.html
echo REALLY?
pause
call wsdl2cpp  -uri clientagent.wsdl -ss -sd -d adb -u
rm -Rf clientagent~
mv clientagent clientagent~
mkdir clientagent
dos2unix src/*
dos2unix resources/*
mv src resources clientagent

pause
