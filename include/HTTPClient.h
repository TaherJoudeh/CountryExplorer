#pragma once
#include <string>
#include <functional>
#include <thread>
#include <atomic>
#include <mutex>

/**
 * @file HTTPClient.h
 * @brief HTTP client for asynchronous API requests
 * 
 * This class handles HTTP GET requests in a separate thread to prevent UI blocking.
 * Uses std::thread for async operations and std::atomic for thread-safe status flags.
 * Implements std::mutex for protecting shared resources.
 */

class HTTPClient {
public:
    /**
     * @brief Callback function type for async responses
     * @param success Whether the request succeeded
     * @param response Response body (JSON string) if successful, error message if failed
     */
    using ResponseCallback = std::function<void(bool success, const std::string& response)>;
    
    HTTPClient();
    ~HTTPClient();
    
    /**
     * @brief Perform asynchronous GET request
     * @param url Target URL
     * @param callback Function to call when request completes
     * 
     * Spawns a new thread to handle the request. Uses std::atomic to track status.
     */
    void getAsync(const std::string& url, ResponseCallback callback);
    
    /**
     * @brief Perform synchronous GET request
     * @param url Target URL
     * @param response Output parameter for response body
     * @return true if request succeeded, false otherwise
     * 
     * Blocks until request completes. Useful for initial data loading.
     */
    bool getSync(const std::string& url, std::string& response);
    
    /**
     * @brief Check if a request is currently in progress
     * @return true if busy, false otherwise
     * 
     * Thread-safe check using std::atomic
     */
    bool isBusy() const;
    
    /**
     * @brief Cancel current request (if possible)
     */
    void cancel();
    
private:
    std::atomic<bool> busy_;           // Thread-safe busy flag
    std::atomic<bool> shouldCancel_;   // Thread-safe cancel flag
    std::mutex threadMutex_;           // Protects thread operations
    std::thread workerThread_;         // Background worker thread
    
    /**
     * @brief Internal worker function that runs in separate thread
     */
    void workerFunction(const std::string& url, ResponseCallback callback);
    
    /**
     * @brief Join worker thread if it's joinable
     */
    void joinThread();
};
