##### project requirements
1. `#include <bsd/string.h>` (linux only)
    - apt-get install libbsd-dev
    
##### tested platform
1. mac(arm64)
2. linux(86_64)

##### steps to run
1. ./compile.sh
2. ./run.sh

##### config file
1. `config/account-config.json` (simnow环境1)
    - ```json
        {
            "EnvName": "Simnow InTrading",
            "MdFrontAddr": [
                "tcp://180.168.146.187:10211",
                "tcp://180.168.146.187:10212",
                "tcp://218.202.237.33:10213"
            ],
            "TraderFrontAddr": [
                "tcp://180.168.146.187:10201",
                "tcp://180.168.146.187:10202",
                "tcp://218.202.237.33:10203"
            ],
            "BrokerID": "9999",
            "UserID": "YOUR_USER_ID",
            "InvestorID": "YOUR_INVESTOR_ID",
            "Password": "YOUR_PASSWD",
            "AppID": "simnow_client_test",
            "AuthCode": "0000000000000000",
            "LoginRemark": "CTP测试",
            "SystemInfoRemark": "ABCDE",
            "MdConDir": "./flow_md_mock",
            "TraderConDir": "./trader_md_mock"
        }
        ```
2. `config/account-config.json` (simnow环境2)
    - ```json    
        {
            "EnvName": "Simnow 7*24",
            "MdFrontAddr": [
                "tcp://180.168.146.187:10131"
            ],
            "TraderFrontAddr": [
                "tcp://180.168.146.187:10130"
            ],
            "UserID": "YOUR_USER_ID",
            "InvestorID": "YOUR_INVESTOR_ID",
            "Password": "YOUR_PASSWD",
            "Password": "Hyl162013@@",
            "AppID": "simnow_client_test",
            "AuthCode": "0000000000000000",
            "LoginRemark": "CTP测试",
            "SystemInfoRemark": "ABCDE",
            "MdConDir": "./flow_md_test",
            "TraderConDir": "./trader_md_test"
        }
    ```
