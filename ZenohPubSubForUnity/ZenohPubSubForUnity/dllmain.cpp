// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#ifdef _WIN32
#include <combaseapi.h>
#endif

#include <chrono>
#include <string>
#include <thread>
#include <functional>
#include <mutex>
#include <memory>

#define ZENOHCXX_ZENOHC
#include "zenoh.hxx"

#if !defined(ZENOHCXX_ZENOHC) && !defined(ZENOHCXX_ZENOHPICO)
#error "Unknown zenoh backend"
#endif

using namespace zenoh;

#ifdef _WIN32
    #define EXPORT __declspec(dllexport)
#else
    #define EXPORT
#endif


namespace {
    std::mutex mutex_pub;
    std::mutex mutex_sub;

    std::string publish_keyexpr;

    std::string subscribe_keyexpr;
    void(*subscribe_callback)(const char*) = nullptr;

    bool is_running = false;

    std::deque<std::string> send_queue;

    const size_t SEND_QUEUE_SIZE_LIMIT = 100; // TODO: tentative

    const char* MallocString(const char* str) {
        size_t n = strlen(str);
#ifdef _WIN32
        char* s = (char*)CoTaskMemAlloc(n + 1);
#else
		char* s = (char*)malloc(n + 1);
#endif
        if (s != nullptr) {
            memcpy(s, str, n + 1);
        }
        return s;
    }

    const char* kind_to_str(SampleKind kind) {
        switch (kind) {
        case SampleKind::Z_SAMPLE_KIND_PUT:
            return "PUT";
        case SampleKind::Z_SAMPLE_KIND_DELETE:
            return "DELETE";
        default:
            return "UNKNOWN";
        }
    }

    void data_handler(const Sample& sample) {

        std::cout << ">> [Subscriber] Received " << kind_to_str(sample.get_kind()) << " ('"
            << sample.get_keyexpr().as_string_view() << "' : '" << sample.get_payload().as_string() << "')\n";

        std::string key = sample.get_keyexpr().as_string_view().data();

        {
            std::lock_guard<std::mutex> lock(mutex_sub);
            if (subscribe_callback) {
                subscribe_callback(MallocString(sample.get_payload().as_string().c_str()));
            }
        }
    }

    void pub_sub_thread() {
        Config config = Config::create_default();
        auto session = Session::open(std::move(config));

        auto sub = session.declare_subscriber(KeyExpr(subscribe_keyexpr.c_str()), &data_handler, closures::none);

        auto pub = session.declare_publisher(KeyExpr(publish_keyexpr.c_str()));

        while (is_running) {
            if (!is_running) {
                break;
            }

            {
                std::lock_guard<std::mutex> lock(mutex_pub);
                for (auto& sendString : send_queue) {
                    auto put_options = Publisher::PutOptions{};
                    put_options.encoding = Encoding("text/plain");
                    pub.put(sendString.c_str(), std::move(put_options));
                }
                send_queue.clear();
            }
            std::this_thread::yield();
        }
    }
}

extern "C" EXPORT void Publish(const char* message) {

	std::lock_guard<std::mutex> lock(mutex_pub);
    if (send_queue.size() >= SEND_QUEUE_SIZE_LIMIT)
        return;
	send_queue.push_back(message);
}

extern "C" EXPORT void StartPubSub(const char* pub_keyexpr, const char* sub_keyexpr, void(*sub_cb)(const char*)) {
	
    std::lock_guard<std::mutex> lock(mutex_sub);
    if (is_running) {
		return;
	}
	
	publish_keyexpr = pub_keyexpr;
	subscribe_keyexpr = sub_keyexpr;
	subscribe_callback = sub_cb;

	is_running = true;
    
    std::thread worker(pub_sub_thread);
    worker.detach();

}

extern "C" EXPORT void Destroy() {
	is_running = false;
    {
        std::lock_guard<std::mutex> lock(mutex_pub);
		send_queue.clear();
    }
    {
        std::lock_guard<std::mutex> lock(mutex_sub);
	    subscribe_callback = nullptr;
    }
}

#ifdef _WIN32
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        Destroy();
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        Destroy();
        break;
    }
    return TRUE;
}
#endif
