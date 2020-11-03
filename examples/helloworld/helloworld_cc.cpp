#include "shim.h"
#include "logging.h"
#include <string>

#define OK "OK"
#define NOT_FOUND "Asset not found"
#define MAX_VALUE_SIZE 1024

std::string storeAsset(
        std::string asset_name,
        int value,
        shim_ctx_ptr_t ctx
        )
{
    LOG_DEBUG("HelloworldCC: +++ storeAsset +++");

    put_state(asset_name.c_str(), (uint8_t*)&value, sizeof(int), ctx);

    return OK;
}

std::string retrieveAsset(
        std::string asset_name,
        shim_ctx_ptr_t ctx
        )
{
    std::string result;
    LOG_DEBUG("HelloworldCC: +++ retrieveAsset +++");

    uint32_t asset_bytes_len = 0;
    uint8_t asset_bytes[MAX_VALUE_SIZE];
    get_state(asset_name.c_str(), asset_bytes, sizeof(asset_bytes), &asset_bytes_len, ctx);

    if (asset_bytes_len > 0)
    {
        result = asset_name + ":" + std::to_string((int)(*asset_bytes));
    }
    else
    {
        result = NOT_FOUND;
    }

    return result;
}

int invoke(
        uint8_t* response,
        uint32_t max_response_len,
        uint32_t* actual_response_len,
        shim_ctx_ptr_t ctx
        )
{
    std::string function_name;
    std::vector<std::string> params;

    get_func_and_params(function_name, params, ctx);

    std::string asset_name = params[0];
    std::string result;

    if (function_name == "storeAsset")
    {
        int value = std::stoi(params[1]);
        result = storeAsset(asset_name, value, ctx);
    }
    else if (function_name == "retrieveAsset")
    {
        result = retrieveAsset(asset_name, ctx);
    }
    else
    {
        // unknown function
        LOG_DEBUG("HelloworldCC: RECEIVED UNKNOWN transaction '%s'", function_name);
        return -1;
    }

    // resultのサイズが問題ないか確認
    int needed_size = result.size();
    if (max_response_len < needed_size)
    {
        // error: バッファが足りない
        LOG_DEBUG("HelloworldCC: Response buffer too small");
        *actual_response_len = 0;
        return -1;
    }

    // resultをresponseにコピー
    memcpy(response, result.c_str(), needed_size);
    *actual_response_len = needed_size;
    LOG_DEBUG("HelloworldCC: Response: %s", result.c_str());
    LOG_DEBUG("HelloworldCC: +++ Executing done +++");

    return 0;
}

