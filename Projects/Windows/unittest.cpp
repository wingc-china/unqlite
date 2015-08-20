#include "stdafx.h"
#include "CppUnitTest.h"

extern "C" {
#include <UnQLite.h>
}
#include <memory>
#include <unique_handle.hxx>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

typedef unique_handle<unqlite *, decltype(unqlite_close)*, nullptr, unqlite_close> unique_unqlite;

namespace UnitTest
{
    TEST_CLASS(UnQLite)
    {
    public:

        TEST_METHOD(SimpleKeyValue)
        {
            unique_unqlite db;

            // Open our database;
            Assert::AreEqual(UNQLITE_OK, unqlite_open(db.address_of(), "test.db", UNQLITE_OPEN_CREATE));
            // Store some records
            Assert::AreEqual(UNQLITE_OK, unqlite_kv_store(db.get(), "test", -1, "Hello World", 11)); //test => 'Hello World'
            // A small formatted string
            Assert::AreEqual(UNQLITE_OK, unqlite_kv_store_fmt(db.get(), "date", -1, "Current date: %d:%d:%d", 2013, 06, 07));
            //Switch to the append interface
            Assert::AreEqual(UNQLITE_OK, unqlite_kv_append(db.get(), "msg", -1, "Hello, ", 7)); //msg => 'Hello, '
            //The second chunk
            Assert::AreEqual(UNQLITE_OK, unqlite_kv_append(db.get(), "msg", -1, "Current time is: ", 17)); //msg => 'Hello, Current time is: '
            //The last formatted chunk
            Assert::AreEqual(UNQLITE_OK, unqlite_kv_append_fmt(db.get(), "msg", -1, "%d:%d:%d", 10, 16, 53)); //msg => 'Hello, Current time is: 10:16:53'

            //Delete a record
            Assert::AreEqual(UNQLITE_OK, unqlite_kv_delete(db.get(), "test", -1));

            //Store 20 random records.
            for (int i = 0; i < 20; ++i)
            {
                char zKey[12]; //Random generated key
                char zData[34]; //Dummy data
                // generate the random key
                Assert::AreEqual(UNQLITE_OK, unqlite_util_random_string(db.get(), zKey, sizeof(zKey)));
                // Perform the insertion
                Assert::AreEqual(UNQLITE_OK, unqlite_kv_store(db.get(), zKey, sizeof(zKey), zData, sizeof(zData)));
            }

            /*
            if (rc != UNQLITE_OK)
            {
                //Insertion fail, Handle error
                const char *zBuf;
                int iLen;
                // Something goes wrong, extract the database error log
                unqlite_config(db.get(), UNQLITE_CONFIG_ERR_LOG, &zBuf, &iLen);
                if (iLen > 0)
                {
                    puts(zBuf);
                }
                if (rc != UNQLITE_BUSY && rc != UNQLITE_NOTIMPLEMENTED){
                    // Rollback
                    unqlite_rollback(db.get());
                }
            }
            */
        }

        TEST_METHOD(Files)
        {
            const char* fileNames[] = { "UnitTest.lib", "UnQLite.lib" };  // Testing with the lib files themselves.
            unqlite_int64 fileSize;
            unique_unqlite db;
            auto closer = [&fileSize](void * pMmapedFile) { unqlite_util_release_mmaped_file(pMmapedFile, fileSize); };
            typedef std::unique_ptr<void, decltype(closer)> unique_mmaped_file; // TODO: Why cannot my unique_handle do this?

            // Open our database;
            Assert::AreEqual(UNQLITE_OK, unqlite_open(db.address_of(), "test.db", UNQLITE_OPEN_CREATE));

            for (const auto & fileName : fileNames)
            {
                void * pMmapedFile;
                unique_mmaped_file mmapedFile(nullptr, closer);
                // Obtain a read-only memory view of the target file;
                Assert::AreEqual(UNQLITE_OK, unqlite_util_load_mmaped_file(fileName, &pMmapedFile, &fileSize));
                mmapedFile.reset(pMmapedFile);
                pMmapedFile = nullptr;
                // Store the whole file in our database;
                Assert::AreEqual(UNQLITE_OK, unqlite_kv_store(db.get(), fileName, -1, mmapedFile.get(), fileSize));
            }
        }

        TEST_METHOD(Cursors)
        {
            // TODO
            Assert::AreEqual(true, false);
        }

        TEST_METHOD(JX9Script)
        {
            // TODO
            Assert::AreEqual(true, false);
        }
    };
}