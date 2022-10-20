/**
 * This file is generated by jsonrpcstub, DO NOT CHANGE IT MANUALLY!
 */

#ifndef JSONRPC_CPP_STUB_STUBCLIENT_H_
#define JSONRPC_CPP_STUB_STUBCLIENT_H_

// #define throw(...)

#include <jsonrpccpp/client.h>

class StubClient : public jsonrpc::Client
{
public:
    StubClient(jsonrpc::IClientConnector& conn,
               jsonrpc::clientVersion_t type = jsonrpc::JSONRPC_CLIENT_V2)
        : jsonrpc::Client(conn, type)
    {
    }

    void navigateTo(const std::string& param1) throw(jsonrpc::JsonRpcException)
    {
        Json::Value p;
        p.append(param1);
        this->CallNotification("navigateTo", p);
    }
    void refresh() throw(jsonrpc::JsonRpcException)
    {
        Json::Value p;
        p = Json::nullValue;
        this->CallNotification("refresh", p);
    }
    void fullscreen() throw(jsonrpc::JsonRpcException)
    {
        Json::Value p;
        p = Json::nullValue;
        this->CallNotification("fullscreen", p);
    }
    void maximize() throw(jsonrpc::JsonRpcException)
    {
        Json::Value p;
        p = Json::nullValue;
        this->CallNotification("maximize", p);
    }
    void minimize() throw(jsonrpc::JsonRpcException)
    {
        Json::Value p;
        p = Json::nullValue;
        this->CallNotification("minimize", p);
    }
    void closeWindow() throw(jsonrpc::JsonRpcException)
    {
        Json::Value p;
        p = Json::nullValue;
        this->CallNotification("closeWindow", p);
    }
    void
    executeScript(const std::string& param1) throw(jsonrpc::JsonRpcException)
    {
        Json::Value p;
        p.append(param1);
        this->CallNotification("executeScript", p);
    }
    std::string
    is_on(const std::string& param1) throw(jsonrpc::JsonRpcException)
    {
        Json::Value p;
        p.append(param1);
        Json::Value result = this->CallMethod("is_on", p);
        if (result.isString())
            return result.asString();
        else
            throw jsonrpc::JsonRpcException(
                jsonrpc::Errors::ERROR_CLIENT_INVALID_RESPONSE,
                result.toStyledString());
    }
    std::string turned_on_off(const std::string& param1,
                              bool param2) throw(jsonrpc::JsonRpcException)
    {
        Json::Value p;
        p.append(param1);
        p.append(param2);
        Json::Value result = this->CallMethod("turned_on_off", p);
        if (result.isString())
            return result.asString();
        else
            throw jsonrpc::JsonRpcException(
                jsonrpc::Errors::ERROR_CLIENT_INVALID_RESPONSE,
                result.toStyledString());
    }
    std::string
    get_brightness(const std::string& param1) throw(jsonrpc::JsonRpcException)
    {
        Json::Value p;
        p.append(param1);
        Json::Value result = this->CallMethod("get_brightness", p);
        if (result.isString())
            return result.asString();
        else
            throw jsonrpc::JsonRpcException(
                jsonrpc::Errors::ERROR_CLIENT_INVALID_RESPONSE,
                result.toStyledString());
    }
    std::string set_brightness(const std::string& param1,
                               int param2) throw(jsonrpc::JsonRpcException)
    {
        Json::Value p;
        p.append(param1);
        p.append(param2);
        Json::Value result = this->CallMethod("set_brightness", p);
        if (result.isString())
            return result.asString();
        else
            throw jsonrpc::JsonRpcException(
                jsonrpc::Errors::ERROR_CLIENT_INVALID_RESPONSE,
                result.toStyledString());
    }
    std::string
    get_speed(const std::string& param1) throw(jsonrpc::JsonRpcException)
    {
        Json::Value p;
        p.append(param1);
        Json::Value result = this->CallMethod("get_speed", p);
        if (result.isString())
            return result.asString();
        else
            throw jsonrpc::JsonRpcException(
                jsonrpc::Errors::ERROR_CLIENT_INVALID_RESPONSE,
                result.toStyledString());
    }
    std::string set_speed(const std::string& param1,
                          int param2) throw(jsonrpc::JsonRpcException)
    {
        Json::Value p;
        p.append(param1);
        p.append(param2);
        Json::Value result = this->CallMethod("set_speed", p);
        if (result.isString())
            return result.asString();
        else
            throw jsonrpc::JsonRpcException(
                jsonrpc::Errors::ERROR_CLIENT_INVALID_RESPONSE,
                result.toStyledString());
    }
    std::string
    get_mode(const std::string& param1) throw(jsonrpc::JsonRpcException)
    {
        Json::Value p;
        p.append(param1);
        Json::Value result = this->CallMethod("get_mode", p);
        if (result.isString())
            return result.asString();
        else
            throw jsonrpc::JsonRpcException(
                jsonrpc::Errors::ERROR_CLIENT_INVALID_RESPONSE,
                result.toStyledString());
    }
    std::string
    set_mode(const std::string& param1,
             const std::string& param2) throw(jsonrpc::JsonRpcException)
    {
        Json::Value p;
        p.append(param1);
        p.append(param2);
        Json::Value result = this->CallMethod("set_mode", p);
        if (result.isString())
            return result.asString();
        else
            throw jsonrpc::JsonRpcException(
                jsonrpc::Errors::ERROR_CLIENT_INVALID_RESPONSE,
                result.toStyledString());
    }
    std::string set_color(const std::string& param1,
                          int param2,
                          int param3,
                          int param4) throw(jsonrpc::JsonRpcException)
    {
        Json::Value p;
        p.append(param1);
        p.append(param2);
        p.append(param3);
        p.append(param4);
        Json::Value result = this->CallMethod("set_color", p);
        if (result.isString())
            return result.asString();
        else
            throw jsonrpc::JsonRpcException(
                jsonrpc::Errors::ERROR_CLIENT_INVALID_RESPONSE,
                result.toStyledString());
    }
};

#endif // JSONRPC_CPP_STUB_STUBCLIENT_H_
