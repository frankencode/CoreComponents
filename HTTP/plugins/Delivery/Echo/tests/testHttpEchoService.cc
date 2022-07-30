#include <cc/HttpServer>
#include <cc/HttpClient>
#include <cc/TempFile>
#include <cc/testing>

int main(int argc, char *argv[])
{
    using namespace cc;

    TestCase {
        "HttpEchoTest",
        []{
            HttpServer server{
                "Node {\n"
                "    address: \"127.0.0.1\"\n"
                "    port: 8787\n"
                "    family: IPv4\n"
                "    concurrency: 1\n"
                "    Echo {\n"
                "        host: *\n"
                "    }\n"
                "}\n"
            };

            server.start();
            auto address = server.waitStarted();
            CC_INSPECT(address);
            String text;
            {
                HttpClient client{address};
                CC_CHECK(client.waitEstablished());
                HttpResponse response = client.query("GET", "/");
                CC_CHECK(response.status() == HttpStatus::OK);
                text = response.payload().readAll();
            }
            CC_INSPECT(text);
            CC_CHECK(text.contains(address.networkAddress()));
            CC_CHECK(text.startsWith("GET"));
            server.shutdown();
            server.wait();
        }
    };

    TestCase {
        "HttpsEchoTest",
        []{
            String testKeyData {
                "-----BEGIN RSA PRIVATE KEY-----\n"
                "MIIJKQIBAAKCAgEAzPmxD241PpvnooO2kXinQub04S4wRyqI+2Ry5HixCtiiewf1\n"
                "AAmnFk5414XnfLOZsOQ/MVyNzipEvXFyAJ4q+YdBpWczzDLRp4MIz8cwSvRSvtT/\n"
                "AuSKI4Gqh/5CwK00hvUcatS7DI++dE0/hpQltEa3tbyYAC7KpcOB9/9jvreU6dw7\n"
                "VF6bVPre+XaKO6/IwiOA590qwCW0klghYLYwyFH2HlYKUuq63b0Hq03RnQS9+e3I\n"
                "afA6HOhnMOtyHalukhI5mAeKE9G5BIZIryF20iC6WHEDIwIoqF6mQyWiLAkG4dWg\n"
                "1W01b/nrWr5fAbgG1CyQXoJ8osQVyTD7KYXwmJTh/SSrRNjiJb0qskkn14ER2C/n\n"
                "U3yiPQYbZ+KgPTIDa5yae+3bTK1UdoWT8txAyT9gkJ91c4dox5mEAS3dnt7vl1iY\n"
                "69YfACOSbdeRnDDXNfegRhRKqYmJ64fRLVVmbR69yaPN3NGRmkOLLyuWN5tWrQjx\n"
                "zi0CWv10g/Jph3sHh1VPjwq9deqBQSFnngH7DhwH1QraRwrfO55f7PRMdYKqeaDx\n"
                "6R+19FxFPd0Z4rEtCKzxgTpg4BFxgjmErX111zXp+IRapllSjz+ym9/Fz2qaKEEE\n"
                "R9f9q8f5aHUoyO+cZXYQVfRE5r8MjsiFXsW+gpiMO5QHkleDsV00ZMHurc0CAwEA\n"
                "AQKCAgEAuk9GyiDBUsxA6i3QVz+UwLHO6ZKjHTyjT+WIrPSq75HjdfLQlZ0we40O\n"
                "N9AljeptiySb79ERr9AptQwVtEwQAckYOTCqGzQh7qGi88E4uxiC0aQmusliak2k\n"
                "5IGOyTVmJjlLACumbQ+G7I1+bWPhNZCF0TG/84KUfcFMNE+1qReSylJ8tv5bMA3r\n"
                "eFJv6TqpPxUoSRh1kDt1ewOSKybPBnXBQOex8JYdDgmQm/tayN8TksW4Qdqx566y\n"
                "hPutORrVPv5DzVUlA0jAZEVucYj4SJyo5FSspjiDHi8R7ryZQG9gCwuzaesfBE3k\n"
                "yBF7p8NOqbJr8wumIAgdSUc1NBPjkBxTDhELEJWud6xZqiBBwdeUz9qbJtJMKUbf\n"
                "zt7oMm4mjTpBqgGU6i9buvVZ22h2CXqdxBpHgAKMaexVOlVnBNvWdvm3R1MT5rWc\n"
                "L3k2T22FudO4CbB0iDdYOJ6Jt4jlAIXqu4MmAg6IdUNrN+k7r0xtxDfE0PnjIY4e\n"
                "EQnC3nWp6owYYrsw4F1KbK3LfD0wLG879/7II3vCnefu2hrqst6DOFHnmYQrVzT5\n"
                "k86TJeMqiJ/frqYbQl8jEtozO7DKKJk79bqsA6Vmdq39MW8q4j+hFO1mHuZDsJVz\n"
                "zyyk1y6nCpOzd/qutcyFxBCVU7Cx3qbonyHz4OMfJLLLNSvzyp0CggEBAOAAdGFx\n"
                "sWnzDLkTRTHIWf/exyef9ltCtBvWOL+zMSdHkObCjdU+8nVsh0Wg6cBCDOeAtzq3\n"
                "sM104Vf+jMdCJZ8Egz5SwvQMIzmwfZXwtmES9Wt8HoayWEXMYC5wsbjrlMITm1MJ\n"
                "PV725Z3KUbtTzipBM8lgv7ITDD26Tt7ACTO1G1xKaM1+JqZdNUEMQw6aJWug94NL\n"
                "i1sZ99Em8PhHr/Hyqu3XoZHPmnWLmI0oQypDlpTW0CCsJ13yl2/UNYTAfwZUB8lA\n"
                "7H8swJEo+InPI4cj6eBzXJ20zhvZpqIH/wL/8Na0Rmq/5L/9peL8I89eLjIDQqSj\n"
                "20bA2HeDRyoGleMCggEBAOpBdTeLuHvGWNFowUauSQEn5TZ9exhcf5BDI7x9VMNr\n"
                "qSFl/hysGX8NVa1NDYaejYO1Tx/WFESEMzrv1CoXmSjVtkPfhX08PHrj+aUPwi5x\n"
                "6x1hoiEwTAWrUHg4B6KhsokPwFnCtIvXb14ONNO14U/8/xJB7nBlku2k1cMw663A\n"
                "jK9/hcIU4cgJlRDT/Kw32PvvAHK1qdStaSCwvl4KwTydZCRC7rHyuxNpcjAo0Zik\n"
                "SDIaYVAwxatfG1g0Mn0kVdZ2AoLoMPKwwUacYLWjZXIEtGKxcy7TKmVLbiiHV3Yn\n"
                "Itdqn0Nse+Y2NU0Q++rddafkDMhw3cKwCI2KioJtfI8CggEAPfiCOg8tKQWfyHgH\n"
                "VZ17IKmuaUz86ASNNYJPkuFtQNOwx9adTJislUXgKqg44wKi9U0L5Ae7tyuVyDVZ\n"
                "Fys6o0KpWI2m98N/307vf5LnsfV/xivBejxO5YiEN3TgBivOrN6lwxQ0UVh27+9w\n"
                "KyYlTFVbkWPdxNM2OMGZXlC8evyoxCCl2+wBn1b8C7Hrv1dv8NXIhB9bP2nwwIFI\n"
                "ezfB+mrLVR+pyGAp+EkVfD/eTQwD1cuUKuXVwk0CDmMkJ0TmH4cR3GDEzO5BnCc8\n"
                "k0umJZYO9xSTU+IYf2X4jq0tpNPP+mZHcPMDwwXWZfShp5YDq7xxLIFUgb+KY44D\n"
                "GyJVRwKCAQEAsBB8sj/ga58bWGEciywpEKZUn4zWPOwgsGPV8zBbyfkcCE8pG5G6\n"
                "JAT4tSUpmlmG0UwvaSU03H5kJtLkqrJL2iYuNHtGUGc6qb64/neGofPbG50rBzjO\n"
                "mqA0/fZWvsQ7dMqS7qPpoQVN3epdiBW2f7EOYUGOnYmqUdTHGUqmiFQlKS52cPXS\n"
                "8M9K63f4+9OJ0WjD9HNXbtLbJcfRfMsmi5FcF+j6kGJ4etGREmNLjSdEX+v14hiW\n"
                "KQR1f+jTXb02itbtckhjqbRxv8l+4mYps/WXQmpOwQ3TWojEBdAbS2GEzVWE2F8s\n"
                "DE4H56FxRggnCwXbGbfC5TX7Z66OBhOWPwKCAQAjrH7I2MSzdTxX1GcXJUZR3kpl\n"
                "z3cOn5v1beN9Gxv3Y4m19wPwHjX0qh+pmih9rgQFJh4m9DFig+IA5OA+UzE6sD5M\n"
                "4VPZfVgVzSk3u+MFvDDi4OG+jY0zZfZ6sEc+Lb7xa/qTb5SwFWhK2fj1UfCa6AsF\n"
                "gksI430GVVV0JpHUP8aO1PFP+i9Ul+JFsuZGh31GT/d+KDIJldus1ClHnjcrXjl9\n"
                "Ewu0dHVi0tN3FlSKTfTfneGYxC7FUkhXpTGOGuPin+IVJW3I6dMcBjyk2nOLJjFT\n"
                "eti3NuVT4H1v7QgItGf1O9oOjmSrlrH6IG2UAU6Ui6DLB+je96AwZcQcuXwn\n"
                "-----END RSA PRIVATE KEY-----\n"
            };

            String testCertData {
                "-----BEGIN CERTIFICATE-----\n"
                "MIIF8jCCA9qgAwIBAgIUIGZsBMXTobfUGm+mCmGErIYcEPAwDQYJKoZIhvcNAQEM\n"
                "BQAweDELMAkGA1UEBhMCREUxEDAOBgNVBAgTB0JhdmFyaWExDzANBgNVBAcTBk11\n"
                "bmljaDEXMBUGA1UEChMOQ29yZUNvbXBvbmVudHMxEjAQBgNVBAMTCTEyNy4wLjAu\n"
                "MTEZMBcGCgmSJomT8ixkARkWCTEyNy4wLjAuMTAeFw0yMTEwMTUxMjA2MDFaFw0z\n"
                "MTEwMTMxMjA2MDVaMHgxCzAJBgNVBAYTAkRFMRAwDgYDVQQIEwdCYXZhcmlhMQ8w\n"
                "DQYDVQQHEwZNdW5pY2gxFzAVBgNVBAoTDkNvcmVDb21wb25lbnRzMRIwEAYDVQQD\n"
                "EwkxMjcuMC4wLjExGTAXBgoJkiaJk/IsZAEZFgkxMjcuMC4wLjEwggIiMA0GCSqG\n"
                "SIb3DQEBAQUAA4ICDwAwggIKAoICAQDM+bEPbjU+m+eig7aReKdC5vThLjBHKoj7\n"
                "ZHLkeLEK2KJ7B/UACacWTnjXhed8s5mw5D8xXI3OKkS9cXIAnir5h0GlZzPMMtGn\n"
                "gwjPxzBK9FK+1P8C5IojgaqH/kLArTSG9Rxq1LsMj750TT+GlCW0Rre1vJgALsql\n"
                "w4H3/2O+t5Tp3DtUXptU+t75doo7r8jCI4Dn3SrAJbSSWCFgtjDIUfYeVgpS6rrd\n"
                "vQerTdGdBL357chp8Doc6Gcw63IdqW6SEjmYB4oT0bkEhkivIXbSILpYcQMjAiio\n"
                "XqZDJaIsCQbh1aDVbTVv+etavl8BuAbULJBegnyixBXJMPsphfCYlOH9JKtE2OIl\n"
                "vSqySSfXgRHYL+dTfKI9Bhtn4qA9MgNrnJp77dtMrVR2hZPy3EDJP2CQn3Vzh2jH\n"
                "mYQBLd2e3u+XWJjr1h8AI5Jt15GcMNc196BGFEqpiYnrh9EtVWZtHr3Jo83c0ZGa\n"
                "Q4svK5Y3m1atCPHOLQJa/XSD8mmHeweHVU+PCr116oFBIWeeAfsOHAfVCtpHCt87\n"
                "nl/s9Ex1gqp5oPHpH7X0XEU93RnisS0IrPGBOmDgEXGCOYStfXXXNen4hFqmWVKP\n"
                "P7Kb38XPapooQQRH1/2rx/lodSjI75xldhBV9ETmvwyOyIVexb6CmIw7lAeSV4Ox\n"
                "XTRkwe6tzQIDAQABo3QwcjAMBgNVHRMBAf8EAjAAMBQGA1UdEQQNMAuCCTEyNy4w\n"
                "LjAuMTAdBgNVHSUEFjAUBggrBgEFBQcDAQYIKwYBBQUHAxEwDgYDVR0PAQH/BAQD\n"
                "AgWgMB0GA1UdDgQWBBTM9Q/8BkmSQ2tkL8urWmu7Ey8jPDANBgkqhkiG9w0BAQwF\n"
                "AAOCAgEAFMFPj1BqXAjiA3r66A7yjYA3eBGhJrGX2vtgqdYSqOn04WT9mFmNAK+W\n"
                "XZpg9AEgkbK/hFGscWhQMdVamluvlvxy+7eRDB6Ur9P4ShhDLUcLm+R06RWBeWbx\n"
                "o2dupgdX4kSsbM2coecGm5Pyo7QZ3O1SPfobvFepB2VVuhhDxiZgc3TmuQZ2TgIt\n"
                "ue3+tnJ0SY6nxbKdx85rkz0vWGfvGcB1MJSSN9A8c59Nsb19BvFW/Kj4w9jP+Zt0\n"
                "fSe2n6LBqkhGm6nhRTqXHXUA9BicKgNJGdI3g5lcWiFZd+pokuGYZCmW0c6qEZSz\n"
                "Eo7Z3kXU7JBHY0pGaDr5uo32SJlqiv+2Qn4QHWzGsrGY5NpDoUEqmz7xq9ICXfbj\n"
                "NMAr0p8zw61V8qo1A0TkNs/KA//w3K6iK9BhR0ts1+GnR8nmXtG8i+1nfPACM42T\n"
                "6iq6aOEPbRHcnzUmeW7empUtiw1gARwB/US5qR48ATYNMWRnIRhPst7PctwwAPCh\n"
                "cLQP+SFTtD0uNkzcn3oWJ/FJTKoFQoA9zZLSoKBt5wDFJKbIe77mmwHCt2iUjBxs\n"
                "n4AxxbKP6vx3LVaKuq4m2WZ6Gx4lXn1oqUJxodQZwnehNk8jxmwFbpqGEdySRSXA\n"
                "f3VF9DD+9qfrkXiKOgmZdNdVHQ+t6UxBl3x/GAek0Exx6nvkABg=\n"
                "-----END CERTIFICATE-----\n"
            };

            TempFile keyFile, certFile;
            keyFile.write(testKeyData);
            keyFile.sync();
            certFile.write(testCertData);
            certFile.sync();

            HttpServer server{
                String{
                    Format{
                        "Node {\n"
                        "    address: \"127.0.0.1\"\n"
                        "    port: 4447\n"
                        "    family: IPv4\n"
                        "    concurrency: 1\n"
                        "    Echo {\n"
                        "        host: *\n"
                        "        security: TLS {\n"
                        "            certificate: %%\n"
                        "            private-key: %%\n"
                        "        }\n"
                        "    }\n"
                        "}\n"
                    } << certFile.path() << keyFile.path()
                }
            };

            server.start();
            auto address = server.waitStarted();
            TlsClientOptions tlsOptions;
            tlsOptions.setTrustFilePath(certFile.path());
            String text;
            {
                HttpClient client{address, tlsOptions};
                CC_CHECK(client.waitEstablished());
                {
                    HttpResponse response = client.query("GET", "/");
                    CC_CHECK(response.status() == HttpStatus::OK);
                    text = response.payload().readAll();
                }
            }
            CC_INSPECT(text);
            CC_CHECK(text.contains(address.networkAddress()));
            CC_CHECK(text.startsWith("GET"));
            server.shutdown();
            server.wait();
        }
    };

    return TestSuite{argc, argv}.run();
}
