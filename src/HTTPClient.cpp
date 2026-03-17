#include "HTTPClient.h"
#include <httplib.h>
#include <iostream>

HTTPClient::HTTPClient() : busy_(false), shouldCancel_(false) {
}

HTTPClient::~HTTPClient() {
    cancel();
    joinThread();
}

void HTTPClient::getAsync(const std::string& url, ResponseCallback callback) {
    // Join previous thread if exists
    joinThread();
    
    // Reset flags
    shouldCancel_.store(false);
    busy_.store(true);
    
    // Start new thread
    std::lock_guard<std::mutex> lock(threadMutex_);
    workerThread_ = std::thread(&HTTPClient::workerFunction, this, url, callback);
}

bool HTTPClient::getSync(const std::string& url, std::string& response) {
    try {
        // Parse URL
        size_t protocolEnd = url.find("://");
        if (protocolEnd == std::string::npos) {
            return false;
        }
        
        size_t hostStart = protocolEnd + 3;
        size_t pathStart = url.find('/', hostStart);
        
        std::string host = url.substr(hostStart, pathStart - hostStart);
        std::string path = (pathStart != std::string::npos) ? url.substr(pathStart) : "/";
        
        // Determine if HTTPS
        bool isHttps = url.substr(0, protocolEnd) == "https";
        
        // Create client
        httplib::Client client(isHttps ? "https://" + host : "http://" + host);
        client.set_follow_location(true);
        client.set_connection_timeout(10, 0); // 10 seconds
        
        // Perform request
        auto res = client.Get(path.c_str());
        
        if (res && res->status == 200) {
            response = res->body;
            return true;
        }
        
        return false;
        
    } catch (const std::exception& e) {
        std::cerr << "HTTP Error: " << e.what() << std::endl;
        return false;
    }
}

bool HTTPClient::isBusy() const {
    return busy_.load();
}

void HTTPClient::cancel() {
    shouldCancel_.store(true);
}

void HTTPClient::workerFunction(const std::string& url, ResponseCallback callback) {
    std::string response;
    bool success = false;
    
    try {
        if (!shouldCancel_.load()) {
            success = getSync(url, response);
        }
    } catch (...) {
        success = false;
        response = "Exception occurred during request";
    }
    
    busy_.store(false);
    
    // Call callback with result
    if (!shouldCancel_.load() && callback) {
        callback(success, response);
    }
}

void HTTPClient::joinThread() {
    std::lock_guard<std::mutex> lock(threadMutex_);
    if (workerThread_.joinable()) {
        workerThread_.join();
    }
}
