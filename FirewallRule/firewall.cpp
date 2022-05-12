#include <windows.h>
#include <stdio.h>
#include <netfw.h>

#pragma warning(disable: 4996) //解决GetVersionEx 过期问题

//写入防火墙，最低支持版本Windows Vista
void CreateFireWallPolicy(char* in_out,LPCTSTR ruleName, LPCTSTR appPath, LPCTSTR ruleDesc, LPCTSTR ruleGroup)
{
    HRESULT hrComInit = S_OK;
    HRESULT hr = S_OK;

    INetFwPolicy2* pNetFwPolicy2 = NULL;
    INetFwRules* pFwRules = NULL;
    INetFwRule* pFwRule = NULL;


    BSTR bstrRuleName = SysAllocString(ruleName);
    BSTR bstrAppName = SysAllocString(appPath);
    BSTR bstrRuleDescription = SysAllocString(ruleDesc);
    BSTR bstrRuleGroup = SysAllocString(ruleGroup);

    // Initialize COM.
    hrComInit = CoInitializeEx(
        0,
        COINIT_APARTMENTTHREADED
    );

    // Ignore RPC_E_CHANGED_MODE; this just means that COM has already been
    // initialized with a different mode. Since we don't care what the mode is,
    // we'll just use the existing mode.
    if (hrComInit != RPC_E_CHANGED_MODE)
    {
        if (FAILED(hrComInit))
        {
            printf("CoInitializeEx failed: 0x%08lx\n", hrComInit);
            goto Cleanup;
        }
    }

    // Retrieve INetFwPolicy2
    hr = CoCreateInstance(
        __uuidof(NetFwPolicy2),
        NULL,
        CLSCTX_INPROC_SERVER,
        __uuidof(INetFwPolicy2),
        (void**)&pNetFwPolicy2);

    if (FAILED(hr))
    {
        printf("CoCreateInstance for INetFwPolicy2 failed: 0x%08lx\n", hr);
        goto Cleanup;
    }

    // Retrieve INetFwRules
    hr = pNetFwPolicy2->get_Rules(&pFwRules);
    if (FAILED(hr))
    {
        printf("get_Rules failed: 0x%08lx\n", hr);
        goto Cleanup;
    }

    //see if existed
    hr = pFwRules->Item(bstrRuleName, &pFwRule);
    if (hr == S_OK)
    {
        pFwRules->Remove(bstrRuleName);
    }

    // Create a new Firewall Rule object.
    hr = CoCreateInstance(
        __uuidof(NetFwRule),
        NULL,
        CLSCTX_INPROC_SERVER,
        __uuidof(INetFwRule),
        (void**)&pFwRule);
    if (FAILED(hr))
    {
        printf("CoCreateInstance for Firewall Rule failed: 0x%08lx\n", hr);
        goto Cleanup;
    }

    // Populate the Firewall Rule object

    if (strcmp(in_out,"in") == 0) {
        pFwRule->put_Direction(NET_FW_RULE_DIR_IN);
    }
    else if (strcmp(in_out, "out") == 0){
        pFwRule->put_Direction(NET_FW_RULE_DIR_OUT);
    }
    pFwRule->put_Name(bstrRuleName);
    pFwRule->put_ApplicationName(bstrAppName);
    pFwRule->put_Action(NET_FW_ACTION_ALLOW);
    pFwRule->put_Enabled(VARIANT_TRUE);
    pFwRule->put_Description(bstrRuleDescription);
    pFwRule->put_Grouping(bstrRuleGroup);

    // Add the Firewall Rule
    hr = pFwRules->Add(pFwRule);
    if (FAILED(hr))
    {
        printf("Firewall Rule Add failed: 0x%08lx\n", hr);
        goto Cleanup;
    }
    else {
        printf("Firewall Rule Create Success.");
    }

Cleanup:

    // Free BSTR's
    SysFreeString(bstrRuleName);
    SysFreeString(bstrAppName);
    SysFreeString(bstrRuleDescription);
    SysFreeString(bstrRuleGroup);


    // Release the INetFwRule object
    if (pFwRule != NULL)
    {
        pFwRule->Release();
    }

    // Release the INetFwRules object
    if (pFwRules != NULL)
    {
        pFwRules->Release();
    }

    // Release the INetFwPolicy2 object
    if (pNetFwPolicy2 != NULL)
    {
        pNetFwPolicy2->Release();
    }

    // Uninitialize COM.
    if (SUCCEEDED(hrComInit))
    {
        CoUninitialize();
    }
}

void DeleteFireWallPolicy(LPCTSTR ruleName)
{
    HRESULT hrComInit = S_OK;
    HRESULT hr = S_OK;

    INetFwPolicy2* pNetFwPolicy2 = NULL;
    INetFwRules* pFwRules = NULL;
    INetFwRule* pFwRule = NULL;

    BSTR bstrRuleName = SysAllocString(ruleName);

    // Initialize COM.
    hrComInit = CoInitializeEx(
        0,
        COINIT_APARTMENTTHREADED
    );

    // Ignore RPC_E_CHANGED_MODE; this just means that COM has already been
    // initialized with a different mode. Since we don't care what the mode is,
    // we'll just use the existing mode.
    if (hrComInit != RPC_E_CHANGED_MODE)
    {
        if (FAILED(hrComInit))
        {
            printf("CoInitializeEx failed: 0x%08lx\n", hrComInit);
            goto Cleanup;
        }
    }

    // Retrieve INetFwPolicy2
    hr = CoCreateInstance(
        __uuidof(NetFwPolicy2),
        NULL,
        CLSCTX_INPROC_SERVER,
        __uuidof(INetFwPolicy2),
        (void**)&pNetFwPolicy2);

    if (FAILED(hr))
    {
        printf("CoCreateInstance for INetFwPolicy2 failed: 0x%08lx\n", hr);
        goto Cleanup;
    }

    // Retrieve INetFwRules
    hr = pNetFwPolicy2->get_Rules(&pFwRules);
    if (FAILED(hr))
    {
        printf("get_Rules failed: 0x%08lx\n", hr);
        goto Cleanup;
    }

    //see if existed
    hr = pFwRules->Item(bstrRuleName, &pFwRule);
    if (hr == S_OK)
    {
        
        pFwRules->Remove(bstrRuleName);
        printf("firewall policy rule -> [%ws] delete success.",ruleName);
    }
    else {
        printf("firewall policy rule -> [%ws] not exist", ruleName);
    }
Cleanup:

    // Free BSTR's
    SysFreeString(bstrRuleName);


    // Release the INetFwRule object
    if (pFwRule != NULL)
    {
        pFwRule->Release();
    }

    // Release the INetFwRules object
    if (pFwRules != NULL)
    {
        pFwRules->Release();
    }

    // Release the INetFwPolicy2 object
    if (pNetFwPolicy2 != NULL)
    {
        pNetFwPolicy2->Release();
    }

    // Uninitialize COM.
    if (SUCCEEDED(hrComInit))
    {
        CoUninitialize();
    }
}

void UpdateFireWallPolicy(LPCTSTR ruleName, int open, int deny)
{
    HRESULT hrComInit = S_OK;
    HRESULT hr = S_OK;

    INetFwPolicy2* pNetFwPolicy2 = NULL;
    INetFwRules* pFwRules = NULL;
    INetFwRule* pFwRule = NULL;

    BSTR bstrRuleName = SysAllocString(ruleName);

    // Initialize COM.
    hrComInit = CoInitializeEx(
        0,
        COINIT_APARTMENTTHREADED
    );

    // Ignore RPC_E_CHANGED_MODE; this just means that COM has already been
    // initialized with a different mode. Since we don't care what the mode is,
    // we'll just use the existing mode.
    if (hrComInit != RPC_E_CHANGED_MODE)
    {
        if (FAILED(hrComInit))
        {
            printf("CoInitializeEx failed: 0x%08lx\n", hrComInit);
            goto Cleanup;
        }
    }

    // Retrieve INetFwPolicy2
    hr = CoCreateInstance(
        __uuidof(NetFwPolicy2),
        NULL,
        CLSCTX_INPROC_SERVER,
        __uuidof(INetFwPolicy2),
        (void**)&pNetFwPolicy2);

    if (FAILED(hr))
    {
        printf("CoCreateInstance for INetFwPolicy2 failed: 0x%08lx\n", hr);
        goto Cleanup;
    }

    // Retrieve INetFwRules
    hr = pNetFwPolicy2->get_Rules(&pFwRules);
    if (FAILED(hr))
    {
        printf("get_Rules failed: 0x%08lx\n", hr);
        goto Cleanup;
    }

    //see if existed
    hr = pFwRules->Item(bstrRuleName, &pFwRule);
    if (hr == S_OK)
    {
        if (open != -1) {
            pFwRule->put_Enabled(open ? VARIANT_TRUE : VARIANT_FALSE);
            printf("The firewall policy set %s", open ? "opend" : "closed");
        }
        if (deny != -1) {
            pFwRule->put_Action(deny ? NET_FW_ACTION_BLOCK : NET_FW_ACTION_ALLOW);
            printf("The firewall policy set %s", deny ? "Block" : "Allow");
        }
    }
    else {
        printf("The firewall policy does not exist");
    }
Cleanup:

    // Free BSTR's
    SysFreeString(bstrRuleName);

    // Release the INetFwRule object
    if (pFwRule != NULL)
    {
        pFwRule->Release();
    }

    // Release the INetFwRules object
    if (pFwRules != NULL)
    {
        pFwRules->Release();
    }

    // Release the INetFwPolicy2 object
    if (pNetFwPolicy2 != NULL)
    {
        pNetFwPolicy2->Release();
    }

    // Uninitialize COM.
    if (SUCCEEDED(hrComInit))
    {
        CoUninitialize();
    }
}
