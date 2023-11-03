/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/Sha512HashSink>
#include <cc/testing>

int main(int argc, char *argv[])
{
    using namespace cc;

    TestCase {
        "Sha512OneBlock",
        []{
            String message = "abc";
            String hash = hex(sha512(message)).breakUp(8).join(" ");
            fout("SHA-512 of \"%%\": %%") << message << hash << nl;
            CC_VERIFY(hash == "DDAF35A1 93617ABA CC417349 AE204131 12E6FA4E 89A97EA2 0A9EEEE6 4B55D39A 2192992A 274FC1A8 36BA3C23 A3FEEBBD 454D4423 643CE80E 2A9AC94F A54CA49F");
        }
    };

    TestCase {
        "Sha512TwoBlock",
        []{
            String message = "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu";
            String hash = hex(sha512(message)).breakUp(8).join(" ");
            fout("SHA-512 of \"%%\": %%") << message << hash << nl;
            CC_VERIFY(hash == "8E959B75 DAE313DA 8CF4F728 14FC143F 8F7779C6 EB9F7FA1 7299AEAD B6889018 501D289E 4900F7E4 331B99DE C4B5433A C7D329EE B6DD2654 5E96E55B 874BE909");
        }
    };

    return TestSuite{argc, argv}.run();
}
