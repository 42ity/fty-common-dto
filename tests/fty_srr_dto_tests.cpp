#include <catch2/catch.hpp>

//  --------------------------------------------------------------------------
//  Self tests
#define SELFTEST_DIR_RO "src/selftest-ro"
#define SELFTEST_DIR_RW "src/selftest-rw"

// color output definition for test function
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"


#define SELFTEST_DIR_RO "src/selftest-ro"
#define SELFTEST_DIR_RW "src/selftest-rw"

#include <cassert>
#include <iostream>
#include <fstream>
#include <vector>
#include <fty_common.h>
#include <fty_common_json.h>
#include "fty_srr_dto.h"
#include <cxxtools/serializationinfo.h>

using namespace dto;
using namespace dto::srr;

TEST_CASE("All the stuff of before")
{
    printf (" * fty_srr_dto_test: ");
    std::vector<std::pair<std::string, bool>> testsResults;

    std::string defaultVersion = "1.0";
    std::string passPhraseDef = "8";
    std::string testNumber;
    std::string testName;

    //Next test
    testNumber = "1.1";
    testName = "Check Save Query";
    printf ("\n-------------------------------------------------------------\n");
    {
        printf (" *=>  Test #%s %s\n", testNumber.c_str (), testName.c_str ());
        try
        {
            Query query1 = createSaveQuery({"test"},"myPassphrase");
            std::cout << query1 << std::endl;

            //test ==
            Query query2 = createSaveQuery({"test"},"myPassphrase");
            if(query1 != query2) throw std::runtime_error("Bad comparaison ==");

            //test !=
            Query query3 = createSaveQuery({"testgg"},"hsGH<hkherjg");
            if(query1 == query3) throw std::runtime_error("Bad comparaison !=");

            //test serialize -> deserialize
            UserData userdata;
            userdata << query1;

            Query query4;
            userdata >> query4;

            if(query1 != query4) throw std::runtime_error("Bad serialization to userdata");

            printf (" *<=  Test #%s > OK\n", testNumber.c_str ());
            testsResults.emplace_back (" Test #" + testNumber + " " + testName, true);

        }
        catch (const std::exception &e) {
            printf (" *<=  Test #%s > Failed\n", testNumber.c_str ());
            printf ("Error: %s\n", e.what ());
            testsResults.emplace_back (" Test #" + testNumber + " " + testName, false);
        }
    }
    printf ("OK\n");

    //Next test
    testNumber = "1.2";
    testName = "Check Restore Query";
    printf ("\n-------------------------------------------------------------\n");
    {
        printf (" *=>  Test #%s %s\n", testNumber.c_str (), testName.c_str ());
        try
        {
            Feature f1;
            f1.set_version("1.0");
            f1.set_data("data 1");

            Query query1 = createRestoreQuery({{"test", f1}}, "myPassphrase", defaultVersion, "myChecksum");
            std::cout << query1 << std::endl;

            //test ==
            Query query2 = createRestoreQuery({{"test", f1}},"myPassphrase", defaultVersion, "myChecksum");
            if(query1 != query2) throw std::runtime_error("Bad comparaison ==");

            //test !=
            Feature f2;
            f2.set_version("1.0");
            f2.set_data("data 2");
            Query query3 = createRestoreQuery({{"test-1", f2}},"hsGH<hkherjg", defaultVersion, "myChecksum");
            if(query1 == query3) throw std::runtime_error("Bad comparaison !=");

            //test serialize -> deserialize
            UserData userdata;
            userdata << query1;

            Query query4;
            userdata >> query4;

            if(query1 != query4) throw std::runtime_error("Bad serialization to userdata");

            Feature f3;
            f3.set_version("1.0");
            f3.set_data("{\"timeout\":\"40\"}");

            Query query5 = createRestoreQuery({{"test", f3}},"myPassphrase", defaultVersion, "myChecksum");
            std::cout << query5 << std::endl;

            UserData userdata1;
            userdata1 << query5;

            Query query6;
            userdata1 >> query6;

            if(query5 != query6) throw std::runtime_error("Bad serialization to userdata");


            printf (" *<=  Test #%s > OK\n", testNumber.c_str ());
            testsResults.emplace_back (" Test #" + testNumber + " " + testName, true);
        }
        catch (const std::exception &e) {
            printf (" *<=  Test #%s > Failed\n", testNumber.c_str ());
            printf ("Error: %s\n", e.what ());
            testsResults.emplace_back (" Test #" + testNumber + " " + testName, false);
        }
    }
    printf ("OK\n");

    //Next test
    testNumber = "1.3";
    testName = "Check Reset Query";
    printf ("\n-------------------------------------------------------------\n");
    {
        printf (" *=>  Test #%s %s\n", testNumber.c_str (), testName.c_str ());
        try
        {
            Query query1 = createResetQuery({"test"}, defaultVersion);
            std::cout << query1 << std::endl;

            //test ==
            Query query2 = createResetQuery({"test"}, defaultVersion);
            if(query1 != query2) throw std::runtime_error("Bad comparaison ==");

            //test !=
            Query query3 = createResetQuery({"testgg"}, defaultVersion);
            if(query1 == query3) throw std::runtime_error("Bad comparaison !=");

            //test serialize -> unserialize
            UserData userdata;
            userdata << query1;

            Query query4;
            userdata >> query4;

            if(query1 != query4) throw std::runtime_error("Bad serialization to userdata");

            printf (" *<=  Test #%s > OK\n", testNumber.c_str ());
            testsResults.emplace_back (" Test #" + testNumber + " " + testName, true);
        }
        catch (const std::exception &e) {
            printf (" *<=  Test #%s > Failed\n", testNumber.c_str ());
            printf ("Error: %s\n", e.what ());
            testsResults.emplace_back (" Test #" + testNumber + " " + testName, false);
        }
    }
    printf ("OK\n");

    //Next test
    testNumber = "1.4";
    testName = "Check List FeatureName Query";
    printf ("\n-------------------------------------------------------------\n");
    {
        printf (" *=>  Test #%s %s\n", testNumber.c_str (), testName.c_str ());
        try
        {
            Query query1 = createListFeatureQuery();
            std::cout << query1 << std::endl;

            //test ==
            Query query2 = createListFeatureQuery();
            if(query1 != query2) throw std::runtime_error("Bad comparaison ==");

            //test !=
            Query query3 = createResetQuery({"testgg"}, defaultVersion);
            if(query1 == query3) throw std::runtime_error("Bad comparaison !=");

            //test serialize -> deserialize
            UserData userdata;
            userdata << query1;

            Query query4;
            userdata >> query4;

            if(query1 != query4) throw std::runtime_error("Bad serialization to userdata");

            printf (" *<=  Test #%s > OK\n", testNumber.c_str ());
            testsResults.emplace_back (" Test #" + testNumber + " " + testName, true);
        }
        catch (const std::exception &e) {
            printf (" *<=  Test #%s > Failed\n", testNumber.c_str ());
            printf ("Error: %s\n", e.what ());
            testsResults.emplace_back (" Test #" + testNumber + " " + testName, false);
        }
    }
    printf ("OK\n");

    //Next test
    testNumber = "2.1";
    testName = "Check Save Response";
    printf ("\n-------------------------------------------------------------\n");
    {
        printf (" *=>  Test #%s %s\n", testNumber.c_str (), testName.c_str ());
        try
        {
            FeatureAndStatus fs1;
            FeatureStatus & s1 = *(fs1.mutable_status());
            s1.set_status(Status::SUCCESS);

            Feature & f1 = *(fs1.mutable_feature());
            f1.set_version("1.0");
            f1.set_data(DATA);

            std::map<FeatureName, FeatureAndStatus> map1;
            map1["test"] = fs1;

            Response r1 = createSaveResponse(map1, defaultVersion);
            std::cout << r1 << std::endl;

            //test ==
            Response r2 = createSaveResponse(map1, defaultVersion);
            if(r1 != r2) throw std::runtime_error("Bad comparaison ==");

            //test !=
            std::map<FeatureName, FeatureAndStatus> map2;
            map2["test2"] = fs1;

            Response r3 = createSaveResponse(map2, defaultVersion);
            if(r1 == r3) throw std::runtime_error("Bad comparaison !=");

            //test serialize -> deserialize
            UserData userdata;
            userdata << r1;

            Response r4;
            userdata >> r4;

            if(r1 != r4) throw std::runtime_error("Bad serialization to userdata");

            printf (" *<=  Test #%s > OK\n", testNumber.c_str ());
            testsResults.emplace_back (" Test #" + testNumber + " " + testName, true);
        }
        catch (const std::exception &e) {
            printf (" *<=  Test #%s > Failed\n", testNumber.c_str ());
            printf ("Error: %s\n", e.what ());
            testsResults.emplace_back (" Test #" + testNumber + " " + testName, false);
        }
    }
    printf ("OK\n");

    //Next test
    testNumber = "2.2";
    testName = "Check Restore Response";
    printf ("\n-------------------------------------------------------------\n");
    {
        printf (" *=>  Test #%s %s\n", testNumber.c_str (), testName.c_str ());
        try
        {
            FeatureStatus s1;
            s1.set_status(Status::SUCCESS);
            s1.set_error("aaaa");

            std::map<FeatureName, FeatureStatus> map1;
            map1["test"] = s1;

            Response r1 = createRestoreResponse(map1);
            std::cout << r1 << std::endl;

            //test ==
            Response r2 = createRestoreResponse(map1);
            if(r1 != r2) throw std::runtime_error("Bad comparaison ==");

            //test !=
            std::map<FeatureName, FeatureStatus> map2;
            map2["test2"] = s1;

            Response r3 = createRestoreResponse(map2);
            if(r1 == r3) throw std::runtime_error("Bad comparaison !=");

            //test serialize -> deserialize
            UserData userdata;
            userdata << r1;

            Response r4;
            userdata >> r4;

            if(r1 != r4) throw std::runtime_error("Bad serialization to userdata");

            printf (" *<=  Test #%s > OK\n", testNumber.c_str ());
            testsResults.emplace_back (" Test #" + testNumber + " " + testName, true);
        }
        catch (const std::exception &e) {
            printf (" *<=  Test #%s > Failed\n", testNumber.c_str ());
            printf ("Error: %s\n", e.what ());
            testsResults.emplace_back (" Test #" + testNumber + " " + testName, false);
        }
    }
    printf ("OK\n");

    //Next test
    testNumber = "2.3";
    testName = "Check Reset Response";
    printf ("\n-------------------------------------------------------------\n");
    {
        printf (" *=>  Test #%s %s\n", testNumber.c_str (), testName.c_str ());
        try
        {
            FeatureStatus s1;
            s1.set_status(Status::SUCCESS);
            s1.set_error(ERROR);

            std::map<FeatureName, FeatureStatus> map1;
            map1["test"] = s1;

            Response r1 = createResetResponse(map1);
            std::cout << r1 << std::endl;

            //test ==
            Response r2 = createResetResponse(map1);
            if(r1 != r2) throw std::runtime_error("Bad comparaison ==");

            //test !=
            std::map<FeatureName, FeatureStatus> map2;
            map2["test2"] = s1;

            Response r3 = createResetResponse(map2);
            if(r1 == r3) throw std::runtime_error("Bad comparaison !=");

            //test serialize -> deserialize
            UserData userdata;
            userdata << r1;

            Response r4;
            userdata >> r4;

            if(r1 != r4) throw std::runtime_error("Bad serialization to userdata");

            printf (" *<=  Test #%s > OK\n", testNumber.c_str ());
            testsResults.emplace_back (" Test #" + testNumber + " " + testName, true);
        }
        catch (const std::exception &e) {
            printf (" *<=  Test #%s > Failed\n", testNumber.c_str ());
            printf ("Error: %s\n", e.what ());
            testsResults.emplace_back (" Test #" + testNumber + " " + testName, false);
        }
    }
    printf ("OK\n");

    //Next test
    testNumber = "2.4";
    testName = "Check List Feature Response";
    printf ("\n-------------------------------------------------------------\n");
    {
        printf (" *=>  Test #%s %s\n", testNumber.c_str (), testName.c_str ());
        try
        {
            FeatureDependencies d1;
            d1.add_dependencies("A");
            d1.add_dependencies("B");

            Response r1 = createListFeatureResponse({{"test", d1}}, defaultVersion, passPhraseDef);
            std::cout << r1 << std::endl;

            //test ==
            Response r2 = createListFeatureResponse({{"test", d1}}, defaultVersion, passPhraseDef);
            if(r1 != r2) throw std::runtime_error("Bad comparaison ==");

            //test !=
            FeatureDependencies d2;
            d1.add_dependencies("C");
            d1.add_dependencies("B");

            Response r3 = createListFeatureResponse({{"test", d2}}, defaultVersion, passPhraseDef);
            if(r1 == r3) throw std::runtime_error("Bad comparaison !=");

            //test serialize -> deserialize
            UserData userdata;
            userdata << r1;

            Response r4;
            userdata >> r4;

            if(r1 != r4) throw std::runtime_error("Bad serialization to userdata");

            printf (" *<=  Test #%s > OK\n", testNumber.c_str ());
            testsResults.emplace_back (" Test #" + testNumber + " " + testName, true);
        }
        catch (const std::exception &e) {
            printf (" *<=  Test #%s > Failed\n", testNumber.c_str ());
            printf ("Error: %s\n", e.what ());
            testsResults.emplace_back (" Test #" + testNumber + " " + testName, false);
        }
    }
    printf ("OK\n");

    //Next test
    testNumber = "3.1";
    testName = "Check add operation on Save Response";
    printf ("\n-------------------------------------------------------------\n");
    {
        printf (" *=>  Test #%s %s\n", testNumber.c_str (), testName.c_str ());
        try
        {
            Feature f1;
            f1.set_version("1.0");
            f1.set_data(DATA);

            FeatureStatus s1;
            s1.set_status(Status::SUCCESS);

            FeatureAndStatus fs1;
            *(fs1.mutable_feature()) = f1;
            *(fs1.mutable_status()) = s1;

            Response r1 = createSaveResponse({{"test",fs1}}, defaultVersion);

            Feature f2;
            f2.set_version("3.5");
            f2.set_data("data-2");

            FeatureStatus s2;
            s2.set_status(Status::FAILED);
            s2.set_error("bad version");

            FeatureAndStatus fs2;
            *(fs2.mutable_feature()) = f2;
            *(fs2.mutable_status()) = s2;

            Response r2 = createSaveResponse({{"test-2",fs2}}, defaultVersion);

            Response r3 = createSaveResponse({{"test",fs1},{"test-2",fs2}}, defaultVersion);

            Response r = r1 + r2;
            std::cout << r << std::endl;

            if(r != r3) throw std::runtime_error("Bad aggregation");

            printf (" *<=  Test #%s > OK\n", testNumber.c_str ());
            testsResults.emplace_back (" Test #" + testNumber + " " + testName, true);
        }
        catch (const std::exception &e) {
            printf (" *<=  Test #%s > Failed\n", testNumber.c_str ());
            printf ("Error: %s\n", e.what ());
            testsResults.emplace_back (" Test #" + testNumber + " " + testName, false);
        }
    }
    printf ("OK\n");

    //Next test
    testNumber = "3.2";
    testName = "Check add operation on Restore Response";
    printf ("\n-------------------------------------------------------------\n");
    {
        printf (" *=>  Test #%s %s\n", testNumber.c_str (), testName.c_str ());
        try
        {
            FeatureStatus s1;
            s1.set_status(Status::SUCCESS);
            Response r1 = createRestoreResponse({{"test", s1}});

            FeatureStatus s2;
            s2.set_status(Status::FAILED);
            s2.set_error("error msg");

            Response r2 = createRestoreResponse({{"test-2", s2}});

            Response r = createRestoreResponse({{"test", s1},{"test-2", s2}});

            Response r3 = r1 + r2;
            std::cout << r3 << std::endl;

            if(r != r3) throw std::runtime_error("Bad aggregation");

            printf (" *<=  Test #%s > OK\n", testNumber.c_str ());
            testsResults.emplace_back (" Test #" + testNumber + " " + testName, true);
        }
        catch (const std::exception &e) {
            printf (" *<=  Test #%s > Failed\n", testNumber.c_str ());
            printf ("Error: %s\n", e.what ());
            testsResults.emplace_back (" Test #" + testNumber + " " + testName, false);
        }
    }
    printf ("OK\n");

    //Next test
    testNumber = "3.3";
    testName = "Check add operation on Reset Response";
    printf ("\n-------------------------------------------------------------\n");
    {
        printf (" *=>  Test #%s %s\n", testNumber.c_str (), testName.c_str ());
        try
        {
            FeatureStatus s1;
            s1.set_status(Status::SUCCESS);
            Response r1 = createResetResponse({{"test", s1}});

            FeatureStatus s2;
            s2.set_status(Status::FAILED);
            s2.set_error("error msg");

            Response r2 = createResetResponse({{"test-2", s2}});

            Response r = createResetResponse({{"test", s1},{"test-2", s2}});

            Response r3 = r1 + r2;
            std::cout << r3 << std::endl;

            if(r != r3) throw std::runtime_error("Bad aggregation");

            printf (" *<=  Test #%s > OK\n", testNumber.c_str ());
            testsResults.emplace_back (" Test #" + testNumber + " " + testName, true);
        }
        catch (const std::exception &e) {
            printf (" *<=  Test #%s > Failed\n", testNumber.c_str ());
            printf ("Error: %s\n", e.what ());
            testsResults.emplace_back (" Test #" + testNumber + " " + testName, false);
        }
    }

    printf ("OK\n");

    //Next test
    testNumber = "3.4";
    testName = "Check add operation on List Feature Response";
    printf ("\n-------------------------------------------------------------\n");
    {
        printf (" *=>  Test #%s %s\n", testNumber.c_str (), testName.c_str ());
        try
        {
            FeatureDependencies d1;
            d1.add_dependencies("A");
            d1.add_dependencies("B");

            Response r1 = createListFeatureResponse({{"test", d1}}, defaultVersion, passPhraseDef);

            FeatureDependencies d2;
            d2.add_dependencies("A");
            d2.add_dependencies("B");

            Response r2 = createListFeatureResponse({{"test2", d2}}, defaultVersion, passPhraseDef);

            Response r3 = createListFeatureResponse({{"test", d1},{"test2", d2}}, defaultVersion, passPhraseDef);

            Response r = r1 + r2;
            std::cout << r << std::endl;

            if(r != r3) throw std::runtime_error("Bad aggregation");

            printf (" *<=  Test #%s > OK\n", testNumber.c_str ());
            testsResults.emplace_back (" Test #" + testNumber + " " + testName, true);
        }
        catch (const std::exception &e) {
            printf (" *<=  Test #%s > Failed\n", testNumber.c_str ());
            printf ("Error: %s\n", e.what ());
            testsResults.emplace_back (" Test #" + testNumber + " " + testName, false);
        }
    }

    printf ("OK\n");

    //Next test
    testNumber = "4.1";
    testName = "Deserialize save query from UI";
    printf ("\n-------------------------------------------------------------\n");
    {
        printf (" *=>  Test #%s %s\n", testNumber.c_str (), testName.c_str ());
        try
        {
            std::string saveQueryJson =R"TEST({"version":"1.0", "passphrase": "", "sessionToken":"D9XQmBnFtU1GX/JhZtQZrIOl", "featuresList": [{"name":"etn-mass-management"}]})TEST";
            Query query = saveQueryFromUiJson (saveQueryJson);

            if(query.parameters_case() != Query::ParametersCase::kSave)
            {
                const cxxtools::SerializationInfo si = deserializeJson(saveQueryJson);
                std::cout << serializeJson(si, true) << std::endl;
                std::cout << query << std::endl;

                throw std::runtime_error("Invalid query type");
            }

            if(query.save().features(0) != "etn-mass-management")
            {
                const cxxtools::SerializationInfo si = deserializeJson(saveQueryJson);
                std::cout << serializeJson(si, true) << std::endl;
                std::cout << query << std::endl;
                throw std::runtime_error("bad deserialisation from UI");
            }


            printf (" *<=  Test #%s > OK\n", testNumber.c_str ());
            testsResults.emplace_back (" Test #" + testNumber + " " + testName, true);
        }
        catch (const std::exception &e) {
            printf (" *<=  Test #%s > Failed\n", testNumber.c_str ());
            printf ("Error: %s\n", e.what ());
            testsResults.emplace_back (" Test #" + testNumber + " " + testName, false);
        }
    }

    printf ("OK\n");

    //Next test
    testNumber = "4.2";
    testName = "Deserialize restore query from UI";
    printf ("\n-------------------------------------------------------------\n");
    {
        printf (" *=>  Test #%s %s\n", testNumber.c_str (), testName.c_str ());
        try
        {
            std::string restoreQueryJson =R"TEST({
                    "passphrase":"a","version":"1.0","checksum":"qDYTMxDem+", "sessionToken":"",
                    "data":[{"automations":{"version":"1.0","data":{"automationList":[{"id":"etn_automation_id_1576571918088",
                    "name":"Test","createdBy":"admin","createdOn":"2019-12-17T08:38:38.088+0000","comments":"","active":false,
                    "timeout":36000,"notification":{"notifyOnFailure":false,"emails":[]}}]}}}]})TEST";
            Query queryRestore = restoreQueryFromUiJson (restoreQueryJson);
            std::cout << queryRestore << std::endl;

            dto::UserData reqData;
            reqData << queryRestore;
            std::cout << reqData.front() << std::endl;

            if(queryRestore.parameters_case() != Query::ParametersCase::kRestore)
            {
                std::cout << queryRestore << std::endl;
                throw std::runtime_error("Invalid query type");
            }

            printf (" *<=  Test #%s > OK\n", testNumber.c_str ());
            testsResults.emplace_back (" Test #" + testNumber + " " + testName, true);
        }
        catch (const std::exception &e) {
            printf (" *<=  Test #%s > Failed\n", testNumber.c_str ());
            printf ("Error: %s\n", e.what ());
            testsResults.emplace_back (" Test #" + testNumber + " " + testName, false);
        }
    }

    //Next test
    testNumber = "4.3";
    testName = "Deserialize restore query from automation";
    printf ("\n-------------------------------------------------------------\n");
    {
        printf (" *=>  Test #%s %s\n", testNumber.c_str (), testName.c_str ());
        try
        {
            std::string restoreQueryJson = R"TEST(
                   {"version": "1.0","passphrase": "my pass phrase","checksum":"qDYTMxDem+","sessionToken":"",
                    "data": [{"automation-settings": {"version": "1.0","status": "success","error": "",
                    "data": {"server": {"timeout": "10000","background": "0","workdir": ".","verbose": "0"}}}},
                    {"automations": {"version": "1.0","status": "success","error": "",
                    "data": {"automationList": [{"id": "etn_automation_id_1576571918088","name":
                    "Test","createdBy": "admin","createdOn": "2019-12-17T08:38:38.088+0000","comments": "","active": false,"timeout": 36000,
                    "notification": {"notifyOnFailure": false,"emails": []},"schedule": "2019-12-17T08:37:00.000Z",
                    "triggerType": "CAT_SCHEDULE","triggers": {"ipmInfraEvent": []},"tasks": [{"index": 0,"name": "Wait 10 seconds",
                    "group": "WAIT","subgroup": "DELAY","properties": [{"key": "duration","value": ["10"]}],"timeout": 3600}]}]}}}]})TEST";

            Query queryRestore = restoreQueryFromUiJson (restoreQueryJson);
            std::cout << queryRestore << std::endl;

            if(queryRestore.parameters_case() != Query::ParametersCase::kRestore)
            {
                std::cout << queryRestore << std::endl;
                throw std::runtime_error("Invalid query type");
            }

            if(queryRestore.restore().map_features_data().at("automation-settings").data() != "{\"server\":{\"timeout\":\"10000\",\"background\":\"0\",\"workdir\":\".\",\"verbose\":\"0\"}}")
            {
                std::cout << queryRestore << std::endl;
                throw std::runtime_error("bad data object serialisation");
            }

            printf (" *<=  Test #%s > OK\n", testNumber.c_str ());
            testsResults.emplace_back (" Test #" + testNumber + " " + testName, true);
        }
        catch (const std::exception &e) {
            printf (" *<=  Test #%s > Failed\n", testNumber.c_str ());
            printf ("Error: %s\n", e.what ());
            testsResults.emplace_back (" Test #" + testNumber + " " + testName, false);
        }
    }

    printf ("OK\n");

    //Next test
    testNumber = "4.4";
    testName = "Serialize save response for UI";
    printf ("\n-------------------------------------------------------------\n");
    {
        printf (" *=>  Test #%s %s\n", testNumber.c_str (), testName.c_str ());
        try
        {
            Feature f1;
            f1.set_version("1.0");
            f1.set_data("{\"timeout\":\"40\"}");

            FeatureStatus s1;
            s1.set_status(Status::SUCCESS);

            FeatureAndStatus fs1;
            *(fs1.mutable_feature()) = f1;
            *(fs1.mutable_status()) = s1;

            Feature f2;
            f2.set_version("1.0");
            f2.set_data("data in text");

            FeatureStatus s2;
            s2.set_status(Status::SUCCESS);

            FeatureAndStatus fs2;
            *(fs2.mutable_feature()) = f2;
            *(fs2.mutable_status()) = s2;


            Response r = createSaveResponse({{"object",fs1},{"no-object",fs2}}, defaultVersion);

            std::string strV1 = "{\"version\":\"1.0\",\"checksum\":\"\",\"status\":\"success\",\"error\":\"\",\"data\":[{\"no-object\":{\"version\":\"1.0\",\"status\":\"success\",\"error\":\"\",\"data\":\"data in text\"}},{\"object\":{\"version\":\"1.0\",\"status\":\"success\",\"error\":\"\",\"data\":{\"timeout\":\"40\"}}}]}";
            std::string strV2 = "{\"version\":\"1.0\",\"checksum\":\"\",\"status\":\"success\",\"error\":\"\",\"data\":[{\"object\":{\"version\":\"1.0\",\"status\":\"success\",\"error\":\"\",\"data\":{\"timeout\":\"40\"}}},{\"no-object\":{\"version\":\"1.0\",\"status\":\"success\",\"error\":\"\",\"data\":\"data in text\"}}]}";

            std::string responseInStr = responseToUiJson(r);
            std::cout << "responseToUiJson " << responseInStr << std::endl;

            if(responseInStr != strV1 && responseInStr != strV2)
            {
                std::cout << responseToUiJson(r) << std::endl;
                throw std::runtime_error("invalid response");
            }

            UserData userdata1;
            userdata1 << r;

            Response r1;
            userdata1 >> r1;
            std::string responseInStrAfterDeserilize = responseToUiJson(r1);
            std::cout << "responseInStrAfterDeserilize " << responseInStrAfterDeserilize << std::endl;

            if(r != r1) throw std::runtime_error("Bad serialization to userdata");

            printf (" *<=  Test #%s > OK\n", testNumber.c_str ());
            testsResults.emplace_back (" Test #" + testNumber + " " + testName, true);
        }
        catch (const std::exception &e) {
            printf (" *<=  Test #%s > Failed\n", testNumber.c_str ());
            printf ("Error: %s\n", e.what ());
            testsResults.emplace_back (" Test #" + testNumber + " " + testName, false);
        }
    }

    //Next test
    testNumber = "4.5";
    testName = "Serialize FeatureStatus from EMC4J";
    printf ("\n-------------------------------------------------------------\n");
    {
        printf (" *=>  Test #%s %s\n", testNumber.c_str (), testName.c_str ());
        try
        {

            std::map<FeatureName, FeatureAndStatus> mapResponse;

            Feature f1;
            f1.set_version("1.0");

            FeatureStatus s1;
            s1.set_status(Status::SUCCESS);

            FeatureAndStatus fs1;
            *(fs1.mutable_feature()) = f1;
            *(fs1.mutable_status()) = s1;

            Feature f2;
            f2.set_version("1.0");

            FeatureStatus s2;
            s2.set_status(Status::SUCCESS);

            FeatureAndStatus fs2;
            *(fs2.mutable_feature()) = f2;
            *(fs2.mutable_status()) = s2;

            mapResponse["automation"] = fs1;
            mapResponse["virtual-asset"] = fs2;

            Response r3 = createSaveResponse(mapResponse, defaultVersion);

            std::cout << r3 << std::endl;

            dto::UserData resp;
            resp.push_back("{\"statusList\":[{\"name\":\"automation\",\"status\":\"success\",\"error\":\"\"}]}");
            resp.push_back("restore");

            Response partialResp;
            resp >> partialResp;

            printf (" *<=  Test #%s > OK\n", testNumber.c_str ());
            testsResults.emplace_back (" Test #" + testNumber + " " + testName, true);
        }
        catch (const std::exception &e) {
            printf (" *<=  Test #%s > Failed\n", testNumber.c_str ());
            printf ("Error: %s\n", e.what ());
            testsResults.emplace_back (" Test #" + testNumber + " " + testName, false);
        }
    }

    printf ("OK\n");

    //Next test
    testNumber = "5.1";
    testName = "Serialize/Unserialize session token";
    printf ("\n-------------------------------------------------------------\n");
    {
        printf (" *=>  Test #%s %s\n", testNumber.c_str (), testName.c_str ());
        try
        {
            UserData userdata;
            std::string sessionToken{"D9XQmBnFtU1GX/JhZtQZrIOl"};
            Query query = createSaveQuery({"test"},"myPassphrase", sessionToken);

            REQUIRE(query.save().session_token() == "D9XQmBnFtU1GX/JhZtQZrIOl");
            std::cout << query << std::endl;
            // test serialize -> deserialize
            userdata << query;
            std::string payload = userdata.front();

            query = saveQueryFromUiJson (payload);
            REQUIRE(query.save().session_token() == "D9XQmBnFtU1GX/JhZtQZrIOl");

            printf (" *<=  Test #%s > OK\n", testNumber.c_str ());
            testsResults.emplace_back (" Test #" + testNumber + " " + testName, true);
        }
        catch (const std::exception &e) {
            printf (" *<=  Test #%s > Failed\n", testNumber.c_str ());
            printf ("Error: %s\n", e.what ());
            testsResults.emplace_back (" Test #" + testNumber + " " + testName, false);
        }
    }

    printf ("OK\n");

    testNumber = "5.2";
    testName = "Adding session token from a fresh payload";
    printf ("\n-------------------------------------------------------------\n");
    {
        printf (" *=>  Test #%s %s\n", testNumber.c_str (), testName.c_str ());
        try
        {
            std::string sessionToken{"D9XQmBnFtU1GX/JhZtQZrIOl"};
            // Build input payload
            cxxtools::SerializationInfo si;
            si.addMember(PASS_PHRASE) <<= "myPassphrase";
            si.addMember(SESSION_TOKEN) <<= sessionToken;
            cxxtools::SerializationInfo& featuresSi = si.addMember(FEATURE_LIST);
            cxxtools::SerializationInfo& featureSi = featuresSi.addMember("");
            featureSi.addMember(FEATURE_NAME) <<= "test";
            featuresSi.setCategory(cxxtools::SerializationInfo::Category::Array);
            // Test if the session token is in the save query
            auto queryWithSessionToken = saveQueryFromUiJson (JSON::writeToString(si));
            REQUIRE(queryWithSessionToken.save().session_token() == "D9XQmBnFtU1GX/JhZtQZrIOl");

            printf (" *<=  Test #%s > OK\n", testNumber.c_str ());
            testsResults.emplace_back (" Test #" + testNumber + " " + testName, true);
        }
        catch (const std::exception &e) {
            printf (" *<=  Test #%s > Failed\n", testNumber.c_str ());
            printf ("Error: %s\n", e.what ());
            testsResults.emplace_back (" Test #" + testNumber + " " + testName, false);
        }
    }

    printf ("OK\n");

    //Collect results
    printf ("\n-------------------------------------------------------------\n");

  uint32_t testsPassed = 0;
  uint32_t testsFailed = 0;


  printf("\tSummary tests from fty_srr_dto_test\n");
  for(const auto & result : testsResults)
  {
    if(result.second)
    {
      printf(ANSI_COLOR_GREEN"\tOK " ANSI_COLOR_RESET "\t%s\n",result.first.c_str());
      testsPassed++;
    }
    else
    {
      printf(ANSI_COLOR_RED"\tNOK" ANSI_COLOR_RESET "\t%s\n",result.first.c_str());
      testsFailed++;
    }
  }

  printf ("\n-------------------------------------------------------------\n");

  if(testsFailed == 0)
  {
    printf(ANSI_COLOR_GREEN"\n %i tests passed, everything is ok\n" ANSI_COLOR_RESET "\n",testsPassed);
        printf ("OK\n");
  }
  else
  {
    printf(ANSI_COLOR_RED"\n!!!!!!!! %i/%i tests did not pass !!!!!!!! \n" ANSI_COLOR_RESET "\n",testsFailed,(testsPassed+testsFailed));
  }

  REQUIRE(testsFailed == 0);
}
