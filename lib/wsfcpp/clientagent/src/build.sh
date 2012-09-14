
    g++ -g -shared -olibClientAgentService.so -I$WSFCPP_HOME/include -I. -I$WSFCPP_HOME/include/axis2-1.6.0 -I$WSFCPP_HOME/include/axis2-1.6.0/platforms -L$WSFCPP_HOME/lib \
        -laxutil \
        -laxis2_axiom \
        -laxis2_engine \
        -laxis2_parser \
        -lpthread \
        -laxis2_http_sender \
        -laxis2_http_receiver \
        -lguththila \
        -lwso2_wsf \
        *.cpp
    

    if ! test -d ClientAgentService; then
        mkdir ClientAgentService;
    fi

    cp libClientAgentService.so ClientAgentService;

    if test -e ./../resources/services.xml; then
    cp ./../resources/services.xml ClientAgentService;
    fi

    