// Copyright 2010-2016 RethinkDB, all rights reserved.
#include "crypto/initialization_guard.hpp"

#ifdef _WIN32
#include "windows.hpp"
#endif

#include <openssl/err.h>
#include <openssl/opensslv.h>
#include <openssl/ssl.h>

#include <vector>

#include "arch/io/concurrency.hpp"
#include "arch/runtime/runtime.hpp"

#if OPENSSL_VERSION_NUMBER < 0x10100000L || defined(LIBRESSL_VERSION_NUMBER)
#define OPENSSL_init_ssl(x, y)     SSL_library_init()
#define OPENSSL_init_crypto(x, y)  SSL_load_error_strings()
#define OPENSSL_cleanup            ERR_free_strings
#endif

namespace crypto {

initialization_guard_t::initialization_guard_t() {
    OPENSSL_init_ssl(0, nullptr);
    OPENSSL_init_crypto(0, nullptr);

    // Make OpenSSL thread-safe by registering the required callbacks
    CRYPTO_THREADID_set_callback([](CRYPTO_THREADID *thread_out) {
        CRYPTO_THREADID_set_numeric(thread_out, get_thread_id().threadnum);
    });
    CRYPTO_set_locking_callback(
        [](int mode, int n, UNUSED const char *file, UNUSED int line) {
            static std::vector<system_rwlock_t> openssl_locks(CRYPTO_num_locks());
            rassert(n >= 0);
            rassert(static_cast<size_t>(n) < openssl_locks.size());
            if (mode & CRYPTO_LOCK) {
                if (mode & CRYPTO_READ) {
                    openssl_locks[n].lock_read();
                } else {
                    rassert(mode & CRYPTO_WRITE);
                    openssl_locks[n].lock_write();
                }
            } else {
                rassert(mode & CRYPTO_UNLOCK);
                openssl_locks[n].unlock();
            }
        });
}

initialization_guard_t::~initialization_guard_t() {
    OPENSSL_cleanup();
}

}  // namespace crypto
