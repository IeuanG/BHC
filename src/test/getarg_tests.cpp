#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-BHC");
    BOOST_CHECK(GetBoolArg("-BHC"));
    BOOST_CHECK(GetBoolArg("-BHC", false));
    BOOST_CHECK(GetBoolArg("-BHC", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-BHCo"));
    BOOST_CHECK(!GetBoolArg("-BHCo", false));
    BOOST_CHECK(GetBoolArg("-BHCo", true));

    ResetArgs("-BHC=0");
    BOOST_CHECK(!GetBoolArg("-BHC"));
    BOOST_CHECK(!GetBoolArg("-BHC", false));
    BOOST_CHECK(!GetBoolArg("-BHC", true));

    ResetArgs("-BHC=1");
    BOOST_CHECK(GetBoolArg("-BHC"));
    BOOST_CHECK(GetBoolArg("-BHC", false));
    BOOST_CHECK(GetBoolArg("-BHC", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noBHC");
    BOOST_CHECK(!GetBoolArg("-BHC"));
    BOOST_CHECK(!GetBoolArg("-BHC", false));
    BOOST_CHECK(!GetBoolArg("-BHC", true));

    ResetArgs("-noBHC=1");
    BOOST_CHECK(!GetBoolArg("-BHC"));
    BOOST_CHECK(!GetBoolArg("-BHC", false));
    BOOST_CHECK(!GetBoolArg("-BHC", true));

    ResetArgs("-BHC -noBHC");  // -BHC should win
    BOOST_CHECK(GetBoolArg("-BHC"));
    BOOST_CHECK(GetBoolArg("-BHC", false));
    BOOST_CHECK(GetBoolArg("-BHC", true));

    ResetArgs("-BHC=1 -noBHC=1");  // -BHC should win
    BOOST_CHECK(GetBoolArg("-BHC"));
    BOOST_CHECK(GetBoolArg("-BHC", false));
    BOOST_CHECK(GetBoolArg("-BHC", true));

    ResetArgs("-BHC=0 -noBHC=0");  // -BHC should win
    BOOST_CHECK(!GetBoolArg("-BHC"));
    BOOST_CHECK(!GetBoolArg("-BHC", false));
    BOOST_CHECK(!GetBoolArg("-BHC", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--BHC=1");
    BOOST_CHECK(GetBoolArg("-BHC"));
    BOOST_CHECK(GetBoolArg("-BHC", false));
    BOOST_CHECK(GetBoolArg("-BHC", true));

    ResetArgs("--noBHC=1");
    BOOST_CHECK(!GetBoolArg("-BHC"));
    BOOST_CHECK(!GetBoolArg("-BHC", false));
    BOOST_CHECK(!GetBoolArg("-BHC", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-BHC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-BHC", "eleven"), "eleven");

    ResetArgs("-BHC -BHC");
    BOOST_CHECK_EQUAL(GetArg("-BHC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-BHC", "eleven"), "");

    ResetArgs("-BHC=");
    BOOST_CHECK_EQUAL(GetArg("-BHC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-BHC", "eleven"), "");

    ResetArgs("-BHC=11");
    BOOST_CHECK_EQUAL(GetArg("-BHC", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-BHC", "eleven"), "11");

    ResetArgs("-BHC=eleven");
    BOOST_CHECK_EQUAL(GetArg("-BHC", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-BHC", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-BHC", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-BHC", 0), 0);

    ResetArgs("-BHC -BHC");
    BOOST_CHECK_EQUAL(GetArg("-BHC", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-BHC", 11), 0);

    ResetArgs("-BHC=11 -BHC=12");
    BOOST_CHECK_EQUAL(GetArg("-BHC", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-BHC", 11), 12);

    ResetArgs("-BHC=NaN -BHC=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-BHC", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-BHC", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--BHC");
    BOOST_CHECK_EQUAL(GetBoolArg("-BHC"), true);

    ResetArgs("--BHC=verbose --BHC=1");
    BOOST_CHECK_EQUAL(GetArg("-BHC", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-BHC", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noBHC");
    BOOST_CHECK(!GetBoolArg("-BHC"));
    BOOST_CHECK(!GetBoolArg("-BHC", true));
    BOOST_CHECK(!GetBoolArg("-BHC", false));

    ResetArgs("-noBHC=1");
    BOOST_CHECK(!GetBoolArg("-BHC"));
    BOOST_CHECK(!GetBoolArg("-BHC", true));
    BOOST_CHECK(!GetBoolArg("-BHC", false));

    ResetArgs("-noBHC=0");
    BOOST_CHECK(GetBoolArg("-BHC"));
    BOOST_CHECK(GetBoolArg("-BHC", true));
    BOOST_CHECK(GetBoolArg("-BHC", false));

    ResetArgs("-BHC --noBHC");
    BOOST_CHECK(GetBoolArg("-BHC"));

    ResetArgs("-noBHC -BHC"); // BHC always wins:
    BOOST_CHECK(GetBoolArg("-BHC"));
}

BOOST_AUTO_TEST_SUITE_END()
