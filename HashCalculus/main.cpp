/*
 * Copyright (c) 2015 Cryptonomex, Inc., and contributors.
 *
 * The MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <iterator>

#include <fc/io/json.hpp>
#include <fc/io/stdio.hpp>
#include <fc/network/http/websocket.hpp>
#include <fc/rpc/cli.hpp>
#include <fc/rpc/websocket_api.hpp>
#include <fc/stacktrace.hpp>

#include <graphene/app/api.hpp>
#include <graphene/chain/config.hpp>
#include <graphene/egenesis/egenesis.hpp>
#include <graphene/utilities/key_conversion.hpp>
#include <graphene/wallet/wallet.hpp>

#include <fc/interprocess/signals.hpp>
#include <boost/program_options.hpp>

#include <fc/log/console_appender.hpp>
#include <fc/log/file_appender.hpp>
#include <fc/log/logger.hpp>
#include <fc/log/logger_config.hpp>

#include <graphene/utilities/git_revision.hpp>
#include <boost/version.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <websocketpp/version.hpp>

using namespace graphene::app;
using namespace graphene::chain;
using namespace graphene::utilities;
using namespace graphene::wallet;
using namespace std;

int main( int argc, char** argv )
{
   std::string hash;
   std::string witness_key = "RVP4iLKwaDRJnSjphSYtCT5F0TonHE4NEtv5MDanjrfjTdhAQ3Xjn";
   uint64_t prev_seed = 1534654114650;

   // шифрованный_секрет = sha512(секрет + sha256(секрет + sha512(cr_seed + witness_pub_key + sha512(текщее_время))))

   auto start = std::chrono::steady_clock::now();

   //18 446 744 073 709 551 615
   for (uint64_t secret = 0; secret != UINT64_MAX; ++secret)
   {
      hash = fc::sha512::hash(
         std::to_string(secret) +
         fc::sha256::hash(
            std::to_string(secret) +
            fc::sha512::hash(
               std::to_string(prev_seed) +
               witness_key +
               fc::sha512::hash(
                  std::to_string(fc::time_point::now().sec_since_epoch())
               ).str()
            ).str()
         ).str()
      );
      if (secret % 100000 == 0) {
         auto current = std::chrono::steady_clock::now();
         std::chrono::duration<double> elapsed_seconds = current-start;
         std::cout << "Hash rate is " + std::to_string(secret / elapsed_seconds.count()) + "h/s\n" ;
      }
   }

   auto end = std::chrono::steady_clock::now();
   std::chrono::duration<double> elapsed_seconds = end-start;
   std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n";

   return 0;
}



